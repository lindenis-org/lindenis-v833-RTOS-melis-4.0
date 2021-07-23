//------------------------------------------------------------------------------------------------------------
//
// fsprocess_inter.h
//
// scott
//
// 2009-12-24 11:18:03
//
//------------------------------------------------------------------------------------------------------------

#ifndef __FSPROCESS_INTER___H____
#define __FSPROCESS_INTER___H____

#include "include/config_info.h"

#define false 0
#define true 1

//#define HAS_DATA_CHECK        1               //数据校验开启的宏

u32 BuildFileSystem(const char *config_file, u8 *SplitBuf, u32 SplitLen, bool bCutSplit);


// inter function
u32 BuildFileSystemOnly(CFG_FSINFO *cfg_fsinfo,
                        FSYS_TOOL_IF *fstool_if,
                        PARTITION_IF *ram_partition_if,
                        PARTITION_IF *file_partition_if,
                        u8 *SplitBuf, u32 SplitLen, bool bCutSplit);

u32 BuildFS(FSYS_TOOL_IF *fstool_if, u8 *SplitBuf, u32 SplitLen, bool bCutSplit, FS_INFO *fs);

u32 __CopyMagicToFS(FSYS_TOOL_IF *fstool_if, u8 *SplitBuf, u32 SplitLen, char disk);

u32 __CopyRootToFS(FSYS_TOOL_IF *fstool_if, char disk, char *root, char *pcdir);

u32 __MakeDirToFS(FSYS_TOOL_IF *fstool_if, char disk, char *root, char *pcdir);

u32 __CopyFileToFS(FSYS_TOOL_IF *fstool_if, char disk, char *root, char *pcfile);

u32 TryRemoveFile(FSYS_TOOL_IF *fstool_if, char *fs_file);

void *RAMPartConstruct(FS_INFO *fsinfo,
                       FSYS_TOOL_IF *fstool_if,
                       PARTITION_IF *partition_if,
                       __fsys_part_t *rampart);

u32 RAMPartDestruct(FS_INFO *fsinfo,
                    FSYS_TOOL_IF *fstool_if,
                    PARTITION_IF *partition_if,
                    void *BaseAddress,
                    __fsys_part_t *rampart);

void *FilePartConstruct(FS_INFO *fsinfo,
                        FSYS_TOOL_IF *fstool_if,
                        PARTITION_IF *partition_if,
                        __fsys_part_t *rampart);

u32 FilePartDestruct(FS_INFO *fsinfo,
                     FSYS_TOOL_IF *fstool_if,
                     PARTITION_IF *partition_if,
                     __fsys_part_t *rampart);

u32 FsysConstruct(FS_INFO *fsinfo, FSYS_TOOL_IF *fstool_if, __fsys_part_t *rampart);

u32 FsysDestruct(FS_INFO *fsinfo, FSYS_TOOL_IF *fstool_if, __fsys_part_t *rampart);

//u32 __CutSplitData(const char *filepath, u8 * SplitBuf, u32 SplitLen, bool bCutSplit);

u32 SaveRAMDataToFile(const char *filename, void *buffer, u32 Len, u8 *SplitBuf, u32 SplitLen, bool bCutSplit);

#ifdef HAS_DATA_CHECK

u32 AddSum(u8 *buffer, u32 buflen, u32 init_sum);

u32 PCFileAddSum(char *pc_file, u32 length);

u32 FSFileAddSum(FSYS_TOOL_IF *fstool_if, char *fs_file, u32 length);

#endif //HAS_DATA_CHECK

#endif //__FSPROCESS_INTER___H____
