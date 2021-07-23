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
#ifndef	__SUN8IW19_DAUDIO_H_
#define	__SUN8IW19_DAUDIO_H_

/* #include <aw_common.h> */
#include <hal_clk.h>
#include <sunxi_hal_gpio.h>
#include <sound/snd_core.h>
#include <sound/snd_pcm.h>
#include <sound/snd_io.h>

#include "sunxi-daudio.h"

#define	DAUDIO_BASE (0x05090000)

/* DAUDIO register definition */
#define	SUNXI_DAUDIO_CTL		0x00
#define	SUNXI_DAUDIO_FMT0		0x04
#define	SUNXI_DAUDIO_FMT1		0x08
#define	SUNXI_DAUDIO_INTSTA		0x0C
#define	SUNXI_DAUDIO_RXFIFO		0x10
#define	SUNXI_DAUDIO_FIFOCTL		0x14
#define	SUNXI_DAUDIO_FIFOSTA		0x18
#define	SUNXI_DAUDIO_INTCTL		0x1C
#define	SUNXI_DAUDIO_TXFIFO		0x20
#define	SUNXI_DAUDIO_CLKDIV		0x24
#define	SUNXI_DAUDIO_TXCNT		0x28
#define	SUNXI_DAUDIO_RXCNT		0x2C
#define	SUNXI_DAUDIO_CHCFG		0x30
#define	SUNXI_DAUDIO_TX0CHSEL		0x34
#define	SUNXI_DAUDIO_TX1CHSEL		0x38
#define	SUNXI_DAUDIO_TX2CHSEL		0x3C
#define	SUNXI_DAUDIO_TX3CHSEL		0x40

#define	SUNXI_DAUDIO_TX0CHMAP0		0x44
#define	SUNXI_DAUDIO_TX0CHMAP1		0x48
#define	SUNXI_DAUDIO_TX1CHMAP0		0x4C
#define	SUNXI_DAUDIO_TX1CHMAP1		0x50
#define	SUNXI_DAUDIO_TX2CHMAP0		0x54
#define	SUNXI_DAUDIO_TX2CHMAP1		0x58
#define	SUNXI_DAUDIO_TX3CHMAP0		0x5C
#define	SUNXI_DAUDIO_TX3CHMAP1		0x60
#define	SUNXI_DAUDIO_RXCHSEL		0x64
#define	SUNXI_DAUDIO_RXCHMAP0		0x68
#define	SUNXI_DAUDIO_RXCHMAP1		0x6C
#define	SUNXI_DAUDIO_RXCHMAP2		0x70
#define	SUNXI_DAUDIO_RXCHMAP3		0x74
#define	SUNXI_DAUDIO_DEBUG		0x78
#define	SUNXI_DAUDIO_REV		0x7C

#define SUNXI_DAUDIO_REG_NUM_MAX (SUNXI_DAUDIO_DEBUG >> 2)

/* SUNXI_DAUDIO_CTL:0x00 */
#define RX_SYNC_EN			21
#define RX_EN_MUX			20
#define	BCLK_OUT			18
#define	LRCK_OUT			17
#define	LRCKR_CTL			16
#define	SDO3_EN				11
#define	SDO2_EN				10
#define	SDO1_EN				9
#define	SDO0_EN				8
#define	MUTE_CTL			6
#define	MODE_SEL			4
#define	LOOP_EN				3
#define	CTL_TXEN			2
#define	CTL_RXEN			1
#define	GLOBAL_EN			0

/* SUNXI_DAUDIO_FMT0:0x04 */
#define	SDI_SYNC_SEL			31
#define	LRCK_WIDTH			30
#define	LRCKR_PERIOD			20
#define	LRCK_POLARITY			19
#define	LRCK_PERIOD			8
#define	BRCK_POLARITY			7
#define	DAUDIO_SAMPLE_RESOLUTION	4
#define	EDGE_TRANSFER			3
#define	SLOT_WIDTH			0

/* SUNXI_DAUDIO_FMT1:0x08 */
#define	RX_MLS				7
#define	TX_MLS				6
#define	SEXT				4
#define	RX_PDM				2
#define	TX_PDM				0

/* SUNXI_DAUDIO_INTSTA:0x0C */
#define	TXU_INT				6
#define	TXO_INT				5
#define	TXE_INT				4
#define	RXU_INT				2
#define RXO_INT				1
#define	RXA_INT				0

/* SUNXI_DAUDIO_FIFOCTL:0x14 */
#define	HUB_EN				31
#define	FIFO_CTL_FTX			25
#define	FIFO_CTL_FRX			24
#define	TXTL				12
#define	RXTL				4
#define	TXIM				2
#define	RXOM				0

/* SUNXI_DAUDIO_FIFOSTA:0x18 */
#define	FIFO_TXE			28
#define	FIFO_TX_CNT			16
#define	FIFO_RXA			8
#define	FIFO_RX_CNT			0

/* SUNXI_DAUDIO_INTCTL:0x1C */
#define	TXDRQEN				7
#define	TXUI_EN				6
#define	TXOI_EN				5
#define	TXEI_EN				4
#define	RXDRQEN				3
#define	RXUIEN				2
#define	RXOIEN				1
#define	RXAIEN				0

/* SUNXI_DAUDIO_CLKDIV:0x24 */
#define	MCLKOUT_EN			8
#define	BCLK_DIV			4
#define	MCLK_DIV			0

/* SUNXI_DAUDIO_CHCFG:0x30 */
#define	TX_SLOT_HIZ			9
#define	TX_STATE			8
#define	RX_SLOT_NUM			4
#define	TX_SLOT_NUM			0

/* SUNXI_DAUDIO_TXnCHSEL:0X34+n*0x04 */
#define	TX_OFFSET			20
#define	TX_CHSEL			16
#define	TX_CHEN				0

/* SUNXI_DAUDIO_RXCHSEL */
#define	RX_OFFSET			20
#define	RX_CHSEL			16

/* sun8iw10 CHMAP default setting */
#define	SUNXI_DEFAULT_CHMAP0		0xFEDCBA98
#define	SUNXI_DEFAULT_CHMAP1		0x76543210

/* RXCHMAP default setting */
#define	SUNXI_DEFAULT_CHMAP		0x76543210

/* Shift & Mask define */

/* SUNXI_DAUDIO_CTL:0x00 */
#define	SUNXI_DAUDIO_MODE_CTL_MASK		3
#define	SUNXI_DAUDIO_MODE_CTL_PCM		0
#define	SUNXI_DAUDIO_MODE_CTL_I2S		1
#define	SUNXI_DAUDIO_MODE_CTL_LEFT		1
#define	SUNXI_DAUDIO_MODE_CTL_RIGHT		2
#define	SUNXI_DAUDIO_MODE_CTL_REVD		3
/* combine LRCK_CLK & BCLK setting */
#define	SUNXI_DAUDIO_LRCK_OUT_MASK		3
#define	SUNXI_DAUDIO_LRCK_OUT_DISABLE		0
#define	SUNXI_DAUDIO_LRCK_OUT_ENABLE		3

/* SUNXI_DAUDIO_FMT0 */
#define	SUNXI_DAUDIO_LRCK_PERIOD_MASK		0x3FF
#define	SUNXI_DAUDIO_SLOT_WIDTH_MASK		7
/* Left Blank */
#define	SUNXI_DAUDIO_SR_MASK			7
#define	SUNXI_DAUDIO_SR_16BIT			3
#define	SUNXI_DAUDIO_SR_24BIT			5
#define	SUNXI_DAUDIO_SR_32BIT			7

#define	SUNXI_DAUDIO_LRCK_POLARITY_NOR		0
#define	SUNXI_DAUDIO_LRCK_POLARITY_INV		1
#define	SUNXI_DAUDIO_BCLK_POLARITY_NOR		0
#define	SUNXI_DAUDIO_BCLK_POLARITY_INV		1

/* SUNXI_DAUDIO_FMT1 */
#define	SUNXI_DAUDIO_FMT1_DEF			0x30

/* SUNXI_DAUDIO_FIFOCTL */
#define	SUNXI_DAUDIO_TXIM_MASK			1
#define	SUNXI_DAUDIO_TXIM_VALID_MSB		0
#define	SUNXI_DAUDIO_TXIM_VALID_LSB		1
/* Left Blank */
#define	SUNXI_DAUDIO_RXOM_MASK			3
/* Expanding 0 at LSB of RX_FIFO */
#define	SUNXI_DAUDIO_RXOM_EXP0			0
/* Expanding sample bit at MSB of RX_FIFO */
#define	SUNXI_DAUDIO_RXOM_EXPH			1
/* Fill RX_FIFO low word be 0 */
#define	SUNXI_DAUDIO_RXOM_TUNL			2
/* Fill RX_FIFO high word be higher sample bit */
#define	SUNXI_DAUDIO_RXOM_TUNH			3

/* SUNXI_DAUDIO_CLKDIV */
#define	SUNXI_DAUDIO_BCLK_DIV_MASK		0xF
#define	SUNXI_DAUDIO_BCLK_DIV_1			1
#define	SUNXI_DAUDIO_BCLK_DIV_2			2
#define	SUNXI_DAUDIO_BCLK_DIV_3			3
#define	SUNXI_DAUDIO_BCLK_DIV_4			4
#define	SUNXI_DAUDIO_BCLK_DIV_5			5
#define	SUNXI_DAUDIO_BCLK_DIV_6			6
#define	SUNXI_DAUDIO_BCLK_DIV_7			7
#define	SUNXI_DAUDIO_BCLK_DIV_8			8
#define	SUNXI_DAUDIO_BCLK_DIV_9			9
#define	SUNXI_DAUDIO_BCLK_DIV_10		10
#define	SUNXI_DAUDIO_BCLK_DIV_11		11
#define	SUNXI_DAUDIO_BCLK_DIV_12		12
#define	SUNXI_DAUDIO_BCLK_DIV_13		13
#define	SUNXI_DAUDIO_BCLK_DIV_14		14
#define	SUNXI_DAUDIO_BCLK_DIV_15		15
/* Left Blank */
#define	SUNXI_DAUDIO_MCLK_DIV_MASK		0xF
#define	SUNXI_DAUDIO_MCLK_DIV_1			1
#define	SUNXI_DAUDIO_MCLK_DIV_2			2
#define	SUNXI_DAUDIO_MCLK_DIV_3			3
#define	SUNXI_DAUDIO_MCLK_DIV_4			4
#define	SUNXI_DAUDIO_MCLK_DIV_5			5
#define	SUNXI_DAUDIO_MCLK_DIV_6			6
#define	SUNXI_DAUDIO_MCLK_DIV_7			7
#define	SUNXI_DAUDIO_MCLK_DIV_8			8
#define	SUNXI_DAUDIO_MCLK_DIV_9			9
#define	SUNXI_DAUDIO_MCLK_DIV_10		10
#define	SUNXI_DAUDIO_MCLK_DIV_11		11
#define	SUNXI_DAUDIO_MCLK_DIV_12		12
#define	SUNXI_DAUDIO_MCLK_DIV_13		13
#define	SUNXI_DAUDIO_MCLK_DIV_14		14
#define	SUNXI_DAUDIO_MCLK_DIV_15		15

/* SUNXI_DAUDIO_CHCFG */
#define	SUNXI_DAUDIO_TX_SLOT_MASK		0XF
#define	SUNXI_DAUDIO_RX_SLOT_MASK		0XF

/* SUNXI_DAUDIO_TX0CHSEL: */
#define	SUNXI_DAUDIO_TX_OFFSET_MASK		3
#define	SUNXI_DAUDIO_TX_OFFSET_0		0
#define	SUNXI_DAUDIO_TX_OFFSET_1		1
/* Left Blank */
#define	SUNXI_DAUDIO_TX_CHEN_MASK		0xFFFF
#define	SUNXI_DAUDIO_TX_CHSEL_MASK		0xF

/* SUNXI_DAUDIO_RXCHSEL */
#define SUNXI_DAUDIO_RX_OFFSET_MASK		3
#define	SUNXI_DAUDIO_RX_CHSEL_MASK		0XF

#define DAUDIO_RXCH_DEF_MAP(x) (x << ((x%4)<<3))
#define DAUDIO_RXCHMAP(x) (0x1f << ((x%4)<<3))

/*
 * For other define.
 */

#define DAUDIO_NAME_LEN		(16)

#define DAUDIO_NUM_MAX		2

/* DRQ Type */
#ifndef DRQDST_DAUDIO_0_TX
#define DRQDST_DAUDIO_0_TX	3
#endif
#ifndef DRQDST_DAUDIO_1_TX
#define DRQDST_DAUDIO_1_TX	4
#endif

#ifndef DRQSRC_DAUDIO_0_RX
#define DRQSRC_DAUDIO_0_RX	3
#endif
#ifndef DRQSRC_DAUDIO_1_RX
#define DRQSRC_DAUDIO_1_RX	4
#endif

/* GPIO define */
#define DAUDIO0_PIN_MCLK \
{.gpio_pin = GPIOH(0), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_BCLK \
{.gpio_pin = GPIOH(1), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_LRCK \
{.gpio_pin = GPIOH(2), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_DOUT \
{.gpio_pin = GPIOH(3), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_DIN	\
{.gpio_pin = GPIOH(4), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}

#define DAUDIO1_PIN_MCLK \
{.gpio_pin = GPIOD(10), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO1_PIN_BCLK \
{.gpio_pin = GPIOD(11), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO1_PIN_LRCK \
{.gpio_pin = GPIOD(12), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO1_PIN_DOUT \
{.gpio_pin = GPIOD(13), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
/*
*#define DAUDIO1_PIN_DOUT1 \
*{.gpio_pin = GPIOD(14), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
*#define DAUDIO1_PIN_DOUT2 \
*{.gpio_pin = GPIOD(15), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
*#define DAUDIO1_PIN_DOUT3 \
*{.gpio_pin = GPIOD(16), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}
*/
#define DAUDIO1_PIN_DIN \
{.gpio_pin = GPIOD(16), .mux = 3, .driv_level = GPIO_DRIVING_LEVEL1}

/*
 * Daudio Params
 *
 *clk_parent:
 *	SUNXI_MODULE_CLK_PLL_AUDIO     0: use pll_audio
 *	SUNXI_MODULE_CLK_PLL_AUDIOX4   1: use pll_audiox4
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
 *pcm_lrck_period	:16/32/64/128/256
 *msb_lsb_first		:0: msb first; 1: lsb first
 *sign_extend		:0: zero pending; 1: sign extend
 *slot_width_select	:8 bit width / 16 bit width / 32 bit width
 *frametype		:0: short frame = 1 clock width;  1: long frame = 2 clock width
 *mclk_div		:0: not output(normal setting this);
 *			:1/2/4/6/8/12/16/24/32/48/64/96/128/176/192:
 *			setting mclk as input clock to external codec,
 *			freq is pll_audio/mclk_div
 *tx_data_mode		:0: 16bit linear PCM; (use) 1: reserved;
 *			:2: 8bit u-law; (no use) 3: 8bit a-law (no use)
 *rx_data_mode		:0: 16bit linear PCM; (use) 1: reserved;
 *			:2: 8bit u-law; (no use) 3: 8bit a-law (no use)
 */

#define DAUDIO0_PARAMS \
{.tdm_num = 0, .clk_parent = 1,\
.daudio_master = 4, .audio_format = 1, .signal_inversion = 1, \
.pcm_lrck_period = 128, .slot_width_select = 32, \
.msb_lsb_first	= 0, .frametype = 0, \
.tx_data_mode = 0, .rx_data_mode = 0, \
.tdm_config = 1, .mclk_div = 1,\
}

#define DAUDIO1_PARAMS \
{.tdm_num = 1, .clk_parent = 1,\
.daudio_master = 4, .audio_format = 1, .signal_inversion = 1, \
.pcm_lrck_period = 128, .slot_width_select = 32, \
.msb_lsb_first	= 1, .frametype = 1, \
.tx_data_mode = 0, .rx_data_mode = 0, \
.tdm_config = 0, .mclk_div = 1,\
}

#define SUNXI_DAUDIO_BCLK			0
#define SUNXI_DAUDIO_LRCK			1
#define SUNXI_DAUDIO_MCLK			2
#define SUNXI_DAUDIO_GEN			3

#define	SUNXI_DAUDIO_RATES	(SNDRV_PCM_RATE_8000_192000 \
				| SNDRV_PCM_RATE_KNOT)

#define SUNXI_DAUDIO_DRQDST(sunxi_daudio, x)			\
	((sunxi_daudio)->playback_dma_param.dma_drq_type_num =	\
				DRQDST_DAUDIO_##x##_TX)
#define SUNXI_DAUDIO_DRQSRC(sunxi_daudio, x)			\
	((sunxi_daudio)->capture_dma_param.dma_drq_type_num =	\
				DRQSRC_DAUDIO_##x##_RX)

/*to clear FIFO*/
#define SUNXI_DAUDIO_FTX_TIMES			1

/* SUNXI_DAUDIO_HUB_ENABLE: Whether to use the hub mode */
#define SUNXI_DAUDIO_HUB_ENABLE

/* SUNXI_ADC_DAUDIO_SYNC: Whether to enable ADC AEC Drive adaptation */
#define SUNXI_ADC_DAUDIO_SYNC

enum sunxi_pcm_adc_i2s_mode {
	ADC_CODEC_SYNC = 0,
	ADC_I2S_SYNC = 1,
	ADC_I2S_RUNNING = 2,
};

extern void sunxi_daudio_rx_drq_enable(bool enable);

#endif	/* __SUN8IW19_DAUDIO_H_ */
