/* PocketDarwin ELF Loader - EBBR UEFI Application
 *
 * Barebones bootloader that enumerates U-Boot storage via UEFI protocols,
 * lists .elf/.elf64 files on each volume, and loads+executes selected ELFs.
 * All output goes through the GOP framebuffer only (no ConOut text).
 * Motorola Harpia: Volume Up/Down navigate, Power selects (mapped as Enter).
 */

#include "uefi.h"
#include "elf.h"
#include "font.h"

/* ------------------------------------------------------------------ */
/* Global state                                                        */
/* ------------------------------------------------------------------ */
static EFI_SYSTEM_TABLE            *gST;
static EFI_BOOT_SERVICES          *gBS;
static EFI_HANDLE                  gImageHandle;

static EFI_GRAPHICS_OUTPUT_PROTOCOL *gGOP;
static void    *gFBBase;
static UINTN    gFBSize;
static UINT32   gFBWidth;
static UINT32   gFBHeight;
static UINT32   gFBStride;
static BOOLEAN  gFBIsBGR;

static EFI_SIMPLE_TEXT_INPUT_PROTOCOL *gConIn;

/* ------------------------------------------------------------------ */
/* Protocol GUIDs as variables (macros expand to initializers)         */
/* ------------------------------------------------------------------ */
static EFI_GUID GraphicsOutputProtocolGuidVar  = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
static EFI_GUID BlockIoProtocolGuidVar         = EFI_BLOCK_IO_PROTOCOL_GUID;
static EFI_GUID SimpleFileSystemProtocolGuidVar = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
static EFI_GUID FileInfoGuidVar                = EFI_FILE_INFO_ID;

/* ------------------------------------------------------------------ */
/* Max file list                                                       */
/* ------------------------------------------------------------------ */
#define MAX_ELF_FILES 64
#define MAX_PATH      256

typedef struct {
    CHAR16  name[MAX_PATH];
    CHAR16  volume[MAX_PATH];
    UINTN   volume_idx;
    BOOLEAN is_64bit;
} elf_entry_t;

static elf_entry_t gFiles[MAX_ELF_FILES];
static UINTN       gFileCount;
static INTN        gCursor;

/* Storage device info */
#define MAX_STORAGE 16
static EFI_HANDLE gStorageHandles[MAX_STORAGE];
static UINTN      gStorageCount;

/* ------------------------------------------------------------------ */
/* String helpers                                                      */
/* ------------------------------------------------------------------ */
static UINTN str16_len(const CHAR16 *s) {
    UINTN n = 0;
    while (s[n]) n++;
    return n;
}

static void str16_copy(CHAR16 *dst, const CHAR16 *src, UINTN max) {
    UINTN i;
    for (i = 0; i < max - 1 && src[i]; i++)
        dst[i] = src[i];
    dst[i] = 0;
}

static void str16_cat(CHAR16 *dst, const CHAR16 *src) {
    UINTN d = str16_len(dst);
    UINTN i = 0;
    while (src[i] && d + i < MAX_PATH - 1) {
        dst[d + i] = src[i];
        i++;
    }
    dst[d + i] = 0;
}

static BOOLEAN str16_eq(const CHAR16 *a, const CHAR16 *b) {
    while (*a && *b) {
        if (*a != *b) return FALSE;
        a++; b++;
    }
    return *a == *b;
}

static BOOLEAN str16_ieq(const CHAR16 *a, const CHAR16 *b) {
    while (*a && *b) {
        CHAR16 ca = *a, cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return FALSE;
        a++; b++;
    }
    return *a == *b;
}

static BOOLEAN str16_ends_with_ieq(const CHAR16 *s, const CHAR16 *suffix) {
    UINTN sl = str16_len(s);
    UINTN fl = str16_len(suffix);
    if (sl < fl) return FALSE;
    return str16_ieq(&s[sl - fl], suffix);
}

static void ch16_to_ch8(const CHAR16 *src, char *dst, UINTN max) {
    UINTN i;
    for (i = 0; i < max - 1 && src[i]; i++)
        dst[i] = (char)src[i];
    dst[i] = 0;
}

static void u64_to_hex_str16(UINT64 val, CHAR16 *buf) {
    const CHAR16 hex[] = u"0123456789abcdef";
    int i;
    buf[0] = '0'; buf[1] = 'x';
    for (i = 15; i >= 0; i--)
        buf[2 + (15 - i)] = hex[(val >> (i * 4)) & 0xf];
    buf[18] = 0;
}

static void u64_to_dec_str16(UINT64 val, CHAR16 *buf) {
    CHAR16 tmp[21];
    int i = 0;
    if (val == 0) { buf[0] = '0'; buf[1] = 0; return; }
    while (val > 0) { tmp[i++] = u'0' + (val % 10); val /= 10; }
    for (int j = 0; j < i; j++) buf[j] = tmp[i - 1 - j];
    buf[i] = 0;
}

/* ------------------------------------------------------------------ */
/* Framebuffer graphics (GOP only)                                     */
/* ------------------------------------------------------------------ */
static inline UINT32 fb_color(UINT8 r, UINT8 g, UINT8 b) {
    if (gFBIsBGR)
        return ((UINT32)r << 16) | ((UINT32)g << 8) | b;
    else
        return ((UINT32)b << 16) | ((UINT32)g << 8) | r;
}

#define COLOR_BLACK   fb_color(0, 0, 0)
#define COLOR_WHITE   fb_color(255, 255, 255)
#define COLOR_CYAN    fb_color(0, 200, 200)
#define COLOR_GREEN   fb_color(0, 255, 0)
#define COLOR_RED     fb_color(255, 0, 0)
#define COLOR_YELLOW  fb_color(255, 255, 0)
#define COLOR_GRAY    fb_color(128, 128, 128)
#define COLOR_DKGRAY  fb_color(48, 48, 48)
#define COLOR_SEL_BG  fb_color(0, 80, 120)

static void fb_clear(UINT32 color) {
    UINT32 *fb = (UINT32 *)gFBBase;
    for (UINTN i = 0; i < (UINTN)gFBStride * gFBHeight; i++)
        fb[i] = color;
}

static void fb_put_pixel(UINTN x, UINTN y, UINT32 color) {
    if (x < gFBWidth && y < gFBHeight)
        ((UINT32 *)gFBBase)[y * gFBStride + x] = color;
}

static void fb_fill_rect(UINTN x, UINTN y, UINTN w, UINTN h, UINT32 color) {
    UINT32 *fb = (UINT32 *)gFBBase;
    for (UINTN row = y; row < y + h && row < gFBHeight; row++)
        for (UINTN col = x; col < x + w && col < gFBWidth; col++)
            fb[row * gFBStride + col] = color;
}

static void fb_putchar(UINTN x, UINTN y, CHAR16 ch, UINT32 fg) {
    if (ch < FONT_FIRST_CHAR || ch > FONT_LAST_CHAR) return;
    const UINT8 *glyph = &font_8x8[(ch - FONT_FIRST_CHAR) * FONT_W];
    for (int row = 0; row < FONT_H; row++) {
        UINT8 bits = glyph[row];
        for (int col = 0; col < FONT_W; col++) {
            if (bits & (0x80 >> col))
                fb_put_pixel(x + col, y + row, fg);
        }
    }
}

static void fb_puts(UINTN x, UINTN y, const CHAR16 *str, UINT32 fg) {
    UINTN cx = x;
    while (*str) {
        if (*str == u'\n') {
            cx = x;
            y += FONT_H + 2;
        } else {
            fb_putchar(cx, y, *str, fg);
            cx += FONT_W;
        }
        str++;
    }
}

static void fb_puts_char8(UINTN x, UINTN y, const char *str, UINT32 fg) {
    UINTN cx = x;
    while (*str) {
        if (*str == '\n') {
            cx = x;
            y += FONT_H + 2;
        } else {
            fb_putchar(cx, y, (CHAR16)(unsigned char)*str, fg);
            cx += FONT_W;
        }
        str++;
    }
}

static UINTN fb_string_width(const CHAR16 *str) {
    return str16_len(str) * FONT_W;
}

/* ------------------------------------------------------------------ */
/* Key input (Harpia: Vol Up=Up, Vol Down=Down, Power=Enter)          */
/* ------------------------------------------------------------------ */
static BOOLEAN wait_for_key(EFI_INPUT_KEY *key) {
    if (!gConIn) return FALSE;
    UINTN index = 0;
    EFI_EVENT events[1] = { gConIn->WaitForKey };
    gBS->WaitForEvent(1, events, &index);
    return !EFI_ERROR(gConIn->ReadKeyStroke(gConIn, key));
}

static BOOLEAN is_select_key(const EFI_INPUT_KEY *key) {
    return key->UnicodeChar == 0x0D || key->UnicodeChar == u'\r' ||
           key->UnicodeChar == u'\n';
}

static BOOLEAN is_up_key(const EFI_INPUT_KEY *key) {
    return key->ScanCode == SCAN_UP;
}

static BOOLEAN is_down_key(const EFI_INPUT_KEY *key) {
    return key->ScanCode == SCAN_DOWN;
}

/* ------------------------------------------------------------------ */
/* Protocol location helpers                                           */
/* ------------------------------------------------------------------ */
static void *locate_protocol(EFI_GUID *guid) {
    void *iface = NULL;
    gBS->LocateProtocol(guid, NULL, &iface);
    return iface;
}

/* ------------------------------------------------------------------ */
/* Storage enumeration (Block IO)                                      */
/* ------------------------------------------------------------------ */
static void enumerate_storage(void) {
    gStorageCount = 0;
    EFI_HANDLE *handles = NULL;
    UINTN count = 0;

    EFI_STATUS s = gBS->LocateHandleBuffer(
        ByProtocol, &BlockIoProtocolGuidVar, NULL, &count, &handles);

    if (EFI_ERROR(s) || !handles || count == 0) return;

    for (UINTN i = 0; i < count && gStorageCount < MAX_STORAGE; i++) {
        EFI_BLOCK_IO_PROTOCOL *bio = NULL;
        s = gBS->OpenProtocol(handles[i], &BlockIoProtocolGuidVar,
                              (void **)&bio, NULL, NULL,
                              EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(s) || !bio) continue;

        if (!bio->Media) continue;

        gStorageHandles[gStorageCount] = handles[i];
        gStorageCount++;
    }

    gBS->FreePool(handles);
}

static void draw_storage_section(UINTN y) {
    fb_puts(10, y, u"Storage Devices:", COLOR_CYAN);
    y += FONT_H + 6;

    if (gStorageCount == 0) {
        fb_puts(30, y, u"(none found)", COLOR_GRAY);
        y += FONT_H + 4;
        return;
    }

    for (UINTN i = 0; i < gStorageCount; i++) {
        EFI_BLOCK_IO_PROTOCOL *bio = NULL;
        EFI_STATUS s = gBS->OpenProtocol(
            gStorageHandles[i], &BlockIoProtocolGuidVar,
            (void **)&bio, NULL, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        CHAR16 line[MAX_PATH];
        str16_copy(line, u"  [", MAX_PATH);

        CHAR16 num[4];
        num[0] = u'0' + (CHAR16)i;
        num[1] = 0;
        str16_cat(line, num);
        str16_cat(line, u"] ");

        if (!EFI_ERROR(s) && bio && bio->Media) {
            if (bio->Media->RemovableMedia)
                str16_cat(line, u"Removable ");
            else
                str16_cat(line, u"Fixed ");

            str16_cat(line, u"BlockDev ");

            CHAR16 lba_str[21];
            u64_to_dec_str16(bio->Media->LastBlock + 1, lba_str);
            str16_cat(line, lba_str);
            str16_cat(line, u" blocks");

            if (gStorageHandles[i]) {
                gBS->CloseProtocol(gStorageHandles[i],
                                   &BlockIoProtocolGuidVar, NULL, NULL);
            }
        } else {
            str16_cat(line, u"Block Device");
        }

        fb_puts(30, y, line, COLOR_WHITE);
        y += FONT_H + 4;
    }
}

/* ------------------------------------------------------------------ */
/* Filesystem scanning (.elf / .elf64)                                 */
/* ------------------------------------------------------------------ */
static void scan_volume_files(EFI_HANDLE handle, const CHAR16 *vol_name) {
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
    EFI_STATUS s = gBS->OpenProtocol(
        handle, &SimpleFileSystemProtocolGuidVar,
        (void **)&fs, NULL, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(s) || !fs) return;

    EFI_FILE_PROTOCOL *root = NULL;
    s = fs->OpenVolume(fs, &root);
    if (EFI_ERROR(s) || !root) return;

    UINTN buf_size = 4096;
    EFI_FILE_INFO *info = NULL;
    s = gBS->AllocatePool(EfiBootServicesData, buf_size, (void **)&info);
    if (EFI_ERROR(s)) { root->Close(root); return; }

    while (1) {
        buf_size = 4096;
        s = root->Read(root, &buf_size, info);
        if (EFI_ERROR(s) || buf_size == 0) break;

        if (info->Attribute & EFI_FILE_DIRECTORY) continue;

        if (str16_ends_with_ieq(info->FileName, u".elf") ||
            str16_ends_with_ieq(info->FileName, u".elf64")) {
            if (gFileCount < MAX_ELF_FILES) {
                elf_entry_t *e = &gFiles[gFileCount];
                str16_copy(e->name, info->FileName, MAX_PATH);
                str16_copy(e->volume, vol_name, MAX_PATH);
                e->volume_idx = gStorageCount; /* mark which scan pass */
                e->is_64bit = str16_ends_with_ieq(info->FileName, u".elf64");
                gFileCount++;
            }
        }
    }

    gBS->FreePool(info);
    root->Close(root);
}

static void enumerate_elf_files(void) {
    gFileCount = 0;
    EFI_HANDLE *handles = NULL;
    UINTN count = 0;

    EFI_STATUS s = gBS->LocateHandleBuffer(
        ByProtocol, &SimpleFileSystemProtocolGuidVar, NULL, &count, &handles);

    if (EFI_ERROR(s) || !handles || count == 0) return;

    for (UINTN i = 0; i < count; i++) {
        CHAR16 vol_name[32];
        str16_copy(vol_name, u"FS", 32);
        CHAR16 num[4];
        num[0] = u'0' + (CHAR16)i;
        num[1] = 0;
        str16_cat(vol_name, num);
        scan_volume_files(handles[i], vol_name);
    }

    gBS->FreePool(handles);
}

static void draw_files_section(UINTN y) {
    fb_puts(10, y, u"ELF Programs:", COLOR_GREEN);
    y += FONT_H + 6;

    if (gFileCount == 0) {
        fb_puts(30, y, u"(no .elf/.elf64 files found)", COLOR_GRAY);
        return;
    }

    for (UINTN i = 0; i < gFileCount; i++) {
        UINT32 fg = COLOR_WHITE;
        UINT32 bg = COLOR_BLACK;
        BOOLEAN selected = ((INTN)i == gCursor);

        if (selected) {
            fg = COLOR_WHITE;
            bg = COLOR_SEL_BG;
        }

        /* Highlight bar */
        if (selected) {
            fb_fill_rect(20, y - 1, gFBWidth - 40, FONT_H + 2, bg);
        }

        CHAR16 prefix[8];
        prefix[0] = selected ? u'>' : u' ';
        prefix[1] = u' ';
        prefix[2] = 0;

        CHAR16 line[MAX_PATH];
        str16_copy(line, prefix, MAX_PATH);
        str16_cat(line, gFiles[i].name);

        if (gFiles[i].is_64bit)
            str16_cat(line, u"  [64-bit]");
        else
            str16_cat(line, u"  [32-bit]");

        fb_puts(30, y, line, fg);
        y += FONT_H + 4;
    }
}

static void draw_ui(void) {
    fb_clear(COLOR_BLACK);

    /* Title bar */
    fb_fill_rect(0, 0, gFBWidth, FONT_H + 8, fb_color(0, 40, 80));
    fb_puts(10, 4, u"PocketDarwin ELF Loader", COLOR_WHITE);

    /* Dimensions */
    CHAR16 dim[64];
    str16_copy(dim, u"FB: ", 64);
    CHAR16 num[16];
    u64_to_dec_str16(gFBWidth, num);
    str16_cat(dim, num);
    str16_cat(dim, u"x");
    u64_to_dec_str16(gFBHeight, num);
    str16_cat(dim, num);
    fb_puts(gFBWidth - fb_string_width(dim) - 10, 4, dim, COLOR_GRAY);

    UINTN y = FONT_H + 12;
    draw_storage_section(y);

    /* skip past storage entries to draw files */
    y = FONT_H + 12 + (gStorageCount + 2) * (FONT_H + 4) + 8;
    draw_files_section(y);

    /* Footer */
    UINTN fy = gFBHeight - FONT_H - 8;
    fb_fill_rect(0, fy - 2, gFBWidth, FONT_H + 6, fb_color(0, 40, 80));
    fb_puts(10, fy, u"VolUp/VolDown: Navigate   Power: Select", COLOR_GRAY);
}

/* ------------------------------------------------------------------ */
/* ELF loading and execution                                          */
/* ------------------------------------------------------------------ */
static EFI_STATUS do_exit_boot_services(void) {
    EFI_STATUS s;
    UINTN mmap_size = 0, key = 0, desc_size = 0;
    UINT32 desc_ver = 0;

    s = gBS->GetMemoryMap(&mmap_size, NULL, &key, &desc_size, &desc_ver);
    if (s != EFI_BUFFER_TOO_SMALL && EFI_ERROR(s)) return s;

    mmap_size += 2 * desc_size;
    EFI_MEMORY_DESCRIPTOR *mmap = NULL;
    s = gBS->AllocatePool(EfiBootServicesData, mmap_size, (void **)&mmap);
    if (EFI_ERROR(s)) return s;

    s = gBS->GetMemoryMap(&mmap_size, mmap, &key, &desc_size, &desc_ver);
    if (EFI_ERROR(s)) { gBS->FreePool(mmap); return s; }

    return gBS->ExitBootServices(gImageHandle, key);
}

static void load_and_boot(elf_entry_t *entry) {
    /* Find the filesystem handle for this volume */
    EFI_HANDLE *fs_handles = NULL;
    UINTN fs_count = 0;
    EFI_STATUS s = gBS->LocateHandleBuffer(
        ByProtocol, &SimpleFileSystemProtocolGuidVar, NULL, &fs_count, &fs_handles);
    if (EFI_ERROR(s) || !fs_handles || fs_count == 0) {
        fb_puts(10, gFBHeight - 30, u"ERROR: No filesystem", COLOR_RED);
        wait_for_key(&(EFI_INPUT_KEY){0});
        return;
    }

    /* Use first filesystem (volume_idx was set to the scan pass index) */
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
    s = gBS->OpenProtocol(
        fs_handles[0], &SimpleFileSystemProtocolGuidVar,
        (void **)&fs, NULL, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(s) || !fs) {
        gBS->FreePool(fs_handles);
        fb_puts(10, gFBHeight - 30, u"ERROR: Cannot open FS", COLOR_RED);
        wait_for_key(&(EFI_INPUT_KEY){0});
        return;
    }

    EFI_FILE_PROTOCOL *root = NULL;
    s = fs->OpenVolume(fs, &root);
    if (EFI_ERROR(s) || !root) {
        gBS->FreePool(fs_handles);
        return;
    }

    EFI_FILE_PROTOCOL *fp = NULL;
    s = root->Open(root, &fp, entry->name, EFI_FILE_MODE_READ, 0);
    root->Close(root);
    if (EFI_ERROR(s) || !fp) {
        fb_puts(10, gFBHeight - 30, u"ERROR: Cannot open file", COLOR_RED);
        wait_for_key(&(EFI_INPUT_KEY){0});
        gBS->FreePool(fs_handles);
        return;
    }

    /* Get file size */
    UINTN info_buf_size = sizeof(EFI_FILE_INFO) + MAX_PATH * 2;
    EFI_FILE_INFO *finfo = NULL;
    gBS->AllocatePool(EfiBootServicesData, info_buf_size, (void **)&finfo);
    s = fp->GetInfo(fp, &FileInfoGuidVar, &info_buf_size, finfo);
    UINT64 file_size = 0;
    if (!EFI_ERROR(s) && finfo)
        file_size = finfo->FileSize;
    if (finfo) gBS->FreePool(finfo);

    if (file_size == 0) {
        fp->Close(fp);
        fb_puts(10, gFBHeight - 30, u"ERROR: Empty file", COLOR_RED);
        wait_for_key(&(EFI_INPUT_KEY){0});
        gBS->FreePool(fs_handles);
        return;
    }

    /* Load file into memory */
    UINTN pages = (file_size + 0xFFF) / 0x1000;
    EFI_PHYSICAL_ADDRESS load_base = 0x20000000; /* 512MB */
    s = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData, pages, &load_base);
    if (EFI_ERROR(s)) {
        fp->Close(fp);
        fb_puts(10, gFBHeight - 30, u"ERROR: Out of memory", COLOR_RED);
        wait_for_key(&(EFI_INPUT_KEY){0});
        gBS->FreePool(fs_handles);
        return;
    }

    UINTN read_size = file_size;
    s = fp->Read(fp, &read_size, (void *)load_base);
    fp->Close(fp);
    gBS->FreePool(fs_handles);

    if (EFI_ERROR(s) || read_size != file_size) {
        fb_puts(10, gFBHeight - 30, u"ERROR: Read failed", COLOR_RED);
        wait_for_key(&(EFI_INPUT_KEY){0});
        return;
    }

    /* Validate ELF */
    UINT8 *elf_data = (UINT8 *)(UINTN)load_base;
    if (!elf_is_valid(elf_data, file_size)) {
        fb_puts(10, gFBHeight - 30, u"ERROR: Not a valid ELF", COLOR_RED);
        wait_for_key(&(EFI_INPUT_KEY){0});
        return;
    }

    /* Parse and load segments */
    if (elf_is_64bit(elf_data)) {
        Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_data;
        Elf64_Phdr *phdrs = (Elf64_Phdr *)(elf_data + ehdr->e_phoff);

        fb_clear(COLOR_BLACK);
        fb_puts(10, 10, u"Loading ELF64...", COLOR_CYAN);

        for (UINTN i = 0; i < ehdr->e_phnum; i++) {
            if (phdrs[i].p_type != PT_LOAD) continue;

            UINT64 seg_filesz = phdrs[i].p_filesz;
            UINT64 seg_memsz  = phdrs[i].p_memsz;
            UINT64 seg_paddr  = phdrs[i].p_paddr;

            if (seg_memsz == 0) continue;

            UINTN seg_pages = (seg_memsz + 0xFFF) / 0x1000;
            EFI_PHYSICAL_ADDRESS seg_addr = seg_paddr;
            s = gBS->AllocatePages(AllocateAddress, EfiLoaderData,
                                   seg_pages, &seg_addr);
            if (EFI_ERROR(s)) {
                /* Try allocating anywhere */
                seg_addr = 0x20000000;
                s = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData,
                                       seg_pages, &seg_addr);
                if (EFI_ERROR(s)) continue;
            }

            if (seg_filesz > 0) {
                gBS->CopyMem((void *)(UINTN)seg_addr,
                             elf_data + phdrs[i].p_offset, seg_filesz);
            }
            if (seg_memsz > seg_filesz) {
                gBS->SetMem((void *)(UINTN)(seg_addr + seg_filesz),
                            seg_memsz - seg_filesz, 0);
            }
        }

        /* Exit boot services */
        do_exit_boot_services();

        /* Jump to ELF64 entry */
        typedef void (*elf64_entry_t)(UINT64, UINT64, UINT64, UINT64);
        elf64_entry_t entry_fn = (elf64_entry_t)ehdr->e_entry;
        entry_fn(0, 0, 0, 0);

    } else {
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_data;
        Elf32_Phdr *phdrs = (Elf32_Phdr *)(elf_data + ehdr->e_phoff);

        fb_clear(COLOR_BLACK);
        fb_puts(10, 10, u"Loading ELF32...", COLOR_CYAN);

        for (UINTN i = 0; i < ehdr->e_phnum; i++) {
            if (phdrs[i].p_type != PT_LOAD) continue;

            UINT32 seg_filesz = phdrs[i].p_filesz;
            UINT32 seg_memsz  = phdrs[i].p_memsz;
            UINT32 seg_paddr  = phdrs[i].p_paddr;

            if (seg_memsz == 0) continue;

            UINTN seg_pages = (seg_memsz + 0xFFF) / 0x1000;
            EFI_PHYSICAL_ADDRESS seg_addr = seg_paddr;
            s = gBS->AllocatePages(AllocateAddress, EfiLoaderData,
                                   seg_pages, &seg_addr);
            if (EFI_ERROR(s)) {
                seg_addr = 0x20000000;
                s = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData,
                                       seg_pages, &seg_addr);
                if (EFI_ERROR(s)) continue;
            }

            if (seg_filesz > 0) {
                gBS->CopyMem((void *)(UINTN)seg_addr,
                             elf_data + phdrs[i].p_offset, seg_filesz);
            }
            if (seg_memsz > seg_filesz) {
                gBS->SetMem((void *)(UINTN)(seg_addr + seg_filesz),
                            seg_memsz - seg_filesz, 0);
            }
        }

        /* Exit boot services */
        do_exit_boot_services();

        /* Jump to ELF32 entry (AArch32 trampoline) */
        typedef void (*elf32_entry_t)(UINT32, UINT32, UINT32);
        UINT32 entry_addr = ehdr->e_entry;

        __asm__ volatile(
            /* Set up AArch32 SVC mode in SPSR */
            "mov x8, #0x1D3\n"
            "msr spsr_el1, x8\n"
            /* Set entry point in ELR_EL1 */
            "mov x9, %0\n"
            "msr elr_el1, x9\n"
            /* Set up r0, r1, r2 via x0-x2 before eret */
            "mov x0, #0\n"
            "mov x1, #0\n"
            "mov x2, #0\n"
            "eret\n"
            : : "r" ((UINT64)entry_addr) : "x8", "x9", "x0", "x1", "x2"
        );
    }

    /* Should never reach here */
    while (1) { __asm__ volatile("wfi"); }
}

/* ------------------------------------------------------------------ */
/* UI loop                                                             */
/* ------------------------------------------------------------------ */
static void ui_loop(void) {
    gCursor = 0;
    BOOLEAN needs_redraw = TRUE;

    while (1) {
        if (needs_redraw) {
            draw_ui();
            needs_redraw = FALSE;
        }

        EFI_INPUT_KEY key;
        if (!wait_for_key(&key)) continue;

        if (is_up_key(&key)) {
            if (gCursor > 0) {
                gCursor--;
                needs_redraw = TRUE;
            }
        } else if (is_down_key(&key)) {
            if (gCursor < (INTN)gFileCount - 1) {
                gCursor++;
                needs_redraw = TRUE;
            }
        } else if (is_select_key(&key)) {
            if (gFileCount > 0 && gCursor >= 0 && gCursor < (INTN)gFileCount) {
                /* Show "loading..." */
                fb_fill_rect(0, gFBHeight - 30, gFBWidth, 30, COLOR_BLACK);
                fb_puts(10, gFBHeight - 24, u"Booting...", COLOR_YELLOW);
                load_and_boot(&gFiles[gCursor]);
                /* If we return, something went wrong */
                needs_redraw = TRUE;
            }
        } else if (key.UnicodeChar == u'q' || key.UnicodeChar == u'Q') {
            return;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Entry point                                                         */
/* ------------------------------------------------------------------ */
EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    gST = SystemTable;
    gBS = SystemTable->BootServices;
    gImageHandle = ImageHandle;

    /* Disable watchdog timer */
    gBS->SetWatchdogTimer(0, 0, 0, NULL);

    /* Get ConIn for key input (we still use this for input) */
    gConIn = SystemTable->ConIn;

    /* We deliberately do NOT use ConOut for output.
     * All display goes through the GOP framebuffer. */

    /* Locate Graphics Output Protocol */
    gGOP = (EFI_GRAPHICS_OUTPUT_PROTOCOL *)locate_protocol(
        &GraphicsOutputProtocolGuidVar);
    if (!gGOP) {
        /* No framebuffer available - nothing we can do */
        if (gST->ConOut)
            gST->ConOut->OutputString(gST->ConOut,
                u"No GOP framebuffer found.\r\n");
        return EFI_UNSUPPORTED;
    }

    /* Use current GOP mode */
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = gGOP->Mode->Info;
    gFBWidth  = info->HorizontalResolution;
    gFBHeight = info->VerticalResolution;
    gFBStride = info->PixelsPerScanLine;
    gFBBase   = (void *)(UINTN)gGOP->Mode->FrameBufferBase;
    gFBSize   = gGOP->Mode->FrameBufferSize;

    /* Detect pixel format */
    if (info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor)
        gFBIsBGR = TRUE;
    else
        gFBIsBGR = FALSE;

    /* Clear screen via framebuffer */
    fb_clear(COLOR_BLACK);

    /* Enumerate storage devices */
    enumerate_storage();

    /* Scan for ELF files on all filesystems */
    enumerate_elf_files();

    /* Main UI loop - Vol Up/Down navigate, Power selects */
    ui_loop();

    return EFI_SUCCESS;
}
