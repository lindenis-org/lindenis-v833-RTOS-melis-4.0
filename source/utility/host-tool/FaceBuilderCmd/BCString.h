// BCString.h: interface for the CBCString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCSTRING_H__CDA98304_350C_4C27_9990_280D252C4CDE__INCLUDED_)
#define AFX_BCSTRING_H__CDA98304_350C_4C27_9990_280D252C4CDE__INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
/*
typedef char CHAR;
typedef unsigned short wchar_t;
typedef wchar_t WCHAR;
*/
#include <stdio.h>

typedef char TCHAR;
typedef bool BOOL;
typedef const char *LPCBCH, *PCBCH;
//typedef char *LPBSTR, *PBSTR;
typedef const char *LPCBSTR, *PCBSTR;

typedef const wchar_t *LPCBWCH, *PCBWCH;
typedef wchar_t *LPBWSTR, *PBWSTR;
typedef const wchar_t *LPCBWSTR, *PCBWSTR;
typedef unsigned int DWORD;

//typedef char * va_list;
//#define NULL 0

#ifdef UNICODE

typedef const wchar_t *LPCBTCH, *PCBTCH;
typedef wchar_t *LPBTSTR, *PBTSTR;
typedef const wchar_t *LPCBTSTR, *PCBTSTR;
#else // #ifdef UNICODE

typedef const char *LPCBTCH, *PCBTCH;
typedef char *LPBTSTR, *PBTSTR;
typedef const char *LPCBTSTR, *PCBTSTR;
#endif  //UNICODE


class CBCString
{
public:
// Constructors

    // constructs empty CString
    CBCString();
    // copy constructor
    CBCString(const CBCString &stringSrc);
    // from an ANSI string (converts to TCHAR)
    CBCString(LPCBSTR lpsz);
    // subset of characters from an ANSI string (converts to TCHAR)
    CBCString(LPCBSTR lpch, int nLength);
    // subset of characters from a UNICODE string (converts to TCHAR)

    // Attributes & Operations

    // get data length
    int GetLength() const;
    // TRUE if zero length
    BOOL IsEmpty() const;
    // clear contents to empty
    void Empty();

    // return single character at zero-based index
    TCHAR GetAt(int nIndex) const;
    // return single character at zero-based index
    TCHAR operator[](int nIndex) const;
    // set a single character at zero-based index
    void SetAt(int nIndex, TCHAR ch);
    // return pointer to const string
    operator LPCBTSTR() const;

    // overloaded assignment

    // ref-counted copy from another CString
    const CBCString &operator=(const CBCString &stringSrc);
    // set string content to single character
    const CBCString &operator=(TCHAR ch);

    // copy string content from ANSI string (converts to TCHAR)
    const CBCString &operator=(LPCBTSTR lpsz);
    // concatenate from another CString
    const CBCString &operator+=(const CBCString &string);

    // concatenate a single character
    const CBCString &operator+=(TCHAR ch);

    // concatenate a UNICODE character after converting it to TCHAR
    const CBCString &operator+=(LPCBTSTR lpsz);


    // straight character comparison
    int Compare(LPCBTSTR lpsz) const;
    // compare ignoring case
    int CompareNoCase(LPCBTSTR lpsz) const;

    // simple sub-string extraction

    // return nCount characters starting at zero-based nFirst
    CBCString Mid(int nFirst, int nCount) const;
    // return all characters starting at zero-based nFirst
    CBCString Mid(int nFirst) const;
    // return first nCount characters in string
    CBCString Left(int nCount) const;
    // return nCount characters from end of string
    CBCString Right(int nCount) const;

    // upper/lower/reverse conversion

    // NLS aware conversion to uppercase
    void MakeUpper();
    // NLS aware conversion to lowercase
    void MakeLower();
    // reverse string right-to-left
    void MakeReverse();

    // trimming whitespace (either side)

    // remove whitespace starting from right edge
    void TrimRight();
    // remove whitespace starting from left side
    void TrimLeft();

    // trimming anything (either side)

    // remove continuous occurrences of chTarget starting from right
    void TrimRight(TCHAR chTarget);
    // remove continuous occcurrences of characters in passed string,
    // starting from right
    void TrimRight(LPCBTSTR lpszTargets);
    // remove continuous occurrences of chTarget starting from left
    void TrimLeft(TCHAR chTarget);
    // remove continuous occcurrences of characters in
    // passed string, starting from left
    void TrimLeft(LPCBTSTR lpszTargets);

    // advanced manipulation

    // replace occurrences of chOld with chNew
    int Replace(TCHAR chOld, TCHAR chNew);
    // replace occurrences of substring lpszOld with lpszNew;
    // empty lpszNew removes instances of lpszOld
    int Replace(LPCBTSTR lpszOld, LPCBTSTR lpszNew);
    // remove occurrences of chRemove
    int Remove(TCHAR chRemove);
    // insert character at zero-based index; concatenates
    // if index is past end of string
    int Insert(int nIndex, TCHAR ch);
    // insert substring at zero-based index; concatenates
    // if index is past end of string
    int Insert(int nIndex, LPCBTSTR pstr);
    // delete nCount characters starting at zero-based index
    int Delete(int nIndex, int nCount = 1);

    // searching

    // find character starting at left, -1 if not found
    int Find(TCHAR ch) const;
    // find character starting at right
    int ReverseFind(TCHAR ch) const;
    // find character starting at zero-based index and going right
    int Find(TCHAR ch, int nStart) const;
    // find first instance of any character in passed string
    int FindOneOf(LPCBTSTR lpszCharSet) const;
    // find first instance of substring
    int Find(LPCBTSTR lpszSub) const;
    // find first instance of substring starting at zero-based index
    int Find(LPCBTSTR lpszSub, int nStart) const;

    // simple formatting

    // printf-like formatting using passed string
    void Format(LPCBTSTR lpszFormat, ...);

    // printf-like formatting using variable arguments parameter
    void FormatV(LPCBTSTR lpszFormat, va_list argList);

    // get pointer to modifiable buffer at least as long as nMinBufLength
    LPBTSTR GetBuffer(int nMinBufLength);


// Implementation
public:
    ~CBCString();

protected:
    LPBTSTR m_pchData;   // pointer to ref counted string data
    DWORD   m_dwBuffLen;
};

#endif // !defined(AFX_BCSTRING_H__CDA98304_350C_4C27_9990_280D252C4CDE__INCLUDED_)
