/* LzmaLib.c -- LZMA library wrapper
2008-08-05
Igor Pavlov
Public domain */

#include "LzmaEnc.h"
#include "LzmaDec.h"
#include "LzmaLib.h"
#include "stdlib.h"

static void *SzAlloc(void *p, size_t size)
{
    p = p;
    return malloc(size);
}
static void SzFree(void *p, void *address)
{
    p = p;
    free(address);
}
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

MY_STDAPI LzmaCompress(unsigned char *dest, size_t  *destLen,
                       const unsigned char *src, size_t  srcLen,
                       unsigned char *outProps, size_t *outPropsSize)
{
    CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    props.level = 5;
    props.dictSize = 32 * 1024;
    props.lc = 3;
    props.lp = 0;
    props.pb = 2;
    props.fb = 32;
    props.numThreads = 1;

    return LzmaEncode(dest, destLen, src, srcLen,
                      &props, outProps, outPropsSize,
                      0, NULL, &g_Alloc, &g_Alloc);
}


MY_STDAPI LzmaUncompress(unsigned char *dest, size_t  *destLen,
                         const unsigned char *src, size_t *srcLen,
                         const unsigned char *props, size_t propsSize)
{
    ELzmaStatus status;
    return LzmaDecode(dest, destLen,
                      src, srcLen,
                      props, (unsigned)propsSize,
                      LZMA_FINISH_ANY, &status, &g_Alloc);
}
