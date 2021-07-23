#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include "basetypes.h"
#include "OsdVectorMan.h"
#include "ParsResText.h"
#include "Markup.h"
#include "basefun.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

DWORD UFT82Unicode(LPBYTE lpBuff, LPWSTR lpWcsCh, DWORD dwLen);
void Unicode2UFT8(LPCWSTR lpWChar, LPBYTE lpBuff, DWORD *dwLen);

void Upper(string *s)
{
    int len = (*s).size() ;
    for (int i = 0; i < len; i++)
    {
        if ((*s)[i] >= 'a' && (*s)[i] <= 'z')
        {
            (*s)[i] += ('A' - 'a') ;
        }
    }
}

void Lower(string *s)
{
    int len = (*s).size() ;
    for (int i = 0; i < len; i++)
    {
        if ((*s)[i] >= 'A' && (*s)[i] <= 'Z')
        {
            (*s)[i] += ('a' - 'A') ;
        }
    }
}

void remove(string str, char ch)
{
    string::iterator it;
    for (it = str.begin(); it != str.end(); ++it)
    {
        if (*it == ch)
        {
            str.erase(it);
        }
    }
}

string FormatString(string *str, const char *lpcszFormat, ...)
{
    char *pszStr = new char[1024 * 5];
    if (NULL != lpcszFormat)
    {
        int i = 0;
        //定义一个va_list型的变量,这个变量是指向参数的指针.
        va_list vArgList;
        //用va_start宏初始化变量,这个宏的第二个参数是第一个可变参数的前一个参数,是一个固定的参数
        va_start(vArgList, lpcszFormat);
        //注意,不要漏掉前面的_
        i = vsnprintf(pszStr, 50, lpcszFormat, vArgList);
        //用va_end宏结束可变参数的获取
        va_end(vArgList);
        memset(pszStr + i, '\0', 1);
    }
    string strResult(pszStr);
    str->swap(strResult);
    delete[]pszStr;
    return *str;
}


//利用递归函数的实现反序输出
int to_change(int num, int n)
{
    int r;
    r = num % n;
    if (num > 0)
        to_change(num / n, n);
    return r;
}

unsigned long _tcstoul(const char *csTmp, int n)
{
    return strtoul(csTmp, NULL, n);
}

long _tcstol(const char *csTmp, int n)
{
    return strtol(csTmp, NULL, n);
}

char *wchar_to_char(wchar_t *WStr)
{
    size_t len = wcslen(WStr) + 1;
    char *CStr;
    CStr = (char *)malloc(len * sizeof(char));
    wcstombs(CStr, WStr, len);
    return CStr;
}

OsdVectorMan::OsdVectorMan()
{
    _strHeadFile.erase(0, _strHeadFile.length());
    _strBinFile.erase(0, _strBinFile.length());
}

OsdVectorMan::~OsdVectorMan()
{
    this->ReleaseAll();
}


int OsdVectorMan::LoadOSDTable(LPCTSTR lpFileName)
{
    int i = 0;
    CMarkup xmlMarkup;

    ReleaseAll();
    CString csTmp;
    TCHAR szDir[MAX_PATH];
    getcwd(szDir, MAX_PATH);

    printf("Load osd %s\n", lpFileName);

    if (xmlMarkup.Load(lpFileName) == FALSE)
    {
        printf("Error in Load %s;\nXML error:%s\n", lpFileName, xmlMarkup.GetError().c_str());
        return __LINE__;
    }

    BOOL bf = xmlMarkup.FindElem("OSDTable");

    if (bf == FALSE)
        return __LINE__;

    xmlMarkup.IntoElem();

    bf = xmlMarkup.FindElem("Groups");
    if (bf == FALSE)
        return __LINE__;

    csTmp = xmlMarkup.GetAttrib("count");
    int ns = atoi(csTmp.c_str());

    xmlMarkup.IntoElem();
    for (int i = 0; i < ns ; i++)
    {
        bf = xmlMarkup.FindElem("OSDItem");
        if (bf == FALSE)
            return __LINE__;

        Osd_Group *pNew = new Osd_Group;

        csTmp = xmlMarkup.GetAttrib("name");
        _tcscpy(pNew->szName, csTmp.c_str());

        csTmp = xmlMarkup.GetAttrib("startid");
        pNew->_nStartID = atoi(csTmp.c_str());

        csTmp = xmlMarkup.GetAttrib("maxid");
        pNew->_nMaxID = atoi(csTmp.c_str());

        csTmp = xmlMarkup.GetAttrib("file");

        CString csFullPath = szDir;
        csFullPath = csFullPath + "/" + csTmp;
        pNew->ImportTxt(csFullPath.c_str());

        _pGroups.push_back(pNew);

    }

    xmlMarkup.OutOfElem();

    if (xmlMarkup.FindElem(_T("LangType")) == FALSE)
    {
        return __LINE__;
    }


    csTmp = xmlMarkup.GetAttrib(_T("count"));

    ns = atoi(csTmp.c_str());

    _ResLanInfo.clear();
    _ResLanInfo.resize(ns);

    xmlMarkup.IntoElem();

    for (i = 0 ; i < ns; i++)
    {
        if (xmlMarkup.FindElem() == FALSE)
            return __LINE__;

        csTmp = xmlMarkup.GetTagName();
        if (_tcscmp(csTmp.c_str(), _T("LangItem")) != 0)
            return __LINE__;

        ResLang_Info &info = _ResLanInfo.at(i);

        csTmp = xmlMarkup.GetAttrib(_T("name"));
        _tcscpy(info.szLangName, csTmp.c_str());

        csTmp = xmlMarkup.GetAttrib(_T("langId"));
        info.nLangID = _tcstoul(csTmp.c_str(), 16);
    }


    ns = _pGroups.size();
    int nLangInfoCnt = _ResLanInfo.size();
    i = 0;
    while (i < ns)
    {
        Osd_Group *pGroup = _pGroups.at(i);
        vector<CString> langName;
        pGroup->GetLangName(langName);
        int nLanCnt = langName.size();
        if (nLanCnt != nLangInfoCnt)
        {
            _tprintf(_T("\nLangType count doesn't matching in %s %d %d\n "), pGroup->szName, nLanCnt, nLangInfoCnt);
            _pGroups.erase(_pGroups.begin() + i);
            delete pGroup;
            ns--;
            continue;
        }

        int nRet = pGroup->MakeSameLangOrder(_ResLanInfo);
        if (nRet)
        {
            _tprintf(_T("\n %d MakeSameLangOrder in %s\n"), nRet, pGroup->szName);
            _pGroups.erase(_pGroups.begin() + i);
            delete pGroup;
            ns--;
            continue;
        }

        i++;
    }

    xmlMarkup.OutOfElem();

    if (xmlMarkup.FindElem(_T("Output")) == FALSE)
    {
        return __LINE__;
    }

    csTmp = xmlMarkup.GetAttrib(_T("bin"));
    TCHAR  szRealPath[MAX_PATH];
    get_full_path(szRealPath, csTmp.data());
    _strBinFile = szRealPath;

    csTmp = xmlMarkup.GetAttrib(_T("head"));
    get_full_path(szRealPath, csTmp.data());
    _strHeadFile = szRealPath;

    return 0;

}

int OsdVectorMan::ImportTxt(Osd_Group *pGroup, LPCTSTR lpFileName)
{
    pGroup->ImportTxt(lpFileName);
    return 0;
}
int OsdVectorMan::ExportTxt(Osd_Group *pGroup, LPCTSTR lpFileName)
{

    CFile cf;
    cf = fopen(lpFileName, "wb+");
    if (!cf)
    {
        return __LINE__;
    }


    TCHAR bu [2048];
    TCHAR *lpBuff;
    lpBuff = bu;
    WORD wHead = 0xfeff;
    fwrite(&wHead, sizeof(WORD), 1, cf);

    _tcscpy(lpBuff, _T("name\t"));

    vector<CString> csNames;
    pGroup->GetLangName(csNames);


    int nLangs = csNames.size();
    for (int j = 0; j < nLangs; j++)
    {
        _tcscat(lpBuff, csNames.at(j).c_str());
        if (j == (nLangs - 1))
            continue;
        _tcscat(lpBuff, _T("\t"));
    }
    _tcscat(lpBuff, _T("\r\n"));

    int nLen = _tcslen(lpBuff);
    fwrite(lpBuff, nLen * sizeof(TCHAR), 1, cf);
    _tcscpy(lpBuff, _T(""));

    int ns = pGroup->GetLangItemCnt();
    if (ns == 0)
        return __LINE__;

    int i;
    for (i = 0; i < ns ; i++)
    {
        MultLan_Item *pItem = pGroup->GetOsdItem(i);

        _tcscpy(lpBuff, _T(""));
        nLangs = pItem->_items.size();
        int j;
        for (j = 0; j < nLangs; j++)
        {
            _tcscat(lpBuff, wchar_to_char(pItem->_items.at(j).pWcStr));
            if (j == (nLangs - 1))
                continue;
            _tcscat(lpBuff, _T("\t"));
        }
        _tcscat(lpBuff, _T("\r\n"));

        nLen = _tcslen(lpBuff);
        fwrite(lpBuff, nLen * sizeof(TCHAR), 1, cf);
    }

    fclose(cf);
    return 0;
}

int  OsdVectorMan::ExportBin(LPCTSTR lpFileName)
{

    if (!CheckUnite())
    {
        printf("CheckUnite error!");
        return __LINE__;
    }

    GenResStruct();

    // Write Head
    CFile fp;
    DWORD dwLen;

    LANG_HEAD m_LHead;
    char *m_pbuffer;

    int nLangCnt = _ResLanInfo.size();
    int nStrCnt = _pMulItems.size();

    // Add ID tab Info
    __u16 *IdTab = new __u16[nStrCnt];
    int nIDTabSize = (sizeof(__u16) * nStrCnt + 3) / 4 * 4;

    if (nLangCnt == 0)
        return __LINE__;

    ResLang_Item *pItem = _ResItems.at(0);
    for (int n = 0; n < nStrCnt; n++)
    {
        ResLang_Item *pItem = _ResItems.at(0);
        IdTab[n] = pItem->_strLans.at(n).nStrID;
    }

    int nLangLstStart = LANGLISTSTART + nIDTabSize;

    fp = fopen(lpFileName, "wb+");

    memcpy(m_LHead.SecName, SEC_NAME, 8);
    m_LHead.version     = 0x0200;// pConfig->Version;
    m_LHead.size        = sizeof(LANG_HEAD);
    m_LHead.LangTabOff  = nLangLstStart;
    m_LHead.LangSize    = LangListStuctLength;
    m_LHead.LangNum     = nLangCnt;
    m_LHead.StringSize  = StringListStructLength;
    m_LHead.StringNum   = nStrCnt;
    m_LHead.align       = 4;    //文件按照4字节方式对齐
    m_LHead.flags       = 0;    //该字段保留

    m_pbuffer = (char *)malloc(sizeof(__s8) * (LANGLISTSTART));
    memset(m_pbuffer, 0, sizeof(__s8) * (LANGLISTSTART));
    memcpy(m_pbuffer, &m_LHead, sizeof(m_LHead));

    fwrite(m_pbuffer, (LANGLISTSTART), 1, fp);

    free(m_pbuffer);

    fwrite(IdTab, sizeof(__u16) * nStrCnt, 1, fp); // Write IDTable

    delete IdTab;

    int dataadd;
    LANG m_LDNode;
    m_pbuffer = (char *)malloc(sizeof(__s8) * LangListStuctLength);

    dataadd = nLangLstStart + LangListStuctLength * nLangCnt +
              nLangCnt * StringListStructLength * (nStrCnt);

    int i;
    for (i = 0; i < nLangCnt; i++)
    {
        STRING m_Str;
        ResLang_Item *pItem = _ResItems.at(i);
        fseek(fp, nLangLstStart + LangListStuctLength * i, SEEK_SET);

        m_LDNode.LangID     = pItem->nLangID;

        m_LDNode.LangOffSet = nLangLstStart + LangListStuctLength * i;
        m_LDNode.LangSize   = LangListStuctLength;
        m_LDNode.StringTabOff   = nLangLstStart + LangListStuctLength * nLangCnt +
                                  i * StringListStructLength * (pItem->_strLans.size());
        memset(m_pbuffer, 0, LangListStuctLength);
        memcpy(m_pbuffer, &m_LDNode, sizeof(m_LDNode));

        fwrite(m_pbuffer, LangListStuctLength, 1, fp);

        fseek(fp, m_LDNode.StringTabOff, SEEK_SET);

        char *pStrTBuff = (char *)malloc(sizeof(__s8) * StringListStructLength);

        for (int j = 0; j < nStrCnt; j++)
        {
            char *s_pbuffer;
            ResLangStr &pStr =  pItem->_strLans.at(j);
            fseek(fp, m_LDNode.StringTabOff + j * StringListStructLength, SEEK_SET);
            m_Str.LangID = pItem->nLangID;
            m_Str.StringID = pStr.nStrID;
            m_Str.size     = pStr.dwLen;
            m_Str.offset   = dataadd;

            memset(pStrTBuff, 0, StringListStructLength);
            memcpy(pStrTBuff, &m_Str, sizeof(m_Str));
            fwrite(pStrTBuff, StringListStructLength, 1, fp);

            fseek(fp, m_Str.offset, SEEK_SET);
            int nSj = ((m_Str.size + 3) / 4) * 4;

            s_pbuffer = (char *)malloc(sizeof(__s8) * nSj);
            memset(s_pbuffer, 0, nSj);
            memcpy(s_pbuffer, pStr.lpUTF8, pStr.dwLen);
            fwrite(s_pbuffer, nSj, 1, fp);

            free(s_pbuffer);

            dataadd += ((m_Str.size + 3) / 4) * 4;

        }
        free(pStrTBuff);
    }
    free(m_pbuffer);

    fclose(fp);
    return 0;

}
int OsdVectorMan::ExportHead(LPCTSTR lpFileName)
{

    CStdioFile cf;
    cf = fopen(lpFileName, "w+");
    CString csLine;
    int len;
    time_t timep;
    struct tm *p;
    time(&timep);
    p = gmtime(&timep);

    char line_data[1024 * 5] = {0};
    char *line = line_data;
    sprintf(line, "%s%s%s%s%s%s%s%s%s%s",
            "/*\n***********************************************************"
            , "***********************************************************\n"
            , "\t\t\t\t\t\t\t\t\t\t\t\t\t\t ePDK\n"
            , "\t\t\t\t\t\t\t\t\t\t the Easy Portable/Player Develop Kits\n"
            , "*\t\t\t\t\t\t\t\t\t\t\t\t RES Compiler Sub-System\n"
            , "*\n* \t\t\t\t\t\t\t\t\t\t\t\t(c) Copyright 2007-2010 \n"
            , "* \t\t\t\t\t\t\t\t\t\t\t\t All Rights Reserved\n"
            , "*\n* Moudle\t: language compiler\n"
            , "* File\t\t: lang.h\n*\n"
            , "* By\t\t: \n");


    sprintf(line, "%s* Version\t: v1.00\n* Date\t\t: %d-%d-%d\n", line, (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);

    fwrite(line, strlen(line), 1, cf);

    line = line_data;
    memset(line, 0, sizeof(line_data));
    sprintf(line, "%s%s%s",
            "*****************************************************************"
            , "*****************************************************\n*/\n\n\n"
            , "#ifndef __LANG__H__\n#define __LANG__H__\n\n// LangId \n");


    fwrite(line, strlen(line), 1, cf);

    int nLanCnt = _ResLanInfo.size();
    int i;
    CString csTmp;
    for (i = 0; i < nLanCnt; i++)
    {
        ResLang_Info &reslfo = _ResLanInfo.at(i);

        csTmp = reslfo.szLangName;
        Upper(&csTmp);
        csLine = FormatString(&csLine, _T("#define\tLANG_%s_TYPE\t\t0x%x\n"), csTmp.c_str(), reslfo.nLangID);
        fwrite(csLine.c_str(), csLine.length(), 1, cf);
    }

    csLine = FormatString(&csLine, _T("\n// StringID \n"));
    fwrite(csLine.c_str(), csLine.length(), 1, cf);


    int ns = _pGroups.size();
    for (i = 0 ; i < ns ; i++)
    {
        Osd_Group *pg = _pGroups.at(i);
        int osdItems =  pg->GetLangItemCnt();
        for (int j = 0 ; j < osdItems; j++)
        {
            MultLan_Item *pMulItem = pg->GetOsdItem(j);
            csTmp = pMulItem->szName;
            pMulItem->nStrID = pg->_nStartID + j;
            remove(csTmp, '.');
            Upper(&csTmp);
            line = line_data;
            memset(line, 0, sizeof(line_data));
            sprintf(line, "#define \t STRING_%s\t\t0x%02x\n", csTmp.c_str(), pg->_nStartID + j);
            csLine = FormatString(&csLine, _T("#define \t STRING_%s\t\t0x%02x\n"), csTmp.c_str(), pg->_nStartID + j);

            fwrite(line, strlen(line), 1, cf);
        }

    }
    csLine = FormatString(&csLine, _T("%s"), _T("\n\n#endif //__LANG__H__ \r\n\n\n"));

    fwrite(csLine.c_str(), csLine.length(), 1, cf);
    fclose(cf);
    return 0;
}

int OsdVectorMan::GetLangInfo(vector<ResLang_Info> &resInfos)
{
    resInfos.clear();
    int ns = _ResLanInfo.size();
    int i;
    for (i = 0; i < ns ; i++)
    {
        ResLang_Info &info = _ResLanInfo.at(i);
        resInfos.push_back(info);
    }
    return 0;
}

int OsdVectorMan::GetGroupCnt()const
{
    return _pGroups.size();

}

Osd_Group *OsdVectorMan::GetOsdGroup(LPCTSTR lpName)
{
    int ns = _pGroups.size();
    for (int i = 0; i < ns; i++)
    {
        Osd_Group *pg = _pGroups.at(i);

        if (_tcscmp(pg->szName, lpName))
            return pg;
    }
    return NULL;
}

void OsdVectorMan::RemoveOsdGroup(LPCTSTR lpName)
{
    int ns = _pGroups.size();
    for (int i = 0; i < ns; i++)
    {
        Osd_Group *pg = _pGroups.at(i);

        if (_tcscmp(pg->szName, lpName) == 0)
        {
            _pGroups.erase(_pGroups.begin() + i);

            delete pg;
            return;
        }
    }
}

Osd_Group *OsdVectorMan::GetOsdGroup(int nIndex)
{
    if (nIndex >= 0 && nIndex < _pGroups.size())
        return _pGroups.at(nIndex);

    return NULL;
}

MultLan_Item  *OsdVectorMan::GetOsdItem(LPCTSTR lpName)
{
    return NULL;
}

MultLan_Item  *OsdVectorMan::GetOsdItem(int ID)
{
    return NULL;
}

void  OsdVectorMan::GetAllMulLanItem(vector<MultLan_Item *> &items)
{
    int ns = _pGroups.size();
    int nStrCnt = 0;
    items.clear();

    for (int i = 0; i < ns; i++)
    {
        Osd_Group *pgs = _pGroups.at(i);
        int nGroupItemCnt = pgs->GetLangItemCnt();

        for (int j = 0 ; j < nGroupItemCnt; j++)
        {
            MultLan_Item *item = pgs->GetOsdItem(j);
            items.push_back(item);
        }
    }
}

int OsdVectorMan::GenResStruct()
{
    ReleaseResItems();

    int ns = _pGroups.size();
    int nStrCnt = 0;

    GetAllMulLanItem(_pMulItems);
    nStrCnt = _pMulItems.size();

    int nLanCnt = _ResLanInfo.size();// csLanNames.size();

    _ResItems.resize(nLanCnt);

    for (int i = 0; i < nLanCnt; i++)
    {
        ResLang_Item *pResLanItem = new ResLang_Item;
        _ResItems.at(i) = pResLanItem;

        pResLanItem->_strLans.reserve(1024 * 8);

        pResLanItem->_strLans.clear();

        pResLanItem->nLangID = _ResLanInfo.at(i).nLangID;

        pResLanItem->_strLans.resize(nStrCnt);

        for (int j = 0; j < nStrCnt; j++)
        {
            ResLangStr &resStrTmp = pResLanItem->_strLans.at(j);
            MultLan_Item *pOrgMul = _pMulItems.at(j);
            MulStr &mulStr = pOrgMul->_items.at(i + 1);
            memset(&resStrTmp, 0, sizeof(resStrTmp));

            resStrTmp.lpUTF8 = new BYTE[mulStr.dwLen * 4];

            memset(resStrTmp.lpUTF8, 0, mulStr.dwLen * 4);
            resStrTmp.nStrID = pOrgMul->nStrID;// j+1;
            Unicode2UFT8(mulStr.pWcStr, resStrTmp.lpUTF8, &resStrTmp.dwLen);
        }
    }
    return 0;
}

void  OsdVectorMan::ReleaseAll()
{
    int ns = _pGroups.size();
    for (int i = 0; i < ns; i++)
    {
        Osd_Group *pGroup = _pGroups.at(i);
        delete pGroup;

    }
    _pGroups.clear();
    ReleaseResItems();
}

#define OSD_API __declspec(dllexport)

int BuildOsd(LPCTSTR lpFileName)
{
    OsdVectorMan osdMan;
    int nRet = osdMan.LoadOSDTable(lpFileName);
    if (nRet != 0)
    {
        return __LINE__;
    }
    if (osdMan.CheckUnite() == FALSE)
    {
        printf("Faile Build\n");
        return __LINE__;
    }
    osdMan.ExportHead();
    osdMan.ExportBin();
    return 0;
}

void OsdVectorMan::ReleaseResItems()
{
    int ns = _ResItems.size();
    for (int i = 0; i < ns; i++)
    {
        ResLang_Item *pItem =  _ResItems.at(i);
        int nSub = pItem->_strLans.size();
        for (int j = 0; j < nSub; j++)
        {
            delete pItem->_strLans.at(j).lpUTF8;
        }
        delete pItem;
    }
    _ResItems.clear();

}

Osd_Group  *OsdVectorMan::AddNewOsdGroup(LPCTSTR lpName)
{
    int ns = _pGroups.size();
    int nMaxID = 0;
    for (int i = 0; i < ns; i++)
    {
        Osd_Group *pTmp = _pGroups.at(i);
        if (nMaxID < pTmp->_nMaxID)
            nMaxID  = pTmp->_nMaxID;
    }

    Osd_Group *pNew = new Osd_Group;
    pNew->_nStartID = nMaxID + 1;
    pNew->_nMaxID = nMaxID + 200;

    _tcscpy(pNew->szName, lpName);
    _pGroups.push_back(pNew);

    return pNew;

}

int OsdVectorMan::GetLangName(vector<CString> csNames)
{
    int ns = _ResLanInfo.size();
    for (int i = 0; i < ns; i++)
    {
        csNames.push_back(_ResLanInfo.at(i).szLangName);
    }
    return 0;
}


int OsdVectorMan::ExportBin()
{
    return ExportBin(_strBinFile.c_str());
}

int OsdVectorMan::ExportHead()
{
    return ExportHead(_strHeadFile.c_str());
}

int OsdVectorMan::SetLangInfo(const vector<ResLang_Info> &resInfos)
{

    int ns = resInfos.size();
    _ResLanInfo.clear();
    _ResLanInfo.resize(ns);

    for (int i = 0; i < ns ; i++)
    {
        ResLang_Info &info = _ResLanInfo.at(i);
        const ResLang_Info &infoSrc = resInfos.at(i);
        info.nLangID = infoSrc.nLangID;
        _tcscpy(info.szLangName, infoSrc.szLangName);
    }
    return 0;

}


Osd_Group::Osd_Group()
{

}

Osd_Group::~Osd_Group()
{

}

MultLan_Item  *Osd_Group::GetOsdItem(LPCTSTR lpName)
{
    int ns = _ParRes.GetLangItemCnt();
    for (int i = 0; i < ns; i++)
    {
        MultLan_Item *item = _ParRes.GetLangItem(i);
        if (_tcscmp(item->szName, lpName) == 0)
            return item;
    }
    return NULL;
}

MultLan_Item  *Osd_Group::GetOsdItem(int nIndex)
{
    return _ParRes.GetLangItem(nIndex);
    return NULL;
}

int Osd_Group::ImportTxt(LPCTSTR lpFileName)
{
    _ParRes.Load(lpFileName);

    int ns = _ParRes.GetLangItemCnt();

    return 0;
}

int Osd_Group::ExportTxt(LPCTSTR lpFileName)
{
    CFile cf;
    cf = fopen(lpFileName, "w+");
    if (!cf)
    {
        return __LINE__;
    }

    TCHAR *lpBuff = new TCHAR[2048];
    WORD wHead = 0xfeff;
    fwrite(&wHead, sizeof(WORD), 1, cf);

    _tcscpy(lpBuff, _T(""));

    vector<CString> csNames;
    GetLangName(csNames);


    int nLangs = csNames.size();
    for (int j = 0; j < nLangs; j++)
    {
        _tcscat(lpBuff, csNames.at(j).c_str());
        if (j == (nLangs - 1))
            continue;
        _tcscat(lpBuff, _T("\t"));
    }
    _tcscat(lpBuff, _T("\r\n"));

    int nLen = _tcslen(lpBuff);
    fwrite(lpBuff, nLen * sizeof(TCHAR), 1, cf);
    _tcscpy(lpBuff, _T(""));

    int ns = GetLangItemCnt();
    if (ns == 0)
        return __LINE__;


    for (int i = 0; i < ns ; i++)
    {
        MultLan_Item *pItem = GetOsdItem(i);

        _tcscpy(lpBuff, _T(""));
        nLangs = pItem->_items.size();
        int j;
        for (j = 0; j < nLangs; j++)
        {
            _tcscat(lpBuff, wchar_to_char(pItem->_items.at(j).pWcStr));
            if (j == (nLangs - 1))
                continue;
            _tcscat(lpBuff, _T("\t"));
        }
        _tcscat(lpBuff, _T("\r\n"));

        nLen = _tcslen(lpBuff);
        fwrite(lpBuff, nLen * sizeof(TCHAR), 1, cf);
    }

    fclose(cf);
    delete lpBuff;
}

void Osd_Group::GetLangName(vector<CString> &csName)
{
    _ParRes.GetLangName(csName);
}

int Osd_Group::GetLangItemCnt()
{
    return _ParRes.GetLangItemCnt();
}

int Osd_Group::MakeSameLangOrder(vector<ResLang_Info> langInfo)
{
    vector<CString> langNames;
    vector<int> nSeriIndex;
    nSeriIndex.clear();
    _ParRes.GetLangName(langNames);

    int nItemCnt = _ParRes.GetLangItemCnt();

    int nLangCnt = langInfo.size();

    if (nLangCnt != langNames.size())
    {
        _tprintf(_T("%s Notmathing count in group %s\n"), __FILE__, this->szName);
        return __LINE__;
    }
    int i;
    for (i = 0 ; i < nLangCnt; i++)
    {
        ResLang_Info &info =  langInfo.at(i);

        CString csName = info.szLangName;

        int nIndex = -1;
        for (int j = 0; j < nLangCnt; j++)
        {
            CString csTmp = langNames.at(j);

            if (_tcscmp(csTmp.c_str(), csName.c_str()) == 0)
            {
                nIndex = j;
                break;
            }
        }
        if (nIndex == -1)
        {
            _tprintf(_T("\n Err:mathing Name %s in group %s\n"), csName.c_str(), szName);
            return __LINE__;
        }
        nSeriIndex.push_back(nIndex);
    }

    for (i = 0; i < nItemCnt; i++)
    {
        MultLan_Item *pMulItem = _ParRes.GetLangItem(i);
        vector<MulStr> mulStrs;
        mulStrs.resize(nLangCnt);
        int j;
        for (j = 0; j < nLangCnt; j++)
        {
            int nIndex = nSeriIndex.at(j);
            MulStr &itemOrg = pMulItem->_items.at(nIndex + 1);
            MulStr &itemDes = mulStrs.at(j);
            memcpy(&itemDes, &itemOrg, sizeof(MulStr));
        }
        for (j = 0; j < nLangCnt; j++)
        {
            MulStr &itemOrg = pMulItem->_items.at(j + 1);
            MulStr &itemDes = mulStrs.at(j);
            memcpy(&itemOrg, &itemDes, sizeof(MulStr));
        }
    }

    MultLan_Item *itemLangInfo = _ParRes.GetLangItem(-1);

    vector<MulStr> mulStrs;
    mulStrs.resize(nLangCnt);
    int j;
    for (j = 0; j < nLangCnt; j++)
    {
        int nIndex = nSeriIndex.at(j);
        MulStr &itemOrg = itemLangInfo->_items.at(nIndex + 1);
        MulStr &itemDes = mulStrs.at(j);
        memcpy(&itemDes, &itemOrg, sizeof(MulStr));
    }
    for (j = 0; j < nLangCnt; j++)
    {
        MulStr &itemOrg = itemLangInfo->_items.at(j + 1);
        MulStr &itemDes = mulStrs.at(j);
        memcpy(&itemOrg, &itemDes, sizeof(MulStr));
    }
    mulStrs.clear();

    return 0;

}

BOOL OsdVectorMan::CheckUnite()
{
    int ns = _pGroups.size();

    for (int i = 0; i < ns; i++)
    {
        Osd_Group *pGrp = _pGroups.at(i);
        for (int j = i + 1; j < ns; j++)
        {
            Osd_Group *pLst = _pGroups.at(j);
            int nCnt = pLst->GetLangItemCnt();
            for (int m = 0; m < nCnt; m++)
            {
                MultLan_Item *pItem = pLst->GetOsdItem(m);
                if (pGrp->GetOsdItem(pItem->szName) != NULL)
                {
                    _tprintf(_T("\nThe Item %s is overlap in Group \"%s\" and \"%s\"\n"),
                             pItem->szName, pGrp->szName, pLst->szName);
                    return FALSE;
                }
            }
        }

    }
    return TRUE;
}
