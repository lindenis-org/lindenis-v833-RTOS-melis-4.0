#ifndef SUNXIKBD_DEBUG_H
#define SUNXIKBD_DEBUG_H

#include "../hal/sunxi_hal_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUNXIKBD_DEBUG
#ifdef  SUNXIKBD_DEBUG
#define SUNXIKBD_INFO(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)
#else
#define SUNXIKBD_INFO(fmt, args...)
#endif

#define SUNXIKBD_ERR(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)


#define INITIAL_VALUE 0xff
#define KEY_MAX_CNT         (8)
#define VOL_NUM             KEY_MAX_CNT
#define MAXIMUM_INPUT_VOLTAGE       1800
#define DEVIATION           100
#define SUNXIKEY_DOWN           (MAXIMUM_INPUT_VOLTAGE-DEVIATION)
#define SUNXIKEY_UP         SUNXIKEY_DOWN
#define MAXIMUM_SCALE           128
#define SCALE_UNIT          (MAXIMUM_INPUT_VOLTAGE/MAXIMUM_SCALE)
#define SAMPLING_FREQUENCY      10
#define EVENT_DATA_SIZE         10

#define EVN_KEY 0x00
#define KEYBOARD_DOWN 0
#define KEYBOARD_UP 1

struct sunxikbd_config
{
    unsigned int measure;
    unsigned int key_num;
    unsigned int scankeycodes[KEY_MAX_CNT];
    unsigned int key_vol[KEY_MAX_CNT];
    char *name;
};


struct sunxikbd_drv_data
{
    unsigned int scankeycodes[KEY_MAX_CNT];
    unsigned char compare_later;
    unsigned char compare_before;
    unsigned char key_code;
    unsigned char last_key_code;
    unsigned char key_cnt;
};

int sunxi_keyboard_init(void);


#ifdef __cplusplus
}
#endif

#endif
