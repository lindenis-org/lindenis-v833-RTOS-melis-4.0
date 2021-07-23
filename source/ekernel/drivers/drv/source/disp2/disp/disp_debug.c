#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include <hal_mem.h>
#include "dev_disp.h"

static int cmd_disp_debug(int argc, const char **argv)
{
	int i = 0, err = 0;
	struct disp_manager *mgr = NULL;
	u32 num_screens;

	num_screens = bsp_disp_feat_get_num_screens();



	if (argc == 1) {
		disp_sys_show();
	} else {

		while(i < argc) {
			/*colorbar*/
			if ( ! strcmp(argv[i], "-c")) {
				if (argc > i+2) {
					i+=1;
					disp_colorbar_store(atoi(argv[i]), atoi(argv[i + 1]));
					i+=1;
				} else {
					DE_WRN("-c para error!\n");
					err++;
				}
			}
			/*switch display*/
			if ( ! strcmp(argv[i], "-s")) {
				if (argc > i+3) {
					i+=1;
					bsp_disp_device_switch(atoi(argv[i]), atoi(argv[i + 1]), atoi(argv[i + 2]));
					i+=2;
				} else {
					DE_WRN("-s para error!\n");
					err++;
				}
			}
			/*dump de data*/
			if ( ! strcmp(argv[i], "-d")) {
				if (argc > i+2) {
					i+=1;
					disp_capture_dump(atoi(argv[i]), argv[i + 1]);
					i+=1;
				} else {
					DE_WRN("-d para error!\n");
					err++;
				}
			}

			/*backlight*/
			if ( ! strcmp(argv[i], "-b")) {
				if (argc > i+2) {
					i+=1;
					if (atoi(argv[i]) < num_screens) {
						DE_WRN("set backligt:lcd%d %d\n", atoi(argv[i]), atoi(argv[i + 1]));
						mgr = g_disp_drv.mgr[atoi(argv[i])];
						mgr->device->set_bright(mgr->device, atoi(argv[i + 1]));
					}
					i+=1;
				} else {
					DE_WRN("-b para error!\n");
					err++;
				}
			}
			++i;
		}
	}
	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_disp_debug, __cmd_disp, disp);
