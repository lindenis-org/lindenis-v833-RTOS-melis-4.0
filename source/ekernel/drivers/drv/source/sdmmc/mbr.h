/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           SDMMC Module
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : mbr.h
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-11-28
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-11-28      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#ifndef __MBR_H__
#define __MBR_H__

#include <module/mod_defs.h>

#define MAX_PART_COUNT                  8                                       // max part count
#define MBR_COPY_NUM                    4                                       // mbr backup count

#define MBR_START_ADDRESS               0x0                                     // mbr start address
#define MBR_SIZE                        1024                                    // mbr size
#define MBR_RESERVED                    (MBR_SIZE - 20 - (MAX_PART_COUNT * 64)) // mbr reserved space

// part information
typedef struct tag_PARTITION
{
    __u32 addrhi;                       // start address high 32 bit
    __u32 addrlo;                       // start address low 32 bit
    __u32 lenhi;                        // size high 32 bit
    __u32 lenlo;                        // size low 32 bit
    __u8  classname[12];                // major device name
    __u8  name[12];                     // minor device name
    __u8  res[24];                      // reserved
} PARTITION;                            // 64 bytes 应该打上 pack

// mbr information
typedef struct tag_MBR
{
    __u32 crc32;                        // crc, from byte 4 to mbr tail
    __u32 version;                      // version
    __u8  magic[8];                     // magic number
    __u8  copy;                         // mbr backup count
    __u8  index;                        // current part no
    __u16 PartCount;                    // part counter
    PARTITION array[MAX_PART_COUNT];    // part info
    __u8  res[MBR_RESERVED];            // reserved space
} MBR;

#endif // __MBR_H__
