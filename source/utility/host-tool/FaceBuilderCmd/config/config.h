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


#include "config_plug_type.h"

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#ifdef PLUGINAPI

#else
#define PLUGINAPI extern "C" __declspec(dllimport)
#endif //

//------------------------------------------------------------------------------------------------------------
#define PLUGIN_TYPE             CONFIG_PLUGIN_TYPE  //
#define PLUGIN_NAME             "config"            //scott note
#define PLUGIN_VERSION          0x0100
#define PLUGIN_AUTHOR           "scottyu"
#define PLUGIN_COPYRIGHT        "scottyu"


//------------------------------------------------------------------------------------------------------------
//插件的通用接口
//------------------------------------------------------------------------------------------------------------
//#include "..\if\plugin_common.h"


//------------------------------------------------------------------------------------------------------------
typedef void *HCONFIG;
typedef void *POS;

//------------------------------------------------------------------------------------------------------------
void *GetConfigInterface();
HCONFIG OpenConfig(char *szFilePath, void *Param);

u32    CloseConfig(HCONFIG hcfg);

//------------------------------------------------------------------------------------------------------------

u32    ConfigDump(HCONFIG hcfg);

//------------------------------------------------------------------------------------------------------------

u32    GetSectionCount(HCONFIG hcfg, u32 *pSectionCount);

POS    GetFirstSection(HCONFIG hcfg);

POS    GetNextSection(HCONFIG hcfg, POS pos, s8 *szSection);

//------------------------------------------------------------------------------------------------------------

u32    GetSectionKeyCount(HCONFIG hcfg, s8 *szSection, u32 *pKeyCount);

POS    GetSectionFirstKey(HCONFIG hcfg, s8 *szSection);

POS    GetSectionNextKey(HCONFIG hcfg, s8 *szSection, POS pos, s8 *szKey);

//------------------------------------------------------------------------------------------------------------

u32    SetKeyValue(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 Value, u32 bCreate);

u32    SetKeyString(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String, u32 bCreate);

//------------------------------------------------------------------------------------------------------------

u32    GetKeyValue(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 *pValue);

u32    GetKeyString(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String);

//------------------------------------------------------------------------------------------------------------

u32    CreateSection(HCONFIG hcfg, s8 *szSection);

u32    DeleteSection(HCONFIG hcfg, s8 *szSection);

u32    DeleteKey(HCONFIG hcfg, s8 *szSection, s8 *szKey);

//------------------------------------------------------------------------------------------------------------


#endif //___CONFIG__H____


