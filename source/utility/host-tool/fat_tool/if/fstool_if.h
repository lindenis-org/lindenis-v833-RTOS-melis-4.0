//------------------------------------------------------------------------------------------------------------
//
// fsys_tool_if.h
//
// 2009-05-20 16:45:36
//
//
//
//------------------------------------------------------------------------------------------------------------


#ifndef __FSYS_TOOL_IF___H____
#define __FSYS_TOOL_IF___H____  1


#include "fstool_plug_type.h"

//------------------------------------------------------------------------------
//                      文件、目录操作
//------------------------------------------------------------------------------
#define ES_SEEK_SET 0   /* start of stream (see fseek)              */
#define ES_SEEK_CUR 1   /* current position in stream (see fseek)   */
#define ES_SEEK_END 2   /* end of stream (see fseek)                */
#define ES_EOF      -1  /* End of file                              */

#define FSYS_ATTR_READONLY    0x00000001
#define FSYS_ATTR_HIDDEN      0x00000002
#define FSYS_ATTR_SYSTEM      0x00000004
#define FSYS_FAT_VOLUME_ID    0x00000008
#define FSYS_ATTR_DIRECTORY   0x00000010
#define FSYS_ATTR_ARCHIVE     0x00000020

#define FSYS_DIRNAME_MAX      256
#define MAX_VOLUME_NAME_LEN   32
#define MAX_FS_NAME_LEN       16
#define MAX_DISK_LETTER_LEN   26
#define MAX_DIR_SCAN_LEVEL    32
typedef unsigned long long          __u64;
typedef struct __ESDIRENT
{
    __u32     d_ino;              /* to be POSIX conform                                  */
    __u8      fatdirattr;         /* FAT only. Contains the "DIR_Attr" field of an entry. */
    __u64     d_size;             /* file size, if directory it will be 0                 */
    __u8      d_name[260];
} ES_DIRENT;

typedef enum
{
    T_FAT12   = 0x0000,
    T_FAT16   = 0x0001,
    T_FAT32   = 0x0002,
    T_NTFS    = 0x0003
} FS_TYPE;

typedef struct
{
    int cluster_size;           //cluster size
    int fs_size;                //file system size
    int tracks;                 //tracek per cylinder
    int sectors;                //sectors per tracks
    int force_umnt;             //force unmount the part if mounted
    int quick;                  //quick format
    int compress;               //set filesystem to have compress attrib
    int subtype;                //sub fs type like fat12/fat16
    int reserve[2];             //reserved space
    char label[12];             //volume label, max len is 11 bytes
} ES_FMTPARA;

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef void   *__hdle;

/* 目录操作 */
typedef __hdle(*pOpendir)(const char *dirname);
typedef __s32(*pClosedir)(__hdle hDir);
typedef ES_DIRENT *(*pReaddir)(__hdle hDir);
typedef void (*pRewinddir)(__hdle hDir);
typedef __s32(*pMkdir)(const char *pdirname);
typedef __s32(*pRmdir)(const char *pdirname);

/* 文件操作 */
typedef __s32(*pRemove)(const char *filename);
typedef __s32(*pRename)(const char *filename, const char *newname);
typedef __hdle(*pFopen)(const char *filename, const char *pMode);
typedef __s32(*pFclose)(__hdle hFile);
typedef __u32(*pFread)(void *pdata, __u32 size, __u32 cnt, __hdle hFile);
typedef __u32(*pFwrite)(const void *pdata, __u32 size, __u32 cnt, __hdle hFile);
typedef __s32(*pFseek)(__hdle hFile, __s32 offset, __s32 whence);
typedef __s32(*pFtell)(__hdle hFile);
typedef __s32(*pFioctrl)(__hdle hFile, __s32 Cmd, __s32 Aux, void *pBuffer);
typedef __s32(*pFerror)(__hdle hFile);
typedef void (*pFerrclr)(__hdle hFile);
typedef __s32(*pFsync)(__hdle hFile);


/* 其他操作 */
typedef __s32(*pFormat)(const char *partname, const char *fsname, ES_FMTPARA *fmtpara);

/* 初始化、结束 */
typedef __u32(*pFsys_init)(void);
typedef __u32(*pFsys_exit)(void);


//------------------------------------------------------------------------------
//                      分区操作
//------------------------------------------------------------------------------
/* part I/O control command */
#define PART_IOC_USR_BASE               0x00000000
#define PART_IOC_SYS_BASE               0x80000000
#define IS_PARTIOCSYS(cmd)              (cmd>=PART_IOC_SYS_BASE)

/* for part internal use    */
#define PART_IOC_SYS_GETNAME            (PART_IOC_SYS_BASE + 0)
#define PART_IOC_SYS_SETFSPRIV          (PART_IOC_SYS_BASE + 1)
#define PART_IOC_SYS_GETFSPRIV          (PART_IOC_SYS_BASE + 2)
#define PART_IOC_SYS_GETLETTER          (PART_IOC_SYS_BASE + 5)

/* for part extern use      */
#define PART_IOC_USR_GETPARTSIZE        (PART_IOC_USR_BASE+0)
#define PART_IOC_USR_FLUSHCACHE         (PART_IOC_USR_BASE+1)
#define PART_IOC_USR_CACHEUSED          (PART_IOC_USR_BASE+2)
#define PART_IOC_USR_CACHEUNUSED        (PART_IOC_USR_BASE+3)

typedef struct __FSYS_PART              __fsys_part_t;      /* 分区         */
typedef struct __FSYS_PDOPS             __fsys_pdops_t;     /* 分区操作     */

struct __FSYS_PDOPS
{
    void   *(*open)(__hdle pPart, void *pArg);
    __u32(*read)(void *pData, __u32 Sector, __u32 N, __hdle pPart);
    __u32(*write)(const void *pData, __u32 Sector, __u32 N, __hdle pPart);
    __s32(*ioctl)(__hdle pPart, __u32 Cmd, __s32 Aux, void *pBuffer);
    __s32(*close)(__hdle pPart, void *pPri);
};
struct __FSYS_PART
{
    char                    letter;      /* 盘符                    */
    __fsys_pdops_t          Opts;        /* 分区操作                */
    __hdle                  hPDPrivate;  /* 分区驱动私有数据块句柄  */
    __hdle                  hFSPrivate;  /* 文件系统私有数据块句柄  */
    __fsys_part_t          *next;
};

typedef __s32(*pPdunreg)(__fsys_part_t *pPD);
typedef __s32(*pPdreg)(__fsys_part_t *pPD);
typedef __s32(*pMntfs)(__fsys_part_t *part);
typedef __s32(*pUmntfs)(__fsys_part_t *part, __s32 force);

//------------------------------------------------------------------------------
//                  fsys_tool插件接口
//------------------------------------------------------------------------------
typedef struct tag_FSYS_TOOL_IF
{

    /* 初始化、结束 */
    pFsys_init   fsys_init;
    pFsys_exit   fsys_exit;

    /* 目录操作 */
    pOpendir     opendir;
    pClosedir    closedir;
    pReaddir     readdir;
    pRewinddir   rewinddir;
    pMkdir       mkdir;
    pRmdir       rmdir;

    /* 文件操作 */
    pRemove      remove;
    pRename      rename;
    pFopen       fopen;
    pFclose      fclose;
    pFread       fread;
    pFwrite      fwrite;
    pFseek       fseek;
    pFtell       ftell;
    pFioctrl     fioctrl;
    pFerror      ferror;
    pFerrclr     ferrclr;
    pFsync       fsync;

    /* 其他操作 */
    pFormat      format;

    /* 分区操作 */
    pPdunreg    pdunreg;
    pPdreg      pdreg;
    pMntfs      mntfs;
    pUmntfs     umntfs;

} __attribute__((packed)) FSYS_TOOL_IF;

#endif //__FSYS_TOOL_IF___H____
