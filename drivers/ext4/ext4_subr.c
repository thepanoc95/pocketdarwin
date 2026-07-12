/*
 * ext4_subr.c - superblock, inode, extent, and block I/O helpers
 */
#include "ext4.h"
#include <sys/buf.h>
#include <sys/ubc.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/errno.h>
#include <string.h>

enum vtype
ext4_ft_to_vtype(uint8_t ft)
{
	switch (ft) {
	case EXT4_FT_REG_FILE: return VREG;
	case EXT4_FT_DIR:      return VDIR;
	case EXT4_FT_CHRDEV:   return VCHR;
	case EXT4_FT_BLKDEV:   return VBLK;
	case EXT4_FT_FIFO:     return VFIFO;
	case EXT4_FT_SOCK:     return VSOCK;
	case EXT4_FT_SYMLINK:  return VLNK;
	default:              return VNON;
	}
}

enum vtype
ext4_mode_to_vtype(uint16_t mode)
{
	switch (mode & EXT4_S_IFMT) {
	case EXT4_S_IFREG:  return VREG;
	case EXT4_S_IFDIR:  return VDIR;
	case EXT4_S_IFCHR:  return VCHR;
	case EXT4_S_IFBLK:  return VBLK;
	case EXT4_S_IFIFO:  return VFIFO;
	case EXT4_S_IFSOCK: return VSOCK;
	case EXT4_S_IFLNK:  return VLNK;
	default:           return VNON;
	}
}

/*
 * Read one fs block (em_blocksize bytes) at physical block pblk from the
 * underlying device.  The device vnode's block size is set to the fs block
 * size at mount time, so fs block numbers map directly.
 */
int
ext4_blkread(struct ext4mount *emp, uint64_t pblk, buf_t *bpp)
{
	buf_t bp = NULL;
	int error;

	error = buf_meta_bread(emp->em_devvp, (daddr64_t)pblk,
	    (int)emp->em_blocksize, NOCRED, &bp);
	if (error) {
		if (bp)
			buf_brelse(bp);
		*bpp = NULL;
		return error;
	}
	*bpp = bp;
	return 0;
}

int
ext4_read_super(struct ext4mount *emp)
{
	struct ext4_super_block *sb = &emp->em_sb;
	buf_t bp = NULL;
	int error;

	/*
	 * The superblock lives at byte offset 1024.  Read it via a 2048-byte
	 * meta read from device block 0 using a temporary 512-byte view so we
	 * don't depend on the fs blocksize (which we don't know yet).  We read
	 * 4 sectors (2KB) at DEV_BSIZE granularity.
	 */
	error = buf_meta_bread(emp->em_devvp, (daddr64_t)0, 2048, NOCRED, &bp);
	if (error) {
		if (bp) buf_brelse(bp);
		E4LOG("superblock read failed: %d", error);
		return error;
	}

	memcpy(sb, (char *)buf_dataptr(bp) + EXT4_SUPERBLOCK_OFFSET, sizeof(*sb));
	buf_brelse(bp);

	if (le16(sb->s_magic) != EXT4_SUPER_MAGIC) {
		E4LOG("bad magic 0x%x", le16(sb->s_magic));
		return EINVAL;
	}

	emp->em_log_blocksize = le32(sb->s_log_block_size);
	emp->em_blocksize     = 1024u << emp->em_log_blocksize;
	emp->em_inodes_per_group = le32(sb->s_inodes_per_group);
	emp->em_blocks_per_group = le32(sb->s_blocks_per_group);
	emp->em_first_data_block = le32(sb->s_first_data_block);
	emp->em_feature_incompat = le32(sb->s_feature_incompat);

	if (le32(sb->s_rev_level) == 0) {
		emp->em_inode_size = EXT4_GOOD_OLD_INODE_SIZE;
		emp->em_desc_size  = 32;
	} else {
		emp->em_inode_size = le16(sb->s_inode_size);
		if (emp->em_inode_size == 0)
			emp->em_inode_size = EXT4_GOOD_OLD_INODE_SIZE;
		emp->em_desc_size = le16(sb->s_desc_size);
		if ((emp->em_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT) == 0 ||
		    emp->em_desc_size < 32)
			emp->em_desc_size = 32;
	}

	emp->em_blocks_count = le32(sb->s_blocks_count_lo);
	if (emp->em_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT)
		emp->em_blocks_count |= ((uint64_t)le32(sb->s_blocks_count_hi)) << 32;
	emp->em_inodes_count = le32(sb->s_inodes_count);

	if (emp->em_blocks_per_group == 0) {
		E4LOG("zero blocks_per_group");
		return EINVAL;
	}
	emp->em_groups_count = (uint32_t)
	    ((emp->em_blocks_count - emp->em_first_data_block +
	     emp->em_blocks_per_group - 1) / emp->em_blocks_per_group);

	E4LOG("mounted: bs=%u ipg=%u bpg=%u isize=%u groups=%u incompat=0x%x blocks=%llu",
	    emp->em_blocksize, emp->em_inodes_per_group, emp->em_blocks_per_group,
	    emp->em_inode_size, emp->em_groups_count, emp->em_feature_incompat,
	    emp->em_blocks_count);
	return 0;
}

int
ext4_write_super(struct ext4mount *emp)
{
	uint64_t pblk = emp->em_first_data_block == 0 ? 0 : emp->em_first_data_block;
	uint32_t off = emp->em_first_data_block == 0 ? EXT4_SUPERBLOCK_OFFSET : 0;
	buf_t bp = NULL;
	int error;

	error = ext4_blkread(emp, pblk, &bp);
	if (error)
		return error;
	memcpy((char *)buf_dataptr(bp) + off, &emp->em_sb, sizeof(emp->em_sb));
	return buf_bwrite(bp);
}

uint64_t
ext4_free_blocks_count(struct ext4mount *emp)
{
	uint64_t free_blocks = le32(emp->em_sb.s_free_blocks_count_lo);

	if (emp->em_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT)
		free_blocks |= ((uint64_t)le32(emp->em_sb.s_free_blocks_count_hi)) << 32;
	return free_blocks;
}

/* Read the group descriptor for group `grp` into `gd`. */
int
ext4_read_group_desc(struct ext4mount *emp, uint32_t grp,
    struct ext4_group_desc *gd)
{
	uint64_t gdt_block;
	uint64_t byteoff;
	uint64_t pblk;
	uint32_t off_in_block;
	buf_t bp = NULL;
	int error;

	/* GDT starts at first_data_block + 1 */
	gdt_block = emp->em_first_data_block + 1;
	byteoff   = (uint64_t)grp * emp->em_desc_size;
	pblk      = gdt_block + (byteoff / emp->em_blocksize);
	off_in_block = (uint32_t)(byteoff % emp->em_blocksize);

	error = ext4_blkread(emp, pblk, &bp);
	if (error)
		return error;

	memset(gd, 0, sizeof(*gd));
	memcpy(gd, (char *)buf_dataptr(bp) + off_in_block,
	    emp->em_desc_size < sizeof(*gd) ? emp->em_desc_size : sizeof(*gd));
	buf_brelse(bp);
	return 0;
}

int
ext4_write_group_desc(struct ext4mount *emp, uint32_t grp,
    const struct ext4_group_desc *gd)
{
	uint64_t gdt_block;
	uint64_t byteoff;
	uint64_t pblk;
	uint32_t off_in_block;
	buf_t bp = NULL;
	int error;

	if (grp >= emp->em_groups_count)
		return EINVAL;

	gdt_block = emp->em_first_data_block + 1;
	byteoff   = (uint64_t)grp * emp->em_desc_size;
	pblk      = gdt_block + (byteoff / emp->em_blocksize);
	off_in_block = (uint32_t)(byteoff % emp->em_blocksize);

	error = ext4_blkread(emp, pblk, &bp);
	if (error)
		return error;
	memcpy((char *)buf_dataptr(bp) + off_in_block, gd,
	    emp->em_desc_size < sizeof(*gd) ? emp->em_desc_size : sizeof(*gd));
	return buf_bwrite(bp);
}

static uint64_t
ext4_gd_block_bitmap(struct ext4mount *emp, const struct ext4_group_desc *gd)
{
	uint64_t blk = le32(gd->bg_block_bitmap_lo);
	if (emp->em_desc_size >= 64)
		blk |= ((uint64_t)le32(gd->bg_block_bitmap_hi)) << 32;
	return blk;
}

static uint64_t
ext4_gd_inode_bitmap(struct ext4mount *emp, const struct ext4_group_desc *gd)
{
	uint64_t blk = le32(gd->bg_inode_bitmap_lo);
	if (emp->em_desc_size >= 64)
		blk |= ((uint64_t)le32(gd->bg_inode_bitmap_hi)) << 32;
	return blk;
}

static bool
ext4_bitmap_test(const uint8_t *map, uint32_t bit)
{
	return (map[bit >> 3] & (uint8_t)(1u << (bit & 7))) != 0;
}

static void
ext4_bitmap_set(uint8_t *map, uint32_t bit)
{
	map[bit >> 3] |= (uint8_t)(1u << (bit & 7));
}

static void
ext4_bitmap_clear(uint8_t *map, uint32_t bit)
{
	map[bit >> 3] &= (uint8_t)~(1u << (bit & 7));
}

static void
ext4_sb_add_free_blocks(struct ext4mount *emp, int64_t delta)
{
	uint64_t free_blocks = le32(emp->em_sb.s_free_blocks_count_lo);
	if (emp->em_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT)
		free_blocks |= ((uint64_t)le32(emp->em_sb.s_free_blocks_count_hi)) << 32;
	free_blocks = (uint64_t)((int64_t)free_blocks + delta);
	emp->em_sb.s_free_blocks_count_lo = le32((uint32_t)free_blocks);
	if (emp->em_feature_incompat & EXT4_FEATURE_INCOMPAT_64BIT)
		emp->em_sb.s_free_blocks_count_hi = le32((uint32_t)(free_blocks >> 32));
}

static void
ext4_gd_add_free_blocks(struct ext4mount *emp, struct ext4_group_desc *gd,
    int32_t delta)
{
	uint32_t free_blocks = le16(gd->bg_free_blocks_count_lo);
	if (emp->em_desc_size >= 64)
		free_blocks |= ((uint32_t)le16(gd->bg_free_blocks_count_hi)) << 16;
	free_blocks = (uint32_t)((int32_t)free_blocks + delta);
	gd->bg_free_blocks_count_lo = le16((uint16_t)free_blocks);
	if (emp->em_desc_size >= 64)
		gd->bg_free_blocks_count_hi = le16((uint16_t)(free_blocks >> 16));
}

static void
ext4_gd_add_free_inodes(struct ext4mount *emp, struct ext4_group_desc *gd,
    int32_t delta)
{
	uint32_t free_inodes = le16(gd->bg_free_inodes_count_lo);
	if (emp->em_desc_size >= 64)
		free_inodes |= ((uint32_t)le16(gd->bg_free_inodes_count_hi)) << 16;
	free_inodes = (uint32_t)((int32_t)free_inodes + delta);
	gd->bg_free_inodes_count_lo = le16((uint16_t)free_inodes);
	if (emp->em_desc_size >= 64)
		gd->bg_free_inodes_count_hi = le16((uint16_t)(free_inodes >> 16));
}

static void
ext4_gd_add_used_dirs(struct ext4mount *emp, struct ext4_group_desc *gd,
    int32_t delta)
{
	uint32_t used_dirs = le16(gd->bg_used_dirs_count_lo);
	if (emp->em_desc_size >= 64)
		used_dirs |= ((uint32_t)le16(gd->bg_used_dirs_count_hi)) << 16;
	used_dirs = (uint32_t)((int32_t)used_dirs + delta);
	gd->bg_used_dirs_count_lo = le16((uint16_t)used_dirs);
	if (emp->em_desc_size >= 64)
		gd->bg_used_dirs_count_hi = le16((uint16_t)(used_dirs >> 16));
}

int
ext4_alloc_inode(struct ext4mount *emp, enum vtype type, ino_t *ino_out)
{
	uint32_t first_ino = le32(emp->em_sb.s_first_ino);
	uint32_t grp;

	if (first_ino == 0)
		first_ino = 11;

	for (grp = 0; grp < emp->em_groups_count; grp++) {
		struct ext4_group_desc gd;
		uint64_t bitmap_block;
		buf_t bp = NULL;
		uint8_t *map;
		uint32_t i, limit;
		int error;

		error = ext4_read_group_desc(emp, grp, &gd);
		if (error)
			return error;
		if (le16(gd.bg_free_inodes_count_lo) == 0)
			continue;

		bitmap_block = ext4_gd_inode_bitmap(emp, &gd);
		error = ext4_blkread(emp, bitmap_block, &bp);
		if (error)
			return error;
		map = (uint8_t *)buf_dataptr(bp);
		limit = emp->em_inodes_per_group;
		if ((uint64_t)(grp + 1) * emp->em_inodes_per_group > emp->em_inodes_count)
			limit = (uint32_t)(emp->em_inodes_count -
			    (uint64_t)grp * emp->em_inodes_per_group);

		for (i = 0; i < limit; i++) {
			ino_t ino = (ino_t)((uint64_t)grp * emp->em_inodes_per_group + i + 1);
			if (ino < first_ino || ext4_bitmap_test(map, i))
				continue;
			ext4_bitmap_set(map, i);
			error = buf_bwrite(bp);
			if (error)
				return error;
			ext4_gd_add_free_inodes(emp, &gd, -1);
			if (type == VDIR)
				ext4_gd_add_used_dirs(emp, &gd, 1);
			error = ext4_write_group_desc(emp, grp, &gd);
			if (error)
				return error;
			emp->em_sb.s_free_inodes_count =
			    le32(le32(emp->em_sb.s_free_inodes_count) - 1);
			error = ext4_write_super(emp);
			if (error)
				return error;
			*ino_out = ino;
			return 0;
		}
		buf_brelse(bp);
	}
	return ENOSPC;
}

int
ext4_free_inode(struct ext4mount *emp, ino_t ino, enum vtype type)
{
	struct ext4_group_desc gd;
	uint32_t grp, index;
	uint64_t bitmap_block;
	buf_t bp = NULL;
	uint8_t *map;
	int error;

	if (ino == 0 || ino > emp->em_inodes_count)
		return EINVAL;
	grp = (uint32_t)((ino - 1) / emp->em_inodes_per_group);
	index = (uint32_t)((ino - 1) % emp->em_inodes_per_group);
	error = ext4_read_group_desc(emp, grp, &gd);
	if (error)
		return error;
	bitmap_block = ext4_gd_inode_bitmap(emp, &gd);
	error = ext4_blkread(emp, bitmap_block, &bp);
	if (error)
		return error;
	map = (uint8_t *)buf_dataptr(bp);
	if (!ext4_bitmap_test(map, index)) {
		buf_brelse(bp);
		return 0;
	}
	ext4_bitmap_clear(map, index);
	error = buf_bwrite(bp);
	if (error)
		return error;
	ext4_gd_add_free_inodes(emp, &gd, 1);
	if (type == VDIR && le16(gd.bg_used_dirs_count_lo) > 0)
		ext4_gd_add_used_dirs(emp, &gd, -1);
	error = ext4_write_group_desc(emp, grp, &gd);
	if (error)
		return error;
	emp->em_sb.s_free_inodes_count =
	    le32(le32(emp->em_sb.s_free_inodes_count) + 1);
	return ext4_write_super(emp);
}

int
ext4_alloc_block(struct ext4mount *emp, uint64_t goal, uint64_t *pblk_out)
{
	uint32_t start_grp = 0;
	uint32_t pass;

	if (goal >= emp->em_first_data_block && goal < emp->em_blocks_count) {
		start_grp = (uint32_t)((goal - emp->em_first_data_block) /
		    emp->em_blocks_per_group);
		if (start_grp >= emp->em_groups_count)
			start_grp = 0;
	}

	for (pass = 0; pass < emp->em_groups_count; pass++) {
		uint32_t grp = (start_grp + pass) % emp->em_groups_count;
		uint64_t group_first = emp->em_first_data_block +
		    (uint64_t)grp * emp->em_blocks_per_group;
		uint32_t limit = emp->em_blocks_per_group;
		struct ext4_group_desc gd;
		uint64_t bitmap_block;
		buf_t bp = NULL;
		uint8_t *map;
		uint32_t i;
		int error;

		if (group_first >= emp->em_blocks_count)
			continue;
		if (group_first + limit > emp->em_blocks_count)
			limit = (uint32_t)(emp->em_blocks_count - group_first);

		error = ext4_read_group_desc(emp, grp, &gd);
		if (error)
			return error;
		if (le16(gd.bg_free_blocks_count_lo) == 0)
			continue;

		bitmap_block = ext4_gd_block_bitmap(emp, &gd);
		error = ext4_blkread(emp, bitmap_block, &bp);
		if (error)
			return error;
		map = (uint8_t *)buf_dataptr(bp);

		for (i = 0; i < limit; i++) {
			uint64_t pblk = group_first + i;
			buf_t zbp = NULL;

			if (pblk < emp->em_first_data_block || ext4_bitmap_test(map, i))
				continue;
			ext4_bitmap_set(map, i);
			error = buf_bwrite(bp);
			if (error)
				return error;
			ext4_gd_add_free_blocks(emp, &gd, -1);
			error = ext4_write_group_desc(emp, grp, &gd);
			if (error)
				return error;
			ext4_sb_add_free_blocks(emp, -1);
			error = ext4_write_super(emp);
			if (error)
				return error;
			error = ext4_blkread(emp, pblk, &zbp);
			if (error)
				return error;
			memset((void *)buf_dataptr(zbp), 0, emp->em_blocksize);
			error = buf_bwrite(zbp);
			if (error)
				return error;
			*pblk_out = pblk;
			return 0;
		}
		buf_brelse(bp);
	}
	return ENOSPC;
}

int
ext4_free_block(struct ext4mount *emp, uint64_t pblk)
{
	struct ext4_group_desc gd;
	uint32_t grp, index;
	uint64_t group_first, bitmap_block;
	buf_t bp = NULL;
	uint8_t *map;
	int error;

	if (pblk < emp->em_first_data_block || pblk >= emp->em_blocks_count)
		return EINVAL;
	grp = (uint32_t)((pblk - emp->em_first_data_block) / emp->em_blocks_per_group);
	if (grp >= emp->em_groups_count)
		return EINVAL;
	group_first = emp->em_first_data_block + (uint64_t)grp * emp->em_blocks_per_group;
	index = (uint32_t)(pblk - group_first);

	error = ext4_read_group_desc(emp, grp, &gd);
	if (error)
		return error;
	bitmap_block = ext4_gd_block_bitmap(emp, &gd);
	error = ext4_blkread(emp, bitmap_block, &bp);
	if (error)
		return error;
	map = (uint8_t *)buf_dataptr(bp);
	if (!ext4_bitmap_test(map, index)) {
		buf_brelse(bp);
		return 0;
	}
	ext4_bitmap_clear(map, index);
	error = buf_bwrite(bp);
	if (error)
		return error;
	ext4_gd_add_free_blocks(emp, &gd, 1);
	error = ext4_write_group_desc(emp, grp, &gd);
	if (error)
		return error;
	ext4_sb_add_free_blocks(emp, 1);
	return ext4_write_super(emp);
}

int
ext4_inode_append_extent(struct ext4mount *emp, struct ext4_inode *inode,
    uint32_t lblk, uint64_t pblk)
{
	struct ext4_extent_header *eh =
	    (struct ext4_extent_header *)inode->i_block;
	struct ext4_extent *ex = (struct ext4_extent *)(eh + 1);
	uint16_t entries, max;

	if ((le32(inode->i_flags) & EXT4_EXTENTS_FL) == 0 ||
	    le16(eh->eh_magic) != EXT4_EXT_MAGIC) {
		memset(inode->i_block, 0, sizeof(inode->i_block));
		inode->i_flags = le32(le32(inode->i_flags) | EXT4_EXTENTS_FL);
		eh->eh_magic = le16(EXT4_EXT_MAGIC);
		eh->eh_entries = 0;
		eh->eh_max = le16((uint16_t)((sizeof(inode->i_block) -
		    sizeof(*eh)) / sizeof(*ex)));
		eh->eh_depth = 0;
		eh->eh_generation = 0;
	}
	if (le16(eh->eh_depth) != 0)
		return EFBIG;

	entries = le16(eh->eh_entries);
	max = le16(eh->eh_max);
	if (max == 0 || max > (sizeof(inode->i_block) - sizeof(*eh)) / sizeof(*ex))
		max = (uint16_t)((sizeof(inode->i_block) - sizeof(*eh)) / sizeof(*ex));

	if (entries > 0) {
		struct ext4_extent *last = &ex[entries - 1];
		uint32_t first = le32(last->ee_block);
		uint16_t len = le16(last->ee_len);
		uint64_t start = le32(last->ee_start_lo) |
		    ((uint64_t)le16(last->ee_start_hi) << 32);

		if (len < 32768 && lblk == first + len && pblk == start + len) {
			last->ee_len = le16((uint16_t)(len + 1));
			return 0;
		}
		if (lblk < first + len)
			return EINVAL;
	}

	if (entries >= max)
		return EFBIG;
	ex[entries].ee_block = le32(lblk);
	ex[entries].ee_len = le16(1);
	ex[entries].ee_start_hi = le16((uint16_t)(pblk >> 32));
	ex[entries].ee_start_lo = le32((uint32_t)pblk);
	eh->eh_entries = le16((uint16_t)(entries + 1));
	(void)emp;
	return 0;
}

int
ext4_inode_free_extents(struct ext4mount *emp, struct ext4_inode *inode)
{
	struct ext4_extent_header *eh =
	    (struct ext4_extent_header *)inode->i_block;
	struct ext4_extent *ex = (struct ext4_extent *)(eh + 1);
	uint16_t entries, depth, i;
	int error = 0;

	if ((le32(inode->i_flags) & EXT4_EXTENTS_FL) == 0)
		return 0;
	if (le16(eh->eh_magic) != EXT4_EXT_MAGIC)
		return EIO;
	depth = le16(eh->eh_depth);
	if (depth != 0)
		return EFBIG;
	entries = le16(eh->eh_entries);
	for (i = 0; i < entries; i++) {
		uint16_t j, len = le16(ex[i].ee_len);
		uint64_t start = le32(ex[i].ee_start_lo) |
		    ((uint64_t)le16(ex[i].ee_start_hi) << 32);
		if (len > 32768)
			len -= 32768;
		for (j = 0; j < len; j++) {
			error = ext4_free_block(emp, start + j);
			if (error)
				return error;
		}
	}
	memset(inode->i_block, 0, sizeof(inode->i_block));
	inode->i_blocks_lo = 0;
	inode->i_size_lo = 0;
	inode->i_size_high = 0;
	return 0;
}

int
ext4_inode_truncate_extents(struct ext4mount *emp, struct ext4_inode *inode,
    uint64_t keep_blocks)
{
	struct ext4_extent_header *eh =
	    (struct ext4_extent_header *)inode->i_block;
	struct ext4_extent *ex = (struct ext4_extent *)(eh + 1);
	uint16_t entries, depth;
	int error = 0;

	if ((le32(inode->i_flags) & EXT4_EXTENTS_FL) == 0)
		return keep_blocks == 0 ? 0 : EFBIG;
	if (le16(eh->eh_magic) != EXT4_EXT_MAGIC)
		return EIO;
	depth = le16(eh->eh_depth);
	if (depth != 0)
		return EFBIG;

	entries = le16(eh->eh_entries);
	while (entries > 0) {
		struct ext4_extent *last = &ex[entries - 1];
		uint32_t first = le32(last->ee_block);
		uint16_t raw_len = le16(last->ee_len);
		uint16_t len = raw_len;
		uint64_t start = le32(last->ee_start_lo) |
		    ((uint64_t)le16(last->ee_start_hi) << 32);
		uint64_t end;

		if (len > 32768)
			len -= 32768;
		end = (uint64_t)first + len;
		if (end <= keep_blocks)
			break;

		if ((uint64_t)first >= keep_blocks) {
			uint16_t j;
			for (j = 0; j < len; j++) {
				error = ext4_free_block(emp, start + j);
				if (error)
					return error;
			}
			entries--;
			eh->eh_entries = le16(entries);
			continue;
		}

		{
			uint16_t new_len = (uint16_t)(keep_blocks - first);
			uint16_t j;
			for (j = new_len; j < len; j++) {
				error = ext4_free_block(emp, start + j);
				if (error)
					return error;
			}
			last->ee_len = le16(new_len);
		}
		break;
	}

	if (keep_blocks == 0) {
		uint16_t max = le16(eh->eh_max);
		memset(inode->i_block, 0, sizeof(inode->i_block));
		eh = (struct ext4_extent_header *)inode->i_block;
		eh->eh_magic = le16(EXT4_EXT_MAGIC);
		eh->eh_entries = 0;
		eh->eh_max = le16(max ? max : (uint16_t)((sizeof(inode->i_block) -
		    sizeof(*eh)) / sizeof(struct ext4_extent)));
		eh->eh_depth = 0;
		eh->eh_generation = 0;
	}
	inode->i_blocks_lo = le32((uint32_t)(keep_blocks * (emp->em_blocksize / 512)));
	return 0;
}

int
ext4_read_inode(struct ext4mount *emp, ino_t ino, struct ext4_inode *out)
{
	struct ext4_group_desc gd;
	uint32_t grp, index;
	uint64_t itable_block;
	uint64_t byteoff, pblk;
	uint32_t off_in_block;
	buf_t bp = NULL;
	int error;

	if (ino == 0 || ino > emp->em_inodes_count) {
		E4LOG("inode %llu out of range", (uint64_t)ino);
		return EINVAL;
	}

	grp   = (uint32_t)((ino - 1) / emp->em_inodes_per_group);
	index = (uint32_t)((ino - 1) % emp->em_inodes_per_group);

	error = ext4_read_group_desc(emp, grp, &gd);
	if (error)
		return error;

	itable_block = le32(gd.bg_inode_table_lo);
	if (emp->em_desc_size >= 64)
		itable_block |= ((uint64_t)le32(gd.bg_inode_table_hi)) << 32;

	byteoff = (uint64_t)index * emp->em_inode_size;
	pblk    = itable_block + (byteoff / emp->em_blocksize);
	off_in_block = (uint32_t)(byteoff % emp->em_blocksize);

	error = ext4_blkread(emp, pblk, &bp);
	if (error)
		return error;

	memset(out, 0, sizeof(*out));
	memcpy(out, (char *)buf_dataptr(bp) + off_in_block,
	    sizeof(*out) < emp->em_inode_size ? sizeof(*out) : emp->em_inode_size);
	buf_brelse(bp);
	return 0;
}

/*
 * Write a (possibly modified) on-disk inode back to the inode table.
 * Read-modify-write the containing block so bytes beyond our struct
 * (extra_isize region, etc.) are preserved. Synchronous.
 */
int
ext4_write_inode(struct ext4mount *emp, ino_t ino, const struct ext4_inode *in)
{
	struct ext4_group_desc gd;
	uint32_t grp, index;
	uint64_t itable_block, byteoff, pblk;
	uint32_t off_in_block;
	buf_t bp = NULL;
	int error;

	if (ino == 0 || ino > emp->em_inodes_count)
		return EINVAL;

	grp   = (uint32_t)((ino - 1) / emp->em_inodes_per_group);
	index = (uint32_t)((ino - 1) % emp->em_inodes_per_group);

	error = ext4_read_group_desc(emp, grp, &gd);
	if (error)
		return error;

	itable_block = le32(gd.bg_inode_table_lo);
	if (emp->em_desc_size >= 64)
		itable_block |= ((uint64_t)le32(gd.bg_inode_table_hi)) << 32;

	byteoff = (uint64_t)index * emp->em_inode_size;
	pblk    = itable_block + (byteoff / emp->em_blocksize);
	off_in_block = (uint32_t)(byteoff % emp->em_blocksize);

	error = ext4_blkread(emp, pblk, &bp);
	if (error)
		return error;

	memcpy((char *)buf_dataptr(bp) + off_in_block, in,
	    sizeof(*in) < emp->em_inode_size ? sizeof(*in) : emp->em_inode_size);
	return buf_bwrite(bp);   /* synchronous; releases bp */
}

/* Walk an extent tree node to resolve a logical block. buf holds the node. */
static int
ext4_extent_lookup(struct ext4mount *emp, char *node, uint32_t lblk,
    uint64_t *pblk_out)
{
	struct ext4_extent_header *eh = (struct ext4_extent_header *)node;
	uint16_t entries, depth, i;

	if (le16(eh->eh_magic) != EXT4_EXT_MAGIC) {
		E4LOG("bad extent magic 0x%x", le16(eh->eh_magic));
		return EIO;
	}
	entries = le16(eh->eh_entries);
	depth   = le16(eh->eh_depth);

	if (depth == 0) {
		struct ext4_extent *ex = (struct ext4_extent *)(eh + 1);
		for (i = 0; i < entries; i++) {
			uint32_t first = le32(ex[i].ee_block);
			uint16_t len   = le16(ex[i].ee_len);
			if (len > 32768) len -= 32768;  /* uninitialized extent */
			if (lblk >= first && lblk < first + len) {
				uint64_t start = le32(ex[i].ee_start_lo) |
				    ((uint64_t)le16(ex[i].ee_start_hi) << 32);
				*pblk_out = start + (lblk - first);
				return 0;
			}
		}
		*pblk_out = 0;   /* hole */
		return 0;
	} else {
		struct ext4_extent_idx *ix = (struct ext4_extent_idx *)(eh + 1);
		uint64_t child = 0;
		int found = 0;
		for (i = 0; i < entries; i++) {
			uint32_t first = le32(ix[i].ei_block);
			if (lblk >= first) {
				child = le32(ix[i].ei_leaf_lo) |
				    ((uint64_t)le16(ix[i].ei_leaf_hi) << 32);
				found = 1;
			} else {
				break;
			}
		}
		if (!found) {
			*pblk_out = 0;
			return 0;
		}
		buf_t bp = NULL;
		int error = ext4_blkread(emp, child, &bp);
		if (error)
			return error;
		/* copy node out so we can release the buffer before recursing */
		char *copy = (char *)_MALLOC(emp->em_blocksize, M_TEMP, M_WAITOK);
		if (!copy) { buf_brelse(bp); return ENOMEM; }
		memcpy(copy, (char *)buf_dataptr(bp), emp->em_blocksize);
		buf_brelse(bp);
		error = ext4_extent_lookup(emp, copy, lblk, pblk_out);
		_FREE(copy, M_TEMP);
		return error;
	}
}

/*
 * Map a logical block of an inode to a physical block.  Returns 0 and sets
 * *pblk_out (0 => hole).  Handles both extent-mapped and classic
 * (direct/indirect) inodes.
 */
int
ext4_bmap(struct ext4mount *emp, struct ext4_inode *inode, uint32_t lblk,
    uint64_t *pblk_out)
{
	uint32_t flags = le32(inode->i_flags);

	if (flags & EXT4_EXTENTS_FL) {
		/* extent root lives inline in i_block[] (60 bytes) */
		return ext4_extent_lookup(emp, (char *)inode->i_block, lblk,
		    pblk_out);
	}

	/* classic block map: 12 direct, then single/double/triple indirect */
	uint32_t ptrs = emp->em_blocksize / 4;   /* pointers per indirect block */
	if (lblk < 12) {
		*pblk_out = le32(inode->i_block[lblk]);
		return 0;
	}
	lblk -= 12;
	if (lblk < ptrs) {
		return ext4_indirect_lookup(emp, le32(inode->i_block[12]), lblk,
		    1, pblk_out);
	}
	lblk -= ptrs;
	if (lblk < ptrs * ptrs) {
		return ext4_indirect_lookup(emp, le32(inode->i_block[13]), lblk,
		    2, pblk_out);
	}
	lblk -= ptrs * ptrs;
	return ext4_indirect_lookup(emp, le32(inode->i_block[14]), lblk,
	    3, pblk_out);
}

/* Resolve `lblk` within an N-level indirect block tree rooted at `blk`. */
int
ext4_indirect_lookup(struct ext4mount *emp, uint32_t blk, uint32_t lblk,
    int level, uint64_t *pblk_out)
{
	uint32_t ptrs = emp->em_blocksize / 4;
	uint32_t idx, sub;
	buf_t bp = NULL;
	int error;
	uint32_t next;

	if (blk == 0) { *pblk_out = 0; return 0; }

	if (level == 1) {
		idx = lblk;
	} else if (level == 2) {
		idx = lblk / ptrs;
		sub = lblk % ptrs;
	} else {
		idx = lblk / (ptrs * ptrs);
		sub = lblk % (ptrs * ptrs);
	}

	error = ext4_blkread(emp, blk, &bp);
	if (error)
		return error;
	next = le32(((uint32_t *)buf_dataptr(bp))[idx]);
	buf_brelse(bp);

	if (level == 1) {
		*pblk_out = next;
		return 0;
	}
	return ext4_indirect_lookup(emp, next, sub, level - 1, pblk_out);
}