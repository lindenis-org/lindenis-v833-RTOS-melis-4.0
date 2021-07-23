#ifndef __DRV_CRYPTO_H__
#define __DRV_CRYPTO_H__

//#define HWCRYPTO_LOCK

#define AES_BLOCK_SIZE		16

#define AES_KEYSIZE_128		128
#define AES_KEYSIZE_192		192
#define AES_KEYSIZE_256		256

/*sunxi crypto cfg*/
#define ADDR_ALIGN_SIZE		0x4

#define AES_MODE_ECB	0
#define AES_MODE_CBC	1

#define CRYPTO_DIR_ENCRYPT	0
#define CRYPTO_DIR_DECRYPT	1

/*define the ctx for aes requtest*/
typedef struct {
	rt_uint8_t *src_buffer;
	rt_uint32_t src_length;
	rt_uint8_t *dst_buffer;
	rt_uint32_t dst_length;
	rt_uint8_t *iv;
	rt_uint8_t *key;
	rt_uint32_t key_length;
	rt_uint32_t dir;
	rt_uint32_t mode;
} crypto_aes_req_ctx_t;

extern int do_aes_crypto(crypto_aes_req_ctx_t *aes_req_ctx);
extern void ce_print_hex(char *_data, int _len, void *_addr);
extern int sunxi_ce_init(void);
int aw_hw_crypto_device_init(void);

#endif /* __DRV_CRYPTO_H__ */
