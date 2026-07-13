/* Linker script for UEFI PE/COFF EFI application (AArch64)
 *
 * Used to produce a shared ELF that is then converted to PE/COFF
 * via objcopy --subsystem=10 (EFI Application).
 */
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(EfiMain)

SECTIONS
{
    . = 0x0;

    .text : {
        *(.text)
        *(.text.*)
    }

    . = ALIGN(4096);

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.eh_frame*)
        *(.gnu.hash*)
        *(.debug*)
        *(.ARM.*)
        *(.got*)
        *(.plt*)
    }
}
