// FaceDocumentData.h: interface for the FaceDocumentData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEDOCUMENTDATA_H__F4AA6472_D23D_4DBD_810F_59407F607674__INCLUDED_)
#define AFX_FACEDOCUMENTDATA_H__F4AA6472_D23D_4DBD_810F_59407F607674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct tag_SYSTEM_HEAD
{
    u32 version;            //
    u32 pid;                //
    u32 vid;                //
    u32 hid;                //
    u32 fid;                //
    u32 filecnt;            //
    s8  image[MAX_PATH];    //
} SYSTEM_HEAD;

typedef struct tag_ITEM_INFO
{
    u8 filepath[MAX_PATH];  //
    u32 zero0;
    u8 main[8];             //
    u32 zero1;
    u8 sub[16];             //
    u32 zero2;
} ITEM_INFO;


class FaceDocumentData
{
public:
    void DumpItem(u32 index);                               //

    void DumpSystemData(void);                              //

    void Dump(void);                                        //

    ITEM_INFO *GetItemData(u32 index);                      //

    SYSTEM_HEAD *GetSystemData(void);                       //

    u32 AllocItemArray(void);                               //

    void Release(void);                                     //

    FaceDocumentData();                                     //

    virtual ~FaceDocumentData();                            //

protected:
    SYSTEM_HEAD       m_system;                             //

    ITEM_INFO        *m_pItemArray;                         //
};

#endif // !defined(AFX_FACEDOCUMENTDATA_H__F4AA6472_D23D_4DBD_810F_59407F607674__INCLUDED_)
