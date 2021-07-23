/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_tool_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: Mini rom image file system tool internal header file.
* Update  : date                auther      ver     notes
*           2011-3-24 22:38:14  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __MINFS_TOOL_I_H__
#define __MINFS_TOOL_I_H__

//win32 header files
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <config/config.h>
//minfs internal header files
#include "minfs_types.h"
#include "eelf.h"
#include "elf_parser.h"
#include "minfs.h"
#include "minfs_config.h"
#include "minfs_calc.h"
#include "minfs_compress.h"
#include "minfs_make.h"
#include "minfs_module.h"
#include "minfs_dump.h"

//__bool MINFS_ValidMinfsDir(WIN32_FIND_DATA *finddata);
//__bool MINFS_ValidMinfsFile(WIN32_FIND_DATA *finddata);
__s32  MINFS_Create(const char *pDir, const char *pFile, const char *pConfig);

#define MINFS_MAKE_DEBUG

#endif  //__MINFS_TOOL_I_H__
