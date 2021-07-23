/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_module.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: melis module file handing exteral header file.
* Update  : date                auther      ver     notes
*           2011-3-24 22:09:22  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __MINFS_MODULE_H__
#define __MINFS_MODULE_H__

//section name of melis module magic
#define MINFS_MODULE_MAGIC          "MAGIC"

//the min byte size of compressed section
#define MINFS_SECTION_COMRPESS_MIN  (1024)

__bool  MINFS_IsCompressedSection(Elf32_Shdr *pSHdr, const char *SectionName);
__bool  MINFS_IsModuleMagicSection(const char *SectionName);
__bool  MINFS_ValidModuleSection(const char *SectionName);
__bool  MINFS_ValidModuleFile(const char *pFile);
__s32 MINFS_CalcMFSModuleFile(const char *pFullPath,
                              __u32 *ExtentLen,
                              __u32 *FDataLen,
                              __minfs_config_t *pConfig);
__s32   MINFS_LoadModuleFile(const char *pFullPath,
                             __minfs_dentry_t *pDEntry,
                             __minfs_context_t *pContext,
                             __minfs_config_t *pConfig);

#endif  /* __MINFS_MODULE_H__ */
