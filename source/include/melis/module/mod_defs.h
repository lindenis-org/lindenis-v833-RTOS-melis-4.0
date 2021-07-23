/*
 * =====================================================================================
 *
 *       Filename:  mod_defs.h
 *
 *    Description:  define module structures.
 *
 *        Version:  Melis3.0
 *         Create:  2018-01-13 14:33:53
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-01-14 11:08:36
 *
 * =====================================================================================
 */
#ifndef _MODS_H_
#define _MODS_H_
#include <typedef.h>

#define EMOD_MAGIC0     'e'
#define EMOD_MAGIC1     'P'
#define EMOD_MAGIC2     'D'
#define EMOD_MAGIC3     'K'
#define EMOD_MAGIC4     '.'
#define EMOD_MAGIC5     'm'
#define EMOD_MAGIC6     'o'
#define EMOD_MAGIC7     'd'

/* base drivers                                     */
#define EMOD_TYPE_DRV_NOP               0x00        /* 0ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ÏµÍ³ï¿½ï¿½ï¿½ï¿½    */
#define EMOD_TYPE_DRV_RTC               0x01        /* rtcï¿½ï¿½ï¿½ï¿½typeï¿½ï¿½ï¿?      */
#define EMOD_TYPE_DRV_KEY               0x02        /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½typeï¿½ï¿½ï¿?     */
#define EMOD_TYPE_DRV_DISP              0x03        /* ï¿½ï¿½Ê¾ï¿½ï¿½ï¿½ï¿½typeï¿½ï¿½ï¿?     */
#define EMOD_TYPE_DRV_MOUSE             0x04        /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½typeï¿½ï¿½ï¿?     */
#define EMOD_TYPE_DRV_TP                0x05        /* tpï¿½ï¿½ï¿½ï¿½typeï¿½ï¿½ï¿?       */
#define EMOD_TYPE_DRV_AUDIO             0x06        /* Audioï¿½ï¿½ï¿½ï¿½typeï¿½ï¿½ï¿?    */
#define EMOD_TYPE_DRV_FM                0x07        /* FM radio driverï¿½ï¿½ï¿?  */
#define EMOD_TYPE_DRV_MP                0x08        /* MP driverï¿½ï¿½ï¿?        */
#define EMOD_TYPE_DRV_CE                0x09        /* CE driverï¿½ï¿½ï¿?        */
#define EMOD_TYPE_DRV_MATRIXKEY         0x0a        /* matrix key driver    */
#define EMOD_TYPE_DRV_DI                0x0b        /* de-interlace driverï¿½ï¿½ï¿?        */
#define EMOD_TYPE_DRV_BT                0x0c        /* bluetooth driverï¿½ï¿½ï¿?  */
#define EMOD_TYPE_DRV_PWM               0x0d        /* pwm driverï¿½ï¿½ï¿?  */
#define EMOD_TYPE_DRV_TR                0x0e        /* rotateï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?  */
#define EMOD_TYPE_DRV_RK                0x0f        /* rotateï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?  */


/* other drivers                                    */
#define EMOD_TYPE_DRV_FILEDISK          0x11
#define EMOD_TYPE_DRV_MONITOR           0x12        /* monitor              */
#define EMOD_TYPE_DRV_POWER             0x13
#define EMOD_TYPE_DRV_CSI               0x14
#define EMOD_TYPE_DRV_STREAM            0x15
#define EMOD_TYPE_DRV_TVD               0x16

#define EMOD_TYPE_MOD_INIT              0x17        /* desktop initial module*/
#define EMOD_TYPE_MOD_LEMON             0x18
#define EMOD_TYPE_MOD_ORANGE            0x18
/* 0x18 and 0x19/0x1a/0x1b/0x1c/0x1d is used by lemon */
#define EMOD_TYPE_MOD_ORCHID            0x1e
#define EMOD_TYPE_MOD_DESKTOP           0x1f

/* bus drivers(except usb, net)                     */
#define EMOD_TYPE_DRV_BUS_BASE          0x20
#define EMOD_TYPE_DRV_UART              (EMOD_TYPE_DRV_BUS_BASE  + 0)
#define EMOD_TYPE_DRV_IIS               (EMOD_TYPE_DRV_BUS_BASE  + 1)
#define EMOD_TYPE_DRV_SPI               (EMOD_TYPE_DRV_BUS_BASE  + 2)
#define EMOD_TYPE_DRV_IIC               (EMOD_TYPE_DRV_BUS_BASE  + 3)
#define EMOD_TYPE_DRV_IR                (EMOD_TYPE_DRV_BUS_BASE  + 4)
#define EMOD_TYPE_DRV_HDMI              (EMOD_TYPE_DRV_BUS_BASE  + 5)
#define EMOD_TYPE_DRV_UPRL              (EMOD_TYPE_DRV_BUS_BASE  + 6)
#define EMOD_TYPE_MOD_UPDATE            (EMOD_TYPE_DRV_BUS_BASE  + 7)
#define EMOD_TYPE_DRV_SMC               (EMOD_TYPE_DRV_BUS_BASE  + 8)

/* memory drivers(0~7)                             */
#define EMOD_TYPE_DRV_MEM_BASE          0x30
#define EMOD_TYPE_DRV_RAMDISK           (EMOD_TYPE_DRV_MEM_BASE  + 0)
#define EMOD_TYPE_DRV_NAND              (EMOD_TYPE_DRV_MEM_BASE  + 1)
#define EMOD_TYPE_DRV_SDMMC             (EMOD_TYPE_DRV_MEM_BASE  + 2)
#define EMOD_TYPE_DRV_NOR               (EMOD_TYPE_DRV_MEM_BASE  + 3)
#define EMOD_TYPE_DRV_HDD               (EMOD_TYPE_DRV_MEM_BASE  + 4)
#define EMOD_TYPE_DRV_MS                (EMOD_TYPE_DRV_MEM_BASE  + 5)
#define EMOD_TYPE_DRV_SDMMC2            (EMOD_TYPE_DRV_MEM_BASE  + 6)
/* part driver (0~7)                               */
#define EMOD_TYPE_DRV_PART_BASE         0x38
#define EMOD_TYPE_DRV_DOSPART           (EMOD_TYPE_DRV_PART_BASE + 0)
#define EMOD_TYPE_DRV_MACPART           (EMOD_TYPE_DRV_PART_BASE + 1)

/* file system driver (0~15)                        */
#define EMOD_TYPE_DRV_VFS_BASE          0x40
#define EMOD_TYPE_DRV_FATFS             (EMOD_TYPE_DRV_VFS_BASE + 0)
#define EMOD_TYPE_DRV_NTFS              (EMOD_TYPE_DRV_VFS_BASE + 1)
#define EMOD_TYPE_MOD_DBMS              (EMOD_TYPE_DRV_VFS_BASE + 2)
#define EMOD_TYPE_MOD_EXT               (EMOD_TYPE_DRV_VFS_BASE + 3)
#define EMOD_TYPE_MOD_CHARSET           (EMOD_TYPE_DRV_VFS_BASE + 4)
#define EMOD_TYPE_MOD_VIDEO_PLAY        (EMOD_TYPE_DRV_VFS_BASE + 5)
#define EMOD_TYPE_MOD_MAPLE             (EMOD_TYPE_DRV_VFS_BASE + 6)
#define EMOD_TYPE_MOD_TSDEMUX           (EMOD_TYPE_DRV_VFS_BASE + 7)
#define EMOD_TYPE_MOD_CMMBDEMUX         (EMOD_TYPE_DRV_VFS_BASE + 8)
#define EMOD_TYPE_MOD_DVBC              (EMOD_TYPE_DRV_VFS_BASE + 9)
#define EMOD_TYPE_MOD_JPEG_DEC          (EMOD_TYPE_DRV_VFS_BASE + 0xa)
#define EMOD_TYPE_MOD_GINGKO            (EMOD_TYPE_DRV_VFS_BASE + 0xb)
#define EMOD_TYPE_SPLY                  (EMOD_TYPE_DRV_VFS_BASE + 0xc)
#define EMOD_TYPE_MOD_REC               (EMOD_TYPE_DRV_VFS_BASE + 0xd)
#define EMOD_TYPE_MOD_MUXER             (EMOD_TYPE_DRV_VFS_BASE + 0xe)
#define EMDO_TYPE_AENC_COM              (EMOD_TYPE_DRV_VFS_BASE + 0xf)

/* multi media driver (0~15)                        */
#define EMOD_TYPE_DRV_MM_BASE           0x50
#define EMOD_TYPE_DRV_AVSYNC            (EMOD_TYPE_DRV_MM_BASE   + 0)
#define EMOD_TYPE_DRV_VDEC              (EMOD_TYPE_DRV_MM_BASE   + 1)
#define EMOD_TYPE_DRV_ADEC              (EMOD_TYPE_DRV_MM_BASE   + 2)
#define EMOD_TYPE_DRV_AENC              (EMOD_TYPE_DRV_MM_BASE   + 3)
#define EMOD_TYPE_DRV_CMMBRX            (EMOD_TYPE_DRV_MM_BASE   + 4)
#define EMOD_TYPE_DRV_TSC               (EMOD_TYPE_DRV_MM_BASE   + 4)   //TSC_DRV_INSTALL_TYPE,ï¿½ï¿½cmmbrxï¿½ï¿½ï¿½ï¿½
#define EMOD_TYPE_PSR_VIDEO             (EMOD_TYPE_DRV_MM_BASE   + 4)   //PSR and TSC share.
#define EMOD_TYPE_PSR_AUDIO             (EMOD_TYPE_DRV_MM_BASE   + 4)   //Share with PSR Video
#define EMOD_TYPE_DRV_VENC              (EMOD_TYPE_DRV_MM_BASE   + 5)
#define EMOD_TYPE_DRV_DEMOD             (EMOD_TYPE_DRV_MM_BASE   + 5)   //DEMOD_DRV_INSTALL_TYPE,ï¿½ï¿½ï¿½ï¿½Ç°ï¿½ï¿½demodulatorï¿½ï¿½ï¿½Ð¾Æ¬ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?
#define EMOD_TYPE_DRV_PSHOW             (EMOD_TYPE_DRV_MM_BASE   + 7)
#define EMOD_TYPE_DRV_PICDEC            (EMOD_TYPE_DRV_MM_BASE   + 8)
#define EMOD_TYPE_DRV_CA                (EMOD_TYPE_DRV_MM_BASE   + 9)
#define EMOD_TYPE_DRV_LDEC_LRC          (EMOD_TYPE_DRV_MM_BASE   + 0xa)
#define EMOD_TYPE_DRV_LDEC_ITXT         (EMOD_TYPE_DRV_MM_BASE   + 0xb)
#define EMDO_TYPE_ADEC_COM              (EMOD_TYPE_DRV_MM_BASE   + 0xc)
#define EMOD_TYPE_VDEC_COM              (EMOD_TYPE_DRV_MM_BASE   + 0xd)
#define EMOD_TYPE_APLY                  (EMOD_TYPE_DRV_MM_BASE   + 0xe)
#define EMOD_TYPE_VPLY                  (EMOD_TYPE_DRV_MM_BASE   + 0xf)

/* usb driver (0~15)                                */
#define EMOD_TYPE_DRV_USB_BASE          0x60
#define EMOD_TYPE_DRV_USBD_BASE         (EMOD_TYPE_DRV_USB_BASE + 0)
#define EMOD_TYPE_DRV_USBH_BASE         (EMOD_TYPE_DRV_USB_BASE + 4)
#define EMOD_TYPE_DRV_USBH1_BASE        (EMOD_TYPE_DRV_USB_BASE + 5)
#define EMOD_TYPE_DRV_USBH2_BASE        (EMOD_TYPE_DRV_USB_BASE + 6)


/* net protocal driver (0~15)                       */
#define EMOD_TYPE_DRV_NET_BASE          0x70

#define EMOD_TYPE_DRV_SAMPLE            0x75

/* user case modules */
#define EMOD_TYPE_MOD_CASE_BASE         0x80
//#define EMOD_TYPE_MOD_UPDATE            (EMOD_TYPE_MOD_CASE_BASE + 0)
/* 0x80/0x81/0x82/0x83 used by bookengine */
#define EMOD_TYPE_MOD_TCLENGINE         (EMOD_TYPE_MOD_CASE_BASE + 5)   /* TCL engine                               */
/* flash module */
#define EMOD_TYPE_MOD_FLASH             (EMOD_TYPE_MOD_CASE_BASE + 6)
#define EMOD_TYPE_MOD_SLIB              (EMOD_TYPE_MOD_CASE_BASE + 7)
#define EMOD_TYPE_MOD_BOOKENGINE        (EMOD_TYPE_MOD_CASE_BASE + 0x10)

#define EMOD_TYPE_EXTERNAL_MODS         0x90
/* reserved driver, can't been used by anyone   */
#define EMOD_TYPE_APPS                  (EMOD_TYPE_EXTERNAL_MODS + 2)
#define EMOD_TYPE_TOP_CEDAR             (EMOD_TYPE_EXTERNAL_MODS + 4)
#define EMOD_TYPE_WILLOW                (EMOD_TYPE_EXTERNAL_MODS + 5)
#define EMOD_TYPE_AUD_SW                (EMOD_TYPE_EXTERNAL_MODS + 6)
#define EMOD_TYPE_PICPSR                (EMOD_TYPE_EXTERNAL_MODS + 7)
#define EMOD_TYPE_MOD_DEC_3D            (EMOD_TYPE_EXTERNAL_MODS + 8)
#define EMOD_TYPE_MOD_PSR_3D            (EMOD_TYPE_EXTERNAL_MODS + 9)


/* reserved driver, can't been used by anyone   */
#define EMOD_TYPE_DRV_RESERVED          0x9e
#define EMOD_TYPE_USER                  0xff

#define EPOS_proc_segsize               (0 * 1024 * 1024)      // process 内存中地址段大小
#define EPOS_umod_segsize               (32 * 1024 * 1024)      // user module 内存中地址段大小
#define EPOS_smod_segsize               (1024 * 1024)           // sys module 内存中地址段大小

#define EPOS_pid_min                    1                       // 最小pid
#define EPOS_pid_max                    63                      // 最多可以有64个进程(pid = 0 ~ 63)占用2G的空间(32M*64)
#define EPOS_umid_min                   64                      //
#define EPOS_umid_max                   95                      // 最多可以有32个用户模块(mid = 64 ~ 95)占用1G的空间(32M*32))
#define EPOS_smid_min                   96                      // 最多可以有128个系统模块(mid = 96 ~ 223)
#define EPOS_smid_max                   0xfc                    // 最多可以有127个系统模块(mid = 96 ~ 223)

#define EPOS_rsvid                      0xfd                    // 系统保留id
#define EPOS_sysid                      0xfe                    // 系统内核id
#define EPOS_curid                      0xff                    // 当前id

#define EPOS_id_mumber                  255
#define EPOS_umod_startaddr             0x80000000
#define EPOS_smod_startaddr             0xe0000000

//************************************************
typedef struct __MP         //mpçš„å…·ä½“ç»“æž„ç”±æ¨¡å—è‡ªå·±è‡ªè¡Œå®šä¹‰ï¼Œå¤§å°ä¸å®šï¼Œä½†å¼ºåˆ¶ç¬¬ä¸?ä¸ªbyteå­˜æ”¾mid
{
    //midå°†ç”¨äºŽè°ƒç”¨å…·ä½“çš„moduleå‡½æ•°æ—¶fsfcç”?
    __u8            mid;
    __u8            userdef;
} __mp;

typedef struct __MIF
{
    __s32(*MInit)(void);                                                                  //在install/reset模块时使用,若模块需要后台任务,可以在此启动
    __s32(*MExit)(void);                                                                  //在uninstall模块时使用,若模块有后台任务,可以在此处关闭
    __mp          *(*MOpen)(__u32 id, __u32 mode);                                        //打开一个模块句柄
    __s32(*MClose)(__mp *mp);                                                             //关闭一个模块句柄
    __u32(*MRead)(void *pdata, __u32 size, __u32 n, __mp *mp);                            //模块读
    __u32(*MWrite)(const void *pdata, __u32 size, __u32 n, __mp *mp);                     //模块写
    __s32(*MIoctrl)(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);                       //模块控制
} __mif_t;

typedef struct __MODS_MGSEC
{
    char            magic[8];           // "ePDK.mod"
    __u32           version;
    __u8            type;               // 0xff: user module(midware)
    __u32           heapaddr;           //heap在32M空间内的位置，通常在最后1M(31*1024*1024)
    //若地址等于0，那么表示进程不需要局部堆，将共用系统堆
    __u32           heapsize;           //heap的初始大小
    __mif_t         mif;                //pcb里第一个任务的首地址
} __module_mgsec_t;

__u32 esMODS_MInstall(const char *mfile, __u8 mode);
__u32 esMODS_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 esMODS_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 esMODS_MClose(__mp *mp);
__s32 esMODS_MUninstall(__u8 id);
__s32 esMODS_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);
__mp *esMODS_MOpen(__u8 id, __u8 mode);

__s32 MInit(void);
__s32 MExit(void);
__mp *MOpen(__u32 mid, __u32 mod);
__s32 MClose(__mp *mp);
__u32 MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#define __getc()                  esSIOS_getchar()
#define __putc(c)                 esSIOS_putarg(c, 'c')

#endif //_MODS_H_
