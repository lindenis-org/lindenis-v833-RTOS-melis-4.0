//------------------------------------------------------------------------------------------------------------
//
//2008-11-01 15:18:04
//
//scott
//
//SingleProcess_if.h
//
//------------------------------------------------------------------------------------------------------------


#ifndef __SINGLEPROCESS_IF___H____
#define __SINGLEPROCESS_IF___H____  1


#include "SingleProcess_plug_type.h"



//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef void *HSINGLE;

typedef HSINGLE(*pSingleProcessOpen)(char *strAppName, char *strID);

typedef u32(*pSingleProcessClose)(HSINGLE hSingle);

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_SINGLEPROCESS_IF
{
    pSingleProcessOpen      SingleProcessOpen;
    pSingleProcessClose     SingleProcessClose;
} SINGLEPROCESS_IF;
#pragma pack(pop)


#endif //__SINGLEPROCESS_IF___H____






