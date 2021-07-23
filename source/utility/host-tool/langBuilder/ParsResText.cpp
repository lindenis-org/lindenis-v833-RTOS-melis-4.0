// ParsResText.cpp: implementation of the ParsResText class.

#include <string.h>
#include <assert.h>
#include "basetypes.h"
#include "ParsResText.h"
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define  BUF_LEN 1024
#define BIT_10000000 0x80
#define BIT_01000000 0x40
#define BIT_00100000 0x20
#define BIT_00010000 0x10
#define BIT_00001000 0x08
#define BIT_00000100 0x04
#define BIT_00000010 0x02

#define BIT_11000000 0xC0
#define BIT_11100000 0xE0
#define BIT_00111111 0x3F
#define BIT_00011111 0x1F
#define BIT_00001111 0x0f

#define WCHAREND     0x0000
#define WCHARTAB     0x0009

#define LANCFGSEC _T("LANGCFG")

extern int _tcstoul(const char *csTmp, int n);
extern void Upper(string *s);

//将字符串小写变为大写
void strtoupper(char *m_str, int max_size)
{
    for (int i = 0; i < max_size; i++)
    {
        if (islower(m_str[i]))
        {
            m_str[i] = toupper(m_str[i]);
        }
    }
}

char *strupr(char *src)
{
    while (*src != '\0')
    {
        if (*src >= 'a' && *src <= 'z')
            *src -= 32;
        src++;
    }
    return src;
}

char *strlwr(char *src)
{
    while (*src != '\0')
    {
        if (*src >= 'A' && *src <= 'Z')
        {
            //*src += 0x20;
            *src += 32;
        }
        src++;
    }
    return src;
}

string TrimRight(string str, char ch)
{
    int i, len ;

    len = str.length();
    char data[len + 1];
    strcpy(data, str.c_str());
    memset(data + len, 0, 1);
    for (i = len - 1; i >= 0; i--)
    {
        if ((data[i] != ch) && (data[i] != 0x0a) && (data[i] != 0x0d))
            break ;
    }
    data[i + 1] = 0 ;
    string result(data);
    return result;
}

string TrimRight(string str)
{
    int i, len ;

    len = str.length();
    char data[len + 1];
    strcpy(data, str.c_str());
    memset(data + len, 0, 1);
    for (i = len - 1; i >= 0; i--)
    {
        if ((data[i] != ' ') && (data[i] != 0x0a) && (data[i] != 0x0d))
            break ;
    }
    data[i + 1] = 0 ;
    string result(data);
    return result;
}

string TrimLeft(string str)
{
    int i , len ;
    len = str.length();
    char data[len + 1];
    strcpy(data, str.c_str());
    memset(data + len, 0, 1);

    for (i = 0; i < len; i++)
    {
        if (data[i] != ' ') break ;
    }
    memmove(data, data + i, len - i + 1);
    string result(data);
    return result;
}

string TrimLeft(string str, char ch)
{
    int i , len ;
    len = str.length();
    char data[len + 1];
    strcpy(data, str.c_str());
    memset(data + len, 0, 1);

    for (i = 0; i < len; i++)
    {
        if (data[i] != ch) break ;
    }
    memmove(data, data + i, len - i + 1);
    string result(data);
    return result;
}

string Mid(string str, int nFirst)
{
    int i , len ;
    len = str.length();
    char data[len - nFirst];
    strcpy(data, str.c_str() + nFirst);
    string result(data);
    return result;
}

string Mid(string str, int nFirst, int count)
{
    int i , len ;
    char data[count + 1];
    strncpy(data, str.c_str() + nFirst, count);
    memset(data + count, 0, 1);
    string result(data);
    return result;
}

void strbanktoline(char *m_str)
{
    char *a;
    ASSERT(m_str != NULL);

    a = m_str;
    while (*a != '\0')
    {
        if (!(((*a >= '0') && (*a <= '9')) || ((*a >= 'a') && (*a <= 'z')) || ((*a >= 'A') && (*a <= 'Z'))))
        {
            *a = '_';
        }
        a++;
    }
}

DWORD get_file_length(CFile pFile)
{
    DWORD iFileLen = 0;
    fseek(pFile, 0, SEEK_END);
    iFileLen = ftell(pFile);
    rewind(pFile);
    return iFileLen;
}

void Unicode2UFT8(LPCWSTR lpWChar, LPBYTE lpBuff, DWORD *dwLen)
{
    DWORD len = wcslen(lpWChar) * 2;
    BYTE  utMask[7] = {BIT_10000000, BIT_01000000, BIT_00100000, BIT_00010000, BIT_00001000, BIT_00000100, BIT_00000010};
    char *test = (char *)malloc(10);
    DWORD dwTolLen = 0;

    int nIndex = 0;
    for (int i = 0; i < len && (lpWChar[i] != 0); i++)
    {
        int nBits = 8;
        int nByte = 0;
        WCHAR wCh = lpWChar[i];
        BYTE bTest = (BYTE)((wCh & 0xff00) >> 8);
        for (int m = 0; m < sizeof(utMask); m++)
        {
            if ((bTest & utMask[m]) != 0)
            {
                break;
            }
            nBits--;
        }
        if (nBits <= 1)
        {
            if (wCh < 0x0080) //if((wCh&BIT_10000000)==0x00) //0 ~ 127
            {
                lpBuff[nIndex] = (BYTE)(wCh & 0x00ff);
            }
            else // 128~255
            {
                BYTE b[2];
                b[0] = (wCh & 0x3f) | BIT_10000000;
                b[1] = ((wCh >> 6) & 0x1f) | BIT_11000000;
                lpBuff[nIndex] = b[1];
                nIndex++;
                lpBuff[nIndex] = b[0];
            }
            nIndex++;
        }
        else
        {
            nBits = 8 + nBits;

            nByte = nBits / 6;

            int nBitMove = 0;
            BYTE bFirstMsk = 0;
            BYTE bufdata [nByte + 1];
            BYTE *lpTmpBuf = bufdata;
            int n;
            for (n = 0; n < nByte; n++)
            {
                lpTmpBuf[n] = (BYTE)(((wCh >> nBitMove) & 0x3f) | BIT_10000000);
                nBitMove += 6;
                bFirstMsk = bFirstMsk | utMask[n];
            }
            bFirstMsk = bFirstMsk | utMask[n];

            lpTmpBuf[n] = (BYTE)(((wCh >> nBitMove) & 0x00ff) | bFirstMsk);

            for (n = nByte; n >= 0; n--)
            {
                lpBuff[nIndex++] = lpTmpBuf[n];
            }
        }
    }

    lpBuff[nIndex] = '\0';
    *dwLen = nIndex + 1;
}

DWORD UFT82Unicode(LPBYTE lpBuff, LPWSTR lpWcsCh, DWORD dwLen)
{

    LPBYTE lpbuf = lpBuff;
    int nIndex = 0;
    int nWcPos = 0;

    BYTE  utMask[6] = {BIT_10000000, BIT_01000000, BIT_00100000, BIT_00010000, BIT_00001000, BIT_00000100};

    DWORD dwTolLen = 0;

    while (nIndex < dwLen)
    {
        BYTE bTest = lpBuff[nIndex];
        int nByte = 0;
        for (int i = 0; i < 3; i++)
        {
            if ((bTest & utMask[i]) == 0x00)
            {
                break;
            }
            nByte++;
        }
        if (nByte == 0) //表示是ASCI码，用一个字节表示
        {
            lpWcsCh[nWcPos] = 0;
            lpWcsCh[nWcPos] |= bTest;
            nWcPos++;
            nIndex++;
        }
        else if (nByte == 1)
        {
            printf(_T("Error"));
            break;
        }
        else if (nByte <= 3)
        {
            LPBYTE pTmp = new BYTE[nByte];
            memcpy(pTmp, lpBuff + nIndex, nByte);
            BYTE bMask = 0x00;
            for (int i = 0; i < nByte; i++)
            {
                bMask |= utMask[i];
            }
            bMask = ~bMask;

            WCHAR wTran = 0x00;
            WCHAR wTmp;
            int i;
            for (i = nByte - 1 ; i > 0; i--)
            {
                wTmp = (pTmp[i] & BIT_00111111);
                wTmp = wTmp << ((nByte - 1 - i) * 6);
                wTran |= wTmp;
            }
            wTmp = (pTmp[0] & bMask) << ((nByte - 1) * 6);
            wTran |= wTmp;

            lpWcsCh[nWcPos++] = wTran;
            nIndex += 3;
            delete pTmp;
        }
        else
            break;
    }
    return nWcPos;

}


void wcsTrimLeft(LPWSTR lpWstr)
{

    UINT nLen = wcslen(lpWstr) * 2;
    WCHAR tmp_data [nLen + 1];
    LPWSTR lpTmp = tmp_data;
    //LPWSTR lpTmp = new WCHAR [nLen+1];
    wcscpy(lpTmp, lpWstr);
    int nIndex = 0;
    while (nIndex < nLen)
    {
        WCHAR wTest = lpTmp[nIndex];

        if (wTest != WCHARTAB && wTest != 0x0020)
            break;
        nIndex++;
    }
    if (nIndex && nIndex < nLen)
    {
        wcscpy(lpWstr, lpTmp + nIndex);
    }
    else if (nIndex == nLen)
    {
        printf("before wcs\n");
        wcscpy(lpWstr, L"");
        printf("exit wcs\n");
    }
    //delete lpTmp;

}


BOOL CheckLocalStr(LPCWSTR wszStr, LPTSTR szStr)
{
#ifndef UNICODE

    TCHAR   szLocal[MAX_PATH];
    wchar_t wszLocal[MAX_PATH];

    //wcstombs()
    int len = wcslen(wszStr) * 2;
    int i;
    for (i = 0; i < len && (wszStr[i] != 0); i++)
    {
        szStr[i] = wszStr[i];
    }
    szStr[i] = 0;
    //int n = wcstombs(szStr,wszStr,wcslen(wszStr)*2+1);
    return TRUE;
#if 0
    int nBytes = WideCharToMultiByte(
                     CP_ACP,
                     0,
                     wszStr,
                     -1,
                     NULL,
                     0,
                     NULL,
                     NULL);
    nBytes = WideCharToMultiByte(
                 CP_ACP,
                 0,
                 wszStr,
                 -1,
                 szLocal,
                 nBytes,
                 NULL,
                 NULL);

    MultiByteToWideChar(CP_ACP, MB_COMPOSITE, szLocal, MAX_PATH, wszLocal, MAX_PATH);

    int nLen = wcslen(wszStr);

    if (memcmp(wszStr, wszLocal, nLen) == 0)
    {
        strcat(szStr, szLocal);
        return TRUE;
    }
    return FALSE;
#endif
#else
    _tcscpy(szStr, wszStr);
    return TRUE;
#endif
}
BOOL CheckValidatStr(LPCTSTR lpStr)
{
    int nLen = strlen(lpStr);
    int nIndex = 0;
    TCHAR pc[MAX_PATH] = _T("");
    strcpy(pc, lpStr);
    strlwr(pc);
    while (nIndex < nLen)
    {
        char chT = pc[nIndex];
        if ((chT <= '9' && chT >= '0')
                || (chT >= 'a' && chT <= 'z')
                || (chT == '_')
                || (chT == ' ')
           )
        {
            nIndex = nIndex + 1;
        }
        else
            return FALSE;

    }
    return TRUE;
#if 0
#ifdef UNICODE
    int nLen = _tcslen(lpStr);
    int nIndex = 0;
    TCHAR pc[MAX_PATH] = _T("");
    _tcscpy(pc, lpStr);
    _wcslwr(pc);
    while (nIndex < nLen)
    {
        char chT = pc[nIndex];
        if ((chT <= '9' && chT >= '0')
                || (chT >= 'a' && chT <= 'z')
                || (chT == '_')
           )
        {
            nIndex++;
        }
        else
            return FALSE;

    }
    return TRUE;
#else
    int nLen = strlen(lpStr);
    int nIndex = 0;
    char pc[MAX_PATH] = "";
    strcpy(pc, lpStr);
    _strlwr(pc);
    while (nIndex < nLen)
    {
        char chT = pc[nIndex];
        if ((chT <= '9' && chT >= '0')
                || (chT >= 'a' && chT <= 'z')
                || (chT == '_')
           )
        {
            nIndex++;
        }
        else
            return FALSE;

    }
    return TRUE;
#endif
#endif
}

// Construction/Destruction
ParsResText::ParsResText()
{
    _MulItems.reserve(2048);
    _MulItems.clear();
    _ResLanCfg.Version = 0x100;
}

ParsResText::~ParsResText()
{
    Release();
}

void ParsResText::Load(LPCTSTR lpFileName)
{
    CFile cf;
    cf = fopen(lpFileName, "rb");
    if (!cf)
    {
        printf(_T("Can't open the file %s %d"), lpFileName, __LINE__);
        return;
    }
    DWORD dwLen = get_file_length(cf);

    LPBYTE lpBuff = new BYTE[dwLen];
    fread(lpBuff, dwLen, 1, cf);
    fclose(cf);
    Load(lpBuff, dwLen);
    delete lpBuff;
}

void ParsResText::Load(LPBYTE lpBuff, DWORD dwLen)
{
    this->Release();

    WCHAR *lpTmp = (WCHAR *)lpBuff;
    if (dwLen < 4)
        return;

    if (lpTmp[0] != 0xFEFF)
    {
        printf("File format is not validate\n");
        return;
    }

    vector<MulStr> BuffItems; // 先解析Buff的每一行
    int nPos = 1;
    int nCurPos = 1;
    DWORD dwRelLen = dwLen / 2;

    while (nCurPos <= dwRelLen)
    {
        if (nCurPos == dwRelLen)
        {
            MulStr mulTmp;
            mulTmp.dwLen =  nCurPos - nPos;
            mulTmp.pWcStr = new WCHAR[mulTmp.dwLen + 1];

            memset(mulTmp.pWcStr, 0, mulTmp.dwLen * sizeof(WCHAR));
            memcpy(mulTmp.pWcStr, lpTmp + nPos, (nCurPos - nPos)*sizeof(WCHAR));
            mulTmp.pWcStr[mulTmp.dwLen] = WCHAREND;
            BuffItems.push_back(mulTmp);
            break;
        }

        if (lpTmp[nCurPos] == 0x0000)
        {
            MulStr mulTmp;
            mulTmp.dwLen =  nCurPos - nPos;
            mulTmp.pWcStr = new WCHAR[mulTmp.dwLen + 1];
            memset(mulTmp.pWcStr, 0, mulTmp.dwLen * sizeof(WCHAR));
            memcpy(mulTmp.pWcStr, lpTmp + nPos, (nCurPos - nPos - 1)*sizeof(WCHAR));
            mulTmp.pWcStr[mulTmp.dwLen] = WCHAREND;
            BuffItems.push_back(mulTmp);
            break;
        }


        if (lpTmp[nCurPos] == 0x000d)
        {
            nCurPos++;
            if (nCurPos == (dwRelLen - 1))
            {
                MulStr mulTmp;
                mulTmp.dwLen =  nCurPos - nPos;
                mulTmp.pWcStr = new WCHAR[mulTmp.dwLen + 1];
                memset(mulTmp.pWcStr, 0, mulTmp.dwLen  * sizeof(WCHAR));
                memcpy(mulTmp.pWcStr, lpTmp + nPos, (nCurPos - nPos - 1) * sizeof(WCHAR));
                mulTmp.pWcStr[mulTmp.dwLen] = WCHAREND;
                //处理多个连续换行符
                if (mulTmp.dwLen > 1)
                {
                    BuffItems.push_back(mulTmp);
                }
                break;
            }
            if (lpTmp[nCurPos] == 0x000a)
            {
                MulStr mulTmp;
                mulTmp.dwLen =  nCurPos - nPos;
                mulTmp.pWcStr = new WCHAR[mulTmp.dwLen + 1];
                memset(mulTmp.pWcStr, 0, mulTmp.dwLen  * sizeof(WCHAR));
                memcpy(mulTmp.pWcStr, lpTmp + nPos, (nCurPos - nPos - 1) * sizeof(WCHAR));
                mulTmp.pWcStr[mulTmp.dwLen] = WCHAREND;
                //处理多个连续换行符
                if (mulTmp.dwLen > 1)
                {
                    BuffItems.push_back(mulTmp);
                }
                nPos = nCurPos + 1;
            }
            else
            {

            }
        }
        else if (lpTmp[nCurPos] == 0x000a)
        {
            //在Linux下换行符为0x0a，在windows下为0x0d,0x0a
            MulStr mulTmp;
            mulTmp.dwLen =  nCurPos - nPos;
            mulTmp.pWcStr = new WCHAR[mulTmp.dwLen + 1];
            memset(mulTmp.pWcStr, 0, mulTmp.dwLen  * sizeof(WCHAR));
            memcpy(mulTmp.pWcStr, lpTmp + nPos, (nCurPos - nPos - 1) * sizeof(WCHAR));
            mulTmp.pWcStr[mulTmp.dwLen] = WCHAREND;
            //处理多个连续换行符
            if (mulTmp.dwLen > 0)
            {
                BuffItems.push_back(mulTmp);
            }
            nPos = nCurPos + 1;
        }
        nCurPos++;
    }

    int ns = BuffItems.size();
    for (int i = 0; i < ns; i++)
    {
        MulStr &item = BuffItems.at(i);
        wcsTrimLeft(item.pWcStr);
        if (wcslen(item.pWcStr) == 0)
        {
            ReleaseStr(item);
            continue;
        }
        MultLan_Item *pMulItem = new MultLan_Item;

        SperateItem(item, pMulItem);
        _MulItems.push_back(pMulItem);

        BOOL bRet = CheckLocalStr(pMulItem->_items.at(0).pWcStr, pMulItem->szName);
        if (bRet == FALSE)
        {
            return ;
        }
        bRet = CheckValidatStr(pMulItem->szName);
        if (bRet == FALSE)
            return;
        //ReleaseStr(item);
    }
    BuffItems.clear();

    ns = _MulItems.size();

    GenResStruct();
}

void ParsResText::UtoUFT8(LPCWSTR lpWChar, LPBYTE lpBuff, DWORD &dwLen)
{
    Unicode2UFT8(lpWChar, lpBuff, &dwLen);
}

void ParsResText::UFT8ToU(LPBYTE lpBuff,  LPWSTR lpWcsCh, DWORD dwLen, DWORD &dwRet)
{
    dwRet = UFT82Unicode(lpBuff, lpWcsCh, dwLen);
}

void ParsResText::ReleaseStr(MulStr &mulStr)
{
    delete mulStr.pWcStr;
}

void ParsResText::ReleaseMulan(MultLan_Item *mulItem)
{
    int ns = mulItem->_items.size();

    for (int i = 0 ; i < ns; i++)
    {
        MulStr &mul = mulItem->_items.at(i);
        //TRACE(_T("Release %s\n"), mul.pWcStr);
        ReleaseStr(mul);
    }
    mulItem->_items.clear();
    delete mulItem;
}


void ParsResText::ReleaseResStr(ResLangStr &ResStr)
{
    delete ResStr.lpUTF8;
}

void ParsResText::ReleaseResItem(ResLang_Item *ResItem)
{
    int ns = ResItem->_strLans.size();
    for (int i = 0 ; i < ns; i++)
    {
        ResLangStr &resStr = ResItem->_strLans.at(i);
        ReleaseResStr(resStr);
    }
    ResItem->_strLans.clear();
    delete ResItem;
}



void ParsResText::Release()
{
    int ns = _MulItems.size();
    for (int i = 0; i < ns; i++)
    {
        ReleaseMulan(_MulItems.at(i));
    }

    ReleaseRes();
    _MulItems.clear();

}

void ParsResText::ReleaseRes()
{
    int ns = _ResItems.size();
    for (int i = 0; i < ns; i ++)
    {
        ReleaseResItem(_ResItems.at(i));
    }
    _ResItems.clear();
}

void ParsResText::SperateItem(const MulStr &mulStr, MultLan_Item *pItem)
{
    DWORD dwLen = wcslen(mulStr.pWcStr) * 2 + 2;
    if (dwLen < 2)
        return;

    WCHAR *pWTmp = mulStr.pWcStr;
    int nIndex = 0;
    pItem->_items.clear();
    int nBegin = 0;

    while (nIndex <= dwLen)
    {
        WCHAR pTest = pWTmp[nIndex];
        if (pTest == WCHAREND)
        {
            MulStr nMulStr;
            int nSecLen = nIndex - nBegin;
            nMulStr.dwLen = nSecLen;
            nMulStr.pWcStr = new WCHAR[nSecLen + 1];
            memcpy(nMulStr.pWcStr, pWTmp + nBegin, nSecLen * sizeof(WCHAR));
            nMulStr.pWcStr[nSecLen] = WCHAREND;
            pItem->_items.push_back(nMulStr);
            break;
        }

        if (pTest == WCHARTAB)
        {
            int nSecLen = nIndex - nBegin;
            if (nSecLen <= 0)
            {
                break;
            }
            MulStr nMulStr;
            nMulStr.dwLen = nSecLen;
            nMulStr.pWcStr = new WCHAR[nSecLen + 1];
            memcpy(nMulStr.pWcStr, pWTmp + nBegin, nSecLen * sizeof(WCHAR));
            nMulStr.pWcStr[nSecLen] = WCHAREND;
            pItem->_items.push_back(nMulStr);
            nIndex++;
            nBegin = nIndex;
            continue;
        }
        nIndex++;
    }

}


int ParsResText::GetLangName(vector<CString> &strLanNs)
{
    int ns = _MulItems.size();
    if (ns == 0)
        return __LINE__;
    strLanNs.clear();
    MultLan_Item *pMulItem = _MulItems.at(0);  // 第一条记录正式语言名称的描述
    ns = pMulItem->_items.size();

    if (ns == 0)
        return __LINE__;

    strLanNs.resize(ns - 1);
    int i;
    for (i = 1; i < ns; i++)
    {
        MulStr &mulsTmp = pMulItem->_items.at(i);
        WCHAR *szCh = mulsTmp.pWcStr;
        TCHAR szName[MAX_PATH] = _T("");
        if (CheckLocalStr(szCh, szName) == FALSE)
        {
            return __LINE__;
        }

        strLanNs.at(i - 1) = szName;
    }
    ns = strLanNs.size();
    while (i < strLanNs.size())
    {
        CString &cs = strLanNs.at(i);
        TrimRight(cs);
        TrimLeft(cs);
        TrimRight(cs, '\t');
        if (cs.empty())
        {
            strLanNs.erase(strLanNs.begin() + i);
            continue;
        }
        i++;
    }

    return 0;
}

int ParsResText::GenResStruct()
{
    int nsStrCnt = _MulItems.size() - 1; // 得到字符串的总数
    ReleaseRes();
    if (nsStrCnt == 0)
        return __LINE__;

    vector<CString> csLanNames;
    int nRet = GetLangName(csLanNames);
    if (nRet != 0)
        return __LINE__;

    LoadResLangInfo();
    int nLanCnt = _ResLanInfo.size();// csLanNames.size();

    _ResItems.resize(nLanCnt);

    for (int i = 0; i < nLanCnt; i++)
    {
        ResLang_Item *pResLanItem = new ResLang_Item;
        _ResItems.at(i) = pResLanItem;

        pResLanItem->_strLans.reserve(1024 * 8);

        pResLanItem->_strLans.clear();

        pResLanItem->nLangID = _ResLanInfo.at(i).nLangID;

        pResLanItem->_strLans.resize(nsStrCnt);

        for (int j = 0; j < nsStrCnt; j++)
        {
            ResLangStr &resStrTmp = pResLanItem->_strLans.at(j);
            MultLan_Item *pOrgMul = _MulItems.at(j + 1);
            MulStr &mulStr = pOrgMul->_items.at(i + 1);
            memset(&resStrTmp, 0, sizeof(resStrTmp));


            resStrTmp.lpUTF8 = new BYTE[mulStr.dwLen * 4];
            memset(resStrTmp.lpUTF8, 0, mulStr.dwLen * 4);
            resStrTmp.nStrID = j + 1;
            //Unicode2UFT8(mulStr.pWcStr, resStrTmp.lpUTF8, &resStrTmp.dwLen);
        }
    }
}



int ParsResText::LoadResLangInfo()
{
    vector<CString> csLangName;
    this->GetLangName(csLangName);
    int nLanCnt = csLangName.size();
    _ResLanInfo.clear();
    _ResLanInfo.resize(nLanCnt);
    for (int i = 0; i < nLanCnt; i++)
    {
        ResLang_Info &reslinfo = _ResLanInfo.at(i);
        CString csLang = csLangName.at(i);
        Upper(&csLang);
        int nUl = csLang.find('_', 0);
        if (nUl != -1)
        {
            CString csID = csLang.substr(nUl, csLang.length());
            reslinfo.nLangID = _tcstoul(csID.c_str(), 16);
            _stprintf(reslinfo.szLangName, _T("LANG_%s_TYPE"), csLang.substr(0, nUl).c_str());
        }
        else
        {
            _tcscpy(reslinfo.szLangName, csLang.c_str());
            reslinfo.nLangID = 0;
        }
    }
    return 0;
}
/*
int ParsResText::ExportResBinFile(LPCTSTR lpFileName)
{

    GenResStruct();

    //////////////////////////////////////////////////////////////////////////
    // Write Head
    FILE* fp = bfOpen(lpFileName, _T("wb"), 1);
    LANG_HEAD m_LHead;
    char *m_pbuffer;


    int nLangCnt = _ResLanInfo.size();
    int nStrCnt = _MulItems.size() - 1;
    memcpy(m_LHead.SecName, SEC_NAME, 8);
    m_LHead.version     = _ResLanCfg.Version;// pConfig->Version;
    m_LHead.size        = sizeof(LANG_HEAD);
    m_LHead.LangTabOff  = LANGLISTSTART;
    m_LHead.LangSize    = LangListStuctLength;
    m_LHead.LangNum     = nLangCnt;
    m_LHead.StringSize  = StringListStructLength;
    m_LHead.StringNum   = nStrCnt;
    m_LHead.align       = 4;    //文件按照4字节方式对齐
    m_LHead.flags       = 0;    //该字段保留

    m_pbuffer = (char *)malloc(sizeof(__s8)*(LANGLISTSTART));
    memset(m_pbuffer, 0, sizeof(__s8)*(LANGLISTSTART));
    memcpy(m_pbuffer, &m_LHead, sizeof(m_LHead));

    fwrite(m_pbuffer, sizeof(__s8), (LANGLISTSTART), fp);
    fflush(fp);
    free(m_pbuffer);

    //////////////////////////////////////////////////////////////////////////
    // Write Strings


    int dataadd;
    LANG m_LDNode;
    m_pbuffer = (char *)malloc(sizeof(__s8)*LangListStuctLength);

    dataadd = LANGLISTSTART+ LangListStuctLength*nLangCnt+
              nLangCnt*StringListStructLength*(nStrCnt);

    for(int i=0; i<nLangCnt; i++)
    {
        STRING m_Str;
        ResLang_Item* pItem = _ResItems.at(i);
        fseek(fp, LANGLISTSTART+LangListStuctLength*i, SEEK_SET);

    //  memcpy(m_LDNode.Name, pDirnode[i].LangName, sizeof(m_LDNode.Name));
        m_LDNode.LangID             = pItem->nLangID;

        m_LDNode.LangOffSet         = LANGLISTSTART+ LangListStuctLength*i;
        m_LDNode.LangSize           = LangListStuctLength;
        m_LDNode.StringTabOff       = LANGLISTSTART+ LangListStuctLength*nLangCnt+
                                      i*StringListStructLength*(pItem->_strLans.size());

        memset(m_pbuffer, 0, LangListStuctLength);
        memcpy(m_pbuffer, &m_LDNode, sizeof(m_LDNode));

        fwrite(m_pbuffer, sizeof(__s8), LangListStuctLength, fp);
        fflush(fp);

        fseek(fp, m_LDNode.StringTabOff, SEEK_SET);
        char * pStrTBuff = (char *)malloc(sizeof(__s8)*StringListStructLength);

        for(int j = 0; j < nStrCnt; j++)
        {
            char * s_pbuffer;
            ResLangStr& pStr =  pItem->_strLans.at(j);
            fseek(fp, m_LDNode.StringTabOff+ j*StringListStructLength, SEEK_SET);
            m_Str.LangID = pItem->nLangID;
            //  memcpy(m_Str.Name, pSLNode[i].StringName, sizeof(m_Str.Name));
            m_Str.StringID = pStr.nStrID;
            m_Str.size     = pStr.dwLen;
            m_Str.offset   = dataadd;
            m_Str.x = pStr.x;
            m_Str.y = pStr.y;

            memset(pStrTBuff, 0, StringListStructLength);
            memcpy(pStrTBuff, &m_Str, sizeof(m_Str));
            fwrite(pStrTBuff, sizeof(__s8), StringListStructLength, fp);
            fflush(fp);


            fseek(fp, m_Str.offset, SEEK_SET );
            int nSj = ((m_Str.size +3)/4)*4;

            s_pbuffer = (char *)malloc(sizeof(__s8)*nSj);
            memset(s_pbuffer, 0, nSj);
            memcpy(s_pbuffer, pStr.lpUTF8, pStr.dwLen);
            fwrite(s_pbuffer, sizeof(__s8), nSj, fp);
            fflush(fp);

            free(s_pbuffer);

            dataadd += ((m_Str.size + 3)/4)*4;

        }
        free(pStrTBuff);

    }
    free(m_pbuffer);

    fclose(fp);
    return 0;
}

*/

MultLan_Item *ParsResText::GetLangItem(int nIndex)
{
    int nRealIndex = nIndex + 1;
    if (nRealIndex >= _MulItems.size())
    {
        return NULL;
    }
    return _MulItems.at(nRealIndex);
}

int ParsResText::GetLangItemCnt()
{
    return _MulItems.size() - 1;
}

int ParsResText::GetLangTypeCnt()
{
    return _ResLanInfo.size();
}

void ParsResText::GetLangInfo(int nIndex, ResLang_Info &info)
{
    if (nIndex >= _ResLanInfo.size())
        return;
    memcpy(&info, &_ResLanInfo.at(nIndex), sizeof(ResLang_Info));
}

void ParsResText::UpdatePosInfo()
{

}

MultLan_Item *ParsResText::GetRawItem(int nIndex)
{
    return _MulItems.at(nIndex);
}

int ParsResText::GetRawItemCnt()
{
    return _MulItems.size();
}
