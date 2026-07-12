/*
 * ext4_vfsops.c - VFS operations + kext entry for read-only ext4
 */
#include "ext4.h"
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/disk.h>
#include <sys/fcntl.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/ubc.h>
#include <mach/kmod.h>
#include <libkern/libkern.h>
#include <string.h>

static vfstable_t ext4_vfsconf;

/* forward decls of the vfsops */
static int ext4_mount(struct mount *mp, vnode_t devvp, user_addr_t data,
    vfs_context_t ctx);
static int ext4_start(struct mount *mp, int flags, vfs_context_t ctx);
static int ext4_unmount(struct mount *mp, int mntflags, vfs_context_t ctx);
static int ext4_root(struct mount *mp, struct vnode **vpp, vfs_context_t ctx);
static int ext4_vfs_getattr(struct mount *mp, struct vfs_attr *fsap,
    vfs_context_t ctx);
static int ext4_sync(struct mount *mp, int waitfor, vfs_context_t ctx);
static int ext4_vfs_vget(struct mount *mp, ino64_t ino, struct vnode **vpp,
    vfs_context_t ctx);
static int ext4_vfs_init(struct vfsconf *vfsp);

struct vfsops ext4_vfsops = {
	.vfs_mount   = ext4_mount,
	.vfs_start   = ext4_start,
	.vfs_unmount = ext4_unmount,
	.vfs_root    = ext4_root,
	.vfs_getattr = ext4_vfs_getattr,
	.vfs_sync    = ext4_sync,
	.vfs_vget    = ext4_vfs_vget,
	.vfs_init    = ext4_vfs_init,
};

static int
ext4_vfs_init(__unused struct vfsconf *vfsp)
{
	return 0;
}

static int
ext4_mount(struct mount *mp, vnode_t devvp, __unused user_addr_t data,
    vfs_context_t ctx)
{
	struct ext4mount *emp;
	struct vfsstatfs *sfs;
	uint32_t bsize;
	int error;

	if (vfs_isupdate(mp)) {
		/* read-only fs: nothing to update */
		return 0;
	}

	emp = (struct ext4mount *)_MALLOC(sizeof(*emp), M_TEMP, M_WAITOK | M_ZERO);
	if (emp == NULL)
		return ENOMEM;

	emp->em_mp    = mp;
	emp->em_devvp = devvp;
	emp->em_dev   = vnode_specrdev(devvp);

	error = ext4_read_super(emp);
	if (error)
		goto fail;

	/*
	 * Point the device vnode's block size at the fs block size so that
	 * buf_meta_bread(devvp, fs_block, em_blocksize) addresses whole fs
	 * blocks directly.
	 */
	bsize = emp->em_blocksize;
	(void)VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&bsize, FWRITE, ctx);

	vfs_setfsprivate(mp, emp);
	/* The generic root mount object is born MNT_RDONLY|MNT_ROOTFS before
	 * ext4_mount() runs, so explicitly clear MNT_RDONLY here or VFS rejects
	 * open-for-write/create before our vnode operations can run. */
	vfs_setflags(mp, MNT_LOCAL);
	vfs_clearflags(mp, MNT_RDONLY);

	/* fill in statfs */
	sfs = vfs_statfs(mp);
	sfs->f_bsize  = emp->em_blocksize;
	sfs->f_iosize = emp->em_blocksize;
	sfs->f_blocks = emp->em_blocks_count;
	sfs->f_bfree  = ext4_free_blocks_count(emp);
	sfs->f_bavail = ext4_free_blocks_count(emp);
	sfs->f_files  = emp->em_inodes_count;
	sfs->f_ffree  = le32(emp->em_sb.s_free_inodes_count);
	sfs->f_fsid.val[0] = (int32_t)emp->em_dev;
	sfs->f_fsid.val[1] = (int32_t)vfs_typenum(mp);
	strlcpy(sfs->f_fstypename, "ext4", sizeof(sfs->f_fstypename));

	E4LOG("mount ok, dev 0x%x", emp->em_dev);
	return 0;

fail:
	_FREE(emp, M_TEMP);
	return error;
}

static int
ext4_start(__unused struct mount *mp, __unused int flags,
    __unused vfs_context_t ctx)
{
	return 0;
}

static int
ext4_unmount(struct mount *mp, int mntflags, __unused vfs_context_t ctx)
{
	struct ext4mount *emp = VFSTOEXT4(mp);
	int flags = 0;
	int error;

	if (mntflags & MNT_FORCE)
		flags |= FORCECLOSE;

	error = vflush(mp, NULLVP, flags);
	if (error)
		return error;

	if (emp) {
		vfs_setfsprivate(mp, NULL);
		_FREE(emp, M_TEMP);
	}
	return 0;
}

static int
ext4_root(struct mount *mp, struct vnode **vpp, __unused vfs_context_t ctx)
{
	struct ext4mount *emp = VFSTOEXT4(mp);

	return ext4_vget(emp, EXT4_ROOT_INO, NULLVP, vpp, NULL);
}

static int
ext4_vfs_vget(struct mount *mp, ino64_t ino, struct vnode **vpp,
    __unused vfs_context_t ctx)
{
	struct ext4mount *emp = VFSTOEXT4(mp);

	return ext4_vget(emp, (ino_t)ino, NULLVP, vpp, NULL);
}

static int
ext4_vfs_getattr(struct mount *mp, struct vfs_attr *fsap,
    __unused vfs_context_t ctx)
{
	struct ext4mount *emp = VFSTOEXT4(mp);

	VFSATTR_RETURN(fsap, f_objcount, emp->em_inodes_count);
	VFSATTR_RETURN(fsap, f_maxobjcount, emp->em_inodes_count);
	VFSATTR_RETURN(fsap, f_bsize, emp->em_blocksize);
	VFSATTR_RETURN(fsap, f_iosize, emp->em_blocksize);
	VFSATTR_RETURN(fsap, f_blocks, emp->em_blocks_count);
	VFSATTR_RETURN(fsap, f_bfree, ext4_free_blocks_count(emp));
	VFSATTR_RETURN(fsap, f_bavail, ext4_free_blocks_count(emp));
	VFSATTR_RETURN(fsap, f_bused, emp->em_blocks_count - ext4_free_blocks_count(emp));
	VFSATTR_RETURN(fsap, f_files, emp->em_inodes_count);
	VFSATTR_RETURN(fsap, f_ffree, le32(emp->em_sb.s_free_inodes_count));
	if (VFSATTR_IS_ACTIVE(fsap, f_capabilities)) {
		vol_capabilities_attr_t *cap = &fsap->f_capabilities;
		cap->capabilities[VOL_CAPABILITIES_FORMAT] =
		    VOL_CAP_FMT_SYMBOLICLINKS | VOL_CAP_FMT_HARDLINKS |
		    VOL_CAP_FMT_CASE_SENSITIVE;
		cap->valid[VOL_CAPABILITIES_FORMAT] =
		    VOL_CAP_FMT_SYMBOLICLINKS | VOL_CAP_FMT_HARDLINKS |
		    VOL_CAP_FMT_CASE_SENSITIVE | VOL_CAP_FMT_CASE_PRESERVING;
		cap->capabilities[VOL_CAPABILITIES_INTERFACES] = 0;
		cap->valid[VOL_CAPABILITIES_INTERFACES] = 0;
		cap->capabilities[VOL_CAPABILITIES_RESERVED1] = 0;
		cap->valid[VOL_CAPABILITIES_RESERVED1] = 0;
		cap->capabilities[VOL_CAPABILITIES_RESERVED2] = 0;
		cap->valid[VOL_CAPABILITIES_RESERVED2] = 0;
		VFSATTR_SET_SUPPORTED(fsap, f_capabilities);
	}
	return 0;
}

static int
ext4_sync(__unused struct mount *mp, __unused int waitfor,
    __unused vfs_context_t ctx)
{
	return 0;   /* read-only */
}

/* ---- filesystem (de)registration ----
 *
 * Called from the ext4 IOService's start()/stop() (ext4_iokit.cpp) rather
 * than a bare kmod MAIN_FUNCTION: in the fileset kernel collection a pure-C
 * kext with only a kmod start routine is never actually started (nothing
 * triggers it), so the filesystem was never registered with vfs_fsadd and
 * every root mount attempt failed. hfs uses the same IOResources/IOBSD
 * personality trick to get its registration invoked; ext4 now mirrors it. */
int
ext4_vfs_register(void)
{
	struct vfs_fsentry vfe;
	struct vnodeopv_desc *opv[1];
	int error;

	if (ext4_vfsconf != NULL)
		return 0;   /* already registered */

	memset(&vfe, 0, sizeof(vfe));
	opv[0] = &ext4_vnodeop_opv_desc;

	vfe.vfe_vfsops   = &ext4_vfsops;
	vfe.vfe_vopcnt   = 1;
	vfe.vfe_opvdescs = opv;
	strlcpy(vfe.vfe_fsname, "ext4", sizeof(vfe.vfe_fsname));
	/* VFS_TBLCANMOUNTROOT: without it vfs_mountroot() skips ext4 entirely
	 * (it only tries filesystems with a vfc_mountroot routine or this flag),
	 * so the root device never gets handed to ext4_mount and every mount
	 * attempt "fails" without ever calling us. With it, the generic
	 * VFS_MOUNT(mp, rootvp, 0, ctx) path invokes ext4_mount for the root. */
	vfe.vfe_flags    = VFS_TBLTHREADSAFE | VFS_TBLFSNODELOCK |
	                   VFS_TBL64BITREADY | VFS_TBLNOTYPENUM |
	                   VFS_TBLLOCALVOL | VFS_TBLGENERICMNTARGS |
	                   VFS_TBLCANMOUNTROOT;

	error = vfs_fsadd(&vfe, &ext4_vfsconf);
	if (error) {
		E4LOG("vfs_fsadd failed: %d", error);
		return error;
	}
	E4LOG("registered ext4 filesystem");
	return 0;
}

int
ext4_vfs_unregister(void)
{
	if (ext4_vfsconf) {
		if (vfs_fsremove(ext4_vfsconf) != 0)
			return -1;
		ext4_vfsconf = NULL;
	}
	return 0;
}