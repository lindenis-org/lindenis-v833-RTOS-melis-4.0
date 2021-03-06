/*
 * =====================================================================================
 *
 *       Filename:  checksum.h
 *
 *    Description:  header file of checksum.c
 *
 *        Version:  2.0
 *         Create:  2017-11-10 11:08:14
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2017-11-10 11:08:43
 *
 * =====================================================================================
 */

#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <stdint.h>

#define BOOT0_MAGIC                     "eGON.BT0"
#define BOOT1_MAGIC                     "eGON.BT1"
#define STAMP_VALUE                     0x5F0A6C39

#define ALIGN(size, align)              ((size + align - 1) & (~(align - 1)))

typedef struct
{
    uint32_t  jump_instruction;   // one intruction jumping to real code
    uint8_t   magic[8];           // ="eGON.BT0" or "eGON.BT1",  not C-style string.
    uint32_t  check_sum;          // generated by PC
    uint32_t  length;             // generated by PC
    uint32_t  pub_head_size;      // the size of boot_file_head_t
    uint8_t   pub_head_vsn[4];    // the version of boot_file_head_t
    uint8_t   file_head_vsn[4];   // the version of boot0_file_head_t or boot1_file_head_t
    uint8_t   Boot_vsn[4];        // Boot version
    uint8_t   eGON_vsn[4];        // eGON version
    uint8_t   platform[8];        // platform information
} boot_file_head_t;

#endif  /*CHECKSUM_H*/

