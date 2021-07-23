#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#if defined(WIN32)
#include <windows.h>
//#elif defined(linux)
//#elif defined(_AIX)
//#include <extension.h>
#else

#define STR_PAGE_SIZE   1024
#define UCHAR_MAX   128
#define TRUE        true
#define FALSE       false

#include <string>
using namespace std;

char *strupr(char *src)
{
    while (*src != '\0')
    {
        if (*src >= 'a' && *src <= 'z')
            /*
             * 在ASCII表里大写字符的值比对应小写字符的值小32.
             * *p -= 0x20; // 0x20的十进制就是32
             */
            *src -= 32;
        src++;
    }
    return (src);
}


char *strlwr(char *src)
{
    while (*src != '\0')
    {
        if (*src > 'A' && *src <= 'Z')
        {
            *src += 32;
        }
        src++;
    }
    return (src);
}

/* constructors */
class CString : public string
{
public:
    /* allocate buffers */
    char     *m_pBuffer;
    int m_nLength;
    int m_nSize;
    char    *m_pDataStart;
    char     *m_pDataEnd;

    CString(int iBufferNum)
    {
        m_pBuffer   = new char[iBufferNum * STR_PAGE_SIZE];         /* 动态申请内存 */
        m_nLength   = 0;                                            /* 长度 */
        m_nSize     = iBufferNum * STR_PAGE_SIZE;                   /* 字符串大小初始化为iBufferNum个4*1024 */
        m_pDataStart = m_pDataEnd = m_pBuffer;                      /* 开始字符串指针全部指向m_pBuffer */
    }


    CString()
    {
        m_pBuffer   = new char[STR_PAGE_SIZE];                      /* 动态申请内存 */
        m_nLength   = 0;                                            /* 长度 */
        m_nSize     = STR_PAGE_SIZE;                                /* 字符串大小初始化为iBufferNum个4*1024 */
        m_pDataStart    = m_pDataEnd = m_pBuffer;                       /* 开始字符串指针全部指向m_pBuffer */
    }


    /* new one character */
    CString(char c, int iBufferNum)                                         /* 参数为字符c和iBufferNum，构造函数 */
    {
        char data[2], len = 2;

        data[0] = c;                                                    /* 将c放在字符数组首位 */
        data[1] = 0;                                                    /* 将字符数组data第二位置为0 */
        if (len > iBufferNum * STR_PAGE_SIZE)                           /* 如果iBufferNum个4*1024小于3 */
        {
            m_pBuffer   = new char[len + 1];                    /* 为字符串类的主要字符串m_pBuffer动态申请内存空间 */
            m_nSize     = len + 1;                              /* 字符串大小为3 */
        }
        else
        {
            m_pBuffer   = new char[iBufferNum * STR_PAGE_SIZE]; /* 字符串申请内存空间 */
            m_nSize     = iBufferNum * STR_PAGE_SIZE;
        }

        /* copy data and set total length */
        CreateFromData(data, len);                                      /* 根据字符数组data开始创建对象m_pDataStart和m_pDataEnd */
    }


    /* copy data from a null terminated string */
    CString(const char *data, int iBufferNum)
    {
        long len = strlen(data);

        if (len > iBufferNum * STR_PAGE_SIZE)           /* 字符串的大小大于制定数量个4*1024 */
        {
            m_pBuffer   = new char[len + 1];    /* 动态申请内存空间 */
            m_nSize     = len + 1;
        }
        else
        {
            m_pBuffer   = new char[iBufferNum * STR_PAGE_SIZE];
            m_nSize     = iBufferNum * STR_PAGE_SIZE;
        }

        /* copy data and set total length */
        CreateFromData(data, len);                      /* 根据字符数组data开始创建对象m_pDataStart和m_pDataEnd */
    }


    /* copy data from a binary data */
    CString(const char *data, long len, int iBufferNum)     /* 构造函数的参数为字符串 */
    {
        if (len > iBufferNum * STR_PAGE_SIZE)           /* 如果字符串的大小大于制定数量个4*1024 */
        {
            m_pBuffer   = new char[len + 1];    /* 动态申请内存空间 */
            m_nSize     = len + 1;
        }
        else
        {
            m_pBuffer   = new char[iBufferNum * STR_PAGE_SIZE];
            m_nSize     = iBufferNum * STR_PAGE_SIZE;
        }

        /* copy data and set total length */
        CreateFromData(data, len);   /* 根据字符数组data开始创建对象m_pDataStart和m_pDataEnd */
    }


    /* destructor */
    ~CString(void)
    {
        SafeDestroy();
    }


    /* destroy allocated memory space and init all pointers */
    void SafeDestroy(void)
    {
        /*
         * SAFEDELETE(m_pBuffer);
         * Important for InitPointers()
         */
        m_nSize = 0;
        Empty();
    }


    void Empty(void)
    {
#ifdef WIN32
        ZeroMemory(m_pBuffer, m_nSize);
#else                                                           /* Linux, AIX */
        memset(m_pBuffer, 0, m_nSize);
#endif
        m_pDataStart    = m_pDataEnd = m_pBuffer;       /* 将该字符串的头指针和尾指针全部指向字符串的头部 */
        m_nLength   = 0;                            /* 字符串长度置为0 */
    }


    char *GetData(void) const                               /* 获得指向该字符串的头部指针 */
    {
        return (m_pDataStart);
    }


    int CreateFromData(const char *data, long len)          /* 根据data和长度len创建对象 */
    {
        Empty();                                        /* 清空 */

        if (len <= 0)
            return (TRUE);

        if (data == NULL)
            return (TRUE);

        /* actually, it's impossible */
        if (len > m_nSize)
            return (FALSE);

        /* copy data and set length */
        memcpy(m_pDataStart, data, len);                        /* 将字符数组复制给m_pDataStart指向内存空间 */
        m_nLength       = len;
        m_pDataStart[m_nLength] = '\0';
        m_pDataEnd      = &(m_pDataStart[m_nLength]);   /* 取得尾指针m_pDataEnd */
        return (TRUE);
    }


    long GetLength(void) const
    {
        return (m_nLength);
    }


    int IsEmpty(void) const
    {
        return (!m_nLength);
    }


    int Grow(int iBufferNum)
    {
        if (iBufferNum <= 0)
            return (0);
        AssignNewSpace(m_nSize + iBufferNum * STR_PAGE_SIZE, 1);   /* 分配新的内存空间，变为原来的两倍，移动1倍 */
        return (1);
    }


    int Append(const char *pSrc, int iLen)
    {
        int total_len;
        char    *pNewStart = NULL;

        if (iLen <= 0)
            return (0);

        total_len = m_nLength + iLen;                                                                   /* append后的总的字符串长度 */

        /* if some space avaliable, defrag it firstly. */
        if (m_nSize > total_len)                                                                        /* 如果原来总内存空间长度大于append后的字符串长度 */
        {
            /* 如果原来剩余空闲空间小于新添加子字符串的长度，而且 */
            if (m_nSize - (m_pDataEnd - m_pBuffer) < iLen && m_pDataStart - m_pBuffer > 0)          /*而且m_pDataStart在m_pBuffer的后面 */
            {
                Defrag();                                                                       /* 调节原来的字符串 */
            }
        }
        else                                                                                            /* 如果原来总内存空间长小于append后的字符串长度，需要开辟新空间 */
        {
            /* allocate new memory space and copy orignal data */
            AssignNewSpace(total_len + 1, 1);                                                       /* 分配新的内存空间，变为原来的两倍，移动1倍 */
        }

        /* get the merge point */
        pNewStart = m_pDataEnd;                                                                         /* 将原来字符串的末尾指针转变成新添加子串的开始指针 */
        if (!pNewStart)
            return (0);
        /* copy data and adjust some pointers */
        memcpy(pNewStart, pSrc, iLen);                                                                  /* 将新添加串pSrc放置在原来串联的末尾 */
        m_nLength       = total_len;                                                            /* 字符串的总长度变化 */
        m_pDataStart[m_nLength] = 0;                                                                    /* 新的字符串最后以为置为0 */
        m_pDataEnd      = &(m_pDataStart[m_nLength]);                                           /* 让m_pDataEnd指向新的字符串的末尾指针 */
        return (1);
    }


    void Defrag(void)
    {
        /* Sure!    重新初始化原来字符串头指针m_pDataStart，指向头部 */
        memmove(m_pBuffer, m_pDataStart, m_nLength);            /* 将m_pDataStart复制给m_pBuffer */
        /* adjust those related pointers */
        m_pDataStart        = m_pBuffer;                    /* m_pDataStart指向m_pBuffer */
        m_pDataStart[m_nLength] = 0;
        m_pDataEnd      = &(m_pDataStart[m_nLength]);   /* 重新获得原来的字符串尾指针m_pDataEnd */
    }


    /* Append another CString to this one */

    int Append(CString *pNewStr)
    {
        char *pNewStart = NULL, *pSrc = NULL, *pDest = NULL;

        int len = pNewStr->GetLength();     /* 新添加子串的长度 */
        int total_len;

        if (len <= 0)
            return (0);

        /* const char * */
        pSrc = pNewStr->GetData();              /* 获得要添加的字符串的头指针 */
        if (!pSrc)
            return (0);

        total_len = m_nLength + len;            /* 新字符串的总长度 = 原来字符串长度 + 添加字符串长度 */

        /* if some space avaliable, defrag it firstly. */
        if (m_nSize - (m_pDataEnd - m_pBuffer) < len && m_pDataStart - m_pBuffer > 0)
        {
            Defrag();
        }

        /* allocate new memory space */
        AssignNewSpace(total_len + 1, 1);                       /* //根据total_len分配新的内存空间 */
        /* get the merge point */
        pNewStart = m_pDataEnd;                                 /* 将原来字符串的末尾指针转变成新添加子串的开始指针 */
        if (!pNewStart)
            return (0);

        /* copy data and adjust some pointers */
        memcpy(pNewStart, pSrc, len);                           /* 将新添加串pSrc放置在原来串联的末尾 */
        m_nLength       = total_len;
        m_pDataStart[m_nLength] = 0;
        m_pDataEnd      = &(m_pDataStart[m_nLength]);   /* 让m_pDataEnd指向新的字符串的末尾指针 */

        return (1);
    }


    /* Adjust start and end pointer of its buffer */

    /* Get one character at give position */

    char GetAt(int nIndex) const
    {
        if (nIndex >= m_nLength)
            return (-1);
        if (nIndex < 0)
            return (-1);

        return (m_pDataStart[nIndex]);
    }


    /* return single character at zero-based index */
    char operator[](int nIndex) const
    {
        if (nIndex >= m_nLength)
            return (-1);

        return (m_pDataStart[nIndex]);
    }


    /* return pointer to const string */
    const char  *LPCTSTR() const       /* 将该字符串转变为const char*字符串常量 */
    {
        return ((const char *) m_pDataStart);
    }


    /* duplicate a string */
    CString *Duplicate(int iBufferNum) const
    {
        CString *pCStr = NULL;

        pCStr = new CString(m_pDataStart, m_nLength, iBufferNum);

        return (pCStr);
    }


    /* copy from another CString */
    const CString &operator=(CString &stringSrc)                    /* 赋值操作符 */
    {
        long len = stringSrc.GetLength();                       /* 取得stringSrc字符串长度 */

        if (len >= m_nSize)                                     /* 如果空间大小不够 */
        {
            AssignNewSpace(len + 1, 0);                     /* don't copy orignal data      //将会申请一块新的内存空间 */
        }

        /* copy data and adjust pointers */
        Empty();                                                /* 清空 */
        memcpy(m_pDataStart, stringSrc.GetData(), len);         /* 将字符串stringSrc复制给新的m_pDataStart */
        m_pDataStart[len]   = 0;
        m_pDataEnd      = &(m_pDataStart[len]);         /* 获得新的m_pDataEnd */
        m_nLength       = len;

        return (*this);
    }


    /* set string content to single character */
    CString &operator=(char c)                      /* 赋值操作符，将字符赋给字符串 */
    {
        if (m_nSize < 2)                        /* c + '\0' length                      //如果内存空间不足 */
        {
            /* it needs to realloc space, but we use new and delete pair */
            SafeDestroy();
            m_pBuffer   = new char[2];  /* 重新申请内存 */
            m_nSize     = 2;
            /* TODO, I don't check the value of this pointer, unkown result. :) */
        }
        m_pDataStart    = m_pBuffer;            /* 获得头指针 */
        m_pDataStart[0] = c;
        m_pDataStart[1] = 0;
        m_pDataEnd  = &(m_pDataStart[1]);   /* 获得尾指针 */
        m_nLength   = 1;

        return (*this);
    }


    /* Set one character at give position */

    void SetAt(long pos, char ch)
    {
        if (pos < m_nLength)
            m_pDataStart[pos] = ch;

        return;
    }


    /* Get the string started from give position */

    void GetManyChars(char *buf, long pos, long len) const
    {
        if (buf == NULL)
            return;

        if (pos >= m_nLength)
            return;

        if (pos + len > m_nLength)
            len = m_nLength - pos;

        if (len > 0)
            memcpy(buf, m_pDataStart + pos, len);    /* 将 m_pDataStart + pos开始长为len的子串复制给buf */
    }


    /* Compare itself with a new string, case-sensitive */

    int Compare(const char *pNewStr) const
    {
        if (pNewStr == NULL)
            return (-1);

        return (strcmp(m_pDataStart, pNewStr));
    }


    /* Compare itself with a new string, case-ignored */
    int CompareNoCase(const char *pNewStr) const
    {
        if (pNewStr == NULL)
            return (-1);

#ifndef WIN32
        return (strcasecmp(m_pDataStart, pNewStr));
#else
        return (stricmp(m_pDataStart, pNewStr));
#endif
    }


    /* find a character start from a give position */
    int Find(int ch, long pos) const
    {
        char *p = NULL;

        if (ch < 0)
            return (-1);

        if (ch > UCHAR_MAX)
            return (-1);

        if (pos < 0)
            return (-1);

        if (pos >= m_nLength)
            return (-1);

        p = (char *) memchr(m_pDataStart + pos, ch, m_nLength - pos);

        if (!p)
            return (-1);
        return (p - m_pDataStart);
    }


    /* find a string start from a give position */
    int Find(const char *str, long pos) const
    {
        long    len;
        char    *p = NULL;

        if (str == NULL)
            return (-1);

        len = strlen(str);


        if (len == 0)
            return (0);

        p = (char *) strstr(m_pDataStart + pos, str);

        if (p == NULL)
            return (-1);
        else
            return (p - m_pDataStart);
    }


    char *GetBuffer(int nMinBufLength)   /* 获得该字符串头指针，并且说明返回的内存空间最小值 */
    {
        Defrag();
        if (nMinBufLength > m_nLength)
            AssignNewSpace(nMinBufLength, 1);
        return (m_pDataStart);
    }


    void ReleaseBuffer(int nNewLength)
    {
        return;
    }


    /* Extracts the left part of a string. */
    CString Left(int count) const
    {
        if (count < 0)
            count = 0;
        if (count >= m_nLength)
            return (*this);

        CString dest(m_pDataStart, count);   /* 调用构造函数新建一个 */
        return (dest);
    }


    /* Extracts the right part of a string. */
    CString Right(int count) const
    {
        if (count < 0)
            count = 0;
        if (count >= m_nLength)
            return (*this);

        CString dest(&(m_pDataStart[m_nLength - count]), count);
        return (dest);
    }


    /* Converts all the characters in this string to uppercase characters. */
    void MakeUpper(void)
    {
        strupr(m_pDataStart);
    }


    /* Converts all the characters in this string to lowercase characters. */
    void MakeLower(void)
    {
        strlwr(m_pDataStart);
    }


    /* TODO: check the space left in the two pading of the whole buffer */

    /* trim the left spaces */
    void TrimLeft(void)   /* //将字符串右边的空格去掉 */
    {
        int start = 0;

        while (isspace(m_pDataStart[start]) && start < m_nLength)
            start++;

        if (start > 0)
        {
            m_pDataStart += start;
        }
    }


    /* trim the right spaces */
    void TrimRight(void)   /* 将字符串右边的空格去掉 */
    {
        int end = m_nLength - 1;

        while (isspace(m_pDataStart[end]) && end >= 0)
            end--;

        if (end < 0)
        {
            end     = 0;
            m_pDataEnd  = &(m_pDataStart[end]);
        }
        else
        {
            m_pDataEnd = &(m_pDataStart[end]);
        }
    }


    /* trim both sides */
    void Trim(void)   /* //将字符串的空格去掉 */
    {
        TrimLeft();
        TrimRight();
    }


    int Replace(const char *lpszOld, const char *lpszNew)
    {
        /* can't have empty or NULL lpszOld */
        if (!lpszOld)
            return (0);
        int nOldLen = strlen(lpszOld);                                                  /* 获得旧字符串的长度 */
        if (nOldLen <= 0)
            return (0);
        int nNewLen = strlen(lpszNew);                                                  /* 获得新字符串的长度 */

        /* loop once to figure out the size of the result string */
        int nCount      = 0;
        char    *lpszStart  = m_pDataStart;
        char    *lpszEnd    = m_pDataEnd;
        char    *lpszTarget;
        while (lpszStart < lpszEnd)                                                     /* 循环处理原有字符串 */
        {
            while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)               /* 如果在字符串lpszStart中发现子串lpszOld */
            {
                nCount++;                                                       /* 子串数量+1 */
                lpszStart = lpszTarget + nOldLen;                               /* 往后定位字符串lpszStart，从第一个子串后开始 */
            }
            lpszStart += strlen(lpszStart) + 1;                                     /* 往后查找 */
        }

        /* if any changes were made, make them */
        if (nCount > 0)                                                                 /* 如果有重复的字符串 */
        {
            /* allocate a new buffer (slow but sure) */
            int nNewLength = m_nLength + (nNewLen - nOldLen) * nCount;              /* 覆盖后总字符串的大小 */
            AssignNewSpace(nNewLength + 1, 1);                                      /* 为新的字符串分配内存空间 */
            /* then we just do it in-place */
            lpszStart   = m_pDataStart;                                         /* 重新初始化m_pDataStart，lpszStart，lpszEnd */
            lpszEnd     = m_pDataEnd;

            /* loop again to actually do the work */
            while (lpszStart < lpszEnd)                                             /* 循环处理原来的字符串 */
            {
                while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)       /* 如果在字符串lpszStart中发现子串lpszOld */
                {
                    int nBalance = lpszEnd - (lpszTarget + nOldLen);        /* 字符串lpszTarget后面的字符数量 */
                    memmove(lpszTarget + nNewLen, lpszTarget + nOldLen,
                            nBalance * sizeof(char));                      /* 移走lpszTarget原来的字符串，并为lpszTarget重新设置为nNewLen大小内存 */
                    memcpy(lpszTarget, lpszNew, nNewLen * sizeof(char));    /* 将新字符串lpszNew覆盖旧的子串lpszTarget */
                    lpszStart       = lpszTarget + nNewLen;         /* 寻找目标字符串后移nNewLen */
                    lpszStart[nBalance] = '\0';
                }
                lpszStart += strlen(lpszStart) + 1;                             /* 寻找目标字符串往后走 */
            }
            m_nLength = nNewLength;
        }

        return (nCount);
    }


    /* format a string */
    void Format(char *fmt, ...)
    {
        char TmpBuffer[STR_PAGE_SIZE];                          /* TODO, should calculate this size dynamically. */

        va_list argList;
        va_start(argList, fmt);
#ifdef WIN32
        _vsnprintf(TmpBuffer, STR_PAGE_SIZE, fmt, argList);     /* just not overwrite something */
#else
        vsnprintf(TmpBuffer, STR_PAGE_SIZE, fmt, argList);      /* just not overwrite something */
#endif
        va_end(argList);
    }


    /* copy string content from ANSI string (converts to TCHAR) */
    const CString &operator=(const char *lpsz)
    {
        int len = strlen(lpsz);

        if (m_nSize < len)      /* c + '\0' length */
        {
            /* it needs to realloc space, but we use new and delete pair */
            SafeDestroy();
            m_pBuffer   = new char[len + 1];
            m_nSize     = len + 1;
            /* TODO, I don't check the value of this pointer, unkown result. :) */
        }
        m_pDataStart = m_pBuffer;
        strcpy((char *) m_pDataStart, lpsz);
        m_pDataStart[len]   = 0;
        m_pDataEnd      = &(m_pDataStart[len]);
        m_nLength       = len;

        return (*this);
    }


    /* concatenate a UNICODE character after converting it to TCHAR */
    const CString &operator+=(const char *lpsz)
    {
        int len = strlen(lpsz);

        if (m_nSize < m_nLength + len + 1)
        {
            AssignNewSpace(m_nLength + len + 1, 1);   /* allocate new space and move orignal data */
        }
        Defrag();
        memcpy(m_pDataEnd, lpsz, len);
        m_pDataEnd  += len;
        *m_pDataEnd = 0;

        return (*this);
    }


    /* concatenate a single character */
    const CString &operator+=(char ch)
    {
        if (m_nSize < m_nLength + 1 + 1)
        {
            AssignNewSpace(m_nLength + 1 + 1, 1);   /* allocate new space and move orignal data */
        }
        Defrag();
        memcpy(m_pDataEnd, &ch, 1);
        m_pDataEnd  += 1;
        *m_pDataEnd = 0;

        return (*this);
    }


    /* concatenate from another CString */
    const CString &operator+=(CString &string)
    {
        if (m_nSize < m_nLength + string.GetLength() + 1)
        {
            AssignNewSpace(m_nLength + string.GetLength() + 1, 1);   /* allocate new space and move orignal data */
        }
        Defrag();
        memcpy(m_pDataEnd, string.GetData(), string.GetLength());
        m_pDataEnd  += string.GetLength();
        *m_pDataEnd = 0;

        return (*this);
    }


    void AssignNewSpace(int iNewTotalSize, int iNeedMove)
    {
        char *pNewSpace = NULL;                                         /* 新的字符串指针，初始化NULL */

        if (iNewTotalSize <= m_nSize)                                   /* 确保新的内存空间大于原来的内存空间 */
            return;

        /* allocate new space */
        pNewSpace = new char [iNewTotalSize];                           /* pNewSpace动态申请内存空间 */
        if (pNewSpace == NULL)
            return;

        if (iNeedMove)
        {
            memcpy(pNewSpace, m_pDataStart, m_nLength + 1);         /* 将原有字符串复制给新申请内存 */
        }
        /* SAFEDELETE(m_pBuffer);              //安全删除原有的字符串m_pBuffer */
        m_pBuffer   = pNewSpace;
        m_pDataStart    = m_pBuffer;
        m_pDataEnd  = &(m_pDataStart[m_nLength]);                   /* 重置m_pBuffer，m_pDataStart，m_pDataEnd */

        /* adjust new size */
        m_nSize = iNewTotalSize;
    }

    /*
     * NOTE: "operator+" is done as friend functions for simplicity
     *      There are three variants:
     *          CString + CString
     * and for ? = TCHAR, LPCTSTR
     *          CString + ?
     *          ? + CString
     */
    void ConcatCopy(const char *str1, int nSrc1Len, const char *str2, int nSrc2Len)
    {
        int nNewLen = nSrc1Len + nSrc2Len;

        AssignNewSpace(nNewLen + 1, 0);

        /* append two string */
        Append(str1, nSrc1Len);
        Append(str2, nSrc2Len);
    }


    /*
     * friend methods
     * Class + Class
     */


    CString operator+(const CString &string1)
    {
        /* CString s; */

        /*
         * s.ConcatCopy(string1.GetData(), string1.GetLength(), string2.GetData(), string2.GetLength());
         * return s;
         */
        if (m_nSize < m_nLength + string1.GetLength() + 1)
        {
            AssignNewSpace(m_nLength + string1.GetLength() + 1, 1);   /* allocate new space and move orignal data */
        }
        Defrag();
        memcpy(m_pDataEnd, string1.GetData(), string1.GetLength());
        m_pDataEnd  += string1.GetLength();
        *m_pDataEnd = 0;

        return (*this);
    }


    /* Class + char */
    CString operator+(char ch)
    {
        if (m_nSize < m_nLength + 1 + 1)
        {
            AssignNewSpace(m_nLength + 1 + 1, 1);   /* allocate new space and move orignal data */
        }
        Defrag();
        memcpy(m_pDataEnd, &ch, 1);
        m_pDataEnd  += 1;
        *m_pDataEnd = 0;

        return (*this);
    }



    /* Class + char * */
    CString operator+(const char *lpsz)
    {
        if (m_nSize < m_nLength + strlen(lpsz) + 1)
        {
            AssignNewSpace(m_nLength + strlen(lpsz) + 1, 1);     /* allocate new space and move orignal data */
        }
        Defrag();
        memcpy(m_pDataEnd, lpsz, strlen(lpsz));
        m_pDataEnd  += strlen(lpsz);
        *m_pDataEnd = 0;

        return (*this);
    }

    /* Compare operators */
    bool operator==(const CString &s2)
    {
        return (this->Compare(s2.GetData()) == 0);
    }


    bool operator==(const char *s2)
    {
        return (this->Compare(s2) == 0);
    }

    bool operator!=(const CString &s2)
    {
        return (this->Compare(s2.GetData()) != 0);
    }


    bool operator!=(const char *s2)
    {
        return (this->Compare(s2) != 0);
    }

    bool operator>(const CString &s2)
    {
        return (this->Compare(s2.GetData()) > 0);
    }


    bool operator>(const char *s2)
    {
        return (this->Compare(s2) > 0);
    }

    bool operator<(const CString &s2)
    {
        return (this->Compare(s2.GetData()) < 0);
    }


    bool operator<(const char *s2)
    {
        return (this->Compare(s2) < 0);
    }


    bool operator>=(const CString &s2)
    {
        return (this->Compare(s2.GetData()) >= 0);
    }


    bool operator>=(const char *s2)
    {
        return (this->Compare(s2) >= 0);
    }


    bool operator<=(const CString &s2)
    {
        return (this->Compare(s2.GetData()) <= 0);
    }


    bool operator<=(const char *s2)
    {
        return (this->Compare(s2) <= 0);
    }
};
#endif
