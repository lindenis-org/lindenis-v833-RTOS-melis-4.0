/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Module Loader
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : module.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-30
* Descript: module loader internal header file.
* Update  : date                auther      ver     notes
*           2011-3-30 16:14:15  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef __MODS_I_H__
#define __MODS_I_H__
#include <typedef.h>
#include <module/mod_defs.h>
#include <common/romldr.h>
#include "loader.h"
#include "exec.h"

typedef struct __MODS_MCB               //进程控制块
{
    __krnl_xcb_t    xcb;
    __u8            type;               //系统模块或用户模块
    __mif_t         mif;                //模块入口，模块的标准函数入口地址存放在此处
    __u32           vmbitmap;           //模块虚拟内存空间占用情况，用于卸载模块时的虚拟内存时间释放
} __module_mcb_t;

#endif //__MODS_I_H__

