/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_module.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: melis module file handing functions.
* Update  : date                auther      ver     notes
*           2011-3-24 22:35:41  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "config/config.h"
#include    "minfs_tool_i.h"

__s32  MINFS_CalcSectionCompressedLen(HELFPSR hPSR, __u32 Index, __u32 *SectionLen)
{
    Elf32_Shdr  *pSHdr;
    __u8        *pSectionData;
    __u8        *pCompressData;
    __u8         OutProps[MINFS_MAX_COMPRESS_PROPS_LEN];
    __u32        OutPropsLen;
    __u32        CompressLen;

    //get this section header
    pSHdr = GetSectionHeader(hPSR, Index);

    //get this section data
    pSectionData = GetSectionData(hPSR, Index);

    //allocate buffer for compressed
    CompressLen   = MINFS_MAX_EXPAND_RATIO * pSHdr->size;
    pCompressData = (__u8 *)malloc(CompressLen);
    if (pCompressData == NULL)
    {
        MSG("allocate buffer for compress data failed\n");
        UnLoadELFFile(hPSR);
        return EPDK_FAIL;
    }
    //initialize props data
    memset(OutProps, 0, MINFS_MAX_COMPRESS_PROPS_LEN);
    OutPropsLen = MINFS_MAX_COMPRESS_PROPS_LEN;

    //compress section data
    if (MINFS_Compress(pCompressData, &CompressLen,
                       pSectionData, pSHdr->size,
                       OutProps, &OutPropsLen) != EPDK_OK)
    {
        MSG("compress data failed\n");
        UnLoadELFFile(hPSR);
        return EPDK_FAIL;
    }
    if (OutPropsLen > MINFS_MAX_COMPRESS_PROPS_LEN)
    {
        MSG("the compressed algorithmic property is too long\n");
        UnLoadELFFile(hPSR);
        return EPDK_FAIL;
    }

    //release compress buffer
    free(pCompressData);

    //compressed section length = OutPropsLen + CompressLen
    (*SectionLen) = OutPropsLen + CompressLen;

    return EPDK_OK;
}

__s32 MINFS_CompressSectionData(HELFPSR hPSR, __u32 Index, __u8 *pBuffer, __u32 *DataLen)
{
    __u8        *pSectionData;
    __u8        *pCompressData;
    __u8         OutProps[MINFS_MAX_COMPRESS_PROPS_LEN];
    __u32        OutPropsLen;
    __u32        CompressLen;
    Elf32_Shdr  *pSHdr;

    //get section header and data
    pSectionData = GetSectionData(hPSR, Index);
    pSHdr = GetSectionHeader(hPSR, Index);

    //allocate buffer for compressed
    CompressLen   = MINFS_MAX_EXPAND_RATIO * pSHdr->size;
    pCompressData = (__u8 *)malloc(CompressLen);
    if (pCompressData == NULL)
    {
        MSG("allocate buffer for compress data failed\n");
        return EPDK_FAIL;
    }

    //initialize props buffer
    memset(OutProps, 0, MINFS_MAX_COMPRESS_PROPS_LEN);
    OutPropsLen = MINFS_MAX_COMPRESS_PROPS_LEN;

    //compress section data
    if (MINFS_Compress(pCompressData, &CompressLen,
                       pSectionData, pSHdr->size,
                       OutProps, &OutPropsLen) != EPDK_OK)
    {
        MSG("compress data failed\n");
        free(pCompressData);
        return EPDK_FAIL;
    }
    if (OutPropsLen > MINFS_MAX_COMPRESS_PROPS_LEN)
    {
        MSG("the compressed algorithmic property is too long\n");
        free(pCompressData);
        return EPDK_FAIL;
    }

    //check dest buffer enough or not
    if ((OutPropsLen + CompressLen) > (*DataLen))
    {
        MSG("compress dest buffer not enough\n");
        free(pCompressData);
        return EPDK_FAIL;
    }

    //store property data and compress data to buffer.
    memcpy(pBuffer, OutProps, OutPropsLen);
    memcpy(pBuffer + OutPropsLen, pCompressData, CompressLen);

    //this section compressed size
    (*DataLen) = OutPropsLen + CompressLen;

    //release compress buffer
    free(pCompressData);

    return EPDK_OK;
}

__s32 MINFS_CalcMFSModuleFile(const char *pFullPath,
                              __u32 *ExtentLen,
                              __u32 *FDataLen,
                              __minfs_config_t *pConfig)
{
    __file_para_t            FilePara;
    HELFPSR                  hPSR;
    __u32                    SectionNum;
    __u32                    Index;
    __u32                    MFSSctCnt;
    __u32                    SectionLen;
    __u8                    *tmpName;
    Elf32_Shdr              *pSHdr;

    //parse this file config parameters
    if (MINFS_ParseFileConfigPara(pConfig, pFullPath, &FilePara) != EPDK_OK)
    {
        MSG("parse file [%s] config parameters failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //load melis module file
    hPSR = LoadELFFile(pFullPath);
    if (hPSR == NULL)
    {
        MSG("load module file [%s] failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //get module file section numbers
    SectionNum = GetSectionsNumber(hPSR);

    //calculate melis module file section parameters of minfs image
    *ExtentLen = 0;
    if (FDataLen)
    {
        *FDataLen  = 0;
    }
    MFSSctCnt  = 0;
    for (Index = 0; Index < SectionNum; Index++)
    {
        tmpName = GetSectionName(hPSR, Index);
        if (MINFS_ValidModuleSection(tmpName) == 0)
        {
            //invalid melis module seciton,
            //just fliter this section
            continue;
        }
        if (FDataLen)
        {
            pSHdr = GetSectionHeader(hPSR, Index);
            if (pSHdr->type != EELF_SHT_NOBITS)
            {
                //this section should allocate file data space
                if (FilePara.CompressON &&
                        MINFS_IsCompressedSection(pSHdr, tmpName))
                {
                    //section should compressed
                    MINFS_CalcSectionCompressedLen(hPSR, Index, &SectionLen);
                }
                else
                {
                    SectionLen = pSHdr->size;
                }
            }
            else
            {
                SectionLen = 0;
            }
            //update file data length
            (*FDataLen) += MINFS_ALIGN(SectionLen, MINFS_DATA_ALIGN);
        }

        //next section
        MFSSctCnt++;
    }

    //unload module file
    UnLoadELFFile(hPSR);

    //module file extent metadata length
    (*ExtentLen) = (MFSSctCnt * MINFS_SECTION_HDR_LEN);

    return EPDK_OK;
}

__s32 MINFS_LoadModuleFile(const char *pFullPath,
                           __minfs_dentry_t *pDEntry,
                           __minfs_context_t *pContext,
                           __minfs_config_t *pConfig)
{
    __u32                    FileLen;
    __file_para_t            FilePara;
    __u8                    *pBuffer;
    __minfs_section_hdr_t   *pMFSSctHdr;
    HELFPSR                 *hPSR;
    __u32                    SectionNum;
    __u8                    *tmpName;
    Elf32_Shdr              *pSHdr;
    __u32                    Index;
    __u32                    MFSSctCnt;
    __u32                    MFSSctLen;
    __u32                    SctDataOffset;

    //parse this file config parameters
    if (MINFS_ParseFileConfigPara(pConfig, pFullPath, &FilePara) != EPDK_OK)
    {
        MSG("parse file [%s] config parameters failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //load melis module file
    hPSR = LoadELFFile(pFullPath);
    if (hPSR == NULL)
    {
        MSG("load module file [%s] failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //allocate current DEntry offset for module file section extent data,
    //the module file first extent item is after name.
    pMFSSctHdr = (__minfs_section_hdr_t *)(pDEntry->MetaData + \
                                           MINFS_ALIGN(pDEntry->NameLen, \
                                                   MINFS_NAME_ALIGN));
    //get module file section numbers
    SectionNum = GetSectionsNumber(hPSR);

    //load sections data to minfs image
    FileLen       = 0;
    SctDataOffset = 0;
    MFSSctCnt     = 0;
    for (Index = 0; Index < SectionNum; Index++)
    {
        tmpName = GetSectionName(hPSR, Index);
        if (MINFS_ValidModuleSection(tmpName) == 0)
        {
            //invalid melis module seciton,
            //just fliter this section
            continue;
        }
        pMFSSctHdr->Attribute = 0;
        if (MINFS_IsModuleMagicSection(tmpName))
        {
            //magic section for melis system loader
            pMFSSctHdr->Attribute |= MINFS_SECTION_ATTR_MAGIC;
        }
        pSHdr = GetSectionHeader(hPSR, Index);

        //the parameters of a section is identify to elf spec
        pMFSSctHdr->Size  = pSHdr->size;
        pMFSSctHdr->VAddr = pSHdr->addr;
        pMFSSctHdr->Type  = pSHdr->type;
        pMFSSctHdr->Flags = pSHdr->flags;

        //process section data
        if (pSHdr->type == EELF_SHT_NOBITS)
        {
            //bss section, no need allocate file space
            MFSSctLen = 0;
            pMFSSctHdr->Offset = 0;
            pMFSSctHdr->RecSize = 0;
            pMFSSctHdr->RecUnPackSize = 0;
        }
        else
        {
            pBuffer = pContext->pBuffer + pContext->CurDataOffset;
            if (FilePara.CompressON &&
                    MINFS_IsCompressedSection(pSHdr, tmpName))
            {
                //section data should compressed
                MFSSctLen = pContext->ImageSize - pContext->CurDataOffset;
                if (MINFS_CompressSectionData(hPSR, Index,
                                              pBuffer, &MFSSctLen)
                        != EPDK_OK)
                {
                    MSG("image file have no free space\n");
                    return EPDK_FAIL;
                }
                pMFSSctHdr->Attribute |= MINFS_SECTION_ATTR_COMPRESS;
            }
            else
            {
                //copy section data directly.
                MFSSctLen = pSHdr->size;

                //check image file have free space to use
                if ((pContext->CurDataOffset + MFSSctLen) > pContext->ImageSize)
                {
                    MSG("image file have no free space\n");
                    return EPDK_FAIL;
                }
                //save section data to image buffer directly
                memcpy(pBuffer, GetSectionData(hPSR, Index), MFSSctLen);
            }
            pMFSSctHdr->Offset = SctDataOffset;
            pMFSSctHdr->RecSize = MFSSctLen;
            pMFSSctHdr->RecUnPackSize = pSHdr->size;
        }

        //adjust current data offset
        pContext->CurDataOffset += MINFS_ALIGN(pMFSSctHdr->RecSize, MINFS_DATA_ALIGN);
        SctDataOffset += MINFS_ALIGN(pMFSSctHdr->RecSize, MINFS_DATA_ALIGN);
        FileLen += MINFS_ALIGN(pMFSSctHdr->RecSize, MINFS_DATA_ALIGN);

        //next section
        pMFSSctHdr++;
        MFSSctCnt++;
    }

    //update this module file dentry parameters
    pDEntry->ExtentLen  = (MFSSctCnt * MINFS_SECTION_HDR_LEN);
    pDEntry->RecLen    += (pDEntry->ExtentLen);
    pDEntry->Size       = FileLen;
    pDEntry->UnPackSize = GetELFFileSize(hPSR);
    if (FilePara.CompressON)
    {
        pDEntry->Attribute |= MINFS_ATTR_COMPRESS;
    }
    pDEntry->Attribute |= MINFS_ATTR_MODULE;

    //unload module file
    UnLoadELFFile(hPSR);

    return EPDK_OK;
}

__bool MINFS_Is_Normal_ELF(const char *pFile)
{
    int i = 0, length;
    length = strlen(pFile);
    if (length < 4)
        return 0;
    if (pFile[length - 1] == 'f' &&
            pFile[length - 2] == 'l' &&
            pFile[length - 3] == 'e' &&
            pFile[length - 4] == '.')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

__bool MINFS_ValidModuleFile(const char *pFile)
{
    FILE            *hFile;
    __elf32_head_t   ElfHdr;


    if (MINFS_Is_Normal_ELF(pFile))
        return 0;
    hFile = fopen(pFile, "rb");
    if (hFile == NULL)
    {
        MSG("MINFS_ValidModuleFile : open file [%s] failed\n", pFile);
        return 0;
    }
    fread(&ElfHdr, 1, sizeof(__elf32_head_t), hFile);
    fclose(hFile);

    //melis system module file type : elf
    if ((ElfHdr.ident[EI_MAG0] == ELFMAG0) &&
            (ElfHdr.ident[EI_MAG1] == ELFMAG1) &&
            (ElfHdr.ident[EI_MAG2] == ELFMAG2) &&
            (ElfHdr.ident[EI_MAG3] == ELFMAG3))
    {
        //valid module file of melis system
        return 1;
    }

    //maybe should check have magic section,
    //by sunny at 2011-3-25 14:30:48.
    return 0;
}

__bool  MINFS_ValidModuleSection(const char *SectionName)
{
    int index;
    static char *FliterSection[] =
    {
        ".note",
        ".comment",
        ".shstrtab"
    };
    if (SectionName[0] == '\0')
    {
        //this empty section no any use for melis loader,
        //by sunny at 2011-3-25 14:34:30.
        return 0;
    }
    for (index = 0; index < sizeof(FliterSection) / sizeof(FliterSection[0]); index++)
    {
        if (strcasecmp(SectionName, FliterSection[index]) == 0)
        {
            return 0;
        }
    }
    //valid section for melis system loader
    return 1;
}

__bool MINFS_IsModuleMagicSection(const char *SectionName)
{
    if (strcasecmp(SectionName, MINFS_MODULE_MAGIC) == 0)
    {
        //magic section for melis system
        return 1;
    }

    return 0;
}

__bool MINFS_IsCompressedSection(Elf32_Shdr *pSHdr, const char *SectionName)
{
    if (strcasecmp(SectionName, MINFS_MODULE_MAGIC) == 0)
    {
        //the magic section should not compressed,
        //by sunny at 2011-3-26 13:35:02.
        return 0;
    }

    if (pSHdr->size > MINFS_SECTION_COMRPESS_MIN)
    {
        //section size bigger then MINFS_SECTION_COMRPESS_MIN,
        //this section should compressed.
        return 1;
    }

    //this section no need to compressed.
    return 0;
}
