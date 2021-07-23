//------------------------------------------------------------------------------------------------------------
//
// fsprocess.h
//
// scott
//
// 2009-12-24 9:46:20
//
//------------------------------------------------------------------------------------------------------------
#ifndef ___FSPROCESS__H____
#define ___FSPROCESS__H____ 1

#include "if\scriptparser_plug_type.h"

#ifdef PLUGINAPI

#else
#define PLUGINAPI extern "C" __declspec(dllimport)
#endif

#define PLUGIN_TYPE             FSPROCESS_PLUGIN_TYPE
#define PLUGIN_NAME             "fsprocess"
#define PLUGIN_VERSION          0x0100
#define PLUGIN_AUTHOR           "scottyu"
#define PLUGIN_COPYRIGHT        "scottyu"

//------------------------------------------------------------------------------------------------------------
//插件的通用接口
//------------------------------------------------------------------------------------------------------------
#include "if\plugin_common.h"

#endif //___FSPROCESS__H____
