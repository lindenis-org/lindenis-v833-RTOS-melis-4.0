//------------------------------------------------------------------------------------------------------------
//
//2008-10-16 8:48:43
//
//scott
//
//config.h
//
//------------------------------------------------------------------------------------------------------------
#ifndef ___CONFIG__H____
#define ___CONFIG__H____    1

#ifdef __cplusplus
extern "C" {
#endif

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

#define OK         0

#define Msg         printf
#define Err         printf

//basetype
typedef signed char         s8;
typedef unsigned char       u8;
typedef short int           s16;
typedef unsigned short      u16;
typedef int                 s32;
typedef unsigned int        u32;


#define _MAX_PATH   260
#define MAX_PATH   260
#define _MAX_DRIVE  3
#define _MAX_DIR    256
#define _MAX_FNAME  256
#define _MAX_EXT    256

#define PLUGIN_NAME             "config"            //scott note
#define PLUGIN_VERSION          0x0100
#define PLUGIN_AUTHOR           "scottyu"
#define PLUGIN_COPYRIGHT        "scottyu"

#define MAX_ALLSECTIONS (260 * 1000)    //全部的段名
#define MAX_SECTION     260             //一个段名长度
#define MAX_ALLKEYS     (260 * 2000)    //全部的键名
#define MAX_KEY         260             //一个键名长度
#define MAX_VALUE       260

typedef void *HCONFIG;
typedef void *POS;



HCONFIG OpenConfig(const char *szFilePath, void *Param);

u32    CloseConfig(HCONFIG hcfg);


u32    ConfigDump(HCONFIG hcfg);


u32    GetSectionCount(HCONFIG hcfg, u32 *pSectionCount);

POS    GetFirstSection(HCONFIG hcfg);

POS    GetNextSection(HCONFIG hcfg, POS pos, s8 *szSection);


u32    GetSectionKeyCount(HCONFIG hcfg, s8 *szSection, u32 *pKeyCount);

POS    GetSectionFirstKey(HCONFIG hcfg, s8 *szSection);

POS    GetSectionNextKey(HCONFIG hcfg, s8 *szSection, POS pos, s8 *szKey);


u32    SetKeyValue(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 Value, u32 bCreate);

u32    SetKeyString(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String, u32 bCreate);


u32    GetKeyValue(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 *pValue);

u32    GetKeyString(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String);


u32    CreateSection(HCONFIG hcfg, s8 *szSection);

u32    DeleteSection(HCONFIG hcfg, s8 *szSection);

u32    DeleteKey(HCONFIG hcfg, s8 *szSection, s8 *szKey);

#ifdef __cplusplus
}
#endif

#endif //___CONFIG__H____
