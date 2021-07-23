//------------------------------------------------------------------------------------------------------------
//
//2008-10-16 8:48:43
//
//scott
//
//config_if.h
//
//------------------------------------------------------------------------------------------------------------


#ifndef ___CONFIG_IF____H____
#define ___CONFIG_IF____H____   1


#include "config_plug_type.h"

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#define MAX_ALLSECTIONS (260 * 1000)    //全部的段名
#define MAX_SECTION     260             //一个段名长度
#define MAX_ALLKEYS     (260 * 2000)    //全部的键名
#define MAX_KEY         260             //一个键名长度
#define MAX_VALUE       260

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef void *HCONFIG;
typedef void *POS;


typedef HCONFIG(*pOpenConfig)(const char *szFilePath, void *Param);
typedef u32(*pCloseConfig)(HCONFIG hcfg);
typedef u32(*pConfigDump)(HCONFIG hcfg);
typedef u32(*pGetSectionCount)(HCONFIG hcfg, u32 *pSectionCount);
typedef POS(*pGetFirstSection)(HCONFIG hcfg);
typedef POS(*pGetNextSection)(HCONFIG hcfg, POS pos, s8 *szSection);
typedef u32(*pGetSectionKeyCount)(HCONFIG hcfg, s8 *szSection, u32 *pKeyCount);
typedef POS(*pGetSectionFirstKey)(HCONFIG hcfg, s8 *szSection);
typedef POS(*pGetSectionNextKey)(HCONFIG hcfg, s8 *szSection, POS pos, s8 *szKey);
typedef u32(*pSetKeyValue)(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 Value, u32 bCreate);
typedef u32(*pSetKeyString)(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String, u32 bCreate);
typedef u32(*pGetKeyValue)(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 *pValue);
typedef u32(*pGetKeyString)(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String);
typedef u32(*pCreateSection)(HCONFIG hcfg, s8 *szSection);
typedef u32(*pDeleteSection)(HCONFIG hcfg, s8 *szSection);
typedef u32(*pDeleteKey)(HCONFIG hcfg, s8 *szSection, s8 *szKey);

typedef struct tag_CONFIG_IF
{
    pOpenConfig          OpenConfig         ;
    pCloseConfig         CloseConfig        ;
    pConfigDump          ConfigDump         ;
    pGetSectionCount     GetSectionCount    ;
    pGetFirstSection     GetFirstSection    ;
    pGetNextSection      GetNextSection     ;
    pGetSectionKeyCount  GetSectionKeyCount ;
    pGetSectionFirstKey  GetSectionFirstKey ;
    pGetSectionNextKey   GetSectionNextKey  ;
    pSetKeyValue         SetKeyValue        ;
    pSetKeyString        SetKeyString       ;
    pGetKeyValue         GetKeyValue        ;
    pGetKeyString        GetKeyString       ;
    pCreateSection       CreateSection      ;
    pDeleteSection       DeleteSection      ;
    pDeleteKey           DeleteKey          ;
} __attribute__((packed)) CONFIG_IF;


#endif //___CONFIG_IF____H____











