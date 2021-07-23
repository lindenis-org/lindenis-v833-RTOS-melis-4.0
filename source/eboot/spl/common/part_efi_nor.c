#include <common.h>
#include <arch/spinor.h>
#include <part_efi_nor.h>

static void *gpt_data_point = NULL;

static int gpt_show_partition_info(char *buf)
{
	int i, j;

	char   char8_name[PARTNAME_SZ] = {0};
	gpt_header     *gpt_head  = (gpt_header*)(buf + GPT_HEAD_OFFSET);
	gpt_entry      *entry  = (gpt_entry*)(buf + GPT_ENTRY_OFFSET);

	printf("---------------------------\n");
	for(i = 0; i < gpt_head->num_partition_entries; i++ ){
		for(j = 0; j < PARTNAME_SZ; j++ ) {
			char8_name[j] = (char)(entry[i].partition_name[j]);
		}
		printf("GPT:%s: 0x%lx  0x%lx\n", char8_name, (long)entry[i].starting_lba, (long)entry[i].ending_lba);
	}
	printf("---------------------------\n");

	return 0;
}

static int is_gpt_valid(void *buffer)
{
	gpt_header *gpt_head = (gpt_header *)(buffer + GPT_HEAD_OFFSET);

	if(gpt_head->signature != GPT_HEADER_SIGNATURE)
	{
		printf("gpt magic error, %llx != %llx\n",gpt_head->signature, GPT_HEADER_SIGNATURE);
		return 0;
	}
	gpt_show_partition_info(buffer);
	return 1;
}

int get_part_info_by_name(char* name, uint* start_sector, uint* sectors)
{
	if(!gpt_data_point){
		printf("Still not initialized gpt,please initialize A first\n");
		return -1;
	}

	int i, j;

	char   char8_name[PARTNAME_SZ] = {0};
	gpt_header     *gpt_head  = (gpt_header*)(gpt_data_point + GPT_HEAD_OFFSET);
	gpt_entry      *entry  = (gpt_entry*)(gpt_data_point + GPT_ENTRY_OFFSET);

	if(gpt_head->signature != GPT_HEADER_SIGNATURE)
	{
		printf("gpt magic error, %llx != %llx\n",gpt_head->signature, GPT_HEADER_SIGNATURE);
		return -1;
	}

	for(i = 0; i < gpt_head->num_partition_entries; i++ ){
		for(j = 0; j < PARTNAME_SZ; j++ ) {
			char8_name[j] = (char)(entry[i].partition_name[j]);
		}
		if(!strcmp(name,char8_name)){
			*start_sector = (uint)entry[i].starting_lba;
			*sectors = ((uint)entry[i].ending_lba + 1) - (uint)entry[i].starting_lba;
			return 0;
		}
	}

	/* can't find the part */
	printf("can't find the part %s\n", name);
	return -1;
}

int init_gpt_nor(void)
{
	uint sectors;
	void  *buffer = NULL;

	if(spinor_init(0)) {
		printf("spinor init fail\n");
		return -1;
	}

	buffer = malloc(SUNXI_MBR_SIZE);
	if (!buffer) {
		printf("GPT: malloc faile\n");
		return -1;
	}

	sectors = 1 * SUNXI_MBR_SIZE / 512;
	printf("GPT load from sector:0x%x, sectors: 0x%x, save in dram: 0x%x\n",
			  CFG_SPINOR_GPT_ARD, sectors, (unsigned int)buffer);
	if (spinor_read(CFG_SPINOR_GPT_ARD, sectors, buffer)) {
		printf("load GPT from spinor fail\n");
		goto error;
	}

	if (is_gpt_valid(buffer)) {
		gpt_data_point = buffer;
	} else {
		printf("read gpt data fail\n");
		goto error;
	}

	return 0;

error:
	free(buffer);
	return -1;
}
