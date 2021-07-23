/*
 * ===========================================================================================
 *
 *       Filename:  thermal.c
 *
 *    Description:  implemtaton of spi driver core based on hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-12-17 17:31:59
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  bantao@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2019-12-18 15:57:57
 *
 * ===========================================================================================
 */
#include <sunxi_drv_thermal.h>
#include <sunxi_drv_efuse.h>

hal_thermal_status_t hal_thermal_init(void)
{
	char buf[8];
	int ret;

	//TODO: clk_init();
	ret = hal_efuse_read("thermal_sensor", buf, 64);
	if (ret <= 0)
		return HAL_THERMAL_STATUS_ERROR_CALIBRATE;

	ths_calibrate((short int *)buf, 4);
	ths_init();

	return HAL_THERMAL_STATUS_OK;
}

hal_thermal_status_t hal_thermal_get_temp(unsigned int num, int *temp)
{
	int ret;

	ret = ths_get_temp(num, temp);
	if (ret)
		return HAL_THERMAL_STATUS_ERROR_BUSY;

	return HAL_THERMAL_STATUS_OK;
}

