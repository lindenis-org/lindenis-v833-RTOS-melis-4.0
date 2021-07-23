/*
* Copyright (c) 2008-2018 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : isp_version.h
* Description :
* History :
* Author  : zhaowei <zhaowei@allwinnertech.com>
* Date    : 2018/02/08
*
*/

#ifndef _ISP_VERSION_H_
#define _ISP_VERSION_H_

#include <isp_debug.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISP_VERSION 521
#define REPO_TAG "isp-500-520-v2.00"
#define REPO_BRANCH "libisp-dev"
#define REPO_COMMIT "6ebfc430bae8055af53560180c78de153e03b91b"
//#define REPO_DATE "Mon Nov 16 11:36:07 2020 +0800"
#define RELEASE_AUTHOR "<linjiandao@allwinnertech.com>"

#define FROM_REPO_BRANCH "isp521-ipc"
#define FROM_REPO_COMMIT "efa5ea932f392e9a75f70a44a23a7c132a4cdda6"
#define REPO_DATE "Mon Nov 16 13:40:47 2020 +0800"


static inline void isp_version_info(void)
{
	ISP_PRINT(">>>>>>>>>>>>>>>>>>>> ISP VERSION INFO <<<<<<<<<<<<<<<<<<<\n"
		"IPCORE: ISP%d\n"
		"branch: %s\n"
		"commit: %s\n"
		"date  : %s\n"
		"author: %s\n"
		"--------------------------------------------------------\n\n",
		ISP_VERSION, REPO_BRANCH, REPO_COMMIT, REPO_DATE, RELEASE_AUTHOR);
}

#ifdef __cplusplus
}
#endif

#endif

