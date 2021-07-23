#ifndef _XR_COMMON_H_
#define _XR_COMMON_H_

#define MAC_ADDR_CONF     "/data/xr_wifi.conf"
#ifdef CONFIG_XR819
#define XRADIO_BOOTLOADER  "/data/firmware/boot_xr819.bin"
#define XRADIO_FIRMWARE    "/data/firmware/fw_xr819.bin"
#define XRADIO_SDD_FILE    "/data/firmware/sdd_xr819.bin"
#define XRADIO_ETF_FW      "/data/firmware/etf_xr819.bin"
#else
#define XRADIO_BOOTLOADER  "/data/firmware/boot_xr829.bin"
#define XRADIO_FIRMWARE    "/data/firmware/fw_xr829.bin"
#define XRADIO_SDD_FILE    "/data/firmware/sdd_xr829.bin"
#define XRADIO_ETF_FW      "/data/firmware/etf_xr829.bin"
#endif
#endif
