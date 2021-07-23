/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 */

#include <common.h>
#include <private_toc.h>
#include <private_uboot.h>
#include <private_tee.h>
#include <mmc_boot0.h>
#include <openssl_ext.h>
#include <arch/efuse.h>
#include <arch/ce.h>
#include <arch/smc.h>
#include <arch/rtc.h>
#include "sboot_toc.h"
#include "sboot_flash.h"
#include "sboot_ceritf.h"
#include <cache_align.h>
#include <lzma/LzmaTools.h>
#include <u-boot/lz4.h>

#ifdef CFG_CAT_TWO_NV
#define EFUSE_NV_BIT (SID_NV1_SIZE + SID_NV2_SIZE)
#else
#define EFUSE_NV_BIT SID_NV1_SIZE
#endif

extern u16 debug_mode;
extern char uart_input_value;

typedef struct _toc_name_addr_tbl
{
	char name[32];
	uint addr;
}toc_name_addr_tbl;

static struct HASH_INFO_TABLE *hash_info_table;
static int hash_table_max_count;

static struct sbrom_toc1_head_info  *toc1_head = NULL;
static struct sbrom_toc1_item_info  *toc1_item = NULL;
static unsigned int go_exec (u32 monitor_entry, u32 uboot_entry, int optee_entry, u32 freertos_entry, unsigned int dram_size);

static toc_name_addr_tbl toc_name_addr[] = {

#ifdef SCP_SRAM_BASE
	{ITEM_SCP_NAME,SCP_SRAM_BASE},
#endif

	{ITEM_LOGO_NAME, 0},
	{ITEM_SHUTDOWNCHARGE_LOGO_NAME, SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF},
	{ITEM_ANDROIDCHARGE_LOGO_NAME, SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_BUFF},
	{ITEM_DTB_NAME, 0x0},
	{ITEM_SOCCFG_NAME, 0x0},
	{ITEM_BDCFG_NAME, 0x0}

};

#define HASH_INFO_MAGIC 0x53454853
struct HASH_INFO_TABLE {
	uint32_t magic;
	uint16_t version;
	uint16_t count;
	struct HASH_INFO info[2];
};

static void prepare_hash_info_for_optee(struct spare_optee_head *optee_head)
{
	int i;
	if (!optee_head) {
		printf("optee head invalid\n");
		return;
	}
	if ((memcmp(optee_head->secure_boot_info.magic,
		    SUNXI_SECURE_BOOT_INFO_MAGIC, 8) == 0) &&
	    (optee_head->secure_boot_info.version >= 1)) {
		optee_head->secure_boot_info.hash_info_count =
			hash_info_table->count >= 10 ? 10 :
						       hash_info_table->count;
		for (i = 0; i < optee_head->secure_boot_info.hash_info_count;
		     i++) {
			strcpy((char *)optee_head->secure_boot_info
				       .toc1_hash_info[i]
				       .name,
			       (char *)hash_info_table->info[i].name);
			memcpy(optee_head->secure_boot_info.toc1_hash_info[i]
				       .hash,
			       hash_info_table->info[i].hash, 32);
		}
	} else {
#if defined(CONFIG_HASH_INFO_TABLE_BASE)
		rtc_set_hash_entry((uint32_t)hash_info_table);
#else
		printf("no place for hash table info");
#endif
	}
}

static int hash_filter(char *name)
{
	const static char *by_pass_hash_list[] = { "u-boot", "scp", "optee" };
	int i;
	for (i = 0; i < ARRAY_SIZE(by_pass_hash_list); i++) {
		if ((strlen(name) == strlen(by_pass_hash_list[i])) &&
		    strcmp(name, by_pass_hash_list[i]) == 0) {
			return -1;
		}
	}
	return 0;
}

static void store_hash_for_optee(char *name, uint8_t *hash)
{
	if (hash_filter(name) != 0) {
		return;
	}
	if (hash_info_table->count >= hash_table_max_count) {
		printf("no room for hash %s\n", name);
		return;
	}
	printf("load %s hash\n", name);
	strcpy((char *)hash_info_table->info[hash_info_table->count].name,
	       name);
	memcpy(hash_info_table->info[hash_info_table->count].hash, hash, 32);
	hash_info_table->count++;
}

static void store_key_hash_for_optee(char *name, sunxi_key_t *pubkey)
{
	uint8_t key_hash[32];
	char key_name[16];
	if (hash_filter(name) != 0) {
		return;
	}

	strcpy(key_name, name);
	if (strcmp("rotpk", key_name) != 0)
		strcat(key_name, "-key");

	sunxi_pubkey_hash_cal((char *)key_hash, pubkey);

	store_hash_for_optee(key_name, key_hash);
}

static int init_hash_table_for_optee(void)
{
	int hash_table_size;
#if defined(CONFIG_HASH_INFO_TABLE_BASE)
	hash_info_table = (struct HASH_INFO_TABLE *)CONFIG_HASH_INFO_TABLE_BASE;
	hash_table_size = CONFIG_HASH_INFO_TABLE_SIZE;
#else
	/*macro impl use 4 byte gap to avoid boarder issue, keep same behavior*/
	hash_table_size = 512 - 4;
	hash_info_table = malloc(hash_table_size);
	if (!hash_info_table) {
		printf("malloc hash table failed\n");
		hash_table_max_count = 0;
		return 0;
	}
#endif

	hash_table_max_count =
		(hash_table_size - sizeof(struct HASH_INFO_TABLE)) /
		sizeof(struct HASH_INFO);
	/*TABLE struct already have 2 HASH_INFO, count them in*/
	hash_table_max_count += 2;

	hash_info_table->magic   = HASH_INFO_MAGIC;
	hash_info_table->version = 0;
	hash_info_table->count   = 0;

	return 0;
}

static void install_rotpk_hash_for_rtos(void* install_addr) {
	int i;
	for( i = 0;i< hash_info_table->count;i++){
		if(strcmp("rotpk",(char*)hash_info_table->info[i].name)==0){
			memcpy(install_addr, hash_info_table->info[i].hash, 32);
			return;
		}
	}
	printf("rotpk hash no found\n");
	return;
}

static int get_item_addr(char* name, uint* addr)
{
	int i = 0;
	for(i = 0; i < ARRAY_SIZE(toc_name_addr); i++)
	{
		if(0 == strcmp(toc_name_addr[i].name, name))
		{
			*addr =  toc_name_addr[i].addr;
			return 0;
		}
	}
	return -1;
}

int get_old_verion(void)
{
	int i, nv, count_32bit, temp_nv = 0;

	count_32bit = EFUSE_NV_BIT/32;

	for(i = count_32bit; i > 0; i--)
	{
		nv = sid_read_key(EFUSE_NV1 + (i - 1)*4);
		if (nv > 0)
		{
			while (nv)
			{
				++temp_nv;
				nv >>= 1;
			}
			nv = temp_nv + (i - 1)*32;
			break;
		}
	}

	return nv;
}


void set_new_version(int new_ver, int old_ver , int word_ofs, int bit_ofs_in_word)
{
	int efuse_nv_format;

	if (new_ver > old_ver)
	{
		printf("the verion need update\n");

		bit_ofs_in_word -= 1;
		efuse_nv_format = (1 << bit_ofs_in_word);
		sid_program_key(EFUSE_NV1 + word_ofs*4, efuse_nv_format);
	}
}

static int sboot_verify_version(int main_v, int sub_v)
{
	int nv, offset_32bit, offset_bit;

	main_v &= 0xff;
	sub_v  = 0;

	if (main_v > EFUSE_NV_BIT)
	{
		printf("the version is too big, %d.0>%d.0\n", main_v, EFUSE_NV_BIT);

		return -1;
	}

	nv = get_old_verion();

	printf("OLD version: %d.0\n", nv);
	printf("NEW version: %d.%d\n", main_v, sub_v);

	if (main_v < nv)
	{
		printf("the version of this image(%d.0) is older than the previous version (%d.0)\n", main_v, nv);

		return -1;
	}

	/* get burn offset */
	offset_32bit = main_v / 32;
	offset_bit = main_v % 32;

	/* burn key to verify version */
	set_new_version(main_v, nv, offset_32bit, offset_bit);

	return 0;

}


int toc1_init(void)
{
	toc1_head = (struct sbrom_toc1_head_info *)CONFIG_BOOTPKG_BASE;
	toc1_item = (struct sbrom_toc1_item_info *)(CONFIG_BOOTPKG_BASE + sizeof(struct sbrom_toc1_head_info));

#ifdef DEBUG
	__maybe_unused struct sbrom_toc1_item_info  *item_head = toc1_item;

	printf("*******************TOC1 Head Message*************************\n");
	printf("Toc_name          = %s\n",   toc1_head->name);
	printf("Toc_magic         = 0x%x\n", toc1_head->magic);
	printf("Toc_add_sum	      = 0x%x\n", toc1_head->add_sum);

	printf("Toc_serial_num    = 0x%x\n", toc1_head->serial_num);
	printf("Toc_status        = 0x%x\n", toc1_head->status);

	printf("Toc_items_nr      = 0x%x\n", toc1_head->items_nr);
	printf("Toc_valid_len     = 0x%x\n", toc1_head->valid_len);
	printf("TOC_MAIN_END      = 0x%x\n", toc1_head->end);
	printf("TOC_MAIN_VERSION  = 0x%x\n", toc1_head->version_main);
	printf("TOC_SUB_VERSION   = 0x%x\n", toc1_head->version_sub);
	printf("***************************************************************\n\n");

	for(i=0;i<toc1_head->items_nr;i++,item_head++)
	{
		printf("\n*******************TOC1 Item Message*************************\n");
		printf("Entry_name        = %s\n",   item_head->name);
		printf("Entry_data_offset = 0x%x\n", item_head->data_offset);
		printf("Entry_data_len    = 0x%x\n", item_head->data_len);

		printf("encrypt	          = 0x%x\n", item_head->encrypt);
		printf("Entry_type        = 0x%x\n", item_head->type);
		printf("run_addr          = 0x%x\n", item_head->run_addr);
		printf("index             = 0x%x\n", item_head->index);

		printf("Entry_call        = 0x%x\n", item_head->run_addr);
		printf("Entry_end         = 0x%x\n", item_head->end);
		printf("***************************************************************\n\n");
	}
#endif
	if (sboot_verify_version(toc1_head->version_main, toc1_head->version_sub))
	{
		return -1;
	}
	return 0;
}


uint toc1_item_read(struct sbrom_toc1_item_info *p_toc_item, void * p_dest, u32 buff_len)
{
	u32 to_read_blk_start = 0;
	u32 to_read_blk_sectors = 0;
	s32 ret = 0;

	if( buff_len  < p_toc_item->data_len )
		return 0;

	to_read_blk_start   = (p_toc_item->data_offset)>>9;
	to_read_blk_sectors = (p_toc_item->data_len + 0x1ff)>>9;

	ret = sunxi_flash_read(to_read_blk_start, to_read_blk_sectors, p_dest);
	if(ret != to_read_blk_sectors)
		return 0;

	return ret * 512;
}

uint toc1_item_read_rootcertif(void * p_dest, u32 buff_len)
{
	u32 to_read_blk_start = 0;
	u32 to_read_blk_sectors = 0;
	s32 ret = 0;
	struct sbrom_toc1_item_info *p_toc_item = toc1_item;

	if( buff_len  < p_toc_item->data_len )
		return 0;

	to_read_blk_start   = (p_toc_item->data_offset)>>9;
	to_read_blk_sectors = (p_toc_item->data_len + 0x1ff)>>9;

	ret = sunxi_flash_read(to_read_blk_start, to_read_blk_sectors, p_dest);
	if(ret != to_read_blk_sectors)
		return 0;

	return ret*512;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :  成功：当前索引号>0   失败：找不到索引号，返回-1
*
*    note          :  自动获取下一个索引号对应的所有成员
*                     如果失败，通常是索引到了尽头
*
************************************************************************************************************
*/
int toc1_item_probe_next(sbrom_toc1_item_group *item_group)
{
	struct sbrom_toc1_head_info  *p_head = toc1_head;
	struct sbrom_toc1_item_info  *item_head;
	char  *item_name = NULL;
	int    i;

	for(i=1; i<p_head->items_nr; i++)
	{
		item_head = toc1_item + i;

		if(item_name != NULL)
		{
			if(!strcmp(item_name, item_head->name))
			{
				if(item_head->type == TOC_ITEM_ENTRY_TYPE_BIN_CERTIF)
				{
					if(item_group->bin_certif == NULL)
						item_group->bin_certif = item_head;
					else
						return -1;
				}
				else if(item_head->type == TOC_ITEM_ENTRY_TYPE_BIN)
				{
					if(item_group->binfile == NULL)
						item_group->binfile = item_head;
					else
						return -2;
				}
				else
				{
					printf("unknown item type\n");
					return -3;
				}
				item_head->reserved[0] = 1;

				return 1;
			}
		}
		else if(!item_head->reserved[0])
		{
			if(item_head->type == TOC_ITEM_ENTRY_TYPE_BIN_CERTIF)
			{
				item_group->bin_certif = item_head;
			}
			else if(item_head->type == TOC_ITEM_ENTRY_TYPE_BIN)
			{
				item_group->binfile = item_head;
			}
			else if(item_head->type == TOC_ITEM_ENTRY_TYPE_LOGO)
			{
				item_head->reserved[0] = 1;
				item_name = item_head->name;
				item_group->binfile = item_head;

				return 1;
			}
			else
			{
				printf("unknown item type\n");

				return -1;
			}
			item_head->reserved[0] = 1;
			item_name = item_head->name;
		}
	}

	if(item_group->bin_certif == NULL)
		return 0;
	else
		return 1;
}

#define  SUNXI_X509_CERTIFF_MAX_LEN   (4096)
int toc1_verify_and_run(u32 dram_size)
{
	sbrom_toc1_item_group item_group;
	int ret;
	uint len, i;
	__maybe_unused int dram_para_offset = 0;
	__maybe_unused void *dram_para_addr = (void *)toc0_config->dram_para;
	u8 buffer[SUNXI_X509_CERTIFF_MAX_LEN];
	u16 pmu_type = 0, key_input = 0; /* TODO: set real value */

	sunxi_certif_info_t  root_certif;
	sunxi_certif_info_t  sub_certif;
	u8  hash_of_file[256];
	uint optee_entry=0;
	uint monitor_entry=0, uboot_entry=0;
	uint freertos_entry=0;
	__maybe_unused struct spare_boot_head_t *bfh = NULL;
	__maybe_unused struct spare_monitor_head *monitor_head = NULL;
	u32 image_base;

	boot_info("probe root certif\n");
	sunxi_ce_open();
	init_hash_table_for_optee();

	memset(buffer, 0, SUNXI_X509_CERTIFF_MAX_LEN);
	len = toc1_item_read_rootcertif(buffer, SUNXI_X509_CERTIFF_MAX_LEN);
	if(!len)
	{
		printf("error: cant read rootkey certif\n");

		return -1;
	}
	if(sunxi_root_certif_pk_verify(&root_certif, buffer, len))
	{
		printf("root certif pk verify failed\n");
		return -1;
	}
	else
	{
		boot_info("certif valid: the root key is valid\n");
	}
	if(sunxi_certif_verify_itself(&root_certif, buffer, len))
	{
		printf("root certif verify itself failed\n");
		return -1;
	}
	store_key_hash_for_optee("rotpk", &root_certif.pubkey);
	do
	{
		memset(&item_group, 0, sizeof(sbrom_toc1_item_group));
		ret = toc1_item_probe_next(&item_group);
		if(ret < 0)
		{
			printf("%s: error in toc1_item_probe_next\n", __func__);
			return -1;
		}
		else if(ret == 0)
		{
			boot_info("sbromsw_toc1_traverse find out all items\n");

			printf("monitor entry=0x%x\n", monitor_entry);
			printf("uboot entry=0x%x\n", uboot_entry);
			printf("optee entry=0x%x\n", optee_entry);
			printf("freertos entry=0x%x\n", freertos_entry);

			monitor_head  = (struct spare_monitor_head *)monitor_entry;
			monitor_head->secureos_base = optee_entry;

			bfh = (struct spare_boot_head_t *)uboot_entry;
			bfh->boot_data.pmu_type = pmu_type;
			bfh->boot_data.uart_input = uart_input_value;
			bfh->boot_data.key_input = key_input;
			bfh->boot_data.debug_mode = debug_mode;
#if 0
			bfh->boot_ext[0].data[0] = pmu_type;
			bfh->boot_ext[0].data[1] = uart_input_value;
			bfh->boot_ext[0].data[2] = lradc_input_value;
			bfh->boot_ext[0].data[3] = debug_enable;
#endif
			go_exec(monitor_entry, uboot_entry, optee_entry, freertos_entry, dram_size);
			return 0;
		}
		if(item_group.bin_certif)
		{
			memset(buffer, 0, SUNXI_X509_CERTIFF_MAX_LEN);
			len = toc1_item_read(item_group.bin_certif, buffer, SUNXI_X509_CERTIFF_MAX_LEN);
			if(!len)
			{
				printf("%s error: cant read content key certif\n", __func__);

				return -1;
			}
			if(sunxi_certif_verify_itself(&sub_certif, buffer, len))
			{
				printf("%s error: cant verify the content certif\n", __func__);

				return -1;
			}

			for(i=0;i<root_certif.extension.extension_num;i++)
			{
				if(!strcmp((const char *)root_certif.extension.name[i], item_group.bin_certif->name))
				{
					boot_info("find %s key stored in root certif\n", item_group.bin_certif->name);

					if(memcmp(root_certif.extension.value[i], sub_certif.pubkey.n+1, sub_certif.pubkey.n_len-1))
					{
						printf("%s key n is incompatible\n", item_group.bin_certif->name);
						printf(">>>>>>>key in rootcertif<<<<<<<<<<\n");
						ndump((u8 *)root_certif.extension.value[i], sub_certif.pubkey.n_len-1);
						printf(">>>>>>>key in certif<<<<<<<<<<\n");
						ndump((u8 *)sub_certif.pubkey.n+1, sub_certif.pubkey.n_len-1);

						return -1;
					}
					if(memcmp(root_certif.extension.value[i] + sub_certif.pubkey.n_len-1, sub_certif.pubkey.e, sub_certif.pubkey.e_len))
					{
						printf("%s key e is incompatible\n", item_group.bin_certif->name);
						printf(">>>>>>>key in rootcertif<<<<<<<<<<\n");
						ndump((u8 *)root_certif.extension.value[i] + sub_certif.pubkey.n_len-1, sub_certif.pubkey.e_len);
						printf(">>>>>>>key in certif<<<<<<<<<<\n");
						ndump((u8 *)sub_certif.pubkey.e, sub_certif.pubkey.e_len);

						return -1;
					}

					store_key_hash_for_optee(
						item_group.bin_certif->name,
						&sub_certif.pubkey);
#ifdef CFG_SUNXI_ITEM_HASH
					store_hash_for_optee(
						item_group.bin_certif->name,
						sub_certif.extension.value[0]);
#endif

					break;
				}
			}
			if(i==root_certif.extension.extension_num)
			{
				printf("cant find %s key stored in root certif", item_group.bin_certif->name);

				return -1;
			}
		}

		if(item_group.binfile)
		{
			uint addr = 0;
			if(0 == get_item_addr(item_group.binfile->name, &addr))
			{
				if (!strcmp(item_group.binfile->name, ITEM_SCP_NAME))
				{
#ifdef SCP_SRAM_BASE
					sunxi_flash_read(item_group.binfile->data_offset/512, (SCP_SRAM_SIZE+511)/512, (void *)SCP_SRAM_BASE);
					sunxi_flash_read((item_group.binfile->data_offset+SCP_CODE_DRAM_OFFSET)/512, (SCP_DRAM_SIZE+511)/512, (void *)SCP_DRAM_BASE);
					sunxi_deassert_arisc();
#endif

				}
				else if (!strcmp(item_group.binfile->name, ITEM_DTB_NAME))
				{
					sunxi_flash_read(
						item_group.binfile->data_offset /
							512,
						(item_group.binfile->data_len +
						 511) / 512,
						(void *)(uboot_entry +
							 SUNXI_DTB_OFFSET));
				}
				else if (!strcmp(item_group.binfile->name, ITEM_SOCCFG_NAME))
				{
					sunxi_flash_read(
						item_group.binfile->data_offset /
							512,
						(item_group.binfile->data_len +
						 511) / 512,
						(void *)(uboot_entry +
							 SUNXI_CFG_OFFSET));
				}
#ifdef CONFIG_RELOCATE_PARAMETER
				else if (!strcmp(item_group.binfile->name,
						 ITEM_PARAMETER_NAME)) {
					sunxi_flash_read(
					    item_group.binfile->data_offset /
						512,
					    (BOOT_PARAMETER_SIZE + 511) / 512,
					    (void *)
						CONFIG_SUNXI_PARAMETER_ADDR);
				}
#endif
#ifdef CONFIG_SUNXI_ESM_HDCP
				else if (!strcmp(item_group.binfile->name,
						 ITEM_ESM_IMG_NAME)) {
					*(uint *)(SUNXI_ESM_IMG_SIZE_ADDR) =
					    item_group.binfile->data_len;
					sunxi_flash_read(
					    item_group.binfile->data_offset /
						512,
					    (item_group.binfile->data_len +
					     511) /
						512,
					    (void *)SUNXI_ESM_IMG_BUFF_ADDR);
				}
#endif
				else if (!strcmp(item_group.binfile->name,
						 ITEM_LOGO_NAME)) {
					*(uint *)(uboot_entry +
						  SUNXI_LOGO_OFFSET) =
						item_group.binfile->data_len;
					sunxi_flash_read(
						item_group.binfile->data_offset / 512,
						(item_group.binfile->data_len + 511) /
							512,
						(void *)(uboot_entry +
							 SUNXI_LOGO_OFFSET + 16));
					set_uboot_func_mask(UBOOT_FUNC_MASK_BIT_BOOTLOGO);
				} else {
#ifdef CONFIG_SUNXI_SMP_BOOT
					if (strncmp(item_group.binfile->name, ITEM_SHUTDOWNCHARGE_LOGO_NAME, sizeof(ITEM_SHUTDOWNCHARGE_LOGO_NAME)) == 0)
					{
						*(uint *)(SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_SIZE_ADDR) =item_group.binfile->data_len;
					}
					else if (strncmp(item_group.binfile->name, ITEM_ANDROIDCHARGE_LOGO_NAME, sizeof(ITEM_ANDROIDCHARGE_LOGO_NAME)) == 0)
					{
						*(uint *)(SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_SIZE_ADDR) = item_group.binfile->data_len;
					}
#endif
					sunxi_flash_read(item_group.binfile->data_offset/512,
						(item_group.binfile->data_len+511)/512,
						(void *)addr);
				}
			}
			else
			{
				image_base = item_group.binfile->run_addr;
#ifdef CFG_SUNXI_GUNZIP
				if(!strcmp(item_group.binfile->name, ITEM_RTOS_GZ_NAME)) {
					image_base = 0x40000000; /* hardcode here, as we can't get image_base from gz header */
				}
#endif
#ifdef CFG_SUNXI_LZ4
				if(!strcmp(item_group.binfile->name, ITEM_RTOS_LZ4_NAME)) {
					image_base = 0x40000000; /* hardcode here, as we can't get image_base from lz4 header */
				}
#endif
#ifdef CFG_SUNXI_LZMA
				if(!strcmp(item_group.binfile->name, ITEM_RTOS_LZMA_NAME)) {
					image_base = 0x40000000; /* hardcode here, as we can't get image_base from lzma header */
				}
#endif

				//读出bin文件内容到内存
				len = sunxi_flash_read(item_group.binfile->data_offset/512, (item_group.binfile->data_len+511)/512, (void *)image_base);
				if(!len)
				{
					printf("%s error: cant read bin file\n", __func__);

					return -1;
				}
				//计算文件hash
				memset(hash_of_file, 0, sizeof(hash_of_file));
				ret = sunxi_sha_calc(hash_of_file, sizeof(hash_of_file), (void *)image_base, item_group.binfile->data_len);
				//ret = sunxi_sha_calc(hash_of_file, sizeof(hash_of_file), (u8 *)0x2a000000, item_group.binfile->data_len);
				if(ret)
				{
					printf("calc sha256 with hardware err\n");

					return -1;
				}
				//使用内容证书的扩展项，和文件hash进行比较
				//开始比较文件hash(小机端阶段计算得到)和证书hash(PC端计算得到)
				if(memcmp(hash_of_file, sub_certif.extension.value[0], 32))
				{
					printf("%s:hash compare is not correct\n",item_group.binfile->name);
					printf(">>>>>>>hash of file<<<<<<<<<<\n");
					ndump((u8 *)hash_of_file, 32);
					printf(">>>>>>>hash in certif<<<<<<<<<<\n");
					ndump((u8 *)sub_certif.extension.value[0], 32);

					return -1;
				}

				boot_info("ready to run %s\n", item_group.binfile->name);
				if(!strcmp(item_group.binfile->name, "monitor"))
					monitor_entry = image_base;
				else if(!strcmp(item_group.binfile->name, "u-boot"))
					uboot_entry = image_base;
				else if(!strcmp(item_group.binfile->name, "optee"))
					optee_entry = image_base;
				else if(!strcmp(item_group.binfile->name, "freertos"))
					freertos_entry = image_base;
#ifdef CFG_SUNXI_GUNZIP
				else if(!strcmp(item_group.binfile->name, ITEM_RTOS_GZ_NAME)) {
					freertos_entry = image_base;
					void *dst = (void *)freertos_entry;
					int dstlen = *(unsigned long *)(CONFIG_BOOTPKG_BASE + item_group.binfile->data_offset + item_group.binfile->data_len - 4);
					unsigned char *src = (unsigned char *)(CONFIG_BOOTPKG_BASE + item_group.binfile->data_offset);
					unsigned long srclen = item_group.binfile->data_len;
					unsigned long *lenp = &srclen;
					ret = gunzip(dst, dstlen, src, lenp);
					if (ret)
						return -1;
				}
#endif
#ifdef CFG_SUNXI_LZ4
				else if(!strcmp(item_group.binfile->name, ITEM_RTOS_LZ4_NAME)) {
					freertos_entry = image_base;
					void *dst = (void *)freertos_entry;
					unsigned int dstlen = 0x800000;
					unsigned char *src = (unsigned char *)(CONFIG_BOOTPKG_BASE + item_group.binfile->data_offset);
					unsigned long srclen = item_group.binfile->data_len;
					int ret = ulz4fn(src, srclen, dst, &dstlen);
					if (ret)
						return -1;
				}
#endif
#ifdef CFG_SUNXI_LZMA
				else if(!strcmp(item_group.binfile->name, ITEM_RTOS_LZMA_NAME)) {
					freertos_entry = image_base;
					SizeT src_len = ~0UL, dst_len = ~0UL;
					void *dst = (void *)image_base;
					unsigned char *src = (unsigned char *)(CONFIG_BOOTPKG_BASE + item_group.binfile->data_offset);
					ret = lzmaBuffToBuffDecompress(dst, &src_len, src, dst_len);
					if (ret)
						return -1;
				}
#endif

			}
		}
	}
	while(1);

	return 0;
}

unsigned int go_exec (u32 monitor_entry, u32 uboot_entry, int optee_entry, u32 freertos_entry, unsigned int dram_size)
{
	struct spare_boot_head_t *bfh = (struct spare_boot_head_t *)uboot_entry;
	toc0_private_head_t *toc0 = (toc0_private_head_t *)CONFIG_TOC0_HEAD_BASE;
	int boot_type = toc0->platform[0]&0xf;
	int card_work_mode = toc0_config->card_work_mode;
	struct sec_mem_region sec_mem_map[5];

	if(!boot_type)
	{
		boot_type = 1;
	}
	else if(boot_type == 1)
	{
		boot_type = 0;
	}
#ifdef CFG_SUNXI_MMC
	else if(boot_type == 2){
		//char  storage_data[384];  // 0-159:nand,160-255: mmc
		set_mmc_para(2,(void *)(toc0_config->storage_data+160), uboot_entry);
	}
#endif
#ifdef CFG_SUNXI_SPINAND
	/* brom boot_type enum is different from uboot, remap it */
	else if (boot_type == 4) {
		boot_type = 5;
	}
#endif

	if(card_work_mode)
	{
		bfh->boot_data.work_mode = card_work_mode;
		boot_info("card_work_mode=%d\n", card_work_mode);
	}

	memset(sec_mem_map, 0, sizeof(sec_mem_map));
	if (monitor_entry) {
		sec_mem_map[0].startAddr = monitor_entry;
		sec_mem_map[0].size      = 0x100000;
	} else if (optee_entry) {
		sec_mem_map[0].startAddr = optee_entry;
		sec_mem_map[0].size      = 0x100000;
	} else {
		/*fall back to uboot, no smc config*/
	}
	sunxi_smc_config(sec_mem_map);
	sunxi_drm_config(SDRAM_OFFSET(dram_size << 20),
			 toc0_config->secure_dram_mbytes << 20);

	if(!freertos_entry) {
		boot_info("storage_type=%d\n", boot_type);
		bfh->boot_data.storage_type = boot_type;
		bfh->boot_data.dram_scan_size = dram_size;
		bfh->boot_data.secureos_exist = optee_entry ? 1 : 0;
		bfh->boot_data.monitor_exist = \
			monitor_entry ? SUNXI_SECURE_MODE_USE_SEC_MONITOR:0;
		memcpy(bfh->boot_data.dram_para, toc0_config->dram_para, 32 * 4);
		bfh->boot_data.func_mask = get_uboot_func_mask(UBOOT_FUNC_MASK_ALL);
	}

	mmu_disable();
	printf("run out of boot0\n");

	prepare_hash_info_for_optee((struct spare_optee_head *)optee_entry);

	if (monitor_entry) {
		struct spare_optee_head  *head = (struct  spare_optee_head  *)optee_entry;
		head->dram_size= dram_size;
		head->drm_size= toc0_config->secure_dram_mbytes;
		boot0_jmp_monitor(monitor_entry);
	}
	else if (optee_entry) {
		struct spare_optee_head  *head = (struct  spare_optee_head  *)optee_entry;

		head->dram_size= dram_size;
		head->drm_size= toc0_config->secure_dram_mbytes;

		memcpy(head->dram_para, toc0_config->dram_para, 32*sizeof(int));
		memcpy(head->chipinfo, &(toc0_config->jtag_gpio[4]), 8);

		asm volatile ("mov lr, %0" :: "r" (uboot_entry) : "memory");
		asm volatile ("bx      %0" :: "r" (optee_entry) : "memory");
	}
	else if (freertos_entry) {
		/* If no rtos head, do not set rotpk hash and dram size */
		if (!memcmp(((struct spare_rtos_head_t *)freertos_entry)->boot_head.magic, "rtos", 4)) {
			install_rotpk_hash_for_rtos(&(((struct spare_rtos_head_t *)freertos_entry)
				       ->rotpk_hash));
			/* update dram size */
			((struct spare_rtos_head_t *)freertos_entry)->rtos_dram_size = dram_size;
		}
		printf("jump to rtos\n");
		boot0_jmp(freertos_entry);
	} else {
		boot0_jmp(uboot_entry);
	}

	return 0;
}
