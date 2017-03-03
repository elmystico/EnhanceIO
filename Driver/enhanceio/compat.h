#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
#define COMPAT_HAVE_ATOMIC64_DEC_IF_POSITIVE
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
#define COMPAT_HAVE_BLKDEV_GET_BY_PATH
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
#define COMPAT_MAKE_REQUEST_FN_RET_VOID
#define COMPAT_MAKE_REQUEST_FN_SUBMITS_IO
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
#define COMPAT_HAVE_WAIT_FOR_COMPLETION_IO
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#define COMPAT_HAVE_STRUCT_BVEC_ITER
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0))
#define COMPAT_HAVE_SMB_MB__AFTER_ATOMIC
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))
#define COMPAT_HAVE_WAIT_ON_BIT_LOCK_ACTION
#define COMPAT_WAIT_FUNCTION_HAS_PARAM
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,3,0))
#define COMPAT_HAVE_BIO_BI_ERROR
#define COMPAT_NO_BIO_GET_NR_VECS
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
#define COMPAT_WAIT_FUNCTION_HAS_2_PARAM
#define COMPAT_MAKE_REQUEST_FN_RET_BLK_QC_T
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0))
#define COMPAT_NO_GENDISK_DRIVERFS_DEV
#define COMPAT_HAVE_BIO_OPF
#endif

/*Include features backported to RedHat kernels*/
#ifdef RHEL_RELEASE_CODE
#include "compat-redhat.h"
#endif

#ifdef COMPAT_NO_GENDISK_DRIVERFS_DEV
#define EIO_DRIVERFS_DEV(GENDISK) disk_to_dev((GENDISK))->parent
#else
#define EIO_DRIVERFS_DEV(GENDISK) ((GENDISK)->driverfs_dev)
#endif

#ifndef COMPAT_HAVE_BIO_OPF
#define bi_opf bi_rw
#define submit_bio(__bio) submit_bio((__bio)->bi_rw, __bio)
#endif

#ifdef COMPAT_WAIT_FUNCTION_HAS_2_PARAM
#elif defined COMPAT_WAIT_FUNCTION_HAS_PARAM
#define eio_wait_schedule(__wait_bit_key, __mode) eio_wait_schedule(__wait_bit_key)
#else
#define eio_wait_schedule(__wait_bit_key, __mode) eio_wait_schedule(void * unused)
#endif

#ifdef COMPAT_NO_BIO_GET_NR_VECS
#define EIO_BIO_GET_NR_VECS(BDEV) BIO_MAX_PAGES
#else
#define EIO_BIO_GET_NR_VECS(BDEV) bio_get_nr_vecs((BDEV))
#endif

#ifndef COMPAT_HAVE_WAIT_ON_BIT_LOCK_ACTION
#define wait_on_bit_lock_action wait_on_bit_lock
#endif

#ifndef COMPAT_HAVE_SMB_MB__AFTER_ATOMIC
#define smp_mb__after_atomic smp_mb__after_clear_bit
#endif

#ifdef COMPAT_HAVE_STRUCT_BVEC_ITER
#define EIO_BIO_BI_SECTOR(BIO) ((BIO)->bi_iter.bi_sector)
#define EIO_BIO_BI_SIZE(BIO) ((BIO)->bi_iter.bi_size)
#define EIO_BIO_BI_IDX(BIO) ((BIO)->bi_iter.bi_idx)
#else
#define EIO_BIO_BI_SECTOR(BIO) ((BIO)->bi_sector)
#define EIO_BIO_BI_SIZE(BIO) ((BIO)->bi_size)
#define EIO_BIO_BI_IDX(BIO) ((BIO)->bi_idx)
#endif

#ifdef INIT_COMPLETION /*kernels before 3.13.0*/
#define reinit_completion(__arg) INIT_COMPLETION(*(__arg))
#endif

#ifndef COMPAT_HAVE_WAIT_FOR_COMPLETION_IO
#define wait_for_completion_io(__arg) wait_for_completion(__arg)
#endif

#ifndef COMPAT_HAVE_ATOMIC64_DEC_IF_POSITIVE
static inline long atomic64_dec_if_positive(atomic64_t *v)
{
        long c, old, dec;
        c = atomic64_read(v);
        for (;;) {
                dec = c - 1;
                if (unlikely(dec < 0))
                         break;
                 old = atomic64_cmpxchg((v), c, dec);
                if (likely(old == c))
                        break;
                c = old;
        }
        return dec;
}
#endif

#ifndef COMPAT_HAVE_BLKDEV_GET_BY_PATH
static inline struct block_device *blkdev_get_by_path(const char *path, fmode_t mode,
                                        void *holder)
{
        struct block_device *bdev;
        int err;

        bdev = lookup_bdev(path);
        if (IS_ERR(bdev))
                return bdev;
        err = blkdev_get(bdev, mode);
        if (err)
                return ERR_PTR(err);
       if ((mode & FMODE_WRITE) && bdev_read_only(bdev)) {
                blkdev_put(bdev, mode);
                return ERR_PTR(-EACCES);
        }
        return bdev;
}
#endif

#ifdef COMPAT_HAVE_BIO_BI_ERROR
#define EIO_BIO_ENDIO(B,E) do { (B)->bi_error = E; bio_endio(B); } while (0)
#define eio_endio(B,E) eio_endio(B)
#define eio_split_endio(B,E) eio_split_endio(B)
#define eio_bio_end_empty_barrier(B,E) eio_bio_end_empty_barrier(B)
#define EIO_ENDIO_FN_START int error = bio->bi_error
#else
#define EIO_BIO_ENDIO(B,E) do { bio_endio(B,E); } while (0)
#define EIO_ENDIO_FN_START do {} while (0)
#endif

#ifdef COMPAT_MAKE_REQUEST_FN_RET_BLK_QC_T
#define MAKE_REQUEST_FN_TYPE blk_qc_t
#define MAKE_REQUEST_FN_RETURN_0 return BLK_QC_T_NONE
#elif defined COMPAT_MAKE_REQUEST_FN_RET_VOID
#define MAKE_REQUEST_FN_TYPE void
#define MAKE_REQUEST_FN_RETURN_0 return
#else /*return type is int for older kernels*/
#define MAKE_REQUEST_FN_TYPE int
#define MAKE_REQUEST_FN_RETURN_0 return 0
#endif
