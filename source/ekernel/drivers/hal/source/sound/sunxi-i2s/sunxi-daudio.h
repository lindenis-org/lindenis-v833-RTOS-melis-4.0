/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef	__SUNXI_DAUDIO_H_
#define	__SUNXI_DAUDIO_H_

/* #include <aw_common.h> */
#include <hal_clk.h>
#include <sunxi_hal_gpio.h>
#include <sound/snd_core.h>
#include <sound/snd_pcm.h>
#include <sound/snd_io.h>

enum sunxi_daudio_clk_parent {
	SUNXI_MODULE_CLK_PLL_AUDIO,
	SUNXI_MODULE_CLK_PLL_AUDIOX4,
};

/*
 *clk_parent:
 *	SUNXI_MODULE_CLK_PLL_AUDIO	1: use as clk parent, 0 : not use
 *      SUNXI_MODULE_CLK_PLL_AUDIOX4	1: use as clk parent, 0 : not use
 *daudio_master:
 *	1: SND_SOC_DAIFMT_CBM_CFM(codec clk & FRM master)		use
 *	2: SND_SOC_DAIFMT_CBS_CFM(codec clk slave & FRM master)		not use
 *	3: SND_SOC_DAIFMT_CBM_CFS(codec clk master & frame slave)	not use
 *	4: SND_SOC_DAIFMT_CBS_CFS(codec clk & FRM slave)		use
 *tdm_config:
 *	0 is pcm; 1 is i2s
 *audio_format:
 *	1:SND_SOC_DAIFMT_I2S(standard i2s format). use
 *	2:SND_SOC_DAIFMT_RIGHT_J(right justfied format).
 *	3:SND_SOC_DAIFMT_LEFT_J(left justfied format)
 *	4:SND_SOC_DAIFMT_DSP_A(pcm. MSB is available on 2nd BCLK rising edge after LRC rising edge). use
 *	5:SND_SOC_DAIFMT_DSP_B(pcm. MSB is available on 1nd BCLK rising edge after LRC rising edge)
 *signal_inversion:
 *	1:SND_SOC_DAIFMT_NB_NF(normal bit clock + frame)  use
 *	2:SND_SOC_DAIFMT_NB_IF(normal BCLK + inv FRM)
 *	3:SND_SOC_DAIFMT_IB_NF(invert BCLK + nor FRM)  use
 *	4:SND_SOC_DAIFMT_IB_IF(invert BCLK + FRM)
 *pcm_lrck_period 	:16/32/64/128/256
 *msb_lsb_first 	:0: msb first; 1: lsb first
 *sign_extend 		:0: zero pending; 1: sign extend
 *slot_width_select 	:8 bit width / 16 bit width / 32 bit width
 *frametype		:0: short frame = 1 clock width;  1: long frame = 2 clock width
 *mclk_div		:0: not output(normal setting this);
 *			:1/2/4/6/8/12/16/24/32/48/64/96/128/176/192:
 *			setting mclk as input clock to external codec,
 *			freq is pll_audio/mclk_div
 *tx_data_mode 		:0: 16bit linear PCM; (use) 1: reserved;
 *			:2: 8bit u-law; (no use) 3: 8bit a-law (no use)
 *rx_data_mode 		:0: 16bit linear PCM; (use) 1: reserved;
 *			:2: 8bit u-law; (no use) 3: 8bit a-law (no use)
 */
struct sunxi_daudio_param {
       uint8_t tdm_num;
       uint8_t clk_parent;
       uint8_t daudio_master;
       uint8_t audio_format;
       uint8_t signal_inversion;
       uint16_t pcm_lrck_period;
       uint8_t msb_lsb_first:1;
       uint8_t sign_extend:2;
       uint8_t tx_data_mode:2;
       uint8_t rx_data_mode:2;
       uint8_t slot_width_select;
       uint8_t frametype;
       uint8_t tdm_config;
       uint16_t mclk_div;
};

struct daudio_pinctrl {
	uint32_t gpio_pin;
	uint32_t mux;
	uint8_t driv_level;
};

struct sunxi_daudio_info {
	struct snd_platform *platform;
	/* freert_rwlock_t gen_lock; */

	hal_clk_id_t pllclk;
	hal_clk_id_t pllclkx4;
	hal_clk_id_t moduleclk;

	struct daudio_pinctrl *pinctrl;
	uint8_t pinctrl_num;

	struct sunxi_daudio_param param;
	struct sunxi_dma_params playback_dma_param;
	struct sunxi_dma_params capture_dma_param;

	uint8_t global_enable;
	unsigned int hub_mode;
	bool playback_en;
	bool capture_en;

};

extern struct snd_pcm_ops sunxi_daudio_pcm_ops;
extern int sunxi_daudio_pcm_new(struct snd_pcm *pcm);
extern void sunxi_daudio_pcm_free(struct snd_pcm *pcm);

/* #ifdef CONFIG_ARCH_SUN8IW19P1 */
#include "sun8iw19-daudio.h"
/* #endif */

#endif	/* __SUNXI_DAUDIO_H_ */
