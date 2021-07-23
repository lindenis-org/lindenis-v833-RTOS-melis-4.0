#ifndef __SYS_DMA_H__
#define __SYS_DMA_H__
#include <typedef.h>
#include <kernel/ktype.h>

extern __s32 DMA_Init(void);
// DMA
extern __hdle esDMA_Request(char *user, __u32 type);
extern __s32 esDMA_Release(__hdle dma);
extern __s32 esDMA_Setting(__hdle dma, void *arg);
extern __s32 esDMA_Start(__hdle dma, __u32 saddr, __u32 daddr, __u32 len);
extern __s32 esDMA_Stop(__hdle dma);
extern __s32 esDMA_Restart(__hdle dma);
extern __u32 esDMA_QueryStat(__hdle dma);
extern __u32 esDMA_QuerySrc(__hdle dma);
extern __u32 esDMA_QueryDst(__hdle dma);
extern __s32 esDMA_EnableINT(__hdle dma, __s32 type);
extern __s32 esDMA_DisableINT(__hdle dma, __s32 type);
extern __u32 esDMA_QueryRestCount(__hdle dma);
extern __s32 esDMA_ChangeMode(__hdle dma, __s32 mode);
extern __s32 esDMA_RegDmaHdler(__hdle dma, __s32 type, __pCBK_t hdler, void *arg);
extern __s32 esDMA_UnregDmaHdler(__hdle dma, __s32 type, __pCBK_t hdler);
extern void esDMA_Information(void);

#endif  /* __SYS_DMA_H__ */
