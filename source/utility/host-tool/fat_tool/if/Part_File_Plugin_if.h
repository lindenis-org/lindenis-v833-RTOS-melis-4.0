#ifndef __PART_FILE_PLUGIN_IF_HEADER_H____
#define __PART_FILE_PLUGIN_IF_HEADER_H____

#define  PART_FILE_PLUGIN_TYPE  "9C3973FD-3EA9-42b8-88D2-587D8104938A"

#define MAX_PATH 260

typedef     void           *__hdle;
typedef     void           *HANDLE;
typedef     unsigned int    __u32;
typedef     int             __s32;
typedef     unsigned char   __u8;

//------------------------------------------------------------------------------
//                  RAM Part分区信息,用于打开分区时的输入参数
//------------------------------------------------------------------------------
#define FILE_PART_NAME      "FilePartiton"
typedef struct FILEPartInfo
{
    HANDLE      pFile;
    char        szFileName[MAX_PATH];
    __u32       nSecCnt;
    __u32       nSectSize;

} FILE_PartInfo;


#endif
