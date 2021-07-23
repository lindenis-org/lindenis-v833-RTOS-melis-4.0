/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                ELF Filter Tool
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : elf_parser.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-13
* Descript: ELF format file parser.
* Update  : date                auther      ver     notes
*           2011-3-13 15:00:54  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include    "elf_parser_i.h"

HELFPSR *LoadELFFile(const char *filename)
{
    FILE         *hFile;
    elf_parser_t *pPSR;

    pPSR = (elf_parser_t *)malloc(sizeof(elf_parser_t));
    if (pPSR == NULL)
    {
        return NULL;
    }

    hFile = fopen(filename, "rb");
    if (hFile == NULL)
    {
        printf("LoadELFFile:open file %s failed\n", filename);
        free(pPSR);
        return NULL;
    }

    //get file size
    fseek(hFile, 0, SEEK_END);
    pPSR->fileLen = (__u32)ftell(hFile);
    fseek(hFile, 0, SEEK_SET);


    pPSR->buffer = (__u8 *)malloc(pPSR->fileLen);
    if (pPSR->buffer == NULL)
    {
        fclose(hFile);
        free(pPSR);
        return NULL;
    }

    memset(pPSR->buffer, 0, pPSR->fileLen);
    fread(pPSR->buffer, 1, pPSR->fileLen, hFile);

    pPSR->pHdr  = (Elf32_Ehdr *)(pPSR->buffer);
    pPSR->pSHdr = (Elf32_Shdr **)(pPSR->buffer + pPSR->pHdr->shoff);

    fclose(hFile);

    //check the source file is valid elf format,
    //valid elf file magic is "elf".
    //by sunny at 2011-3-16 10:06:00.
    if ((pPSR->pHdr->ident[EI_MAG0] != ELFMAG0) ||
            (pPSR->pHdr->ident[EI_MAG1] != ELFMAG1) ||
            (pPSR->pHdr->ident[EI_MAG2] != ELFMAG2) ||
            (pPSR->pHdr->ident[EI_MAG3] != ELFMAG3)
       )
    {
        printf("invalid elf format file\n");
        free(pPSR->buffer);
        pPSR->buffer = NULL;
        free(pPSR);
        return NULL;
    }

    return (HELFPSR)pPSR;
}

__s32 UnLoadELFFile(HELFPSR hPsr)
{
    elf_parser_t *pPSR;
    if (hPsr == NULL)
    {
        return EPDK_FAIL;
    }
    pPSR = (elf_parser_t *)hPsr;

    free(pPSR->buffer);
    pPSR->buffer = NULL;
    free(pPSR);

    return EPDK_OK;
}

__s32 GetELFFileSize(HELFPSR hPsr)
{
    elf_parser_t *pPSR;
    if (hPsr == NULL)
    {
        return 0;
    }
    pPSR = (elf_parser_t *)hPsr;

    return pPSR->fileLen;
}

__u32 GetSectionsNumber(HELFPSR hPsr)
{
    elf_parser_t *pPSR;
    if (hPsr == NULL)
    {
        return 0;
    }
    pPSR = (elf_parser_t *)hPsr;

    return pPSR->pHdr->shnum;
}

Elf32_Ehdr *GetELFHeader(HELFPSR hPsr)
{
    elf_parser_t *pPSR;
    if (hPsr == NULL)
    {
        return NULL;
    }
    pPSR = (elf_parser_t *)hPsr;

    return pPSR->pHdr;
}

Elf32_Shdr *GetSectionHeader(HELFPSR hPsr, __u32 index)
{
    elf_parser_t    *pPSR;
    Elf32_Shdr *pSHdr;

    if (hPsr == NULL)
    {
        return NULL;
    }
    pPSR = (elf_parser_t *)hPsr;

    pSHdr = (Elf32_Shdr *)(pPSR->buffer + pPSR->pHdr->shoff + index * sizeof(Elf32_Shdr));

    return pSHdr;
}

Elf32_Shdr *GetStrSectionHeader(HELFPSR hPsr)
{
    elf_parser_t    *pPSR;
    Elf32_Shdr      *pStrSHdr;

    if (hPsr == NULL)
    {
        return NULL;
    }
    pPSR = (elf_parser_t *)hPsr;

    pStrSHdr = GetSectionHeader(hPsr, pPSR->pHdr->shstrndx);

    return pStrSHdr;
}

__u8 *GetSectionName(HELFPSR hPsr, __u32 index)
{
    elf_parser_t    *pPSR;
    Elf32_Shdr      *pSHdr;
    Elf32_Shdr      *pStrSHdr;

    if (hPsr == NULL)
    {
        return NULL;
    }
    pPSR = (elf_parser_t *)hPsr;

    pSHdr = GetSectionHeader(hPsr, index);
    pStrSHdr = GetStrSectionHeader(hPsr);

    return (pPSR->buffer + pStrSHdr->offset + pSHdr->name);
}

__u8 *GetSectionData(HELFPSR hPsr, __u32 index)
{
    elf_parser_t    *pPSR;
    Elf32_Shdr      *pSHdr;

    if (hPsr == NULL)
    {
        return NULL;
    }
    pPSR = (elf_parser_t *)hPsr;

    pSHdr = GetSectionHeader(hPsr, index);
    return (pPSR->buffer + pSHdr->offset);
}
