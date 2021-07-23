/* for common defines ever simulte or not */
#ifndef __FSYS_LIBS_H__
#define __FSYS_LIBS_H__

#define  simulate   1

#define  _FS_DEBUG_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ctype.h"
#include "fsys_types.h"

#undef  ffs
#define ffs         fsyslib_ffs


#define strdup(s) \
    ({  \
      size_t __len__;   \
      void * __new__;   \
                                        \
      __len__ = strlen (s) + 1;         \
      __new__ = malloc (__len__);       \
      if (__new__) memcpy (__new__, s, __len__);        \
                    \
      __new__;      \
    })

int fsyslib_ffs(int i);

#define     DRAM_BASE       1
#define     DRAM_SIZE       1

/* Global error codes */
#define FSYS_ERR_OK                     (__s16)0x0000
#define FSYS_ERR_EOF                    (__s16)0xfff0
#define FSYS_ERR_DISKFULL               (__s16)0xffe0
#define FSYS_ERR_INVALIDPAR             (__s16)0xffd0
#define FSYS_ERR_WRITEONLY              (__s16)0xffc0
#define FSYS_ERR_READONLY               (__s16)0xffb0
#define FSYS_ERR_READERROR              (__s16)0xffa0
#define FSYS_ERR_WRITEERROR             (__s16)0xff90
#define FSYS_ERR_DISKCHANGED            (__s16)0xff80
#define FSYS_ERR_CLOSE                  (__s16)0xff70

#define EPOS_sysid                      0xfe       // 系统内核id
#define EPOS_curid                      0xff       // 当前id

#define KRNL_priolevel0         0                   /* reserved for system(highest level)  */
#define KRNL_priolevel1         1
#define KRNL_priolevel2         2
#define KRNL_priolevel3         3
#define KRNL_priolevel4         4
#define KRNL_priolevel5         5
#define KRNL_priolevel6         6
#define KRNL_priolevel7         7                   /* reserved for system(lowest level)                            */

#define  OS_PRIO_SELF           0xFFu               /* Indicate SELF priority                                       */
#define  OS_TASK_DEL_REQ        62u

__u32 CPU_SR_Save(void);
void  CPU_SR_Restore(__u32 x);
__krnl_event_t  *esKRNL_QCreate(__u16 size);
__u8 esKRNL_TCreate(void (*thread)(void *p_arg),
                    void *p_arg, __u32 stksize, __u16 id_priolevel);
void  esKRNL_TimeDly(__u16 ticks);
void  *esKRNL_QPend(__krnl_event_t *pevent, __u16 timeout, __u8 *err);
__u32 esKRNL_Time(void);
__s8 esKRNL_TDel(__u8 prio);
__s8 esKRNL_TDelReq(__u8 prio);
__krnl_event_t  *esKRNL_SemDel(__krnl_event_t *pevent, __u8 opt, __u8 *err);
void  esKRNL_SemPend(__krnl_event_t *pevent, __u16 timeout, __u8 *err);
__u8  esKRNL_SemPost(__krnl_event_t *pevent);
__krnl_event_t  *esKRNL_SemCreate(__u16 cnt);
__u32 esKSRV_Random(__u32 max);

#undef  __inf
#define __inf                   printf
#undef  __wrn
#define __wrn                   printf
#undef  __err
#define __err                   printf
#undef  printk
#define printk                  printf
#undef  time
#define time(t)                 (t)
#undef  srandom
#define srandom(s)              (s)
#undef  random
#define random(x)               (0)
#define palloc(size)            malloc(size)
#define pfree(addr, size)       free(addr)
#define eLIBs_strchr            strchr
#define eLIBs_strlen            strlen
#define eLIBs_strncmp           strncmp
#define eLIBs_strcmp            strcmp
#define eLIBs_atoi              atoi
#define eLIBs_toupper(x)        toupper(x)
#define eLIBs_strcat            strcat
#define eLIBs_memset            memset
#define eLIBs_memcpy            memcpy
#define eLIBs_strncpy           strncpy
#define eLIBs_strcpy            strcpy
#define eLIBs_memcmp            memcmp
#define esMEMS_Calloc(x,y,z)    calloc(y,z)
#define esMEMS_Malloc(x,y)      malloc(y)
#define esMEMS_Mfree(x,y)       free(y)
#define esMEMS_Palloc(size, mode)   malloc((size)<<10)
#define esMEMS_Pfree(addr, size)    free(addr)
#define esMEMS_Balloc(size)         malloc(size)
#define esMEMS_Bfree(addr, size)    free(addr)
#define ENTER_CRITICAL(cpu_sr) {cpu_sr = CPU_SR_Save();}
#define EXIT_CRITICAL(cpu_sr)  {CPU_SR_Restore(cpu_sr);}

extern void eLIBs_int2str_dec(int input , char *str);
extern void eLIBs_uint2str_dec(unsigned int input, char *str);
extern void eLIBs_int2str_hex(int input, char *str, int hex_flag);

#endif      /* __FSYS_LIBS_H__ */

