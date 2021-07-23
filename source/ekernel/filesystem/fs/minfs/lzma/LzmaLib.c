/* LzmaLib.c -- LZMA library wrapper
2008-08-05
Igor Pavlov
Public domain */
#include "LzmaDec.h"
#include "LzmaLib.h"
#include "fsys_libs.h"

static void *SzAlloc(void *p, size_t size)
{
    p = p;
    return k_malloc(size);
}
static void SzFree(void *p, void *address)
{
    p = p;
    k_free(address);
}
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

int LzmaUncompress(unsigned char *dest, size_t  *destLen,
                   const unsigned char *src, size_t *srcLen,
                   const unsigned char *props, size_t propsSize)
{
    ELzmaStatus status;
    return LzmaDecode(dest, destLen,
                      src, srcLen,
                      props, (unsigned)propsSize,
                      LZMA_FINISH_ANY, &status, &g_Alloc);
}
