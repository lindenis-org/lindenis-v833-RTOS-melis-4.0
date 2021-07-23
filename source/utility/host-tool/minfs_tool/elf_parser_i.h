/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                ELF Filter Tool
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : ELFParser_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-13
* Descript: ELF format file parser.
* Update  : date                auther      ver     notes
*           2011-3-13 15:00:54  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __ELF_PARSER_I_H__
#define __ELF_PARSER_I_H__

#include "minfs_tool_i.h"

typedef struct ELF_PARSER
{
    __u8        *buffer;
    __u32        fileLen;
    Elf32_Ehdr  *pHdr;
    Elf32_Shdr **pSHdr;
} elf_parser_t;

#endif  /* __ELF_PARSER_I_H__ */
