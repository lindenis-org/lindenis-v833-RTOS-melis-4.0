#include <stdlib.h>
#include <hal_reset.h>

//static LIST_HEAD(reset_control_list);

int reset_control_register(struct reset_control_dev *rcdev) //for reset system
{
    return 0;
}

int reset_control_unregister(struct reset_control *reset) //for reset system
{
    return 0;
}

struct reset_control *hal_reset_control_get(hal_reset_type_t type_id, u32 reset_id)
{
    return NULL;
}


int hal_reset_control_assert(struct reset_control *reset)
{
    return 0;
}

int hal_reset_control_deassert(struct reset_control *reset)
{
    return 0;
}

int hal_reset_control_reset(struct reset_control *reset)
{
    return 0;
}

int hal_reset_control_status(struct reset_control *reset)
{
    return 0;
}

