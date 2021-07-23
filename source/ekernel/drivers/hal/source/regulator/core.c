/*
 * Copyright (c) 2020 Allwinner Technology Co., Ltd. ALL rights reserved.
 */

#include <debug.h>
#include <sunxi_hal_regulator.h>

int hal_regulator_get_voltage(struct regulator_dev *rdev,
			      int *vol_uV)
{
	int ret = 0;

	if (rdev && rdev->ops && rdev->ops->get_voltage) {
		ret = rdev->ops->get_voltage(rdev, vol_uV);
	} else {
		software_break();
		__err("fatal error.");
	}
	return ret;
}

int hal_regulator_set_voltage(struct regulator_dev *rdev,
			      int target_uV)
{
	int ret = 0;

	if (rdev && rdev->ops && rdev->ops->set_voltage) {
		ret = rdev->ops->set_voltage(rdev, target_uV);
	} else {
		software_break();
		__err("fatal error.");
	}
	return ret;
}

int hal_regulator_enable(struct regulator_dev *rdev)
{
	int ret = 0;

	if (rdev && rdev->ops && rdev->ops->enable) {
		ret = rdev->ops->enable(rdev);
	} else {
		software_break();
		__err("fatal error.");
	}
	return ret;
}

int hal_regulator_disable(struct regulator_dev *rdev)
{
	int ret = 0;

	if (rdev && rdev->ops && rdev->ops->disable) {
		ret = rdev->ops->disable(rdev);
	} else {
		software_break();
		__err("fatal error.");
	}

	return ret;
}
