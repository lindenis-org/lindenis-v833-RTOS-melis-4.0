#ifndef __OSD_STR_DEF_HEADER___H____
#define __OSD_STR_DEF_HEADER___H____
typedef unsigned short ushort;

#include <vector>
using namespace std;

// 原始数据结构  ID1 name1  strLan1 strLan2 strLan3 . . .  strLan4
//       ID2 name2  strLan1 strLan2 strLan3 . . .  strLan4
typedef struct _tag_MulStr
{
    DWORD   dwLen;
    WCHAR  *pWcStr;
} MulStr;

typedef struct _tag_MultLan_Item
{
    TCHAR           szName[32];
    UINT            nStrID;;
    vector<MulStr>  _items;
} MultLan_Item;

typedef struct _tag_ResLanInfo
{
    ushort  nLangID;
    TCHAR   szLangName[64];
} ResLang_Info;

#endif
