/*
 * Copyright (c) 2020 Allwinner Technology Co., Ltd. ALL rights reserved.
 */

#include <sunxi_hal_regulator.h>

int __attribute__((weak)) hal_regulator_get_voltage(struct regulator_dev *rdev,
						    int *vol_uV)
{
	return 0;
};

int __attribute__((weak)) hal_regulator_set_voltage(struct regulator_dev *rdev,
						    int target_uV)
{
	return 0;
};

int __attribute__((weak)) hal_regulator_enable(struct regulator_dev *rdev)
{
	return 0;
};

int __attribute__((weak)) hal_regulator_disable(struct regulator_dev *rdev)
{
	return 0;
};

int __attribute__((weak)) hal_regulator_get(unsigned int request_flag,
					    struct regulator_dev *rdev)
{
	return 0;
}
