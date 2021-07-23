/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                ELF Filter Tool
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : elf_parser.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-13
* Descript: ELF format file parser.
* Update  : date                auther      ver     notes
*           2011-3-13 15:00:54  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __ELF_PARSER_H__
#define __ELF_PARSER_H__

typedef void *HELFPSR;

typedef __elf32_head_t  Elf32_Ehdr;
typedef __elf32_shead_t Elf32_Shdr;

HELFPSR     *LoadELFFile(const char *filename);
__s32        UnLoadELFFile(HELFPSR hPsr);
__s32        GetELFFileSize(HELFPSR hPsr);
__u32        GetSectionsNumber(HELFPSR hPsr);
Elf32_Ehdr  *GetELFHeader(HELFPSR hPsr);
__u8        *GetSectionName(HELFPSR hPsr, __u32 index);
Elf32_Shdr  *GetSectionHeader(HELFPSR hPsr, __u32 index);
__u8        *GetSectionData(HELFPSR hPsr, __u32 index);
Elf32_Shdr  *GetStrSectionHeader(HELFPSR hPsr);

#endif  /* __ELF_PARSER_H__ */
