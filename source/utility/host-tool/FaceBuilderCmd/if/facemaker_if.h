//------------------------------------------------------------------------------------------------------------
//
// facemaker_if.h
//
// 2009-5-21 14:37:04
//
// sunny
//
//------------------------------------------------------------------------------------------------------------


#ifndef __FACEMAKER_IF___H____
#define __FACEMAKER_IF___H____  1


#include "facemaker_plug_type.h"


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------

typedef  void *HFACE;

typedef HFACE(*pCreateFace)(char *resFile, char *resHeadFile);

typedef u32(*pSetSysInfo)(HFACE hFace, void *sysinfo, u32 size);

typedef u32(*pSetResCount)(HFACE hFace, u32 resCount);

typedef u32(*pSetResInfo)(HFACE hFace, void *resInfo, u32 size, u32 index);

typedef u32(*pBuildFace)(HFACE hFace, char *res_template, void *res_if, u32 isClone);

typedef u32(*pCloseFace)(HFACE hFace);

// u32 return OK means function operation success, otherwise means failed .



//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_FACE_IF
{

    pCreateFace   CreateFace ;      //

    pSetSysInfo   SetSysInfo ;      //

    pSetResCount  SetResCount;      //

    pSetResInfo   SetResInfo ;      //

    pBuildFace    BuildFace  ;      //

    pCloseFace    CloseFace  ;      //

} FACE_IF;
#pragma pack(pop)


#endif //__FACEMAKER_IF___H____




