// OsdVectorMan.h: interface for the OsdVectorMan class.

#if !defined(AFX_OSDVECTORMAN_H__29500686_E3E7_4B36_9AD2_16EDC2111DBF__INCLUDED_)
#define AFX_OSDVECTORMAN_H__29500686_E3E7_4B36_9AD2_16EDC2111DBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "osd_str_def.h"
#include "ParsResText.h"


#ifdef _USRDLL
//#define  OSD_API __declspec(dllexport)
#else
//#define  OSD_API __declspec(dllimport)
#endif


class Osd_Group
{
public:

    Osd_Group();
    ~Osd_Group();
public:

    int MakeSameLangOrder(vector<ResLang_Info> langInfo);

    MultLan_Item  *GetOsdItem(LPCTSTR lpName);
    MultLan_Item  *GetOsdItem(int nIndex);

    int ImportTxt(LPCTSTR lpFileName);
    int ExportTxt(LPCTSTR lpFileName);

    void GetLangName(vector<CString> &csName);
    int GetLangItemCnt();


public:
    TCHAR szName[32];
    UINT _nStartID;
    UINT _nMaxID;

private:
    ParsResText _ParRes;
};

class OsdVectorMan
{
public:
    OsdVectorMan();
    virtual ~OsdVectorMan();

    int LoadOSDTable(LPCTSTR lpFileName);

    int ImportTxt(Osd_Group *pGroup, LPCTSTR lpFileName);
    int ExportTxt(Osd_Group *pGroup, LPCTSTR lpFileName);

    int ExportBin(LPCTSTR lpFileName);
    int ExportHead(LPCTSTR lpFileName);

    int ExportBin();
    int ExportHead();

    int GetLangInfo(vector<ResLang_Info> &resInfos);
    int SetLangInfo(const vector<ResLang_Info> &resInfos);
    int GetLangName(vector<CString> csNames);

    int GetGroupCnt()const;

    Osd_Group *GetOsdGroup(LPCTSTR lpName);
    Osd_Group *GetOsdGroup(int nIndex);
    Osd_Group *AddNewOsdGroup(LPCTSTR lpName);
    void RemoveOsdGroup(LPCTSTR lpName);

    MultLan_Item  *GetOsdItem(LPCTSTR lpName);
    MultLan_Item  *GetOsdItem(int ID);

    void GetAllMulLanItem(vector<MultLan_Item *> &items);

    void ReleaseAll();
    void ReleaseResItems();
    int GenResStruct();

    BOOL CheckUnite();

private:
    vector<Osd_Group *>   _pGroups;
    vector<ResLang_Info>  _ResLanInfo; //语言标识
    vector<ResLang_Item *> _ResItems; // 输出结构
    vector<MultLan_Item *> _pMulItems;
    string  _strHeadFile;
    string  _strBinFile;
};

#endif // AFX_OSDVECTORMAN_H__29500686_E3E7_4B36_9AD2_16EDC2111DBF__INCLUDED_
