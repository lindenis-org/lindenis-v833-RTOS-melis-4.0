/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/

#ifndef __SUNXI_SID_H__
#define __SUNXI_SID_H__

#define SUNXI_SID_BASE			0x03006000
#define SID_PRCTL				(SUNXI_SID_BASE + 0x40)
#define SID_PRKEY				(SUNXI_SID_BASE + 0x50)
#define SID_RDKEY				(SUNXI_SID_BASE + 0x60)
#define SJTAG_AT0				(SUNXI_SID_BASE + 0x80)
#define SJTAG_AT1				(SUNXI_SID_BASE + 0x84)
#define SJTAG_S					(SUNXI_SID_BASE + 0x88)

#define EFUSE_SRAM              (SUNXI_SID_BASE + 0x200)
#define EFUSE_SECURE_MODE       (SUNXI_SID_BASE + 0xA0)

#define EFUSE_CHIPD             (0x00) /* 0x0-0xf, 128bits */
#define EFUSE_BROM_CONFIG       (0x10) /* 16 bit config, 16 bits try */
#define EFUSE_BROM_TRY          (0X12) /* 16 bit*/
#define EFUSE_THERMAL_SENSOR    (0x14) /* 0x14-0x1b, 64bits */
#define EFUSE_AUDIO_BIAS        (0X1C) /* 8bit */
#define EFUSE_LDOA              (0x1D) /* 8bit */
#define EFUSE_LDOB              (0x1E) /* 8bit */
#define EFUSE_DDR_CFG           (0x1F) /* 8bit */
#define EFUSE_TF_ZONE           (0x20) /* 0x20-0x2f, 128bits */
#define EFUSE_ROTPK             (0x30) /* 0x30-0x4f  256bits */
#define EFUSE_NV1               (0x50) /* 0x50-0x53, 32bits */
#define EFUSE_PSENSOR           (0x54) /* 0x54-0x57, 32bits */
#define EFUSE_ANTI_BLUSH        (0x58) /* 0x58-0x5b, 32bits */
#define EFUSE_RESERVED          (0x5C) /* 0x5c-0x7b, 256bits */

/* write protect */
#define EFUSE_WRITE_PROTECT     (0x7C) /* 0x7C-0x7D, 16bits */
/* read  protect */
#define EFUSE_READ_PROTECT      (0x7E) /* 0x7E-0x7F, 16bits */


#define SID_OP_LOCK  (0xAC)

#define SECURE_BIT_OFFSET 11
/*It can not be seen.*/
#define EFUSE_PRIVATE (0)
/*After burned ,cpu can not access.*/
#define EFUSE_NACCESS (1)

#define EFUSE_RO (2)	/* burn read_protect bit disable */
#define EFUSE_RW (3)	/* burn read/write_protect bit disable */

#define EFUSE_ACL_SET_BRUN_BIT      (1<<29)
#define EFUSE_ACL_SET_RD_FORBID_BIT (1<<30)
#define EFUSE_BRUN_RD_OFFSET_MASK    (0xFFFFFF)

#define EFUSE_DEF_ITEM(name,offset,size_bits,rd_offset,burn_offset,acl) \
{name,offset,size_bits,rd_offset,burn_offset,acl}

typedef enum efuse_err
{
	EFUSE_ERR_ARG = -1,
	EFUSE_ERR_KEY_NAME_WRONG = -2,
	EFUSE_ERR_KEY_SIZE_TOO_BIG = -3,
	EFUSE_ERR_PRIVATE = -4,
	EFUSE_ERR_ALREADY_BURNED = -5,
	EFUSE_ERR_READ_FORBID = -6,
	EFUSE_ERR_BURN_TIMING = -7,
	EFUSE_ERR_NO_ACCESS = -8,
	EFUSE_ERR_INVALID_ROTPK = -9,
}
efuse_err_e;

/* internal struct */
typedef struct efuse_key_map_new{
	#define SUNXI_KEY_NAME_LEN	64
	char name[SUNXI_KEY_NAME_LEN];	/* key_name */
	int offset;	/* key_addr offset */
	int size;	 /* unit: bit */
	int rd_fbd_offset;	/* key can read or not */
	int burned_flg_offset;	/* key has burned or not */
	int sw_rule;
}efuse_key_map_new_t;

efuse_key_map_new_t *efuse_search_key_by_name(const char *key_name);
int efuse_acl_ck(efuse_key_map_new_t *key_map,int burn);
void efuse_set_cfg_flg(int efuse_cfg_base,int bit_offset);
int efuse_uni_burn_key(unsigned int key_index, unsigned int key_value);
unsigned int efuse_sram_read_key(unsigned int key_index);
int efuse_set_security_mode(void);
int efuse_get_security_mode(void);

#endif    /*  #ifndef __EFUSE_H__  */
