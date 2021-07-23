#include    "fsys_types.h"
#include    "if/fstool_if.h"


/* 目录操作 */
extern __hdle esFSYS_opendir(const char * /*dirname*/);
extern __s32  esFSYS_closedir(__hdle /*hDir*/);
extern __hdle esFSYS_readdir(__hdle /*hDir*/);
extern void   esFSYS_rewinddir(__hdle /*hDir*/);
extern __s32  esFSYS_mkdir(const char * /*pdirname*/);
extern __s32  esFSYS_rmdir(const char * /*pdirname*/);

/* 文件操作 */
extern __s32  esFSYS_remove(const char * /*filename*/);
extern __s32  esFSYS_rename(const char * /*filename*/, const char * /*newname*/);
extern __hdle esFSYS_fopen(const char * /*filename*/, const char * /*pMode*/);
extern __s32  esFSYS_fclose(__hdle /*hFile*/);
extern __u32  esFSYS_fread(void * /*pdata*/, __u32 /*size*/, __u32 /*cnt*/, __hdle /*hFile*/);
extern __u32  esFSYS_fwrite(const void * /*pdata*/, __u32 /*size*/, __u32 /*cnt*/, __hdle /*hFile*/);
extern __s32  esFSYS_fseek(__hdle /*hFile*/, __s32 /*offset*/, __s32 /*whence*/);
extern __s32  esFSYS_ftell(__hdle /*hFile*/);
extern __s32  esFSYS_fseekex(__hdle /*hFile*/, __s32 /*l_offset*/, __s32 /* h_offset*/, __s32 /*whence*/);
extern __s32  esFSYS_ftellex(__hdle /*hFile*/, __s32 * /*l_offset*/, __s32 * /* h_offset*/);
extern __s32  esFSYS_fioctrl(__hdle /*hFile*/, __s32 /*Cmd*/, __s32 /*Aux*/, void * /*pBuffer*/);
extern __s32  esFSYS_ferror(__hdle /*hFile*/);
extern void   esFSYS_ferrclr(__hdle /*hFile*/);

/* 其它操作 */
extern __s32  esFSYS_format(const char */*partname*/, const char */*fstype*/, __hdle /*fmtpara*/);
extern __s32  esFSYS_statfs(const char * /*pathname*/, __hdle /*buf*/);
extern __s32  esFSYS_fsdbg(const char * /*cmd*/, const char * /*para*/);
extern __s32  esFSYS_fsync(__hdle /*hFile*/);
extern __s32  esFSYS_fstat(__hdle /*hFile*/, void * /*stat_buf*/);
extern __s32  esFSYS_statpt(const char * /*PartName*/, __hdle /*stat buf*/);
extern __s32  esFSYS_setfs(char * /*partname*/, __u32 /*cmd*/, __s32 /*aux*/, char * /*para*/);

extern __s32  esFSYS_mntfs(__hdle part);
extern __s32  esFSYS_umntfs(__hdle part, __s32 force);

extern __s32  esFSYS_pdunreg(__hdle hPD);
extern __s32  esFSYS_pdreg(__hdle hPD);

extern __u32  FSYS_Init(void);
extern __u32  FSYS_Exit(void);



//------------------------------------------------------------------------------
//
//                          file operations
//
//------------------------------------------------------------------------------
static __s32 fsys_remove(const char *filename)
{
    return esFSYS_remove(filename);
}

static __s32 fsys_rename(const char *newname, const char *oldname)
{
    return esFSYS_rename(newname, oldname);
}

static __hdle fsys_fopen(const char *filename, const char *mode)
{
    return esFSYS_fopen(filename, mode);
}

static __s32 fsys_fclose(__hdle stream)
{
    return esFSYS_fclose(stream);
}

static __s32 fsys_fsync(__hdle stream)
{
    return esFSYS_fsync(stream);
}

__size fsys_fread(void *ptr, __size size, __size nmemb, __hdle stream)
{
    return esFSYS_fread(ptr, size, nmemb, stream);
}

__size fsys_fwrite(const void *ptr, __size size, __size nmemb, __hdle stream)
{
    return esFSYS_fwrite(ptr, size, nmemb, stream);
}

static __s32 fsys_fseek(__hdle stream, __s32 offset, int whence)
{
    return esFSYS_fseek(stream, offset, whence);
}

static __s32 fsys_ftell(__hdle stream)
{
    return esFSYS_ftell(stream);
}

static __s32 fsys_fioctrl(__hdle stream, __s32 Cmd, __s32 Aux, void *pBuffer)
{
    return esFSYS_fioctrl(stream, Cmd, Aux, pBuffer);
}

static __s32 fsys_ferror(__hdle stream)
{
    return esFSYS_ferror(stream);
}

static void fsys_ferrclr(__hdle stream)
{
    esFSYS_ferrclr(stream);
}

//------------------------------------------------------------------------------
//
//                          directory operations
//
//------------------------------------------------------------------------------
static __hdle fsys_opendir(const char *dirname)
{
    return esFSYS_opendir(dirname);
}

static __s32 fsys_closedir(__hdle hDir)
{
    return esFSYS_closedir(hDir);
}

ES_DIRENT *fsys_readdir(__hdle hDir)
{
    return (ES_DIRENT *)esFSYS_readdir(hDir);
}

void fsys_rewinddir(__hdle hDir)
{
    esFSYS_rewinddir(hDir);
}

static __s32 fsys_mkdir(const char *dirname)
{
    return esFSYS_mkdir(dirname);
}

static __s32 fsys_rmdir(const char *dirname)
{
    return esFSYS_rmdir(dirname);
}

//------------------------------------------------------------------------------
//
//                          other operations
//
//------------------------------------------------------------------------------
static __s32 fsys_format(const char *partname, const char *fsname, ES_FMTPARA *fmtpara)
{
    return esFSYS_format(partname, fsname, (__hdle)fmtpara);
}

//------------------------------------------------------------------------------
//
//                          part opeartions
//
//------------------------------------------------------------------------------

static __s32 fsys_pdreg(__fsys_part_t *pPD)
{
    return esFSYS_pdreg((__hdle)pPD);
}

static __s32 fsys_pdunreg(__fsys_part_t *pPD)
{
    return esFSYS_pdunreg((__hdle)pPD);
}

static __s32 fsys_mntfs(__fsys_part_t *part)
{
    return esFSYS_mntfs((__hdle)part);
}

static __s32 fsys_umntfs(__fsys_part_t *part, __s32 force)
{
    return esFSYS_umntfs((__hdle)part, force);
}

FSYS_TOOL_IF fsTool;
FSYS_TOOL_IF *GetInterface(const char *plgID)
{
    fsTool.fsys_init = FSYS_Init;
    fsTool.fsys_exit = FSYS_Exit;
//dir operation
    fsTool.opendir   = fsys_opendir;
    fsTool.closedir  = fsys_closedir;
    fsTool.readdir   = fsys_readdir;
    fsTool.rewinddir = fsys_rewinddir;
    fsTool.mkdir     = fsys_mkdir;
    fsTool.rmdir     = fsys_rmdir;
//file operation
    fsTool.remove    = fsys_remove;
    fsTool.rename    = fsys_rename;
    fsTool.fopen     = fsys_fopen;
    fsTool.fclose    = fsys_fclose;
    fsTool.fread     = fsys_fread;
    fsTool.fwrite    = fsys_fwrite;
    fsTool.fseek     = fsys_fseek;
    fsTool.ftell     = fsys_ftell;
    fsTool.fioctrl   = fsys_fioctrl;
    fsTool.ferror    = fsys_ferror;
    fsTool.ferrclr   = fsys_ferrclr;
//other operation
    fsTool.format    = fsys_format;
//part operation
    fsTool.pdreg     = fsys_pdreg;
    fsTool.pdunreg   = fsys_pdunreg;
    fsTool.mntfs     = fsys_mntfs;
    fsTool.umntfs    = fsys_umntfs;

    return &fsTool;
}
