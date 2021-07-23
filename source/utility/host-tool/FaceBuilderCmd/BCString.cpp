// BCString.cpp: implementation of the CBCString class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "IniParasPlg.h"

#ifndef _BCSTRING_CPP
#define _BCSTRING_CPP

#include <xcomplier_def.h>
#include "BCString.h"
#include <assert.h>
#include <string.h>

// Construction/Destruction
CBCString::CBCString()
{
    m_pchData = NULL;
}

CBCString::~CBCString()
{
    if (m_pchData)
        delete m_pchData;
}

CBCString::CBCString(const CBCString &stringSrc)
{
    int nLen = stringSrc.GetLength() + 1;
    m_pchData = new char[nLen];
    memset(m_pchData, 0x00, nLen);
    strcpy(m_pchData, stringSrc);
}

// from an ANSI string (converts to TCHAR)
CBCString::CBCString(LPCBSTR lpsz)
{
    int nLen = _tcslen(lpsz) + 1;
    m_pchData = new char[nLen];
    memset(m_pchData, 0x00, nLen);
    strcpy(m_pchData, lpsz);
}

// subset of characters from an ANSI string (converts to TCHAR)
CBCString::CBCString(LPCBSTR lpch, int nLength)
{
    int nLen = nLength + 1;
    m_pchData = new char[nLen];
    memset(m_pchData, 0x00, nLen);
    memcpy(m_pchData, lpch, nLen);
}

// Attributes & Operations

// get data length
int CBCString::GetLength() const
{
    return _tcslen(m_pchData);
}
// TRUE if zero length
BOOL CBCString::IsEmpty() const
{
    int nLen = _tcslen(m_pchData);
    if (nLen == 0)
    {
        return TRUE;
    }
    else
        return FALSE;
}
// clear contents to empty
void CBCString::Empty()
{
    int nLen = _tcslen(m_pchData);
    memset(m_pchData, 0x00, nLen);
}

// return single character at zero-based index
TCHAR CBCString::GetAt(int nIndex) const
{
    return m_pchData[nIndex];
}

// return single character at zero-based index
TCHAR CBCString::operator[](int nIndex) const
{
    return m_pchData[nIndex];
}

// set a single character at zero-based index
void CBCString::SetAt(int nIndex, TCHAR ch)
{
    m_pchData[nIndex] = ch;
}
// return pointer to const string
CBCString::operator LPCBTSTR() const
{
    return m_pchData;
}

// ref-counted copy from another CString
const CBCString &CBCString::operator=(const CBCString &stringSrc)
{
    if (m_pchData)
        delete m_pchData;
    int nLen = stringSrc.GetLength() + 1;
    m_pchData = new char[nLen];
    memset(m_pchData, 0x00, nLen);
    strcpy(m_pchData, stringSrc);
    return *this;
}

// set string content to single character
const CBCString &CBCString::operator=(TCHAR ch)
{
    if (m_pchData == NULL)
    {
        int nLen = 2;
        m_pchData = new char[nLen];
        memset(m_pchData, 0x00, nLen);
    }
    m_pchData[0] = ch;
    m_pchData[1] = '\0';
    return *this;

}

// copy string content from ANSI string (converts to TCHAR)
const CBCString &CBCString::operator=(LPCBTSTR lpsz)
{
    if (m_pchData)
        delete m_pchData;
    int nLen = _tcslen(lpsz) + 1;
    m_pchData = new char[nLen];
    memset(m_pchData, 0x00, nLen);
    strcpy(m_pchData, lpsz);
    return *this;
}

// string concatenation
// concatenate from another CString
const CBCString &CBCString::operator+=(const CBCString &string)
{
    int nLen = string.GetLength() + 1;
    int nOrgLen = GetLength();

    char *pChNew = new char[nLen + nOrgLen];
    memset(pChNew, 0x00, nLen + nOrgLen);
    strcpy(pChNew, m_pchData);
    strcat(pChNew, string);

    delete m_pchData;
    m_pchData = pChNew;
    return *this;
}

// concatenate a single character
const CBCString &CBCString::operator+=(TCHAR ch)
{
    int nLen = 2;
    int nOrgLen = GetLength();

    char *pChNew = new char[nLen + nOrgLen];
    memset(pChNew, 0x00, nLen + nOrgLen);
    strcpy(pChNew, m_pchData);
    pChNew[nOrgLen] = ch;

    delete m_pchData;
    m_pchData = pChNew;
    return *this;
}

// concatenate a UNICODE character after converting it to TCHAR
const CBCString &CBCString::operator+=(LPCBTSTR lpsz)
{

    int nLen = _tcslen(lpsz) + 1;
    int nOrgLen = GetLength();

    char *pChNew = new char[nLen + nOrgLen];
    memset(pChNew, 0x00, nLen + nOrgLen);
    strcpy(pChNew, m_pchData);
    strcat(pChNew, lpsz);

    delete m_pchData;
    m_pchData = pChNew;
    return *this;
}

// straight character comparison
int CBCString::Compare(LPCBTSTR lpsz) const
{
    return strcmp(lpsz, m_pchData);
}
// compare ignoring case
int CBCString::CompareNoCase(LPCBTSTR lpsz) const
{
    CBCString csTmp = lpsz;
    CBCString csThis = *this;
    csTmp.MakeLower();
    csThis.MakeLower();
    return strcmp(csTmp, csThis);
}

// return nCount characters starting at zero-based nFirst
CBCString CBCString::Mid(int nFirst, int nCount) const
{
    CBCString csTmp;

    int nLen = nCount + 1;
    char *pch = new char[nLen];
    memset(pch, 0x00, nLen);
    memcpy(pch, m_pchData + nFirst, nCount);
    csTmp = pch;
    delete pch;
    return csTmp;
}

// return all characters starting at zero-based nFirst
CBCString CBCString::Mid(int nFirst) const
{
    CBCString csTmp;

    int nLen = GetLength() - nFirst + 1;
    char *pch = new char[nLen];
    memset(pch, 0x00, nLen);
    memcpy(pch, m_pchData + nFirst, nLen - 1);
    csTmp = pch;
    delete pch;
    return csTmp;
}

// return first nCount characters in string
CBCString CBCString::Left(int nCount) const
{
    CBCString csTmp;
    int nLen = nCount + 1;
    char *pch = new char[nLen];
    memset(pch, 0x00, nLen);
    memcpy(pch, m_pchData, nLen - 1);
    csTmp = pch;
    delete pch;
    return csTmp;
}

// return nCount characters from end of string
CBCString CBCString::Right(int nCount) const
{
    CBCString csTmp;
    int nLen = nCount + 1;
    char *pch = new char[nLen];
    memset(pch, 0x00, nLen);
    memcpy(pch, m_pchData + GetLength() - nCount, nCount);
    csTmp = pch;
    delete pch;
    return csTmp;
}

// NLS aware conversion to uppercase
void CBCString::MakeUpper()
{
    int nLen = GetLength() + 1;
    char *pCh = new char [nLen];
    memset(pCh, 0, nLen);
    int i;
    for (i = 0 ; i < nLen ; i++)
    {
        pCh[i] = toupper(m_pchData[i]);
    }
    m_pchData = pCh;

    //  _strupr(m_pchData);
}
// NLS aware conversion to lowercase

void CBCString::MakeLower()
{
    int nLen = GetLength() + 1;
    char *pCh = new char [nLen];
    memset(pCh, 0, nLen);
    int i;
    for (i = 0 ; i < nLen ; i++)
    {
        pCh[i] = tolower(m_pchData[i]);
    }
    m_pchData = pCh;

    //  _strlwr(m_pchData);
}

// reverse string right-to-left
void CBCString::MakeReverse()
{
    int nLen = GetLength() + 1;
    char *pTm = new char[nLen];
    memset(pTm, 0x00, nLen);

    int nIndex = GetLength() - 1;
    int nNewIndex = 0;
    while (nIndex >= 0)
    {
        pTm[nNewIndex++] = m_pchData[nIndex];
        nIndex--;
    }
    delete m_pchData;
    m_pchData = pTm;

}
// remove whitespace starting from right edge
void CBCString::TrimRight()
{
    int nLen = GetLength() + 1;
    int nIndex = GetLength() - 1;
    while (nIndex >= 0)
    {
        if (m_pchData[nIndex] == ' ')
        {
            m_pchData[nIndex] = '\0';
            nIndex--;
            continue;
        }
        else
            break;
    }
}

// remove whitespace starting from left side
void CBCString::TrimLeft()
{
    int nLen = GetLength() + 1;
    char *pTm = new char[nLen];
    memset(pTm, 0x00, nLen);
    /*
        int nIndex = GetLength() - 1;
        int nNewIndex = 0;
    */
    int nIndex = 0;
    while (nIndex < nLen)
    {
        if (m_pchData[nIndex] == ' ')
        {
            nIndex++;
            continue;
        }
        break;
    }
    if (nIndex == nLen)
    {
        memset(m_pchData, 0x00, nLen);
        delete m_pchData;
        return;
    }
    memcpy(pTm, m_pchData + nIndex, nLen - nIndex - 1);

    delete m_pchData;
    m_pchData = pTm;
}

// trimming anything (either side)

// remove continuous occurrences of chTarget starting from right
void CBCString::TrimRight(TCHAR chTarget)
{
    int nLen = GetLength() + 1;
    int nIndex = GetLength() - 1;
    while (nIndex >= 0)
    {
        if (m_pchData[nIndex] == chTarget)
        {
            m_pchData[nIndex] = '\0';
            nIndex--;
            continue;
        }
        else
            break;
    }
}
// remove continuous occcurrences of characters in passed string,
// starting from right
void CBCString::TrimRight(LPCBTSTR lpszTargets)
{
    LPBTSTR lpsz = m_pchData;
    LPBTSTR lpszLast = NULL;

    while (*lpsz != '\0')
    {
        if (_tcschr(lpszTargets, *lpsz) != NULL)
        {
            if (lpszLast == NULL)
                lpszLast = lpsz;
        }
        else
            lpszLast = NULL;
        lpsz = _tcsinc(lpsz);
    }

    if (lpszLast != NULL)
    {
        // truncate at left-most matching character
        *lpszLast = '\0';
    }
}
// remove continuous occurrences of chTarget starting from left

void CBCString::TrimLeft(TCHAR chTarget)
{
    int nLen = GetLength() + 1;
    char *pTm = new char[nLen];
    memset(pTm, 0x00, nLen);
    /*
        int nIndex = GetLength() - 1;
        int nNewIndex = 0;
    */
    int nIndex = 0;
    while (nIndex < nLen)
    {
        if (m_pchData[nIndex] == chTarget)
        {
            nIndex++;
            continue;
        }
        break;
    }
    if (nIndex == nLen)
    {
        memset(m_pchData, 0x00, nLen);
        delete m_pchData;
        return;
    }
    memcpy(pTm, m_pchData + nIndex, nLen - nIndex - 1);

    delete m_pchData;
    m_pchData = pTm;
}

// advanced manipulation
// replace occurrences of chOld with chNew
int CBCString::Replace(TCHAR chOld, TCHAR chNew)
{
    LPBTSTR lpsz = m_pchData;

    int nCount = 0;
    while (*lpsz != '\0')
    {
        if (*lpsz == chOld)
        {
            nCount++;
            *lpsz = chNew;
        }
        lpsz++;
    }
    return nCount;
}
// replace occurrences of substring lpszOld with lpszNew;
// empty lpszNew removes instances of lpszOld
int CBCString::Replace(LPCBTSTR lpszOld, LPCBTSTR lpszNew)
{
    int nSourceLen = _tcslen(lpszOld);
    if (nSourceLen == 0)
        return 0;
    int nReplacementLen = _tcslen(lpszNew);
    CBCString csTmp;

    // loop once to figure out the size of the result string
    int nCount = 0;
    LPBTSTR lpszStart = m_pchData;
    LPBTSTR lpszEnd = m_pchData + GetLength();
    LPBTSTR lpszTarget;
    while (lpszStart < lpszEnd)
    {
        lpszTarget = _tcsstr(lpszStart, lpszOld);
        if (lpszStart == NULL)
        {
            csTmp += lpszStart;
            break;
        }
        else
        {
            int nChCount = lpszTarget - lpszStart;
            char *pTmp = new char[nCount + 1 + nReplacementLen] ;
            memset(pTmp, 0x00, nChCount + 1);
            memcpy(pTmp, lpszStart, nChCount);
            strcat(pTmp, lpszNew);
            csTmp += pTmp;
            delete pTmp;
            nCount++;
        }
        lpszStart = lpszTarget + nSourceLen;
    }

    delete m_pchData;
    m_pchData = new char[csTmp.GetLength() + 1];
    memcpy(m_pchData, (LPCBTSTR)csTmp, csTmp.GetLength());
    return nCount;
}

// remove occurrences of chRemove
int CBCString::Remove(TCHAR chRemove)
{
    int nLen = GetLength() + 1;
    char *pTm = new char[nLen];
    memset(pTm, 0x00, nLen);

    int nIndex = GetLength() - 1;
    int nNewIndex = 0;
    int nCount = 0;
    while (nIndex < nLen)
    {
        if (m_pchData[nIndex] == chRemove)
        {
            nIndex++;
            nCount++;
            continue;
        }
        pTm[nNewIndex++] = m_pchData[nIndex];
        nIndex++;
        break;
    }
    memset(m_pchData, 0x00, nLen);
    strcpy(m_pchData, pTm);
    delete pTm;
    return nCount;

}
// insert character at zero-based index; concatenates
// if index is past end of string

int CBCString::Insert(int nIndex, TCHAR ch)
{
    int nLen = GetLength() + 2;
    char *pTm = new char[nLen];
    memset(pTm, 0x00, nLen);
    memcpy(pTm, m_pchData, nIndex);
    pTm[nIndex] = ch;
    memcpy(pTm + nIndex + 1, m_pchData + nIndex, GetLength() - nIndex);
    delete m_pchData;
    m_pchData = pTm;
    return 0;
}
// insert substring at zero-based index; concatenates
// if index is past end of string
int CBCString::Insert(int nIndex, LPCBTSTR pstr)
{
    int nSubLen = _tcslen(pstr);
    int nLen = GetLength() + 1 + nSubLen;
    char *pTm = new char[nLen];
    memset(pTm, 0x00, nLen);
    memcpy(pTm, m_pchData, nIndex);
    strcpy(pTm, pstr);
    memcpy(pTm + nIndex + nSubLen, m_pchData + nIndex, GetLength() - nIndex);

    delete m_pchData;
    m_pchData = pTm;
    return 0;
}
// delete nCount characters starting at zero-based index
int CBCString::Delete(int nIndex, int nCount)
{
    int nLen = GetLength() + 1;
    char *pTm = new char[nLen];
    memset(pTm, 0x00, nLen);
    memcpy(pTm, m_pchData, nIndex);

    memcpy(pTm + nIndex, m_pchData + nIndex + nCount, GetLength() - nIndex - nCount);
    memset(m_pchData, 0x00, nLen);
    strcpy(m_pchData, pTm);
    delete pTm;
    return 0;
}

// searching

// find character starting at left, -1 if not found
int CBCString::Find(TCHAR ch) const
{
    return Find(ch, 0);

}

// find character starting at right
int CBCString::ReverseFind(TCHAR ch) const
{
    int nLength = GetLength();
    int nIndex = nLength - 1;
    while (nIndex >= 0)
    {
        if (m_pchData[nIndex] == ch)
        {
            return nIndex;
        }
        nIndex--;
    }
    return -1;
}
// find character starting at zero-based index and going right
int CBCString::Find(TCHAR ch, int nStart) const
{
    int nLength = GetLength();
    if (nStart >= nLength)
        return -1;

    // find first single character
    LPBTSTR lpsz = _tcschr(m_pchData + nStart, (_TUCHAR)ch);

    // return -1 if not found and index otherwise
    return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

// find first instance of substring
int CBCString::Find(LPCBTSTR lpszSub) const
{
    return Find(lpszSub, 0);
}

// find first instance of substring starting at zero-based index
int CBCString::Find(LPCBTSTR lpszSub, int nStart) const
{

    // loop once to figure out the size of the result string
    int nCount = 0;
    LPBTSTR lpszStart = m_pchData + nStart;
    LPBTSTR lpszEnd = m_pchData + GetLength();
    LPBTSTR lpszTarget;
    while (lpszStart < lpszEnd)
    {
        lpszTarget = _tcsstr(lpszStart, lpszSub);
        if (lpszStart == NULL)
        {
            return -1;
        }
        else
        {
            return lpszTarget - lpszStart;
        }
    }
    return -1;

}

// simple formatting
// printf-like formatting using passed string
void CBCString::Format(LPCBTSTR lpszFormat, ...)
{
    va_list argList;
    va_start(argList, lpszFormat);
    FormatV(lpszFormat, argList);
    va_end(argList);
}

// printf-like formatting using variable arguments parameter
void CBCString::FormatV(LPCBTSTR lpszFormat, va_list argList)
{

    va_list argListSave;// = argList;
    va_copy(argListSave, argList);
    va_end(argListSave);


    // make a guess at the maximum length of the resulting string
    int nMaxLen = 0;
    for (LPCBTSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
    {
        // handle '%' character, but watch out for '%%'
        if (*lpsz != '%' || *(lpsz = _tcsinc(lpsz)) == '%')
        {
            nMaxLen += _tclen(lpsz);
            continue;
        }

        int nItemLen = 0;

        // handle '%' character with format
        int nWidth = 0;
        for (; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
        {
            // check for valid flags
            if (*lpsz == '#')
                nMaxLen += 2;   // for '0x'
            else if (*lpsz == '*')
                nWidth = va_arg(argList, int);
            else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
                     *lpsz == ' ')
                ;
            else // hit non-flag character
                break;
        }
        // get width and skip it
        if (nWidth == 0)
        {
            // width indicated by
            nWidth = _ttoi(lpsz);
            for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
                ;
        }
        assert(nWidth >= 0);

        int nPrecision = 0;
        if (*lpsz == '.')
        {
            // skip past '.' separator (width.precision)
            lpsz = _tcsinc(lpsz);

            // get precision and skip it
            if (*lpsz == '*')
            {
                nPrecision = va_arg(argList, int);
                lpsz = _tcsinc(lpsz);
            }
            else
            {
                nPrecision = _ttoi(lpsz);
                for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
                    ;
            }
            assert(nPrecision >= 0);
        }

        // should be on type modifier or specifier

        // now should be on specifier
        switch (*lpsz)
        {
                // single characters
            case 'c':
            case 'C':
                nItemLen = 2;
                va_arg(argList, int);
                break;

                // strings
            case 's':
            {
                LPCTSTR pstrNextArg = va_arg(argList, LPCBTSTR);
                if (pstrNextArg == NULL)
                    nItemLen = 6;  // "(null)"
                else
                {
                    nItemLen = lstrlen(pstrNextArg);
                    nItemLen = max(1, nItemLen);
                }
            }
            break;
        }
        // adjust nItemLen for strings
        if (nItemLen != 0)
        {
            if (nPrecision != 0)
                nItemLen = min(nItemLen, nPrecision);
            nItemLen = max(nItemLen, nWidth);
        }
        else
        {
            switch (*lpsz)
            {
                    // integers
                case 'd':
                case 'i':
                case 'u':
                case 'x':
                case 'X':
                case 'o':
                    va_arg(argList, int);
                    nItemLen = 32;
                    nItemLen = max(nItemLen, nWidth + nPrecision);
                    break;
                case 'e':
                case 'g':
                case 'G':
                    va_arg(argList, double);
                    nItemLen = 128;
                    nItemLen = max(nItemLen, nWidth + nPrecision);
                    break;

                case 'f':
                {
                    double f;
                    LPBTSTR pszTemp;

                    // 312 == strlen("-1+(309 zeroes).")
                    // 309 zeroes == max precision of a double
                    // 6 == adjustment in case precision is not specified,
                    //   which means that the precision defaults to 6
                    pszTemp = (LPBTSTR)malloc(max(nWidth, 312 + nPrecision + 6));

                    f = va_arg(argList, double);
                    _stprintf(pszTemp, _T("%*.*f"), nWidth, nPrecision + 6, f);
                    nItemLen = _tcslen(pszTemp);
                }
                break;

                case 'p':
                    va_arg(argList, void *);
                    nItemLen = 32;
                    nItemLen = max(nItemLen, nWidth + nPrecision);
                    break;
                    // no output
                case 'n':
                    va_arg(argList, int *);
                    break;

                default:
                    assert(FALSE);  // unknown formatting option
            }
        }
        // adjust nMaxLen for output nItemLen
        nMaxLen += nItemLen;
    }

    if (m_pchData)
        delete m_pchData;
    m_pchData = new char[nMaxLen];

    _vstprintf(m_pchData, lpszFormat, argList);

    va_end(argList);
}

// get pointer to modifiable buffer at least as long as nMinBufLength
LPBTSTR CBCString::GetBuffer(int nMinBufLength)
{
    return m_pchData;
}

#endif //_BCSTRING_CPP
