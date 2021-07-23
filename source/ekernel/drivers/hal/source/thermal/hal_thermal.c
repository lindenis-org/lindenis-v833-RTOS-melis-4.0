

#include <stdint.h>
#include <stdio.h>
#include <sunxi_hal_thermal.h>

#define readb(addr)	(*((volatile unsigned char  *)(addr)))
#define readw(addr)	(*((volatile unsigned short *)(addr)))
#define readl(addr)	(*((volatile unsigned long  *)(addr)))
#define writeb(v, addr)	(*((volatile unsigned char  *)(addr)) = (unsigned char)(v))
#define writew(v, addr)	(*((volatile unsigned short *)(addr)) = (unsigned short)(v))
#define writel(v, addr)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

void ths_init(void)
{
	/*
	 * clkin = 24MHz
	 * T acquire = clkin / (x + 1)
	 *           = 20us
	 */
	writel(THS_CTRL_T_ACQ(479), THS_CTL);
	/* average over 4 samples */
	writel(THS_FILTER_EN | THS_FILTER_TYPE(1), THS_MFC);
	/* period = (x + 1) * 4096 / clkin; ~10ms */
	writel(THS_PC_TEMP_PERIOD(58), THS_PCTL);
	/* enable sensor */
	writel(THS_NUM, THS_EN);
}

void ths_calibrate(short int *buf, unsigned int len)
{
	unsigned int i;
	int ft_temp;

	//TODO: clk init
	writel(0x10001, 0x030019fc);
//	writel(, 0x03001f30);

	if (!buf[0] || len < 2 + 2 * THS_NUM)
		return ;
//		return -1;

	ft_temp = buf[0] & FT_TEMP_MASK;

	for (i = 0; i < THS_NUM; i++) {
		int reg = (int)buf[i + 1];
		int sensor_temp = ths_reg2temp(reg);
		int delta, cdata, calib_offest;

		/*
		 * To calculate the calibration value:
		 *
		 * X(in Celsius) = Ts - ft_temp
		 * delta = X * 10000 / TEMP_TO_REG
		 * cdata = CALIBRATE_DEFAULT - delta
		 *
		 * cdata: calibration value
		 */
		delta = (sensor_temp - ft_temp * 100) * 10 / TEMP_TO_REG;
		cdata = CALIBRATE_DEFAULT - delta;
		if (cdata & ~TEMP_CALIB_MASK) {
			/*
			 * Calibration value more than 12-bit, but calibration
			 * register is 12-bit. In this case, ths hardware can
			 * still work without calibration, although the data
			 * won't be so accurate.
			 */
			continue;
		}

		calib_offest = THS_CALIB + (i / 2) * 0x4;

		if (i % 2) {
			writel((readl(THS_CALIB) & TEMP_CALIB_MASK) |
			       (cdata << 16), calib_offest);
		} else {
			writel(cdata, calib_offest);
		}
	}
}

/* Temp Unit: millidegree Celsius */
int ths_reg2temp(int reg)
{
	return (reg + OFFSET) * SCALE;
}

int ths_get_temp(unsigned int num, int *temp)
{
	uint32_t val;

	val = readl(THS_DATA + 0x4 * num);

	/* ths have no data yet */
	if (!val)
		return -1;

	*temp = ths_reg2temp(val);

	return 0;
}

