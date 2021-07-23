/*
**********************************************************************************************************************
*                                                   ePOS
*                               the Easy Portable/Player Operation System
*                                          file system sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : sys_fsys_i.h
* Version: V1.0
* By     : steven.zgj
**********************************************************************************************************************
*/

#ifndef _SYS_FSYS_I_H_
#define _SYS_FSYS_I_H_

#include "fsys_types.h"

#include "fsys_libs.h"

//#include "if\fstool_if.h"
#include "if/fstool_if.h"  //modify by gongxf

//#include "part.h"
#include "part.h"

#include "fs_i.h"

//#include "include\fsys_fs_i.h"
//#include "include\fsys_fsfat_i.h"
#include "include/fsys_fs_i.h"     //modify by gongxf
#include "include/fsys_fsfat_i.h"  //modify by gongxf

//#include "include\fsys_part_i.h"
#include "include/fsys_part_i.h"    //modify by gongxf

#endif  //#define _SYS_FSYS_I_H_
