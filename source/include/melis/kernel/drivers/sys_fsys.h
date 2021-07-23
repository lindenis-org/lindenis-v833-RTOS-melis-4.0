#ifndef __SYS_FSYS_H__
#define __SYS_FSYS_H__
#include <typedef.h>
#include <stdbool.h>
#include "sys_device.h"

//melis legacy fseek semantic. else posix semantic
#define FSEEK_LEGACY

#define SEEK_SET                        0       //from begin of the file
#define SEEK_CUR                        1       //current position
#define SEEK_END                        2       //from end of the file
#define FS_ATTR_READONLY                0x00000001 /* readonly fs */

#define FSYS_VERSION                    0x01000000UL   /* 1.00.0000 */

/* Maximum size of a directory name */
#define FSYS_DIRNAME_MAX                1024

/* maximum size of volume name */
#define MAX_VOLUME_NAME_LEN             512

/* maximum size of part name */
#define MAX_PART_NAME_LEN               MAX_VOLUME_NAME_LEN

/* maximum size of file system name */
#define MAX_FS_NAME_LEN                 16

/* file attributes */
#define FSYS_ATTR_READONLY              0x00000001
#define FSYS_ATTR_HIDDEN                0x00000002
#define FSYS_ATTR_SYSTEM                0x00000004
#define FSYS_FAT_VOLUME_ID              0x00000008
#define FSYS_ATTR_DIRECTORY             0x00000010
#define FSYS_ATTR_ARCHIVE               0x00000020

#define FSYS_FAT_DENTRY_SIZE            0x20
#define MAX_DISK_LETTER_LEN             26
#define MAX_DIR_SCAN_LEVEL              32

/* names of supported file system */
#define FSYS_NTFS_NAME                  "NTFS"
#define FSYS_FAT_NAME                   "FAT"
#define FSYS_EXFAT_NAME                 "exFAT"
#define FSYS_CDFS_NAME                  "CDFS"
#define FSYS_UDF_NAME                   "UDF"
/* Global error codes */
#define FSYS_ERR_OK                     (__s16)0x0000
#define FSYS_ERR_EOF                    (__s16)0xfff0
#define FSYS_ERR_DISKFULL               (__s16)0xffe0
#define FSYS_ERR_INVALIDPAR             (__s16)0xffd0
#define FSYS_ERR_WRITEONLY              (__s16)0xffc0
#define FSYS_ERR_READONLY               (__s16)0xffb0
#define FSYS_ERR_READERROR              (__s16)0xffa0
#define FSYS_ERR_WRITEERROR             (__s16)0xff90
#define FSYS_ERR_DISKCHANGED            (__s16)0xff80
#define FSYS_ERR_CLOSE                  (__s16)0xff70

/* 设备占用盘符 */
#define FSYS_MAX_FPARTS                 16          /* 自由分配的分区项个数 */
#define FSYS_MAX_XPARTS                 8           /* 系统内部固定分配的分区项个数 */
#define FSYS_MAX_UPARTS                 2           /* 方案用户自定义的分区项个数 */
#define FSYS_MAX_PARTS                  (FSYS_MAX_FPARTS + FSYS_MAX_XPARTS + FSYS_MAX_UPARTS)
/* 固定分配的分区项个数 */
#define PART_LETTER_FREE                0xff        /* 根据设备注册先后顺序自由分配盘符 */
#define PART_LETTER_FREESTART           'E'         /* 自由分配盘符的起始盘符 */
#define PART_LETTER_USER                0xfe        /* 方案用户新增分区根据注册的先后顺利
                                                     * 在U~W范围内分配盘符(从W到U倒序分配) */
#define PART_LETTER_USERSTART           'V'         /* 方案新增分区的起始盘符V(倒序分配V-U...) */
#define PART_LETTER_DMS                 'B'         /* 设备分区盘符 */
#define PART_LETTER_RAMDISK             'C'         /* RAMDISK盘符 */
#define PART_LETTER_ROOTFS              'D'         /* ROOTFS盘符 */
#define PART_LETTER_UDISK               'E'         /* udisk盘符 */
#define PART_LETTER_SYSDATA             'Z'         /* 系统数据区盘符 */
#define PART_LETTER_SYSBOOT             'Y'         /* 系统启动区盘符 */
#define PART_LETTER_BOOTFS              'X'         /* eGON2启动分区 */
#define PART_LETTER_RESERVEV            'V'         /* 保留未用盘符 */
#define PART_LETTER_RESERVEU            'U'         /* 保留未用盘符 */
#define PART_LETTER_RESERVEA            'A'         /* 保留未用盘符 */

/* GlobalSY constants*/
#define FSYS_SEEK_CUR                   1       //current position
#define FSYS_SEEK_END                   2       //from end of the file
#define FSYS_SEEK_SET                   0       //from begin of the file

/********************* 旧part && dev ioctl cmd ******************/
/* I/O coSYmmands */
#define FSYS_CMD_FLUSH_CACHE            1000L
#define FSYS_CMD_CHK_DSKCHANGE          1010L
#define FSYS_CMD_READ_SECTOR            1100L
#define FSYS_CMD_WRITE_SECTOR           1110L
#define FSYS_CMD_FORMAT_MEDIA           2222L
#define FSYS_CMD_FORMAT_AUTO            2333L
#define FSYS_CMD_INC_BUSYCNT            3001L
#define FSYS_CMD_DEC_BUSYCNT            3002L
#define FSYS_CMD_GET_DISKFREE           4000L
#define FSYS_CMD_GET_DEVINFO            4011L
#define FSYS_CMD_FLASH_ERASE_CHIP       9001L
/* part I/O coSYmmands */
#define FSYS_PART_CMD_GET_PARTSIZE      1000L       /* 获取part大小，byte为单位                                     */
#define FSYS_PART_CMD_FLUSH_CACHE       1001L       /* flash cache                                                  */
#define FSYS_PART_CMD_INC_BUSYCNT       1100L       /* 增加busy count                                               */
#define FSYS_PART_CMD_DEC_BUSYCNT       1101L       /* 减少busy count                                               */
#define FSYS_PART_CMD_GET_STATUS        1102L       /* 获取状态                                                     */
#define FSYS_PART_CMD_GET_INFO          1103L       /* 获取info                                                     */
#define FSYS_PART_CMD_FORMAT            1104L       /* 格式化                                                       */
#define FSYS_PART_MODE_CACHE_USED       1002L       /* 启动cache机制                                                */
#define FSYS_PART_MODE_CACHE_UNUSED     1003L       /* 关闭cache机制                                                */
/******************** 旧part && dev ioctl cmd  ******************/

/* part I/O control command */
#define PART_IOC_USR_BASE               0x00000000
#define PART_IOC_SYS_BASE               0x80000000
#define IS_PARTIOCSYS(cmd)              (cmd>=PART_IOC_SYS_BASE)
/* for sys */
#define PART_IOC_SYS_GETNAME            (PART_IOC_SYS_BASE + 0)
#define PART_IOC_SYS_SETFSPRIV          (PART_IOC_SYS_BASE + 1)
#define PART_IOC_SYS_GETFSPRIV          (PART_IOC_SYS_BASE + 2)
#define PART_IOC_SYS_GETPDPRIV          (PART_IOC_SYS_BASE + 3)
#define PART_IOC_SYS_GETDEV             (PART_IOC_SYS_BASE + 4)
#define PART_IOC_SYS_GETLETTER          (PART_IOC_SYS_BASE + 5)
#define PART_IOC_SYS_GETLASTPART        (PART_IOC_SYS_BASE + 6)

/* for raw part */
#define PART_IOC_USR_GETPARTSIZE        (PART_IOC_USR_BASE+0)
#define PART_IOC_USR_FLUSHCACHE         (PART_IOC_USR_BASE+1)
#define PART_IOC_USR_CACHEUSED          (PART_IOC_USR_BASE+2)
#define PART_IOC_USR_CACHEUNUSED        (PART_IOC_USR_BASE+3)
#define PART_IOC_USR_GETSCTSIZE         (PART_IOC_USR_BASE+4)
/* for dms part */
#define PART_IOC_USR_GETITEM            (PART_IOC_USR_BASE+5)
#define PART_IOC_USR_GETDEVCLSROOT      (PART_IOC_USR_BASE+6)
/* for cd-rom part */
#define PART_IOC_CDROM_LAST_WRITTEN     (PART_IOC_USR_BASE+7)
#define PART_IOC_CDROM_MULTISESSION     (PART_IOC_USR_BASE+8)

/* fs I/O control command */
#define FS_IOC_USR_BASE                 0x00000000
#define FS_IOC_SYS_BASE                 0x80000000
#define IS_FSIOCSYS(cmd)                (cmd>=FS_IOC_SYS_BASE)
/* for sys */
#define FS_IOC_SYS_GETFLAGS             (FS_IOC_SYS_BASE+0x80)
#define FS_IOC_SYS_SETFLAGS             (FS_IOC_SYS_BASE+0x81)
#define FS_IOC_SYS_GETVERSION           (FS_IOC_SYS_BASE+0x82)
#define FS_IOC_SYS_SETVERSION           (FS_IOC_SYS_BASE+0x83)
#define FS_IOC_SYS_GETATTR              (FS_IOC_SYS_BASE+0x84)

/* for fat fs */
#define FS_IOC_USR_DEBUG_FATCHUNK       (FS_IOC_SYS_BASE+0x100)
#define FSYS_SECTOR_SIZE                512
#define FSYS_PART_MEDIACHANGED          0x0001

/* for minfs fs */
#define MINFS_IOC_GET_UNCOMPRESS_FILE_SIZE  (FS_IOC_SYS_BASE+0x200)
#define MINFS_IOC_GET_UNCOMPRESS_FILE_DATA  (FS_IOC_SYS_BASE+0x201)

#define DEVFS_NODE_TYPE                 1
#define DEVFS_CLASS_TYPE                2

/* flags for esFSYS_statfs (get file system status) operation */
#define FSYS_KSTATUS_TYPE       0x00000001  /* get fs magic type            */
#define FSYS_KSTATUS_BSIZE      0x00000002  /* get fs block size            */
#define FSYS_KSTATUS_BLOCKS     0x00000004  /* get fs total blocks number   */
#define FSYS_KSTATUS_BFREE      0x00000008  /* get fs free blocks number(for NTFS it is very slowly) */
#define FSYS_KSTATUS_FILES      0x00000010  /* get fs total files number    */
#define FSYS_KSTATUS_NAMELEN    0x00000100  /* get fs name length           */
#define FSYS_KSTATUS_ATTR       0x00000200  /* get fs attribute(eg:READONLY and so on) */
#define FSYS_KSTATUS_MEDIAATTR  0x00000400  /* get fs media attribute       */
#define FSYS_KSTATUS_VOLLETTER  0x00000800  /* get fs volume letter         */
#define FSYS_KSTATUS_VOLNAME    0x00001000  /* get fs volume name           */
#define FSYS_KSTATUS_FSNAME     0x00002000  /* get fs type name             */
#define FSYS_KSTATUS_ALL        0x0000FFFF  /* get fs all kernal status information*/

struct ktimestamp
{
    __u16       year;
    __u8        month;
    __u8        day;
    __u8        hour;
    __u8        minute;
    __u8        second;
    __u8        milliseconds;
};

struct kstat
{
    unsigned short    mode;
    unsigned int      nlink;
    long long         size;
    long long         pos;
    struct ktimestamp atime;
    struct ktimestamp mtime;
    struct ktimestamp ctime;
    unsigned long     blksize;
    unsigned long long  blocks;
};

/*
 * Kernel pointers have redundant information, so we can use a
 * scheme where we can return either an error code or a dentry
 * pointer with the same return value.
 *
 * This should be a per-architecture thing, to allow different
 * error and pointer decisions.
 */
#define MAX_ERRNO   4095

#define IS_ERR_VALUE(x) ((x) >= (unsigned long)-MAX_ERRNO)

static inline void *ERR_PTR(long error)
{
    return (void *) error;
}

static inline long PTR_ERR(const void *ptr)
{
    return (long) ptr;
}

static inline long IS_ERR(const long ptr)
{
    return IS_ERR_VALUE((unsigned long)ptr);
}

static inline bool IS_ERR_OR_NULL(const void *ptr)
{
	return (!ptr) || IS_ERR_VALUE((unsigned long)ptr);
}

struct kstatpt
{
    char *p_pdname;             /* part driver name, like "dmspart" or "dospart" */
    __u32 p_bsize;              /* part block size */
    __u32 p_blocks;             /* part blocks number */

    __u32 p_attr;               /* copy from part.attr, indecate part attribute like rw mode and formated */
    __u8  p_status;             /* copy of part.satus, fs used or raw used or dead */
    __u8  p_name[MAX_PART_NAME_LEN];    /* strcopy from part.dname, it is part name derived from dev */

    /* the device information */
    __u8  p_devname[MAX_DEV_NODE_NAME_LEN];       /* device node name */
    __u8  p_devclassname[MAX_DEV_CLASS_NAME_LEN]; /* device class name*/
    __u32 p_devattr;                              /* device attribute */
};

#undef EPERM
#undef ENOENT
#undef ESRCH
#undef EINTR
#undef EIO
#undef ENXIO
#undef E2BIG
#undef ENOEXEC
#undef EBADF
#undef ECHILD
#undef EAGAIN
#undef ENOMEM
#undef EACCES
#undef EFAULT
#undef ENOTBLK
#undef EBUSY
#undef EEXIST
#undef EXDEV
#undef ENODEV
#undef ENOTDIR
#undef EISDIR
#undef EINVAL
#undef ENFILE
#undef EMFILE
#undef ENOTTY
#undef ETXTBSY
#undef EFBIG
#undef ENOSPC
#undef ESPIPE
#undef EROFS
#undef EMLINK
#undef EPIPE
#undef EDOM
#undef ERANGE
#undef EDEADLK
#undef ENAMETOOLONG
#undef ENOLCK
#undef ENOSYS
#undef ENOTEMPTY
#undef ELOOP
#undef EWOULDBLOCK
#undef ENOMSG
#undef EIDRM
#undef ECHRNG
#undef EL2NSYNC
#undef EL3HLT
#undef EL3RST
#undef ELNRNG
#undef EUNATCH
#undef ENOCSI
#undef EL2HLT
#undef EBADE
#undef EBADR
#undef EXFULL
#undef ENOANO
#undef EBADRQC
#undef EBADSLT
#undef EDEADLOCK
#undef EBFONT
#undef ENOSTR
#undef ENODATA
#undef ETIME
#undef ENOSR
#undef ENONET
#undef ENOPKG
#undef EREMOTE
#undef ENOLINK
#undef EADV
#undef ESRMNT
#undef ECOMM
#undef EPROTO
#undef EMULTIHOP
#undef EDOTDOT
#undef EBADMSG
#undef EOVERFLOW
#undef ENOTUNIQ
#undef EBADFD
#undef EREMCHG
#undef ELIBACC
#undef ELIBBAD
#undef ELIBSCN
#undef ELIBMAX
#undef ELIBEXEC
#undef EILSEQ
#undef ERESTART
#undef ESTRPIPE
#undef EUSERS
#undef ENOTSOCK
#undef EDESTADDRREQ
#undef EMSGSIZE
#undef EPROTOTYPE
#undef ENOPROTOOPT
#undef EPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#undef EOPNOTSUPP
#undef EPFNOSUPPORT
#undef EAFNOSUPPORT
#undef EADDRINUSE
#undef EADDRNOTAVAIL
#undef ENETDOWN
#undef ENETUNREACH
#undef ENETRESET
#undef ECONNABORTED
#undef ECONNRESET
#undef ENOBUFS
#undef EISCONN
#undef ENOTCONN
#undef ESHUTDOWN
#undef ETOOMANYREFS
#undef ETIMEDOUT
#undef ECONNREFUSED
#undef EHOSTDOWN
#undef EHOSTUNREACH
#undef EALREADY
#undef EINPROGRESS
#undef ESTALE
#undef EUCLEAN
#undef ENOTNAM
#undef ENAVAIL
#undef EISNAM
#undef EREMOTEIO
#undef EDQUOT
#undef ENOMEDIUM
#undef EMEDIUMTYPE
#undef ECANCELED
#undef ENOKEY
#undef EKEYEXPIRED
#undef EKEYREVOKED
#undef EKEYREJECTED
#undef EOWNERDEAD
#undef ENOTRECOVERABLE

#define EPERM           1 /* Operation not permitted */
#define ENOENT          2 /* No such file or directory */
#define ESRCH           3 /* No such process */
#define EINTR           4 /* Interrupted system call */
#define EIO             5 /* I/O error */
#define ENXIO           6 /* No such device or address */
#define E2BIG           7 /* Argument list too long */
#define ENOEXEC         8 /* Exec format error */
#define EBADF           9 /* Bad file number */
#define ECHILD          10  /* No child processes */
#define EAGAIN          11  /* Try again */
#define ENOMEM          12  /* Out of memory */
#define EACCES          13  /* Permission denied */
#define EFAULT          14  /* Bad address */
#define ENOTBLK         15  /* Block device required */
#define EBUSY           16  /* Device or resource busy */
#define EEXIST          17  /* File exists */
#define EXDEV           18  /* Cross-device link */
#define ENODEV          19  /* No such device */
#define ENOTDIR         20  /* Not a directory */
#define EISDIR          21  /* Is a directory */
#define EINVAL          22  /* Invalid argument */
#define ENFILE          23  /* File table overflow */
#define EMFILE          24  /* Too many open files */
#define ENOTTY          25  /* Not a typewriter */
#define ETXTBSY         26  /* Text file busy */
#define EFBIG           27  /* File too large */
#define ENOSPC          28  /* No space left on device */
#define ESPIPE          29  /* Illegal seek */
#define EROFS           30  /* Read-only file system */
#define EMLINK          31  /* Too many links */
#define EPIPE           32  /* Broken pipe */
#define EDOM            33  /* Math argument out of domain of func */
#define ERANGE          34  /* Math result not representable */
#define EDEADLK         35  /* Resource deadlock would occur */
#define ENAMETOOLONG    36  /* File name too long */
#define ENOLCK          37  /* No record locks available */
#define ENOSYS          38  /* Function not implemented */
#define ENOTEMPTY       39  /* Directory not empty */
#define ELOOP           40  /* Too many symbolic links encountered */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */
#define ENOMSG          42  /* No message of desired type */
#define EIDRM           43  /* Identifier removed */
#define ECHRNG          44  /* Channel number out of range */
#define EL2NSYNC        45  /* Level 2 not synchronized */
#define EL3HLT          46  /* Level 3 halted */
#define EL3RST          47  /* Level 3 reset */
#define ELNRNG          48  /* Link number out of range */
#define EUNATCH         49  /* Protocol driver not attached */
#define ENOCSI          50  /* No CSI structure available */
#define EL2HLT          51  /* Level 2 halted */
#define EBADE           52  /* Invalid exchange */
#define EBADR           53  /* Invalid request descriptor */
#define EXFULL          54  /* Exchange full */
#define ENOANO          55  /* No anode */
#define EBADRQC         56  /* Invalid request code */
#define EBADSLT         57  /* Invalid slot */
#define EDEADLOCK       EDEADLK
#define EBFONT          59  /* Bad font file format */
#define ENOSTR          60  /* Device not a stream */
#define ENODATA         61  /* No data available */
#define ETIME           62  /* Timer expired */
#define ENOSR           63  /* Out of streams resources */
#define ENONET          64  /* Machine is not on the network */
#define ENOPKG          65  /* Package not installed */
#define EREMOTE         66  /* Object is remote */
#define ENOLINK         67  /* Link has been severed */
#define EADV            68  /* Advertise error */
#define ESRMNT          69  /* Srmount error */
#define ECOMM           70  /* Communication error on send */
#define EPROTO          71  /* Protocol error */
#define EMULTIHOP       72  /* Multihop attempted */
#define EDOTDOT         73  /* RFS specific error */
#define EBADMSG         74  /* Not a data message */
#define EOVERFLOW       75  /* Value too large for defined data type */
#define ENOTUNIQ        76  /* Name not unique on network */
#define EBADFD          77  /* File descriptor in bad state */
#define EREMCHG         78  /* Remote address changed */
#define ELIBACC         79  /* Can not access a needed shared library */
#define ELIBBAD         80  /* Accessing a corrupted shared library */
#define ELIBSCN         81  /* .lib section in a.out corrupted */
#define ELIBMAX         82  /* Attempting to link in too many shared libraries */
#define ELIBEXEC        83  /* Cannot exec a shared library directly */
#define EILSEQ          84  /* Illegal byte sequence */
#define ERESTART        85  /* Interrupted system call should be restarted */
#define ESTRPIPE        86  /* Streams pipe error */
#define EUSERS          87  /* Too many users */
#define ENOTSOCK        88  /* Socket operation on non-socket */
#define EDESTADDRREQ    89  /* Destination address required */
#define EMSGSIZE        90  /* Message too long */
#define EPROTOTYPE      91  /* Protocol wrong type for socket */
#define ENOPROTOOPT     92  /* Protocol not available */
#define EPROTONOSUPPORT 93  /* Protocol not supported */
#define ESOCKTNOSUPPORT 94  /* Socket type not supported */
#define EOPNOTSUPP      95  /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT    96  /* Protocol family not supported */
#define EAFNOSUPPORT    97  /* Address family not supported by protocol */
#define EADDRINUSE      98  /* Address already in use */
#define EADDRNOTAVAIL   99  /* Cannot assign requested address */
#define ENETDOWN        100 /* Network is down */
#define ENETUNREACH     101 /* Network is unreachable */
#define ENETRESET       102 /* Network dropped connection because of reset */
#define ECONNABORTED    103 /* Software caused connection abort */
#define ECONNRESET      104 /* Connection reset by peer */
#define ENOBUFS         105 /* No buffer space available */
#define EISCONN         106 /* Transport endpoint is already connected */
#define ENOTCONN        107 /* Transport endpoint is not connected */
#define ESHUTDOWN       108 /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS    109 /* Too many references: cannot splice */
#define ETIMEDOUT       110 /* Connection timed out */
#define ECONNREFUSED    111 /* Connection refused */
#define EHOSTDOWN       112 /* Host is down */
#define EHOSTUNREACH    113 /* No route to host */
#define EALREADY        114 /* Operation already in progress */
#define EINPROGRESS     115 /* Operation now in progress */
#define ESTALE          116 /* Stale NFS file handle */
#define EUCLEAN         117 /* Structure needs cleaning */
#define ENOTNAM         118 /* Not a XENIX named type file */
#define ENAVAIL         119 /* No XENIX semaphores available */
#define EISNAM          120 /* Is a named type file */
#define EREMOTEIO       121 /* Remote I/O error */
#define EDQUOT          122 /* Quota exceeded */
#define ENOMEDIUM       123 /* No medium found */
#define EMEDIUMTYPE     124 /* Wrong medium type */
#define ECANCELED       125 /* Operation Canceled */
#define ENOKEY          126 /* Required key not available */
#define EKEYEXPIRED     127 /* Key has expired */
#define EKEYREVOKED     128 /* Key has been revoked */
#define EKEYREJECTED    129 /* Key was rejected by service */
#define EOWNERDEAD      130 /* Owner died */
#define ENOTRECOVERABLE 131 /* State not recoverable */

__u32 fsys_init(void);
__hdle esFSYS_opendir(const char *pDirName);
__hdle esFSYS_readdir(__hdle hDir);
__hdle esFSYS_fopen(const char *pFileName, const char *pMode) ;
__hdle esFSYS_popen(const char *PartName, const char *pMode);
__hdle esFSYS_fd2file(__s32  fd);
void  esFSYS_ferrclr(__hdle hFile);
void  esFSYS_rewinddir(__hdle hDir);
void  esFSYS_perrclr(__hdle hPart);
__s32 esFSYS_closedir(__hdle hDir);
__s32 esFSYS_mkdir(const char *pDirName);
__s32 esFSYS_rmdir(const char *pDirName);
__s32 esFSYS_remove(const char *pFileName);
__s32 esFSYS_rename(const char *newname, const char *oldname);
__s32 esFSYS_fclose(__hdle hFile);
__u32 esFSYS_fread(void *pData, __u32 Size, __u32 N, __hdle hFile);
__u32 esFSYS_fwrite(const void *pData, __u32 Size, __u32 N, __hdle hFile);
__s32 esFSYS_fseek(__hdle hFile, __s32 Offset, __s32 Whence);
__s32 esFSYS_ftell(__hdle hFile);
__s32 esFSYS_fseekex(__hdle hFile, __s32 l_off, __s32 h_off, __s32 Whence);
__s32 esFSYS_ftellex(__hdle hFile, __s32 *l_pos, __s32 *h_pos);
__s32 esFSYS_fioctrl(__hdle hFile, __s32 Cmd, __s32 Aux, void *pBuffer);
__s32 esFSYS_ferror(__hdle hFile);
__s32 esFSYS_premove(const char *pFileName);
__s32 esFSYS_prename(const char *newfilename, const char *oldfilename);
__s32 esFSYS_pclose(__hdle hPart);
__u32 esFSYS_pread(void *pData, __u32 sector, __u32 n, __hdle hPart);
__u32 esFSYS_pwrite(const void *pData, __u32 Sector, __u32 N, __hdle hPart);
__s32 esFSYS_pioctrl(__hdle hPart, __u32 cmd, __s32 aux, void *pbuffer);
__s32 esFSYS_perror(__hdle hPart);
__s32 esFSYS_mntparts(__hdle hNode);
__s32 esFSYS_umntparts(__hdle hNode, __u32 force);
__s32 esFSYS_fsreg(__hdle hFS);
__s32 esFSYS_fsunreg(__hdle hFS);
__s32 esFSYS_pdreg(__hdle hPD);
__s32 esFSYS_pdunreg(__hdle hPD);
__s32 esFSYS_partfslck(char *partname);
__s32 esFSYS_partfsunlck(char *partname);
__s32 esFSYS_mntfs(__hdle part);
__s32 esFSYS_umntfs(__hdle part, __s32 force);
__s32 esFSYS_format(const char *partname, const char *fstype, __hdle fmtpara);
__s32 esFSYS_statfs(const char *path, __hdle buf, __u32 flags);
__s32 esFSYS_fsdbg(const char *cmd, const char *para);
__s32 esFSYS_fsync(__hdle hFile);
__s32 esFSYS_fstat(__hdle hFile, void *stat_buf);
__s32 esFSYS_statpt(const char *path, __hdle buf);
__s32 esFSYS_setfs(char *partname, __u32 cmd, __s32 aux, char *para);
__s32 esFSYS_getfscharset(const char *partname, __s32 *pCharset);
__s32 esFSYS_file2fd(__hdle hFile);
__s32 esFSYS_open(const char *name, __s32 flag, __s32 prems);
__s32 esFSYS_querypartupdateflag(const char *path, __bool *flag);
__s32 esFSYS_clearpartupdateflag(const char *path);
__s32 esFSYS_ftruncate(__hdle  filehandle, __u32 length);

#endif
