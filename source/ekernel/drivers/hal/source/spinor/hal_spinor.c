#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include <log.h>
#include <sunxi_hal_spinor.h>

#include "spinor_config.h"

#define SUPPORT_4B_ADDR

#define BIT(x) (1 << x)
#ifdef SUPPORT_4B_ADDR
#define FIX_4B_ADDR(cmd) (nor->addr_width == 4 ? cmd ## 4B : cmd)
#else
#define FIX_4B_ADDR(cmd) (cmd)
#endif
#define NOR_CMD_READ 0x03
#define NOR_CMD_FAST_READ 0x0B
#define NOR_CMD_DUAL_READ 0x3B
#define NOR_CMD_QUAD_READ 0x6B
#define NOR_CMD_DUAL_IO_READ 0xBB
#define NOR_CMD_QUAD_IO_READ 0xEB
#define NOR_CMD_PROG 0x02
#define NOR_CMD_QUAD_PROG 0x32
#define NOR_CMD_QUAD_IO_PROG 0x38
#define NOR_CMD_ERASE_BLK4K 0x20
#define NOR_CMD_ERASE_BLK32K 0x52
#define NOR_CMD_ERASE_BLK64K 0xD8
#define NOR_CMD_ERASE_CHIP 0x60
#define NOR_CMD_WREN 0x06
#define NOR_CMD_WRDI 0x04
#define NOR_CMD_READ_SR 0x05
#define NOR_CMD_WRITE_SR 0x01
#define NOR_CMD_READ_CR 0x15
#define NOR_CMD_READ_SCUR 0x2B
#define NOR_CMD_RESET_EN 0x66
#define NOR_CMD_RESET 0x99
#define NOR_CMD_RDID 0x9F

#define NOR_CMD_BLK_LOCK_ALL (0x7E)
#define NOR_CMD_BLK_UNLOCK_ALL (0x98)
#ifdef SUPPORT_4B_ADDR
#define NOR_CMD_EN4B 0xB7
#define NOR_CMD_EX4B 0xE9
#define NOR_CMD_MXC_EX4B 0x29
#define NOR_CMD_READ4B 0x13
#define NOR_CMD_FAST_READ4B 0x0C
#define NOR_CMD_DUAL_READ4B 0x3C
#define NOR_CMD_QUAD_READ4B 0x6C
#define NOR_CMD_DUAL_IO_READ4B 0xBC
#define NOR_CMD_QUAD_IO_READ4B 0xEC
#define NOR_CMD_PROG4B 0x12
#define NOR_CMD_QUAD_PROG4B 0x34
#define NOR_CMD_QUAD_IO_PROG4B 0x3E
#define NOR_CMD_ERASE_BLK4K4B 0x21
#define NOR_CMD_ERASE_BLK32K4B 0x5C
#define NOR_CMD_ERASE_BLK64K4B 0xDC
#endif
#define NOR_BUSY_MASK BIT(0)
#define NOR_SR_BIT_SRP BIT(7)
#define NOR_SR_BIT_WEL BIT(1)
#define NOR_DEFAULT_FREQUENCY 50
#define NOR_PAGE_SIZE 256
#define NOR_PROTECT_BP_SHIFT 2

#define NOR_MXIC_CMD_READ_CR 0x15
#define NOR_MXIC_CMD_RDSCUR 0x2B
#define NOR_MXIC_CMD_WRSCUR 0x2F
#define NOR_MXIC_CMD_WPSEL 0x68
#define NOR_MXIC_CMD_ESSPB 0xE4
#define NOR_MXIC_CMD_RDSPB 0xE2
#define NOR_MXIC_CMD_WRSPB 0xE3
#define NOR_MXIC_CMD_RDDPB 0xE0
#define NOR_MXIC_CMD_WRDPB 0xE1
#define NOR_MXIC_WPSEL_BIT BIT(7)
#define NOR_MXIC_QE_BIT BIT(6)

#define NOR_XMC_QE_BIT BIT(6)
#define NOR_XMC_CMD_WRPLB 0xA6
#define NOR_XMC_CMD_RDASP 0x2B
#define NOR_XMC_CMD_PGASP 0x2F
#define NOR_XMC_CMD_RDDYB 0xFA
#define NOR_XMC_CMD_RDDYB4B 0xE0
#define NOR_XMC_CMD_WRDYB 0xFB
#define NOR_XMC_CMD_WRDYB4B 0xE1
#define NOR_XMC_TBPARM_BIT BIT(15)
#define NOR_XMC_PSTMLB_BIT BIT(1)
#define NOR_XMC_PWDMLB_BIT BIT(2)
#define NOR_XMC_DYB_LOCK (0x00)
#define NOR_XMC_DYB_UNLOCK (0xFF)

#define NOR_MXIC_PT_MASK (BIT(3) | BIT(2) | BIT(1) | BIT(0))
#define NOR_MXIC_PT_TB BIT(3)
#define NOR_MXIC_DPB_LOCK (0xFF)
#define NOR_MXIC_DPB_UNLOCK (0x00)
#define NOR_GD_QE_BIT BIT(1)
#define NOR_GD_CMP_BIT BIT(6)
#define NOR_GD_CMD_RDSR2 0x35
#define NOR_GD_CMD_WRSR2 0x31

#define NOR_GD_PT_MASK (BIT(3) | BIT(2) | BIT(1) | BIT(0))
#define NOR_ESMT_CMD_EN_OTP (0x3A)
#define NOR_ESMT_CMD_VOLATILE_WREN 0x50
#define NOR_ESMT_PT_MASK (BIT(3) | BIT(2) | BIT(1) | BIT(0))
#define NOR_ESMT_PT_TB BIT(3)
#define NOR_WINBOND_CMD_BLK_LOCK_STATUS (0x3D)
#define NOR_WINBOND_CMD_BLK_LOCK (0x36)
#define NOR_WINBOND_CMD_BLK_UNLOCK (0x39)
#define NOR_WINBOND_CMD_WRITE_SR3 0x11
#define NOR_WINBOND_CMD_READ_SR3 0x15
#define NOR_WINBOND_WPS_MASK BIT(2)
#define NOR_WINBOND_RCV_BIT BIT(3)
#define NOR_WINBOND_PT_MASK (BIT(2) | BIT(1) | BIT(0))
#define NOR_WEL_BIT (1 << 1)

/* XTX */
#define NOR_XTX_QE_BIT (0x01 << 1)
#define NOR_XTX_WPS_BIT BIT(4)
#define NOR_XTX_CMD_READ_SR1 (0x35)
#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

#define SZ_4K       (4 * 1024)
#define SZ_32K      (32 * 1024)
#define SZ_64K      (64 * 1024)
#define SZ_128K     (128 * 1024)
#define SZ_256K     (256 * 1024)
#define SZ_512K     (512 * 1024)
#define SZ_1M       (1 * 1024 * 1024)
#define SZ_2M       (2 * 1024 * 1024)
#define SZ_4M       (4 * 1024 * 1024)
#define SZ_8M       (8 * 1024 * 1024)
#define SZ_12M      (12 * 1024 * 1024)
#define SZ_14M      (14 * 1024 * 1024)
#define SZ_15M      (15 * 1024 * 1024)
#define SZ_15872K   (15872 * 1024)
#define SZ_16128K   (16128 * 1024)
#define SZ_16M      (16 * 1024 * 1024)
#define SZ_32M      (32 * 1024 * 1024)
#define SZ_64M      (64 * 1024 * 1024)

#define MAX_WAIT_LOOP ((unsigned int)(-1))
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

#define FACTORY_MXIC 0xC2
#define FACTORY_GD 0xC8
#define FACTORY_ESMT 0x1C
#define FACTORY_WINBOND 0xEF
#define NOR_PROTECT_ALL ((unsigned int)(-1))
#define NOR_PROTECT_HALF (NOR_PROTECT_ALL - 1)
#define NOR_PROTECT_DEFAULT (NOR_PROTECT_HALF - 1)
#define NOR_PROTECT_NONE (NOR_PROTECT_DEFAULT - 1)

#define ALIGN(x, a) __ALIGN_KERNEL((x), (a))
#define ALIGN_DOWN(x, a) __ALIGN_KERNEL((x) - ((a) - 1), (a))
#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))

#define CONFIG_DRIVERS_NOR_FLASH_READ_QUAD y
#define CONFIG_DRIVERS_NOR_FLASH_PROG_QUAD y

extern const sunxi_hal_driver_spi_t sunxi_hal_spi_driver;
const sunxi_hal_driver_spi_t *hal_spi_driver = &sunxi_hal_spi_driver;
static int nor_xtx_quad_mode(void);

#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
static struct nor_protection gd_protection[] =
{
    { .boundary = 1, .bp = 0, .flag = 0 },
    { .boundary = SZ_256K, .bp = BIT(3) | BIT(0), .flag = 0 },
    { .boundary = SZ_512K, .bp = BIT(3) | BIT(1), .flag = 0 },
    { .boundary = SZ_1M, .bp = BIT(3) | BIT(1) | BIT(0), .flag = 0 },
    { .boundary = SZ_2M, .bp = BIT(3) | BIT(2), .flag = 0 },
    { .boundary = SZ_4M, .bp = BIT(3) | BIT(2) | BIT(0), .flag = 0 },
    { .boundary = SZ_8M, .bp = BIT(3) | BIT(2) | BIT(1), .flag = 0 },
    { .boundary = SZ_12M, .bp = BIT(2) | BIT(0), .flag = SET_CMP },
    { .boundary = SZ_14M, .bp = BIT(2), .flag = SET_CMP },
    { .boundary = SZ_15M, .bp = BIT(1) | BIT(0), .flag = SET_CMP },
    { .boundary = SZ_15872K, .bp = BIT(1), .flag = SET_CMP },
    { .boundary = SZ_16128K, .bp = BIT(0), .flag = SET_CMP },
    { .boundary = SZ_16M, .bp = BIT(2) | BIT(1) | BIT(0), .flag = 0},
};

static struct nor_protection esmt_protection[] =
{
    { .boundary = 1, .bp = 0, .flag = 0 },
    { .boundary = SZ_256K, .bp = BIT(3) | BIT(0), .flag = 0 },
    { .boundary = SZ_512K, .bp = BIT(3) | BIT(1), .flag = 0 },
    { .boundary = SZ_1M, .bp = BIT(3) | BIT(1) | BIT(0), .flag = 0 },
    { .boundary = SZ_2M, .bp = BIT(3) | BIT(2), .flag = 0 },
    { .boundary = SZ_4M, .bp = BIT(3) | BIT(2) | BIT(0), .flag = 0 },
    { .boundary = SZ_8M, .bp = BIT(3) | BIT(2) | BIT(1), .flag = 0 },
    { .boundary = SZ_12M, .bp = BIT(2) | BIT(0), .flag = SET_TB },
    { .boundary = SZ_14M, .bp = BIT(2), .flag = SET_TB },
    { .boundary = SZ_15M, .bp = BIT(1) | BIT(0), .flag = SET_TB },
    { .boundary = SZ_15872K, .bp = BIT(1), .flag = SET_TB },
    { .boundary = SZ_16128K, .bp = BIT(0), .flag = SET_TB },
    { .boundary = SZ_16M, .bp = BIT(3) | BIT(2) | BIT(1) | BIT(0), .flag = 0 },
};
#endif

struct nor_info nor_ids[] =
{
    /* MXIC */
    {
        .name = "mx25l6433f",
        .id = {0xc2, 0x20, 0x17},
        .blk_size = 4 * 1024,
        .blk_cnt = 2048,
#ifdef CONFIG_SUPPORT_INDIVIDUAL_BLOCK_MEMORY_PROTECTION
        .flag = EN_IO_PROG_X4 | EN_INDIVIDUAL_PROTECT_MODE,
#else
        .flag = EN_IO_PROG_X4,
#endif
    },
    {
        .name = "mx25l12833f",
        .id = {0xc2, 0x20, 0x18},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
#ifdef CONFIG_SUPPORT_INDIVIDUAL_BLOCK_MEMORY_PROTECTION
        .flag = EN_IO_PROG_X4 | EN_INDIVIDUAL_PROTECT_MODE,
#else
        .flag = EN_IO_PROG_X4,
#endif
    },
#ifdef SUPPORT_4B_ADDR
    {
        .name = "mx25l51234g",
        .id = {0xc2, 0x20, 0x1a},
        .blk_size = 4 * 1024,
        .blk_cnt = 16384,
        .flag = EN_IO_PROG_X4,
    },
#endif
    /* Winbond */
    {
        .name = "w25q64jv",
        .id = {0xef, 0x40, 0x17},
        .blk_size = 4 * 1024,
        .blk_cnt = 2048,
        .flag = 0,
    },
    {
        .name = "w25q128jv",
        .id = {0xef, 0x40, 0x18},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
#ifdef CONFIG_SUPPORT_INDIVIDUAL_BLOCK_MEMORY_PROTECTION
        .flag = EN_INDIVIDUAL_PROTECT_MODE,
#else
        .flag = 0,
#endif
    },
    /* FM */
    {
        .name = "fm25w128",
        .id = {0xa1, 0x28, 0x18},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
        .flag = 0,
    },
    /* ESMT */
    {
        .name = "en25qh128",
        .id = {0x1c, 0x70, 0x18},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
        .pt = esmt_protection,
        .pt_len = ARRAY_SIZE(esmt_protection),
        /**
         * T/B bit is OTP. If we protect 12M, using T/B, we can not turn back
         * to 8M/4M/... And, ESMT saied we can use 50h, which will not really
         * write to OTP status register, however it do not work.
         * So, we drop these codes until T/B is tested set/unset ok.
         */
#if 0
        .pt_def = SZ_12M,
#else
        .pt_def = SZ_8M,
#endif
#endif
        .flag = 0,
    },
    /* GD */
    {
        .name = "GD25Q127C",
        .id = {0xc8, 0x40, 0x18},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
        .pt = gd_protection,
        .pt_len = ARRAY_SIZE(gd_protection),
        .pt_def = SZ_12M,
#endif
        .flag = 0,
    },
    {
        .name = "GD25Q256",
        .id = {0xc8, 0x40, 0x19},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
        .flag = 0,
    },
    /* XMC */
    {
        .name = "XM25QH256B",
        .id = {0x20, 0x60, 0x19},
        .blk_size = 4 * 1024,
        .blk_cnt = 8192,
#ifdef CONFIG_SUPPORT_INDIVIDUAL_BLOCK_MEMORY_PROTECTION
        .flag = EN_INDIVIDUAL_PROTECT_MODE,
#else
        .flag = 0,
#endif
    },
    /* XTX */
    {
        .name = "xt25f128",
        .id = {0x0b, 0x40, 0x18},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
#ifdef CONFIG_SUPPORT_INDIVIDUAL_BLOCK_MEMORY_PROTECTION
        .flag = EN_INDIVIDUAL_PROTECT_MODE,
#else
        .flag = 0,
#endif
    },
    /* Default */
    {
        .name = "Default",
        .id = {0, 0, 0},
        .blk_size = 4 * 1024,
        .blk_cnt = 4096,
        .flag = 0,
    }
};

static sunxi_hal_spinor_info hal_spinor_info = {0};

static sunxi_hal_version_t hal_spinor_driver =
{
    SUNXI_HAL_SPINOR_API_VERSION,
    SUNXI_HAL_SPINOR_DRV_VERSION
};

static const sunxi_hal_spinor_capabilities_t spinor_driver_capabilities =
{
    1, 0, 1, 0
};

static sunxi_hal_spinor_status_t spinor_status = {0, 0, 0};

static struct nor_flash g_nor = {0};
static struct nor_flash *nor = &g_nor;

static void nor_msleep(unsigned int msec)
{
    /* usleep(msec * 1000); */
    int msleep(unsigned int msecs);
    msleep(msec);
}

static int nor_lock(void)
{
    return hal_sem_wait(nor->hal_sem);
}

static int nor_unlock(void)
{
    return hal_sem_post(nor->hal_sem);
}

static int nor_lock_init(void)
{
    return hal_sem_create(&nor->hal_sem, 1);
}

static int nor_lock_deinit(void)
{
    return hal_sem_delete(nor->hal_sem);
}

static int cmd_bit(unsigned char cmd)
{
    switch (cmd)
    {
        case NOR_CMD_DUAL_READ:
        case NOR_CMD_DUAL_IO_READ:
#ifdef SUPPORT_4B_ADDR
        case NOR_CMD_DUAL_READ4B:
        case NOR_CMD_DUAL_IO_READ4B:
#endif
            return 2;
        case NOR_CMD_QUAD_READ:
        case NOR_CMD_QUAD_IO_READ:
#ifdef SUPPORT_4B_ADDR
        case NOR_CMD_QUAD_READ4B:
        case NOR_CMD_QUAD_IO_READ4B:
#endif
            return 4;
        case NOR_CMD_QUAD_PROG:
        case NOR_CMD_QUAD_IO_PROG:
#ifdef SUPPORT_4B_ADDR
        case NOR_CMD_QUAD_PROG4B:
        case NOR_CMD_QUAD_IO_PROG4B:
#endif
            return 4;
        default:
            return 1;
    }
}

static int nor_need_quad_mode(void)
{
    return (cmd_bit(nor->cmd_read) == 4 || cmd_bit(nor->cmd_write) == 4);
}
static int nor_read_write(int hlen, void *tbuf, int tlen, void *rbuf, int rlen)
{
    hal_spi_master_transfer_t tr;
    unsigned char cmd = *(unsigned char *)tbuf;
    int ret;

    memset(&tr, 0, sizeof(tr));
    tr.tx_buf = tbuf;
    tr.tx_len = tlen;
    tr.rx_buf = rbuf;
    tr.rx_len = rlen;
    tr.tx_single_len = hlen;
    tr.dummy_byte = 0;

    tr.rx_nbits = tr.tx_nbits = SPI_NBITS_SINGLE;
    switch (cmd)
    {
        case NOR_CMD_FAST_READ:
#ifdef SUPPORT_4B_ADDR
        case NOR_CMD_FAST_READ4B:
#endif
            tr.dummy_byte = 1;
            break;
        case NOR_CMD_DUAL_READ:
        case NOR_CMD_DUAL_IO_READ:
#ifdef SUPPORT_4B_ADDR
        case NOR_CMD_DUAL_READ4B:
        case NOR_CMD_DUAL_IO_READ4B:
#endif
            tr.rx_nbits = SPI_NBITS_DUAL;
            tr.dummy_byte = 1;
            break;
        case NOR_CMD_QUAD_READ:
        case NOR_CMD_QUAD_IO_READ:
#ifdef SUPPORT_4B_ADDR
        case NOR_CMD_QUAD_READ4B:
        case NOR_CMD_QUAD_IO_READ4B:
#endif
            tr.rx_nbits = SPI_NBITS_QUAD;
            tr.dummy_byte = 1;
            break;
        case NOR_CMD_QUAD_PROG:
        case NOR_CMD_QUAD_IO_PROG:
#ifdef SUPPORT_4B_ADDR
        case NOR_CMD_QUAD_PROG4B:
        case NOR_CMD_QUAD_IO_PROG4B:
#endif
            tr.tx_nbits = SPI_NBITS_QUAD;
            break;
    }

    /* ret = hal_spi_driver->transfer(nor->spim.port, &tr); */
    ret = hal_spi_driver->control(nor->spim.port, SPI_WRITE_READ, &tr);

    if (ret)
    {
        pr_err("spi transfer failed %d\n", ret);
    }
    return ret;
}

static int nor_read_status(unsigned char *sr)
{
    int ret;
    char cmd[1] = {NOR_CMD_READ_SR};
    char reg[2] = {0};

    ret = nor_read_write(1, cmd, 1, reg, 2);
    if (ret)
    {
        pr_err("read status register fail\n");
        return ret;
    }

    *sr = reg[1];
    return 0;
}

static int nor_mxic_read_conf_reg(unsigned char *cr)
{
    int ret;
    char cmd[1] = {NOR_CMD_READ_CR};
    char reg[2] = {0};

    ret = nor_read_write(1, cmd, 1, reg, 2);
    if (ret)
    {
        pr_err("read configure register fail");
        return ret;
    }

    *cr = reg[1];
    return 0;
}

static int nor_is_busy(void)
{
    int ret;
    unsigned char reg;

    ret = nor_read_status(&reg);
    if (ret)
    {
        return ret;
    }

    if (reg & NOR_BUSY_MASK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static int nor_wait_ready_ms(unsigned int ms, unsigned int times)
{
    unsigned int _ms = ms, _times = times;

    do
    {
        if (nor_is_busy() == false)
        {
            return 0;
        }
        if (_ms)
        {
            unsigned int per_ms = 1000 / CONFIG_HZ;

            nor_msleep(per_ms);
            _ms -= MIN(per_ms, _ms);
        }
    } while (_ms > 0);

    do
    {
        if (nor_is_busy() == false)
        {
            return 0;
        }
    } while (_times-- > 0);

    /* check the last time */
    if (nor_is_busy() == false)
    {
        return 0;
    }

    pr_err("wait nor flash for %d ms and %d loop timeout", ms, times);
    return -EBUSY;
}

static int nor_send_cmd(unsigned char cmd)
{
    int ret;
    ret = nor_read_write(1, &cmd, 1, NULL, 0);
    if (ret)
    {
        return ret;
    }
    return nor_wait_ready_ms(0, 500);
}

#ifdef SUPPORT_4B_ADDR
static int nor_set_4byte(int enable)
{
    int ret;
    char cmd[1];
    cmd[0] = enable ? NOR_CMD_EN4B : NOR_CMD_EX4B;

    switch (nor->info->id[0])
    {
        case FACTORY_XMC:
            cmd[0] = enable ? NOR_CMD_EN4B : NOR_CMD_MXC_EX4B;
            break;
        default:
            break;
    }

    ret = nor_read_write(1, cmd, 1, NULL, 0);
    if (ret)
    {
        pr_err("set 4byte %d fail\n", enable);
        return ret;
    }
    pr_debug("set 4byte %d success\n", enable);
    return 0;
}
#endif

static int nor_read_id(char *id, int len)
{
    int ret;
    char cmd[1] = {NOR_CMD_RDID};

    if (nor_wait_ready_ms(0, 500))
    {
        pr_err("nor is busy before read id");
        return -EBUSY;
    }

    ret = nor_read_write(1, cmd, 1, id, MIN(len, (int)MAX_ID_LEN));
    if (ret)
    {
        pr_err("read nor id failed - %d", ret);
    }
    return ret;
}

static int nor_reset(void)
{
    int ret;
    char cmd[2] = {NOR_CMD_RESET_EN, NOR_CMD_RESET};

    ret = nor_read_write(2, cmd, 2, NULL, 0);
    if (ret)
    {
        pr_err("reset nor failed - %d", ret);
    }
    return ret;
}

static int nor_write_enable(void)
{
    int ret;
    unsigned char sr;

    ret = nor_send_cmd(NOR_CMD_WREN);
    if (ret)
    {
        pr_err("send WREN failed - %d\n", ret);
        return ret;
    }

    ret = nor_read_status(&sr);
    if (ret)
    {
        return ret;
    }

    if (!(sr & NOR_SR_BIT_WEL))
    {
        pr_err("enable write failed, sr=%d\n", sr);
        return -EINVAL;
    }
    return 0;
}

static int nor_write_disable(void)
{
    int ret;
    ret = nor_send_cmd(NOR_CMD_WRDI);
    if (ret)
    {
        pr_err("send WRDI failed - %d\n", ret);
        return ret;
    }
    return 0;
}

static struct nor_info *match_nor(char *id, int id_len)
{
    int i;
    struct nor_info *info;

    for (i = 0; i < ARRAY_SIZE(nor_ids) - 1; i++)
    {
        info = &nor_ids[i];
        if (!memcmp(info->id, id, id_len))
        {
            pr_debug("match nor %s on table", info->name);
            return info;
        }
    }
    pr_warn("unrecognized id (hex): %02x %02x %02x", id[0], id[1], id[2]);
    pr_warn("set it to 8M(64K block) default");
    return &nor_ids[i];
}

static int nor_write_status(unsigned char *sr, unsigned int len)
{
    int ret, i;
    char tbuf[5] = {0};
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    if (len > 5)
    {
        return -EINVAL;
    }
    tbuf[0] = NOR_CMD_WRITE_SR;
    for (i = 0; i < len; i++)
    {
        tbuf[i + 1] = *(sr + i);
    }
    i++;
    ret = nor_read_write(i, tbuf, i, NULL, 0);
    if (ret)
    {
        pr_err("write status register fail\n");
        return ret;
    }
    return nor_wait_ready_ms(10, MAX_WAIT_LOOP);
}

#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
static struct nor_protection *nor_match_pt(unsigned int addr)
{
    struct nor_info *info = nor->info;
    struct nor_protection *pt = NULL;
    int i;
    /* in case over the max, we set the max default */
    pt = &info->pt[info->pt_len - 1];
    for (i = 1; i < info->pt_len; i++)
    {
        if (addr < info->pt[i].boundary)
        {
            pt = &info->pt[i - 1];
            break;
        }
    }
    return pt;
}
#endif

/* mxic private function */
#if 0
static int nor_mxic_read_conf_reg(unsigned char *cr)
{
    int ret;
    char cmd[1] = {NOR_MXIC_CMD_READ_CR};
    char reg[2] = {0};
    ret = nor_read_write(1, cmd, 1, reg, 2);
    if (ret)
    {
        pr_err("read configure register fail\n");
        return ret;
    }
    *cr = reg[1];
    return 0;
}
#endif

static int nor_mxic_quad_mode(void)
{
    int ret;
    unsigned char sr;

    ret = nor_read_status(&sr);
    if (ret)
    {
        return ret;
    }

    sr |= NOR_MXIC_QE_BIT;
    /*
     * If individual protect mode is enable, BP can't not write. If BP bits
     * are set, write status will go failed, which make QE bit set failed.
     */
    if (nor->info->flag & EN_INDIVIDUAL_PROTECT_MODE)
    {
        sr &= ~(NOR_MXIC_PT_MASK << NOR_PROTECT_BP_SHIFT);
    }
    ret = nor_write_status(&sr, 1);
    if (ret)
    {
        return ret;
    }

    ret = nor_read_status(&sr);
    if (ret)
    {
        return ret;
    }
    if (!(sr & NOR_MXIC_QE_BIT))
    {
        pr_err("set mxic QE failed (0x%x)\n", sr);
        return -EINVAL;
    }
    return 0;
}

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static int nor_mxic_set_wps(void)
{
    int ret;
    unsigned char cmd, sr;
    cmd = NOR_MXIC_CMD_RDSCUR;
    ret = nor_read_write(1, &cmd, 1, &sr, 1);
    if (ret)
    {
        return ret;
    }
    if (sr & NOR_MXIC_WPSEL_BIT)
    {
        return 0;
    }
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    ret = nor_send_cmd(NOR_MXIC_CMD_WPSEL);
    if (ret)
    {
        return ret;
    }
    ret = nor_wait_ready_ms(10, 100 * 1000);
    if (ret)
    {
        return ret;
    }
    cmd = NOR_MXIC_CMD_RDSCUR;
    ret = nor_read_write(1, &cmd, 1, &sr, 1);
    if (ret)
    {
        return ret;
    }
    return !(sr & NOR_MXIC_WPSEL_BIT);
}

static int nor_mxic_reset_spb(void)
{
    int ret;
    unsigned char cmd;
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    cmd = NOR_MXIC_CMD_ESSPB;
    ret = nor_read_write(1, &cmd, 1, NULL, 0);
    if (ret)
    {
        return ret;
    }
    ret = nor_wait_ready_ms(10, 500);
    if (ret)
    {
        return ret;
    }
    return 0;
}
#endif

static int nor_mxic_factory_init(void)
{
    int ret;
    if (nor_need_quad_mode())
    {
        ret = nor_mxic_quad_mode();
        if (ret)
        {
            return ret;
        }
    }
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    if (nor->info->flag & EN_INDIVIDUAL_PROTECT_MODE)
    {
        ret = nor_mxic_set_wps();
        if (ret)
        {
            return ret;
        }
        ret = nor_mxic_reset_spb();
        if (ret)
        {
            return ret;
        }
    }
#endif
    return 0;
}

#define NOR_GD_CMD_RDSR2 0x35

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static int nor_mxic_blk_islock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[5], st;

    tbuf[0] = NOR_MXIC_CMD_RDDPB;
    tbuf[1] = addr >> 24;
    tbuf[2] = addr >> 16;
    tbuf[3] = addr >> 8;
    tbuf[4] = addr & 0xFF;
    ret = nor_read_write(5, tbuf, 5, &st, 1);
    if (ret)
    {
        return ret;
    }

    return st == 0xFF ? 1 : 0;
}

static int nor_mxic_blk_unlock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[6];

    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = NOR_MXIC_CMD_WRDPB;
    tbuf[1] = addr >> 24;
    tbuf[2] = addr >> 16;
    tbuf[3] = addr >> 8;
    tbuf[4] = addr & 0xFF;
    tbuf[5] = NOR_MXIC_DPB_UNLOCK;
    ret = nor_read_write(6, tbuf, 6, NULL, 0);
    if (ret)
    {
        pr_err("mxic unlock 0x%x failed - %d\n", addr, ret);
        return ret;
    }
    if (nor_mxic_blk_islock(addr) == 1)
    {
        return -EBUSY;
    }
    return 0;
}
static int nor_mxic_blk_lock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[6];
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = NOR_MXIC_CMD_WRDPB;
    tbuf[1] = addr >> 24;
    tbuf[2] = addr >> 16;
    tbuf[3] = addr >> 8;
    tbuf[4] = addr & 0xFF;
    tbuf[5] = NOR_MXIC_DPB_LOCK;
    ret = nor_read_write(6, tbuf, 6, NULL, 0);
    if (ret)
    {
        pr_err("mxic lock 0x%x failed - %d\n", addr, ret);
        return ret;
    }
    if (nor_mxic_blk_islock(addr) == false)
    {
        return -EBUSY;
    }
    return 0;
}
#endif

#if 0
static int nor_esmt_enter_otp_mode(void)
{
    int ret;
    char cmd = NOR_ESMT_CMD_EN_OTP;
    ret = nor_read_write(1, &cmd, 1, NULL, 0);
    if (ret)
    {
        pr_err("enter OTP mode failed - %d\n", ret);
        return ret;
    }
    return nor_wait_ready_ms(0, 500);
}
static int nor_esmt_volatile_write_enable(void)
{
    int ret;
    char cmd = NOR_ESMT_CMD_VOLATILE_WREN;
    ret = nor_read_write(1, &cmd, 1, NULL, 0);
    if (ret)
    {
        pr_err("send volatile WREN failed - %d\n", ret);
        return ret;
    }
    return nor_wait_ready_ms(0, 500);
}
static int nor_esmt_set_tb(int tb)
{
    int ret;
    unsigned char sr;
    unsigned char buf[2];
    ret = nor_esmt_enter_otp_mode();
    if (ret)
    {
        return ret;
    }
    ret = nor_esmt_volatile_write_enable();
    if (ret)
    {
        goto out;
    }
    ret = nor_read_status(&sr);
    if (ret)
    {
        goto out;
    }
    ret = 0;
    if (tb)
    {
        if (sr & NOR_ESMT_PT_TB)
        {
            goto out;
        }
        sr |= NOR_ESMT_PT_TB;
    }
    else
    {
        if (!(sr & NOR_ESMT_PT_TB))
        {
            goto out;
        }
        sr &= ~NOR_ESMT_PT_TB;
    }
    buf[0] = NOR_CMD_WRITE_SR;
    buf[1] = sr;
    ret = nor_read_write(2, buf, 2, NULL, 0);
    if (ret)
    {
        pr_err("write status register fail\n");
        goto out;
    }
    ret = nor_wait_ready_ms(10, 100 * 1000);
    if (ret)
    {
        goto out;
    }
    ret = 0;
out:
    nor_write_disable();
    return ret;
}
#endif

#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
static int nor_esmt_bp_unprotect(struct nor_protection *pt)
{
    int ret = -EBUSY;
    unsigned char sr, bp = 0;
    if (!pt)
    {
        return -EINVAL;
    }
    if (nor_wait_ready_ms(0, 500))
    {
        return -EBUSY;
    }
    ret = nor_read_status(&sr);
    if (ret)
    {
        return ret;
    }
    bp = (sr >> NOR_PROTECT_BP_SHIFT) & NOR_ESMT_PT_MASK;
    if (bp == (unsigned char)pt->bp)
    {
        return 0;
    }
    sr &= ~NOR_SR_BIT_SRP;
    sr &= ~(NOR_ESMT_PT_MASK << NOR_PROTECT_BP_SHIFT);
    sr |= pt->bp << NOR_PROTECT_BP_SHIFT;
    ret = nor_write_status(&sr, 1);
    if (ret)
    {
        pr_err("esmt unprotect failed\n");
        return ret;
    }
#if 0
    if (pt->flag & SET_TB)
    {
        return nor_esmt_set_tb(true);
    }
    else
    {
        return nor_esmt_set_tb(false);
    }
#else
    return 0;
#endif
}
#endif

static int nor_gd_quad_mode(void)
{
    int ret;
    unsigned char cmd[3];
    char reg[2] = {0};

    cmd[0] = NOR_GD_CMD_RDSR2;
    ret = nor_read_write(1, cmd, 1, reg, 2);
    if (ret)
    {
        pr_err("read status register2 fail");
        return ret;
    }

    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }

    cmd[0] = NOR_GD_CMD_WRSR2;
    cmd[1] = reg[1] | NOR_GD_QE_BIT;
    ret = nor_read_write(2, cmd, 2, NULL, 0);
    if (ret)
    {
        pr_err("set status register fail");
        return ret;
    }

    if (nor_wait_ready_ms(0, 500))
    {
        pr_err("wait set qd mode failed");
        return -EBUSY;
    }

    cmd[0] = NOR_GD_CMD_RDSR2;
    ret = nor_read_write(1, cmd, 1, reg, 2);
    if (ret)
    {
        pr_err("read status register2 fail");
        return ret;
    }
    if (!(reg[1] & NOR_GD_QE_BIT))
    {
        pr_err("set gd QE failed");
        return -EINVAL;
    }
    return 0;
}

static int nor_gd_factory_init(void)
{
    if (nor_need_quad_mode())
    {
        return nor_gd_quad_mode();
    }
    return 0;
}

static int nor_gd_set_cmp(int enable)
{
    int ret;
    unsigned char tbuf[2], reg[2];
    tbuf[0] = NOR_GD_CMD_RDSR2;
    ret = nor_read_write(1, tbuf, 1, reg, 2);
    if (ret)
    {
        pr_err("GD read status register-2 fail\n");
        return ret;
    }
    if (enable)
    {
        if (reg[0] & NOR_GD_CMP_BIT)
        {
            return 0;
        }
        reg[0] |= NOR_GD_CMP_BIT;
    }
    else
    {
        if (!(reg[0] & NOR_GD_CMP_BIT))
        {
            return 0;
        }
        reg[0] &= ~NOR_GD_CMP_BIT;
    }
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = NOR_GD_CMD_WRSR2;
    tbuf[1] = reg[0];
    ret = nor_read_write(2, tbuf, 2, NULL, 0);
    if (ret)
    {
        pr_err("set status register-2 fail\n");
        return ret;
    }
    return nor_wait_ready_ms(5, 1000 * 1000);
}

#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
static int nor_gd_bp_unprotect(struct nor_protection *pt)
{
    int ret = -EBUSY;
    unsigned char sr, bp = 0;
    if (!pt)
    {
        return -EINVAL;
    }
    if (nor_wait_ready_ms(0, 500))
    {
        return -EBUSY;
    }
    ret = nor_read_status(&sr);
    if (ret)
    {
        return ret;
    }
    bp = (sr >> NOR_PROTECT_BP_SHIFT) & NOR_GD_PT_MASK;
    if (bp != (unsigned char)pt->bp)
    {
        sr &= ~(NOR_GD_PT_MASK << NOR_PROTECT_BP_SHIFT);
        sr |= pt->bp << NOR_PROTECT_BP_SHIFT;
        ret = nor_write_status(&sr, 1);
        if (ret)
        {
            pr_err("gd unprotect failed\n");
            return ret;
        }
    }
    return nor_gd_set_cmp(pt->flag & SET_CMP);
}
#endif

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static int nor_winbond_set_wps_rcv(int wps, int rcv)
{
    int ret;
    unsigned char tbuf[2], reg[2], sr3;
    tbuf[0] = NOR_WINBOND_CMD_READ_SR3;
    ret = nor_read_write(1, tbuf, 1, reg, 2);
    if (ret)
    {
        pr_err("winbond read status register-3 fail\n");
        return ret;
    }
    sr3 = reg[0];
    if (wps)
    {
        sr3 |= NOR_WINBOND_WPS_MASK;
    }
    else
    {
        sr3 &= ~NOR_WINBOND_WPS_MASK;
    }
    if (rcv)
    {
        sr3 |= NOR_WINBOND_RCV_BIT;
    }
    else
    {
        sr3 &= ~NOR_WINBOND_RCV_BIT;
    }
    if (sr3 == reg[0])
    {
        return 0;
    }
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = NOR_WINBOND_CMD_WRITE_SR3;
    tbuf[1] = sr3;
    ret = nor_read_write(2, tbuf, 2, NULL, 0);
    if (ret)
    {
        return ret;
    }
    return nor_wait_ready_ms(10, 100 * 1000);
}
#endif

static int nor_winbond_factory_init(void)
{
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    if (nor->info->flag & EN_INDIVIDUAL_PROTECT_MODE)
    {
        return nor_winbond_set_wps_rcv(true, true);
    }
    else
    {
        return nor_winbond_set_wps_rcv(false, true);
    }
#else
    return 0;
#endif
}

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static bool nor_winbond_blk_islock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[4], st;
    tbuf[0] = NOR_WINBOND_CMD_BLK_LOCK_STATUS;
    tbuf[1] = addr >> 16;
    tbuf[2] = addr >> 8;
    tbuf[3] = addr & 0xFF;
    ret = nor_read_write(4, tbuf, 4, &st, 1);
    if (ret)
    {
        return ret;
    }
    return st & 0x1 ? true : false;
}
static int nor_winbond_blk_lock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[4];
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = NOR_WINBOND_CMD_BLK_LOCK;
    tbuf[1] = addr >> 16;
    tbuf[2] = addr >> 8;
    tbuf[3] = addr & 0xFF;
    ret = nor_read_write(4, tbuf, 4, NULL, 0);
    if (ret)
    {
        return ret;
    }
    if (nor_winbond_blk_islock(addr) == true)
    {
        return 0;
    }
    return -EBUSY;
}
static int nor_winbond_blk_unlock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[4];
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = NOR_WINBOND_CMD_BLK_UNLOCK;
    tbuf[1] = addr >> 16;
    tbuf[2] = addr >> 8;
    tbuf[3] = addr & 0xFF;
    ret = nor_read_write(4, tbuf, 4, NULL, 0);
    if (ret)
    {
        return ret;
    }
    if (nor_winbond_blk_islock(addr) == true)
    {
        return -EBUSY;
    }
    return 0;
}
#endif

static int nor_xtx_read_status1(unsigned char *sr1)
{
    int ret;
    char cmd[1] = {NOR_XTX_CMD_READ_SR1};
    char reg[2] = {0};
    ret = nor_read_write(1, cmd, 1, reg, 2);
    if (ret)
    {
        pr_err("read xtx status1 register fail\n");
        return ret;
    }
    *sr1 = reg[1];
    return 0;
}

static int nor_xtx_quad_mode(void)
{
    int ret;
    unsigned char sr[2];

    ret = nor_xtx_read_status1(&sr[1]);
    if (ret)
    {
        return ret;
    }

    if (sr[1] & NOR_XTX_QE_BIT)
    {
        return 0;
    }

    sr[1] |= NOR_XTX_QE_BIT;

    ret = nor_read_status(&sr[0]);
    if (ret)
    {
        return ret;
    }

    ret = nor_write_status(sr, 2);
    if (ret)
    {
        return ret;
    }

    ret = nor_xtx_read_status1(&sr[1]);
    if (ret)
    {
        return ret;
    }
    if (!(sr[1] & NOR_XTX_QE_BIT))
    {
        pr_err("set xtx QE failed (0x%x)\n", sr[1]);
        return -EINVAL;
    }
    return 0;
}

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static int nor_xtx_set_wps(int wps)
{
    int ret;
    unsigned char sr[2];
    unsigned char sr1;
    ret = nor_xtx_read_status1(&sr1);
    if (ret)
    {
        return ret;
    }
    if (wps)
    {
        sr[1] = sr1 | NOR_XTX_WPS_BIT;
    }
    else
    {
        sr[1] = sr1 & ~NOR_XTX_WPS_BIT;
    }
    if (sr1 == sr[1])
    {
        return 0;
    }
    ret = nor_read_status(&sr[0]);
    if (ret)
    {
        return ret;
    }
    ret = nor_write_status(sr, 2);
    if (ret)
    {
        return ret;
    }
    ret = nor_xtx_read_status1(&sr1);
    if (ret)
    {
        return ret;
    }
    if ((sr1 & NOR_XTX_WPS_BIT) != (sr[1] & NOR_XTX_WPS_BIT))
    {
        pr_err("set xtx wps %d failed (0x%x)\n", wps, sr1);
        return -EINVAL;
    }
    return 0;
}
#endif

static int nor_xtx_factory_init(void)
{
    int ret;
    if (nor_need_quad_mode())
    {
        ret = nor_xtx_quad_mode();
        if (ret)
        {
            return ret;
        }
    }
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    if (nor->info->flag & EN_INDIVIDUAL_PROTECT_MODE)
    {
        return nor_xtx_set_wps(true);
    }
#endif
    return 0;
}

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static int nor_xmc_blk_islock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[5], st = 0;

    tbuf[0] = FIX_4B_ADDR(NOR_XMC_CMD_RDDYB);
    tbuf[1] = addr >> 24;
    tbuf[2] = addr >> 16;
    tbuf[3] = addr >> 8;
    tbuf[4] = addr & 0xFF;
    ret = nor_read_write(5, tbuf, 5, &st, 1);
    if (ret)
    {
        return ret;
    }

    return st == NOR_XMC_DYB_LOCK ? 1 : 0;
}

static int nor_xmc_blk_unlock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[6];

    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = FIX_4B_ADDR(NOR_XMC_CMD_WRDYB);
    tbuf[1] = addr >> 24;
    tbuf[2] = addr >> 16;
    tbuf[3] = addr >> 8;
    tbuf[4] = addr & 0xFF;
    tbuf[5] = NOR_XMC_DYB_UNLOCK;
    ret = nor_read_write(6, tbuf, 6, NULL, 0);
    if (ret)
    {
        pr_err("xmc unlock 0x%x failed - %d\n", addr, ret);
        return ret;
    }
    if (nor_xmc_blk_islock(addr) == 1)
    {
        pr_err("xmc unlock 0x%x failed - still is lock!\n", addr);
        return -EBUSY;
    }
    return 0;
}
static int nor_xmc_blk_lock(unsigned int addr)
{
    int ret;
    unsigned char tbuf[6];
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    tbuf[0] = FIX_4B_ADDR(NOR_XMC_CMD_WRDYB);
    tbuf[1] = addr >> 24;
    tbuf[2] = addr >> 16;
    tbuf[3] = addr >> 8;
    tbuf[4] = addr & 0xFF;
    tbuf[5] = NOR_XMC_DYB_LOCK;
    ret = nor_read_write(6, tbuf, 6, NULL, 0);
    if (ret)
    {
        pr_err("mxic lock 0x%x failed - %d\n", addr, ret);
        return ret;
    }
    if (nor_xmc_blk_islock(addr) == false)
    {
        return -EBUSY;
    }
    return 0;
}

/*
 * The function not need
 */
static int nor_xmc_set_wps(void)
{
#if 0
    int ret;
    unsigned char cmd;
    unsigned short sr = 0;

    cmd = NOR_XMC_CMD_RDASP;
    ret = nor_read_write(1, &cmd, 1, &sr, 2);
    if (ret)
    {
        return ret;
    }
    return ret;
#else
    return 0;
#endif
}
#endif

static int nor_xmc_quad_mode(void)
{
    int ret;
    unsigned char sr;

    ret = nor_read_status(&sr);
    if (ret)
    {
        return ret;
    }

    sr |= NOR_XMC_QE_BIT;
    ret = nor_write_status(&sr, 1);
    if (ret)
    {
        return ret;
    }

    ret = nor_read_status(&sr);
    if (ret)
    {
        return ret;
    }
    if (!(sr & NOR_XMC_QE_BIT))
    {
        pr_err("set xmc QE failed (0x%x)\n", sr);
        return -EINVAL;
    }
    return 0;
}

static int nor_xmc_factory_init(void)
{
    int ret = -1;
    if (nor_need_quad_mode())
    {
        ret = nor_xmc_quad_mode();
        if (ret)
        {
            return ret;
        }
    }
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    if (nor->info->flag & EN_INDIVIDUAL_PROTECT_MODE)
    {
        ret = nor_xmc_set_wps();
        if (ret)
        {
            return ret;
        }
    }
#endif
    return ret;
}

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static int nor_generic_blk_lock_all(void)
{
    int ret;
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    ret = nor_send_cmd(NOR_CMD_BLK_LOCK_ALL);
    if (ret)
    {
        pr_err("lock all block failed - %d\n", ret);
        return ret;
    }
    return 0;
}
static int nor_generic_blk_unlock_all(void)
{
    int ret;
    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
    ret = nor_send_cmd(NOR_CMD_BLK_UNLOCK_ALL);
    if (ret)
    {
        pr_err("unlock all block failed - %d\n", ret);
        return ret;
    }
    return 0;
}
static int nor_blk_lock_all(void)
{
    pr_debug("try to lock all individual blocks\n");
    switch (nor->info->id[0])
    {
        case FACTORY_MXIC:
        case FACTORY_WINBOND:
        case FACTORY_XTX:
        case FACTORY_XMC:
            return nor_generic_blk_lock_all();
        default:
            return 0;
    }
}
static int nor_blk_unlock_all(void)
{
    pr_debug("try to unlock all individual blocks\n");
    switch (nor->info->id[0])
    {
        case FACTORY_MXIC:
        case FACTORY_WINBOND:
        case FACTORY_XTX:
        case FACTORY_XMC:
            return nor_generic_blk_unlock_all();
        default:
            return 0;
    }
}
static int nor_blk_lock(unsigned int addr)
{
    pr_debug("try to lock 0x%x\n", addr);
    switch (nor->info->id[0])
    {
        case FACTORY_MXIC:
            return nor_mxic_blk_lock(addr);
        case FACTORY_XTX:
        case FACTORY_WINBOND:
            return nor_winbond_blk_lock(addr);
        case FACTORY_XMC:
            return nor_xmc_blk_lock(addr);
        default:
            return 0;
    }
}
static int nor_blk_unlock(unsigned int addr)
{
    pr_debug("try to unlock 0x%x\n", addr);
    switch (nor->info->id[0])
    {
        case FACTORY_MXIC:
            return nor_mxic_blk_unlock(addr);
        case FACTORY_XTX:
        case FACTORY_WINBOND:
            return nor_winbond_blk_unlock(addr);
        case FACTORY_XMC:
            return nor_xmc_blk_unlock(addr);
        default:
            return 0;
    }
}
#endif

static int nor_factory_init(void)
{
    int ret = 0;
    switch (nor->info->id[0])
    {
        case FACTORY_GD:
            ret = nor_gd_factory_init();
            break;
        case FACTORY_MXIC:
            ret = nor_mxic_factory_init();
            break;
        case FACTORY_WINBOND:
            ret = nor_winbond_factory_init();
            break;
        case FACTORY_XTX:
            ret = nor_xtx_factory_init();
            break;
        case FACTORY_XMC:
            ret = nor_xmc_factory_init();
            break;
        default:
            ret = 0;
    }
    if (ret)
    {
        pr_err("factory init failed\n");
    }
    return ret;
}

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
static unsigned int nor_blk_fix_protect_addr(unsigned int addr)
{
    switch (nor->info->id[0])
    {
        case FACTORY_XMC:
            if (addr < (32 * SZ_4K))
            {
                addr = ALIGN_DOWN(addr, SZ_4K);
            }
            else
            {
                addr = ALIGN_DOWN(addr, SZ_64K);
            }
            break;
        case FACTORY_WINBOND:
        case FACTORY_MXIC:
        case FACTORY_XTX:
        default:
            if (addr > nor->total_size - SZ_64K || addr < SZ_64K)
            {
                addr = ALIGN_DOWN(addr, SZ_4K);
            }
            else
            {
                addr = ALIGN_DOWN(addr, SZ_64K);
            }
            break;
    }
    return addr;
}

#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
static int nor_bp_unprotect(unsigned int addr)
{
    struct nor_info *info = nor->info;
    struct nor_protection *pt;
    int ret;
    if (addr < nor->total_size)
    {
        if (addr >= nor->pt_now)
        {
            return 0;
        }
    }
    else
    {
        /*
         * if NOR_FLAG_PT_NOT_EXPAND is set, we should not expand protected
         * area. By this way, OTA worker can unlock a large area and
         * nor_write()/nor_erase() can not turn back to default.
         */
        if (nor->flag & NOR_FLAG_PT_NOT_EXPAND)
        {
            return 0;
        }
        if (addr == NOR_PROTECT_ALL)
        {
            addr = (nor->total_size);
        }
        else if (addr == NOR_PROTECT_HALF)
        {
            addr = (nor->total_size) / 2;
        }
        else if (addr == NOR_PROTECT_DEFAULT)
        {
            addr = info->pt_def;
        }
        else if (addr == NOR_PROTECT_NONE)
        {
            addr = 0;
        }
        if (addr == nor->pt_now)
        {
            return 0;
        }
    }
    pt = nor_match_pt(addr);
    if (!pt)
    {
        pr_err("no match protection mode for addr 0x%x\n", addr);
        return -EINVAL;
    }
    pr_debug("try to write/erase 0x%x, unprotect [0x%x-Max]\n", addr, pt->boundary);
    switch (nor->info->id[0])
    {
#if 0 /* mxic can protect in sector/block unit, so, drop pt_mode */
        case FACTORY_MXIC:
            ret = nor_mxic_bp_unprotect(pt);
            break;
#endif
        case FACTORY_ESMT:
            ret = nor_esmt_bp_unprotect(pt);
            break;
        case FACTORY_GD:
            ret = nor_gd_bp_unprotect(pt);
            break;
        default:
            return 0;
    }
    if (ret)
    {
        pr_err("addr 0x%x bp unportect failed\n", addr);
    }
    else
    {
        nor->pt_now = pt->boundary;
    }
    return ret;
}
#endif

static int nor_blk_unprotect(unsigned int addr)
{
    int ret = 0;
    switch (addr)
    {
        case NOR_PROTECT_HALF:
        case NOR_PROTECT_ALL:
            ret = nor_blk_lock_all();
            break;
        case NOR_PROTECT_NONE:
            ret = nor_blk_unlock_all();
            break;
        case NOR_PROTECT_DEFAULT:
            if (nor->pt_now < nor->total_size)
            {
                ret = nor_blk_lock(nor->pt_now);
            }
            else
            {
                ret = nor_blk_lock_all();
            }
            break;
        default:
            addr = nor_blk_fix_protect_addr(addr);
            ret = nor_blk_unlock(addr);
            break;
    }
    if (ret)
    {
        pr_err("addr 0x%x blk unprotect failed\n", addr);
    }
    else
    {
        if (addr < nor->total_size && nor->pt_now < nor->total_size)
        {
            pr_warn("last lock addr 0x%x may has not unlocked\n", nor->pt_now);
        }
        nor->pt_now = addr;
    }
    return ret;
}
static int nor_unprotect_do(unsigned int addr)
{
    struct nor_info *info = nor->info;
#if defined(CONFIG_DRIVERS_NOR_FLASH_TEST)
    extern int test_protect;
    if (!test_protect)
    {
        return 0;
    }
#endif
    if (info->flag & EN_INDIVIDUAL_PROTECT_MODE)
    {
        return nor_blk_unprotect(addr);
    }
#ifdef CONFIG_SUPPORT_STATUS_REGISTER_MEMORY_PROTECTION
    else if (info->pt)
    {
        return nor_bp_unprotect(addr);
    }
#endif
    return 0;
}
#endif

static int nor_set_quad_mode(void)
{
    switch (nor->info->id[0])
    {
        case FACTORY_MXIC:
            return nor_mxic_quad_mode();
        case FACTORY_GD:
            return nor_gd_quad_mode();
        case FACTORY_XMC:
            return nor_xmc_quad_mode();
        case FACTORY_XTX:
            return nor_xtx_quad_mode();
        default:
            return 0;
    }
}

static int nor_scan(void)
{
    int ret;
    char id[MAX_ID_LEN];
    int read_mode;
    int write_mode;

    ret = nor_read_id(id, MAX_ID_LEN);
    if (ret)
    {
        goto err;
    }

    if (get_read_mode(&read_mode))
    {
        read_mode = DEFAULT_READ_MODE;
    }
    if (get_write_mode(&write_mode))
    {
        write_mode = DEFAULT_WRITE_MODE;
    }

    read_mode = (read_mode > write_mode) ? write_mode : read_mode;
    write_mode = read_mode;

    nor->info = match_nor(id, MAX_ID_LEN);

    nor->total_size = nor->info->blk_cnt * nor->info->blk_size;
    nor->addr_width = 3;
#ifdef SUPPORT_4B_ADDR
    if (nor->total_size > (16 * 1024 * 1024))
    {
        nor->addr_width = 4;
        nor_set_4byte(1);
    }
#endif

    nor->page_size = NOR_PAGE_SIZE;
#if defined(CONFIG_DRIVERS_NOR_FLASH_4K_SECTORS)
    nor->blk_size = 4 * 1024;
    nor->cmd_erase = NOR_CMD_ERASE_BLK4K;
#else
    nor->blk_size = nor->info->blk_size;
    switch (nor->blk_size)
    {
        case 64 * 1024:
            nor->cmd_erase = NOR_CMD_ERASE_BLK64K;
            break;
        case 32 * 1024:
            nor->cmd_erase = NOR_CMD_ERASE_BLK32K;
            break;
        case 4 * 1024:
            nor->cmd_erase = NOR_CMD_ERASE_BLK4K;
            break;
        default:
            pr_err("invalid blk size %u", nor->info->blk_size);
            ret = -EINVAL;
            goto err;
    }
#endif

    /* program property */
    nor->w_cmd_slen = nor->addr_width == 4 ? 5 : 4;
    if (write_mode == SPINOR_QUAD_MODE)
    {
#ifdef CONFIG_DRIVERS_NOR_FLASH_PROG_QUAD
        nor->cmd_write = FIX_4B_ADDR(NOR_CMD_QUAD_PROG);
        if (nor->info->flag & EN_IO_PROG_X4)
        {
            nor->cmd_write = FIX_4B_ADDR(NOR_CMD_QUAD_IO_PROG);
            nor->w_cmd_slen = 1;
        }
#else
        nor->cmd_write = FIX_4B_ADDR(NOR_CMD_PROG);
#endif
    }
    else
    {
        nor->cmd_write = FIX_4B_ADDR(NOR_CMD_PROG);
    }

    /* read property */
    nor->r_cmd_slen = nor->addr_width == 4 ? 6 : 5;
    if (read_mode == SPINOR_SINGLE_MODE)
    {
        nor->cmd_read = FIX_4B_ADDR(NOR_CMD_READ);
    }
    else if (read_mode == SPINOR_DUAL_MODE)
    {
#if defined(CONFIG_DRIVERS_NOR_FLASH_READ_DUAL)
        nor->cmd_read = FIX_4B_ADDR(NOR_CMD_DUAL_READ);
        if (nor->info->flag & EN_IO_READ_X2)
        {
            nor->cmd_read = FIX_4B_ADDR(NOR_CMD_DUAL_IO_READ);
            nor->r_cmd_slen = 1;
        }
#else
        nor->cmd_read = FIX_4B_ADDR(NOR_CMD_READ);
#endif
    }
    else if (read_mode == SPINOR_QUAD_MODE)
    {
#if defined(CONFIG_DRIVERS_NOR_FLASH_READ_QUAD)
        nor->cmd_read = FIX_4B_ADDR(NOR_CMD_QUAD_READ);
        if (nor->info->flag & EN_IO_READ_X4)
        {
            nor->cmd_read = FIX_4B_ADDR(NOR_CMD_QUAD_IO_READ);
            nor->r_cmd_slen = 1;
        }
#else
        nor->cmd_read = FIX_4B_ADDR(NOR_CMD_READ);
#endif
    }
    else
    {
        nor->cmd_read = FIX_4B_ADDR(NOR_CMD_READ);
    }


#if defined(CONFIG_DRIVERS_NOR_FLASH_READ_FAST)
    nor->cmd_read = FIX_4B_ADDR(NOR_CMD_FAST_READ);
#endif

    if (cmd_bit(nor->cmd_read) == 4 || cmd_bit(nor->cmd_write) == 4)
    {
        ret = nor_set_quad_mode();
        if (ret)
        {
            goto err;
        }
    }

    return 0;
err:
    pr_err("scan nor flash failed");
    nor->info = NULL;
    return ret;
}

static int nor_spi_master_init(struct spi_master *spim)
{
    int ret;
    void *script_handler = NULL;
    int frequency = 0;
    int port = HAL_SPI_MASTER_0;

    if (get_clock_frequency(&frequency))
    {
        frequency = NOR_DEFAULT_FREQUENCY * 1000 * 1000;
    }

    if (get_spi_port(&port))
    {
        port = HAL_SPI_MASTER_0;
    }

    spim->port = port;
    spim->cfg.clock_frequency = frequency;
    spim->cfg.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spim->cfg.cpha = HAL_SPI_MASTER_CLOCK_PHASE0;
    spim->cfg.cpol = HAL_SPI_MASTER_CLOCK_POLARITY0;
    spim->cfg.bit_order = HAL_SPI_MASTER_LSB_FIRST;

    ret = hal_spi_driver->initialize(spim->port, &spim->cfg);
    if (ret != HAL_SPI_MASTER_STATUS_OK)
    {
        pr_err("init spi master failed - %d", ret);
    }
    return ret;
}

static int nor_erase_do(char cmd, unsigned int addr)
{
    int ret = -EBUSY;
    char tbuf[5] = {0};
    int cmdlen;

    if (!nor->info)
    {
        goto out;
    }

    ret = nor_write_enable();
    if (ret)
    {
        goto out;
    }

    tbuf[0] = cmd;
#ifdef SUPPORT_4B_ADDR
    if (nor->addr_width == 4)
    {
        tbuf[1] = addr >> 24;
        tbuf[2] = addr >> 16;
        tbuf[3] = addr >> 8;
        tbuf[4] = addr & 0xFF;
        cmdlen = 5;
    }
    else
    {
#endif
        tbuf[1] = addr >> 16;
        tbuf[2] = addr >> 8;
        tbuf[3] = addr & 0xFF;
        cmdlen = 4;
#ifdef SUPPORT_4B_ADDR
    }
#endif

    ret = nor_read_write(4, tbuf, cmdlen, NULL, 0);
    if (ret)
    {
        goto out;
    }

out:
    if (ret)
    {
        pr_err("erase address 0x%x with cmd 0x%x failed", addr, cmd);
    }
    return ret;
}

static int nor_read_do(unsigned int addr, const char *buf, unsigned int len)
{
    char cmd[5] = {0};
    int cmdlen;

    if (len > NOR_PAGE_SIZE)
    {
        return -EINVAL;
    }

    cmd[0] = nor->cmd_read;
#ifdef SUPPORT_4B_ADDR
    if (nor->addr_width == 4)
    {
        cmd[1] = addr >> 24;
        cmd[2] = addr >> 16;
        cmd[3] = addr >> 8;
        cmd[4] = addr & 0xFF;
        cmdlen = 5;
    }
    else
    {
#endif
        cmd[1] = addr >> 16;
        cmd[2] = addr >> 8;
        cmd[3] = addr & 0xFF;
        cmdlen = 4;
#ifdef SUPPORT_4B_ADDR
    }
#endif
    return nor_read_write(nor->r_cmd_slen, cmd, cmdlen, (void *)buf, len);
}

static inline int nor_erase_4k(unsigned int addr)
{
    int ret;
    char cmd;

#ifdef SUPPORT_4B_ADDR
    if (nor->addr_width == 4)
    {
        cmd = (char)NOR_CMD_ERASE_BLK4K4B;
    }
    else
#endif
        cmd = (char)NOR_CMD_ERASE_BLK4K;

    ret = nor_erase_do(cmd, addr);
    if (ret)
    {
        return ret;
    }
    return nor_wait_ready_ms(30, MAX_WAIT_LOOP);
}

static inline int nor_erase_32k(unsigned int addr)
{
    int ret;
    char cmd;

#ifdef SUPPORT_4B_ADDR
    if (nor->addr_width == 4)
    {
        cmd = (char)NOR_CMD_ERASE_BLK32K4B;
    }
    else
#endif
        cmd = (char)NOR_CMD_ERASE_BLK32K;

    ret = nor_erase_do(cmd, addr);
    if (ret)
    {
        return ret;
    }
    return nor_wait_ready_ms(120, MAX_WAIT_LOOP);
}

static inline int nor_erase_64k(unsigned int addr)
{
    int ret;
    char cmd;

#ifdef SUPPORT_4B_ADDR
    if (nor->addr_width == 4)
    {
        cmd = (char)NOR_CMD_ERASE_BLK64K4B;
    }
    else
#endif
        cmd = (char)NOR_CMD_ERASE_BLK64K;

    ret = nor_erase_do(cmd, addr);
    if (ret)
    {
        return ret;
    }
    return nor_wait_ready_ms(150, MAX_WAIT_LOOP);
}

static inline int nor_erase_all(void)
{
    int ret = -1;
    char cmd = NOR_CMD_ERASE_CHIP;

    if (!nor->info)
    {
        goto out;
    }

    pr_debug("try to erase all chip");

    ret = nor_write_enable();
    if (ret)
    {
        goto out;
    }

    ret = nor_read_write(1, &cmd, 1, NULL, 0);
    if (ret)
    {
        goto out;
    }
    ret = nor_wait_ready_ms(26 * 1000, MAX_WAIT_LOOP);

out:
    if (ret)
    {
        pr_err("erase all chip failed");
    }
    return ret;
}


static int nor_write_do(unsigned int addr, const char *buf, unsigned int len)
{
    int ret = -EINVAL;
    char tbuf[NOR_PAGE_SIZE + 5] = {0};
    int cmdlen;

    if (len > NOR_PAGE_SIZE)
    {
        return -EINVAL;
    }

    ret = nor_write_enable();
    if (ret)
    {
        return ret;
    }
#ifdef SUPPORT_4B_ADDR
    if (nor->addr_width == 4)
    {
        tbuf[0] = nor->cmd_write;
        tbuf[1] = addr >> 24;
        tbuf[2] = addr >> 16;
        tbuf[3] = addr >> 8;
        tbuf[4] = addr & 0xFF;
        cmdlen = 5;
    }
    else
    {
#endif
        tbuf[0] = nor->cmd_write;
        tbuf[1] = addr >> 16;
        tbuf[2] = addr >> 8;
        tbuf[3] = addr & 0xFF;
        cmdlen = 4;
#ifdef SUPPORT_4B_ADDR
    }
#endif
    memcpy(tbuf + cmdlen, buf, MIN(len, (unsigned int)NOR_PAGE_SIZE));
    ret = nor_read_write(nor->w_cmd_slen, tbuf, len + cmdlen, NULL, 0);
    if (ret)
    {
        return ret;
    }

    return nor_wait_ready_ms(0, 8 * 1000);
}

static unsigned int nor_total_size(void)
{
    if (!nor->info)
    {
        return 0;
    }
    return nor->total_size;
}

static sunxi_hal_version_t spinor_get_version(int32_t dev)
{
    HAL_ARG_UNUSED(dev);
    return hal_spinor_driver;
}

static sunxi_hal_spinor_capabilities_t spinor_get_capabilities(void)
{
    return spinor_driver_capabilities;
}

static sunxi_hal_spinor_status_t spinor_get_status(void)
{
    return spinor_status;
}

int32_t spinor_initialize(sunxi_hal_spinor_signal_event_t cb_event)
{
    int ret = 0;

    if (nor->info)
    {
        return 0;
    }

    ret = nor_lock_init();
    if (ret < 0)
    {
        pr_err("create hal_sem lock for nor_flash failed");
        goto out;
    }

    ret = nor_lock();
    if (ret)
    {
        pr_err("init: lock nor failed");
        goto out;
    }

    ret = nor_spi_master_init(&nor->spim);
    if (ret)
    {
        goto unlock;
    }

    ret = nor_reset();
    if (ret)
    {
        goto unlock;
    }

    ret = nor_scan();
    if (ret)
    {
        goto unlock;
    }
    pr_notice("Nor Flash %s size %uMB write %dbit read %dbit blk size %uKB",
              nor->info->name, nor->total_size / 1024 / 1024,
              cmd_bit(nor->cmd_write), cmd_bit(nor->cmd_read), nor->blk_size / 1024);
    pr_notice("Nor Flash ID (hex): %02x %02x %02x", nor->info->id[0],
              nor->info->id[1], nor->info->id[2]);

    ret = nor_factory_init();
    if (ret)
    {
        goto unlock;
    }

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    nor->pt_now = NOR_PROTECT_ALL;
    ret = nor_unprotect_do(NOR_PROTECT_DEFAULT);
    if (ret)
    {
        goto unlock;
    }
#endif

unlock:
    if (nor_unlock())
    {
        ret = -EBUSY;
        pr_err("unlock nor failed");
    }
out:
    if (ret)
    {
        pr_err("init nor flash failed");
    }
    else
    {
        pr_info("nor flash init ok");
    }
    return ret;
}

static int32_t spinor_uninitialize(void)
{
    if (nor_lock())
    {
        pr_err("deinit: lock nor failed");
        return -EBUSY;
    }

    if (nor->info)
    {
#ifdef SUPPORT_4B_ADDR
        if (nor->addr_width == 4)
        {
            nor_set_4byte(0);
        }
#endif
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
        if (nor_unprotect_do(NOR_PROTECT_NONE))
        {
            pr_err("uninit: remove write protection!");
        }
#endif
        /* hal_spi_master_deinit(nor->spim.port); */
    }

    if (nor_unlock())
    {
        pr_err("uninit: unlock nor failed");
        nor_lock_deinit();
        memset(nor, 0, sizeof(*nor));
        return -EBUSY;
    }

    nor_lock_deinit();
    memset(nor, 0, sizeof(*nor));
    return SUNXI_HAL_OK;
}

static int32_t spinor_power_control(sunxi_hal_power_state_e state)
{
    return SUNXI_HAL_OK;
}

int32_t spinor_read_data(uint32_t addr, const void *buf, uint32_t cnt)
{
    int ret = -1;

    ret = nor_lock();
    if (ret)
    {
        pr_err("read: lock nor failed");
        goto out;
    }

    ret = -EBUSY;
    if (!nor->info)
    {
        goto unlock;
    }

    pr_debug("try to read addr 0x%x with len %u", addr, cnt);

    if (nor_wait_ready_ms(0, 500))
    {
        goto unlock;
    }

    while (cnt)
    {
        unsigned int rlen = MIN(cnt, (unsigned int)NOR_PAGE_SIZE);

        ret = nor_read_do(addr, buf, rlen);
        if (ret)
        {
            goto unlock;
        }

        addr += rlen;
        buf += rlen;
        cnt -= rlen;
    }

unlock:
    if (nor_unlock())
    {
        ret = -EBUSY;
        pr_err("read: unlock nor failed");
    }
out:
    if (ret)
    {
        pr_err("read address 0x%x with len %u failed", addr, cnt);
    }
    return ret;
}


int32_t spinor_program_data(uint32_t addr, const void *buf, uint32_t cnt)
{
    int ret;

    ret = nor_lock();
    if (ret)
    {
        pr_err("write: lock nor failed");
        goto out;
    }

    ret = -EBUSY;
    if (!nor->info)
    {
        goto unlock;
    }

    pr_debug("try to write addr 0x%x with len %u", addr, cnt);

    if (nor_wait_ready_ms(0, 500))
    {
        goto unlock;
    }

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    ret = nor_unprotect_do(addr);
    if (ret)
    {
        goto unlock;
    }
#endif

    while (cnt)
    {
        unsigned int wlen = MIN(cnt, (unsigned int)nor->page_size);

        ret = nor_write_do(addr, (const char *)buf, wlen);
        if (ret)
        {
            goto unprotect;
        }

        addr += wlen;
        buf += wlen;
        cnt -= wlen;
    }

unprotect:
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    nor_unprotect_do(NOR_PROTECT_DEFAULT);
#endif
unlock:
    if (nor_unlock())
    {
        ret = -EBUSY;
        pr_err("write: unlock nor failed");
    }
out:
    if (ret)
    {
        pr_err("write address 0x%x with len %u failed", addr, cnt);
    }
    return ret;
}

int32_t spinor_erase_sector(uint32_t addr, uint32_t size)
{
    int ret = 0;
    unsigned int total_size;

    ret = nor_lock();
    if (ret)
    {
        pr_err("erase: lock nor failed");
        goto out;
    }

    /*
     * nor erase size can be 4k/32k/64k, driver should erase block size base
     * on given size. Here must be align to 4k
     */
    if (size % SZ_4K)
    {
        pr_err("erase size 4k is not align to %u", size);
        ret = -EINVAL;
        goto unlock;
    }

    total_size = nor->total_size;
    if (addr + size > total_size)
    {
        pr_err("addr 0x%x with size %u over total size %u",
               addr, size, total_size);
        ret = -EINVAL;
        goto unlock;
    }

    pr_debug("try to erase addr 0x%x with size %u", addr, size);

#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    ret = nor_unprotect_do(addr);
    if (ret)
    {
        goto unlock;
    }
#endif

    if (addr == 0 && size == total_size)
    {
        ret = nor_erase_all();
        goto unprotect;
    }

    while (size >= SZ_64K)
    {
        if (nor->info->flag & NO_ERASE_64K)
        {
            break;
        }
        if (addr % SZ_64K)
        {
            break;
        }
        pr_debug("try to erase 64k from %u", addr);
        ret = nor_erase_64k(addr);
        if (ret)
        {
            pr_err("try to erase 64k from %u failed\n", addr);
            goto unprotect;
        }
        addr += SZ_64K;
        size -= SZ_64K;
    }

    while (size >= SZ_32K)
    {
        if (addr % SZ_32K)
        {
            break;
        }
        pr_debug("try to erase 32k from %u", addr);
        ret = nor_erase_32k(addr);
        if (ret)
        {
            pr_err("try to erase 32k from %u failed\n", addr);
            goto unprotect;
        }
        addr += SZ_32K;
        size -= SZ_32K;
    }

    while (size >= SZ_4K)
    {
        if (addr % SZ_4K)
        {
            break;
        }
        pr_debug("try to erase 4k from %u", addr);
        ret = nor_erase_4k(addr);
        if (ret)
        {
            pr_err("try to erase 4k from %u failed\n", addr);
            goto unprotect;
        }
        addr += SZ_4K;
        size -= SZ_4K;
    }

    if (size)
    {
        pr_err("erase fail as addr %u not align 64k/32k/4k", addr);
        ret = -EINVAL;
    }

unprotect:
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
    nor_unprotect_do(NOR_PROTECT_DEFAULT);
#endif
unlock:
    if (nor_unlock())
    {
        ret = -EBUSY;
        pr_err("erase: unlock nor failed\n");
    }
out:
    if (ret)
    {
        pr_err("erase addr 0x%08x, size %d failed!\n", addr, size);
    }
    return ret;
}

static int32_t spinor_erase_chip(void)
{
    int ret = -1;
    char cmd = NOR_CMD_ERASE_CHIP;

    if (!nor->info)
    {
        goto out;
    }

    ret = nor_lock();
    if (ret)
    {
        pr_err("erase: lock nor failed");
        goto out;
    }

    pr_debug("try to erase all chip");

    ret = nor_write_enable();
    if (ret)
    {
        goto unlock;
    }

    ret = nor_read_write(1, &cmd, 1, NULL, 0);
    if (ret)
    {
        goto unlock;
    }
    ret = nor_wait_ready_ms(26 * 1000, 1000 * 1000);

unlock:
    if (nor_unlock())
    {
        ret = -EBUSY;
        pr_err("erase: unlock nor failed");
    }
out:
    if (ret)
    {
        pr_err("erase all chip failed");
    }
    return ret;
}

/**
 * unportect addr on nor
 *
 * Why call it unprotect?
 * We can call it when we want to write/erase to @addr. This function will
 * modify protect mode to alignmemt down. For example:
 * we want to write to 0x80000a (8M + 10Byte), we can call as:
 *      nor_unprotect(0x80000a);
 * It will scan @info->pt and get protect mode whose boundary is alignmemt
 * down to @addr:
 *      { .boundary = SZ_8M, .bp = BIT(3), .flag = SET_TB },
 * Then, this function will just change to protect [0,8M) to ensure
 * @addr(0x80000a) is writable.
 *
 * Why should we need this function?
 * In some board, CPU will keep sending data to flash when power fail.
 * However when flash work on voltage, witch is lower than spec, flash will
 * write to unexpected address. That may damage the old data. So, we need this
 * function to protect the read-only data. We unprotect it for general writing
 * for OTA updating.
 *
 * Note:
 * After write to @addr, you are support to call to turn back to default.
 *      nor_unprotect(NOR_PROTECT_DEFAULT);
 */
#ifdef CONFIG_SUPPORT_WRITE_PROTECTION
int nor_unprotect(unsigned int addr)
{
    int ret;

    ret = nor_lock();
    if (ret)
    {
        pr_err("erase: lock nor failed\n");
        return ret;
    }

    ret = -EBUSY;
    if (!nor->info)
    {
        goto unlock;
    }

    /*
     * no allow to control under individual protect, as nor driver will do
     * lock/unlock every time write/erase. This cause little time.
     */
    if (nor->info->flag & EN_INDIVIDUAL_PROTECT_MODE)
    {
        ret = 0;
        goto unlock;
    }

    switch (addr)
    {
        case NOR_PROTECT_NONE:
        case NOR_PROTECT_DEFAULT:
            /* we should unset this flag to ensure it can turn back to default */
            nor->flag &= ~NOR_FLAG_PT_NOT_EXPAND;
            break;
        default:
            /* we set this flag to ensure no one can expand protected */
            nor->flag |= NOR_FLAG_PT_NOT_EXPAND;
    }

    ret = nor_unprotect_do(addr);
unlock:
    if (nor_unlock())
    {
        ret = -EBUSY;
        pr_err("erase: unlock nor failed\n");
    }
    return ret;
}
#endif

static sunxi_hal_spinor_info *spinor_get_info(void)
{
    sunxi_hal_spinor_info *info;
    if (!nor->info)
    {
        return NULL;
    }

    info = &hal_spinor_info;
    info->sector_size = nor->info->blk_size;
    info->sector_count = nor->info->blk_cnt;
    info->page_size = nor->page_size;

    return info;
}

static void spinor_signal_event(uint32_t event)
{

}

static int32_t spinor_control(int32_t dev, uint32_t command, uint32_t arg)
{
    return SUNXI_HAL_OK;
}

const sunxi_hal_driver_spinor_t sunxi_hal_spinor_driver =
{
    .get_version  = spinor_get_version,
    .get_capabilities = spinor_get_capabilities,
    .initialize = spinor_initialize,
    .uninitialize = spinor_uninitialize,
    .power_control = spinor_power_control,
    .read_data = spinor_read_data,
    .program_data = spinor_program_data,
    .erase_sector = spinor_erase_sector,
    .erase_chip = spinor_erase_chip,
    .get_status = spinor_get_status,
    .get_info = spinor_get_info,
    .signal_event = spinor_signal_event,
    .control = spinor_control,
};
