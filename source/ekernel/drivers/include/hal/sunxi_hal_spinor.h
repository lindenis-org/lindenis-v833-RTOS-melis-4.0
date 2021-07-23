#ifndef SUNXI_HAL_SPINOR_H
#define SUNXI_HAL_SPINOR_H

#include <stdint.h>
#include <sunxi_hal_common.h>
#include <sunxi_hal_spi.h>
#include <hal_sem.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_SUPPORT_WRITE_PROTECTION 1
#define CONFIG_SUPPORT_INDIVIDUAL_BLOCK_MEMORY_PROTECTION 1
//#define CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION 0

#define SUNXI_HAL_SPINOR_API_VERSION 1
#define SUNXI_HAL_SPINOR_DRV_VERSION 0

typedef enum sunxi_hal_spinor_signal_event
{
    ARM_FLASH_EVENT_READY = (1UL << 0),
    ARM_FLASH_EVENT_ERROR = (1UL << 1),
} sunxi_hal_spinor_signal_event_t;

typedef struct sunxi_hal_spinor_status
{
    uint32_t busy:  1;
    uint32_t error: 1;
    uint32_t reserved: 30;
} sunxi_hal_spinor_status_t;

typedef struct _sunxi_hal_spinor_sector_info
{
    uint32_t start;
    uint32_t end;
} sunxi_hal_spinor_sector_info;

typedef struct _sunxi_hal_spinor_info
{
    sunxi_hal_spinor_sector_info *sector_info;
    uint32_t sector_count;
    uint32_t sector_size;
    uint32_t page_size;
    uint32_t program_unit;
    uint8_t  erased_value;
    uint8_t  reserved[3];
} sunxi_hal_spinor_info;

typedef struct sunxi_hal_spinor_capabilities
{
    uint32_t event_ready: 1;
    uint32_t data_width: 2;
    uint32_t erase_chip: 1;
    uint32_t reserved: 28;
} sunxi_hal_spinor_capabilities_t;

typedef struct sunxi_hal_driver_spinor
{
    sunxi_hal_version_t (*get_version)(int32_t dev);
    sunxi_hal_spinor_capabilities_t (*get_capabilities)(void);
    int32_t (*initialize)(sunxi_hal_spinor_signal_event_t cb_event);
    int32_t (*uninitialize)(void);
    int32_t (*power_control)(sunxi_hal_power_state_e state);
    int32_t (*read_data)(uint32_t addr, const void *data, uint32_t cnt);
    int32_t (*program_data)(uint32_t addr, const void *data, uint32_t cnt);
    int32_t (*erase_sector)(uint32_t addr, uint32_t len);
    int32_t (*erase_chip)(void);
    sunxi_hal_spinor_status_t (*get_status)(void);
    sunxi_hal_spinor_info *(*get_info)(void);
    void (* signal_event)(uint32_t event);
    int32_t (*control)(int32_t dev, uint32_t command, uint32_t arg);
} sunxi_hal_driver_spinor_t;

extern const sunxi_hal_driver_spinor_t sunxi_hal_spinor_driver;

#define MAX_ID_LEN 3

#define FACTORY_MXIC 0xC2
#define FACTORY_GD 0xC8
#define FACTORY_XMC 0x20
#define FACTORY_XTX 0x0B

#define BIT(x) (1 << x)
#define NOR_FLAG_PT_NOT_EXPAND BIT(0)

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
struct nor_protection {
    unsigned int boundary; /*  protected addr [0, boundary) */
    int bp:8;
    int flag:24;
#define SET_TB BIT(0)
#define SET_CMP BIT(1)
};
#endif

struct nor_info
{
    char *name;
    unsigned char id[MAX_ID_LEN];
    unsigned int blk_size;
    unsigned int blk_cnt;

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    struct nor_protection *pt;
    unsigned int pt_len;
    unsigned int pt_def;
#endif
    int flag;
#define EN_IO_PROG_X4 (1 << 1)
#define EN_IO_READ_X2 (1 << 2)
#define EN_IO_READ_X4 (1 << 3)
#define EN_INDIVIDUAL_PROTECT_MODE (1 << 4)
#define NO_ERASE_64K  (1 << 5)
};

struct spi_master
{
    hal_spi_master_port_t port;
    hal_spi_master_config_t cfg;
};

struct nor_flash
{
    unsigned char cmd_read;
    unsigned char cmd_write;
    unsigned char cmd_erase;

    unsigned int r_cmd_len: 3;
    unsigned int r_cmd_slen: 3;
    unsigned int w_cmd_len: 3;
    unsigned int w_cmd_slen: 3;
    unsigned int erase_wait_ms;
    unsigned int blk_size;
    unsigned int page_size;
    unsigned int total_size;
    unsigned int addr_width;
    struct spi_master spim;
    struct nor_info *info;
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    unsigned int pt_now;
#endif
    unsigned int flag;
    hal_sem_t hal_sem;
};

#ifdef __cplusplus
}
#endif

#endif  /*SUNXI_HAL_SPINOR_H*/
