/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2008 RuggedCom, Inc.
 * Richard Retanubun <RichardRetanubun@RuggedCom.com>
 */

/*
 * See also linux/fs/partitions/efi.h
 *
 * EFI GUID Partition Table
 * Per Intel EFI Specification v1.02
 * http://developer.intel.com/technology/efi/efi.htm
*/

#ifndef _DISK_PART_EFI_H
#define _DISK_PART_EFI_H

typedef struct {
	u8 b[16];
} efi_guid_t;

#define SUNXI_MBR_SIZE  (16 * 1024)
#define GPT_HEADER_SIGNATURE 0x5452415020494645ULL

typedef u16 efi_char16_t;

typedef struct _gpt_header {
	__le64 signature;
	__le32 revision;
	__le32 header_size;
	__le32 header_crc32;
	__le32 reserved1;
	__le64 my_lba;
	__le64 alternate_lba;
	__le64 first_usable_lba;
	__le64 last_usable_lba;
	efi_guid_t disk_guid;
	__le64 partition_entry_lba;
	__le32 num_partition_entries;
	__le32 sizeof_partition_entry;
	__le32 partition_entry_array_crc32;
} __packed gpt_header;

typedef union _gpt_entry_attributes {
	struct {
		u64 required_to_function:1;
		u64 no_block_io_protocol:1;
		u64 legacy_bios_bootable:1;
		/* u64 reserved:45; */
		u64 reserved:27;
		u64 user_type:16;
		u64 ro:1;
		u64 keydata:1;
		u64 type_guid_specific:16;
	} fields;
	unsigned long long raw;
} __packed gpt_entry_attributes;

#define PARTNAME_SZ	(72 / sizeof(efi_char16_t))
typedef struct _gpt_entry {
	efi_guid_t partition_type_guid;
	efi_guid_t unique_partition_guid;
	__le64 starting_lba;
	__le64 ending_lba;
	gpt_entry_attributes attributes;
	efi_char16_t partition_name[PARTNAME_SZ];
} __packed gpt_entry;

int get_part_info_by_name(char* name,uint* start_sector,uint* sectors);
int init_gpt_nor(void);

#define  GPT_ENTRY_OFFSET        1024
#define  GPT_HEAD_OFFSET         512

#endif	/* _DISK_PART_EFI_H */
