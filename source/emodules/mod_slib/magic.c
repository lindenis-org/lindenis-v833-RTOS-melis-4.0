/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              module magic segment
*
*                               (c) Copyright 2010-2012, Sunny China
*                                           All Rights Reserved
*
* File    : magic.c
* By      : Sunny
* Version : V1.00
* Date    : 2011-4-30 14:31:49
*********************************************************************************************************
*/

#include "mod_slib.h"

const __module_mgsec_t __attribute__((section(".magic"))) FlashModInfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_MOD_SLIB,                     //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &SLIB_MInit,
        &SLIB_MExit,
        &SLIB_MOpen,
        &SLIB_MClose,
        &SLIB_MRead,
        &SLIB_MWrite,
        &SLIB_MIoctrl
    }
};

unsigned long __stack_chk_guard;
void __stack_chk_fail(void)
{
    while(1);
}
