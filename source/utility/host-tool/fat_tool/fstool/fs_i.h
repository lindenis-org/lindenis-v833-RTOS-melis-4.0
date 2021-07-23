
#ifndef     __FS_I_H__
#define     __FS_I_H__

/* 目录操作 */
__hdle esFSYS_opendir(const char * /*dirname*/);
__s32  esFSYS_closedir(__hdle /*hDir*/);
__hdle esFSYS_readdir(__hdle /*hDir*/);
void   esFSYS_rewinddir(__hdle /*hDir*/);
__s32  esFSYS_mkdir(const char * /*pdirname*/);
__s32  esFSYS_rmdir(const char * /*pdirname*/);

/* 文件操作 */
__s32  esFSYS_remove(const char * /*filename*/);
__s32  esFSYS_rename(const char * /*filename*/, const char * /*newname*/);
__hdle esFSYS_fopen(const char * /*filename*/, const char * /*pMode*/);
__s32  esFSYS_fclose(__hdle /*hFile*/);
__u32  esFSYS_fread(void * /*pdata*/, __u32 /*size*/, __u32 /*cnt*/, __hdle /*hFile*/);
__u32  esFSYS_fwrite(const void * /*pdata*/, __u32 /*size*/, __u32 /*cnt*/, __hdle /*hFile*/);
__s32  esFSYS_fseek(__hdle /*hFile*/, __s32 /*offset*/, __s32 /*whence*/);
__s32  esFSYS_ftell(__hdle /*hFile*/);
__s32  esFSYS_fseekex(__hdle /*hFile*/, __s32 /*l_offset*/, __s32 /* h_offset*/, __s32 /*whence*/);
__s32  esFSYS_ftellex(__hdle /*hFile*/, __s32 * /*l_offset*/, __s32 * /* h_offset*/);
__s32  esFSYS_fioctrl(__hdle /*hFile*/, __s32 /*Cmd*/, __s32 /*Aux*/, void * /*pBuffer*/);
__s32  esFSYS_ferror(__hdle /*hFile*/);
void   esFSYS_ferrclr(__hdle /*hFile*/);

/* 其它操作 */
__s32  esFSYS_format(const char */*partname*/, const char */*fstype*/, __hdle /*fmtpara*/);
__s32  esFSYS_statfs(const char * /*pathname*/, __hdle /*buf*/);
__s32  esFSYS_fsdbg(const char * /*cmd*/, const char * /*para*/);
__s32  esFSYS_fsync(__hdle /*hFile*/);
__s32  esFSYS_fstat(__hdle /*hFile*/, void * /*stat_buf*/);
__s32  esFSYS_statpt(const char * /*PartName*/, __hdle /*stat buf*/);
__s32  esFSYS_setfs(char * /*partname*/, __u32 /*cmd*/, __s32 /*aux*/, char * /*para*/);

__s32  esFSYS_mntfs(__hdle part);
__s32  esFSYS_umntfs(__hdle part, __s32 force);
__s32  esFSYS_fsreg(__hdle hFS);
__s32  esFSYS_fsunreg(__hdle hFS);

/* 分区操作 */
__s32  esFSYS_pdunreg(__hdle hPD);
__s32  esFSYS_pdreg(__hdle hPD);

__u32  FSYS_Init(void);
__u32  FSYS_Exit(void);

#endif      /* __FS_I_H__ */


