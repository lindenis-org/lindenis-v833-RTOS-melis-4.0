// ParsResText.h: interface for the ParsResText class.
// Sam Liu
// 2010-7-5

#ifndef AFX_PARSRESTEXT_H__EB2C69DF_9541_4F3E_81C2_9E877901CEED__INCLUDED_
#define AFX_PARSRESTEXT_H__EB2C69DF_9541_4F3E_81C2_9E877901CEED__INCLUDED_
#include "osd_str_def.h"

#if _MSC_VER > 1000
#pragma once
#endif

typedef unsigned short ushort;

// Config
typedef struct _tag_Res_Lan_Config
{
    int Version;
    TCHAR szInput[MAX_PATH];
    TCHAR szOutput[MAX_PATH];
    TCHAR szHeadFName[MAX_PATH];
    TCHAR szPosFName[MAX_PATH];
} ResLanConfig;


// 输出数据 Lan1: ID1 ID2 ID3 ID4....IDn
//          Lan2: ID1 ID2 ID3 ID4....IDn
typedef struct _tag_ResLangStr
{
    ushort nStrID;
    LPBYTE lpUTF8;
    DWORD dwLen;
    //  WORD x;
    //  WORD y;
} ResLangStr; // Lang字符串 UFT8格式

typedef struct _tag_ResLangItem
{
    ushort nLangID;
    vector<ResLangStr> _strLans;
} ResLang_Item;


// The structs write in the bin file
#define SEC_NAME        ".langdat"  //8字节长度，节名
#define LANGLISTSTART   44

typedef signed char __s8;
typedef unsigned short __u16;
typedef unsigned int   __u32;

//语言资源文件数据格式定义
typedef struct tag_LANG_HEAD
{
    __s8        SecName[8];     //节名
    __u16       version;        //版本：0x0100
    __u16       size;           //LANG_HEAD数据结构大小
    __u32       LangTabOff;     //LANG表起始地址偏移量----------------->
    __u16       LangSize;       //LANG表中每个条目大小
    __u32       LangNum;        //LANG表条目个数,既语言种类个数
    __u16       StringSize;     //字符串size
    __u32       StringNum;      //字符串个数；
    __u32       align;          //数据边界对齐模式；
    __u32       flags;          //标志:SYSRES,LIBRES,APPRES
} __attribute__((packed))LANG_HEAD; //34 byte

typedef struct tag_LANG
{
    __u16       LangID;             //0X400 0X420
    __u32       LangOffSet;         //LANG 条目起始地址偏移量
    __u32       LangSize;           //LANG 条目大小
    __u32       StringTabOff;       //字符表起始地址
} __attribute__((packed)) LANG;

typedef struct tag_STRING
{
    __u16       LangID;         //LangID
    __u16       StringID;       //such as:0x0001
    __u32       offset;         //string data位置
    __u32       size;           //data数据长度
} __attribute__((packed)) STRING;

#define LangListStuctLength     ( ( (sizeof(LANG)+3  )/4)*4 )   //4字节对齐
#define StringListStructLength  ( ( (sizeof(STRING)+3)/4)*4 )


class ParsResText
{
public:
    ParsResText();
    virtual ~ParsResText();
public:

    void            Load(LPBYTE lpBuff, DWORD dwLen);
    void            Load(LPCTSTR lpFileName);

    // For LangBuild
    MultLan_Item   *GetLangItem(int nIndex);
    int             GetLangItemCnt();
    int             GetLangTypeCnt();
    void            GetLangInfo(int nIndex, ResLang_Info &info);
    int             GetLangName(vector<CString> &strLanNs); // Get the language name, return 0 if success

    // For Autoz Convert
    MultLan_Item   *GetRawItem(int nIndex);
    int             GetRawItemCnt();

private:
    void    UpdatePosInfo();                // Get Position information from the script;
    int     GenResStruct();                 // Convert mul to res, return 0 if success
    int     LoadResLangInfo();              // Get Language infomation whitch includes the name and the ID number

private:
    void    Release();
    void    ReleaseRes();
    void    ReleaseStr(MulStr &mulStr);
    void    ReleaseMulan(MultLan_Item *mulItem);

    void    ReleaseResStr(ResLangStr &ResStr);
    void    ReleaseResItem(ResLang_Item *ResItem);


    void    SperateItem(const MulStr &mulStr, MultLan_Item *pItem);

    void    UtoUFT8(LPCWSTR lpWChar, LPBYTE lpBuff, DWORD &dwLen);
    void    UFT8ToU(LPBYTE lpBuff,    LPWSTR lpWcsCh, DWORD dwLen, DWORD &dwRet);
private:
    UINT _nType;                       // 0 ASCI 1 Unicode
    vector<MultLan_Item *> _MulItems;  // 原始结构
    vector<ResLang_Item *> _ResItems;  // 输出结构
    vector<ResLang_Info>  _ResLanInfo; //语言标识
    ResLanConfig          _ResLanCfg;
    CString               _strWorkDir;
};

#endif //AFX_PARSRESTEXT_H__EB2C69DF_9541_4F3E_81C2_9E877901CEED__INCLUDED_
