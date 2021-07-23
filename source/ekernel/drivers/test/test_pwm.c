#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>

#include <sunxi_drv_pwm.h>
#include <sunxi_hal_pwm.h>

static int cmd_drv_pwm(int argc, char **argv)
{

    struct pwm_config *config ;
    uint8_t port;

    if (argc < 2)
    {
        printf("Usage:drv pwm port\n");
        return -1;
    }

    port = strtol(argv[1], NULL, 0);
    printf("port = %d\n", port);

    config = (struct pwm_config *)malloc(sizeof(struct pwm_config));

    config->duty_ns   = 8000000;
    config->period_ns = 10000000;
    config->polarity  = PWM_POLARITY_NORMAL;

    rt_device_t fd;

    fd = rt_device_find("pwm");

    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);

    rt_device_init(fd);

    rt_device_control(fd, port, config);

    rt_device_close(fd);

    return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_drv_pwm, __cmd_drvpwm, drv pwm test code);

static int cmd_pwm(int argc, char **argv)
{

    struct pwm_config *config ;
    uint8_t port;

    if (argc < 2)
    {
        printf("Usage: pwm port\n");
        return -1;
    }

    port = strtol(argv[1], NULL, 0);
    printf("port = %d\n", port);

    config = (struct pwm_config *)malloc(sizeof(struct pwm_config));

    config->duty_ns   = 8000000;
    config->period_ns = 10000000;
    config->polarity  = PWM_POLARITY_NORMAL;

    hal_pwm_init();

    hal_pwm_control(port, config);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_pwm, __cmd_pwm, pwm test code);
