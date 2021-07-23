#include <getopt.h>
#include <log.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sunxi_hal_spi.h>
#include <sunxi_drv_spi.h>
#include <hal_timer.h>
#include <sys/time.h>

static hal_spi_master_config_t cfg =
{
    .clock_frequency = 10*1000*1000,//10MHz
    .slave_port = HAL_SPI_MASTER_SLAVE_0,
    .cpha = HAL_SPI_MASTER_CLOCK_PHASE0,
    .cpol = HAL_SPI_MASTER_CLOCK_POLARITY0,
    //.bit_order = HAL_SPI_MASTER_LSB_FIRST,//LSB
};//SPI configure

static int cmd_drv_spi(int argc, const char **argv)
{
    char tbuf[10]={0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    char rbuf[10];
    hal_spi_master_transfer_t tr;
    tr.tx_buf = tbuf;
    tr.tx_len = 10;
    tr.rx_buf = rbuf;
    tr.rx_len = 10;
    tr.tx_single_len = tr.tx_len;
    tr.dummy_byte = 0;
    tr.rx_nbits = tr.tx_nbits = SPI_NBITS_SINGLE;
    rt_device_t fd;

    fd = rt_device_find("spi1");
    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);
    rt_device_control(fd, SPI_CONFIG, &cfg);
    rt_device_write((rt_device_t)fd, 0, tbuf, 10);
    rt_device_read((rt_device_t)fd, 0, tbuf, 10);
    printf("SPI1 cmd receive  data is %s \n", rbuf);
    tbuf[0]=0xAA;tbuf[1]=0xBB;
    rt_device_control(fd, SPI_WRITE_READ, &tr);
    rt_device_close((rt_device_t)fd);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_drv_spi, __cmd_drvspi, rtthread spi test code);


#define TEST_READ 0
#define TEST_WRITE 1
static int cmd_hal_spi(int argc, const char **argv)
{
    hal_spi_master_port_t  port;
    char tbuf[10]={0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    char rbuf[10];
    char rw = 1;
    int c;

    if (argc < 3)
    {
        printk("Usage:\n");
        printk("\tspi <port> <-r|-w> \n");
        return -1;
    }

    port = strtol(argv[1], NULL, 0);
    while ((c = getopt(argc, (char *const *)argv, "r:w")) != -1)
    {
        switch (c)
        {
            case 'r':
                rw = TEST_READ;
                break;
            case 'w':
                rw = TEST_WRITE;
                break;
        }
    }

    hal_spi_init(port, NULL);
    //hal_spi_control(port, I2C_SLAVE, &addr);
    if (rw == TEST_READ)
    {
        hal_spi_read(port, rbuf, 10);
        printk("rbuf: %s\n", rbuf);
    }
    else if (rw == TEST_WRITE)
    {
        hal_spi_write(port, tbuf, 10);
    }
    hal_spi_uninit(port);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_spi, __cmd_spi, hal spi test code);

#define SPI_SLAVE_GPIO_CTL
#define SLAVE_GPIO	GPIOH(4)
#define FRAME_HEAD_LEN 4
static int cmd_hal_spi_slave(int argc, const char **argv)
{
    hal_spi_master_port_t  port;
    char tbuf[4096]={0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    char rbuf[4096];
    char frame_head[FRAME_HEAD_LEN];
    int i, verbose = 0;

    if (argc < 2)
    {
        printk("Usage:\n");
        printk("\tspi <port> [v]\n");
        return -1;
    }

    port = strtol(argv[1], NULL, 0);
    if (argc > 2)
	    verbose = *argv[2] == 'v' ? 1:0;
    for (i = 0; i < sizeof(tbuf); i++)
        tbuf[i] = i;

    hal_spi_init(port, NULL);

#ifdef SPI_SLAVE_GPIO_CTL
    hal_gpio_pinmux_set_function(SLAVE_GPIO, GPIO_DIRECTION_OUTPUT);//output
#endif
    while(1) {
#ifdef SPI_SLAVE_GPIO_CTL
	hal_gpio_set_data(SLAVE_GPIO, GPIO_DATA_HIGH);
#endif
        while (hal_spi_read(port, frame_head, FRAME_HEAD_LEN) != 0);
#ifdef SPI_SLAVE_GPIO_CTL
	hal_gpio_set_data(SLAVE_GPIO, GPIO_DATA_LOW);
#endif
	if (frame_head[0] != 0x3A) {
		printk("frame head:0x%02x 0x%02x 0x%02x 0x%02x\n", frame_head[0], frame_head[1], frame_head[2], frame_head[3]);
		printk("SPI SLAVE FRAME ERR, CODE:0x%x \n", frame_head[0]);
		break;
	}

	//0:slave write, 1:slave read
	if (frame_head[1]) {
		memset(rbuf, 0, sizeof(rbuf));
#ifdef SPI_SLAVE_GPIO_CTL
		hal_gpio_set_data(SLAVE_GPIO, GPIO_DATA_HIGH);
#endif
		while (hal_spi_read(port, rbuf, frame_head[2] + (frame_head[3]*256)) != 0);
#ifdef SPI_SLAVE_GPIO_CTL
		hal_gpio_set_data(SLAVE_GPIO, GPIO_DATA_LOW);
#endif
		for (i = 0; i < frame_head[2] + (frame_head[3]*256); i++) {
			if (rbuf[i] != (i & 0xff)) {
				printk("data err[%d]=0x%02x\n", i, rbuf[i]);
				goto end;
			}
		}
		printk("SLAVE READ: %d\n", frame_head[2] + (frame_head[3]*256));
		if (verbose) {
			printk("receive:");
			for (i = 0; i < frame_head[2] + (frame_head[3]*256); i++)
				printk("0x%x ", rbuf[i]);
			printk("\n");
		}
	} else {
#ifdef SPI_SLAVE_GPIO_CTL
		hal_gpio_set_data(SLAVE_GPIO, GPIO_DATA_HIGH);
#endif
		hal_spi_write(port, tbuf, frame_head[2] + (frame_head[3]*256));
#ifdef SPI_SLAVE_GPIO_CTL
		hal_gpio_set_data(SLAVE_GPIO, GPIO_DATA_LOW);
#endif
		printk("SLAVE WRITE: %d\n", frame_head[2] + (frame_head[3]*256));
	}
    }
end:
    hal_spi_uninit(port);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_spi_slave, __cmd_spi_slave, hal spi test code);

void udelay(unsigned int us);
void spi_slave_ready(int timeout)
{
    gpio_data_t gpio_state;
#ifdef SPI_SLAVE_GPIO_CTL
    do {
	hal_gpio_get_data(SLAVE_GPIO, &gpio_state);
	udelay(timeout);
    } while (gpio_state == GPIO_DATA_LOW);
#else
    udelay(timeout);
#endif
}

static int cmd_hal_spi_master(int argc, const char **argv)
{
    hal_spi_master_port_t  port;
    char tbuf[4096]={0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xb0};
    char rbuf[4096];
    char frame_head[FRAME_HEAD_LEN] = {0x3A, TEST_READ, 0, 0};
    int i = 0, loop_cnt = 1, data_len = 1024;
    int timeout = 3300, timeout_late = 50;
    struct timeval tm_start, tm_end, tm_run;
    if (argc < 5)
    {
        printk("Usage:\n");
        printk("\tspi <port> <w/r> <data_len> <loop_cnt> [time_us] [time_late]\n");
        return -1;
    }

    port = strtol(argv[1], NULL, 0);
    data_len = strtol(argv[3], NULL, 0);
    loop_cnt = strtol(argv[4], NULL, 0);
    frame_head[2] = data_len & 0xff;
    frame_head[3] = (data_len >> 8) & 0xff;
    switch (*argv[2])
    {
	case 'r':
		printk("SPI_MASTER READ %d\n", frame_head[2] + (frame_head[3]*256));
		frame_head[1] = TEST_READ;
		break;
	case 'w':
		printk("SPI_MASTER WRITE %d\n", frame_head[2] + (frame_head[3]*256));
		frame_head[1] = TEST_WRITE;
		break;
    }
    if (argc > 5) {
	    timeout = strtol(argv[5], NULL, 0);
	    if (argc > 6)
		    timeout_late = strtol(argv[6], NULL, 0);
    }

    hal_spi_init(port, NULL);

#ifdef SPI_SLAVE_GPIO_CTL
    hal_gpio_pinmux_set_function(SLAVE_GPIO, GPIO_DIRECTION_INPUT);//input
#endif
    for(i = 0; i < sizeof(tbuf); i++){
        tbuf[i] = i;
    }
    gettimeofday(&tm_start, NULL);
    while(loop_cnt--){
rety:
	spi_slave_ready(timeout_late);
        hal_spi_write(port, frame_head, FRAME_HEAD_LEN);
        if (frame_head[1] == TEST_READ) {
            memset(rbuf, 0, sizeof(rbuf));
	    spi_slave_ready(timeout);
            hal_spi_read(port, rbuf, frame_head[2] + (frame_head[3]*256));
            for (i = 0; i < frame_head[2] + (frame_head[3]*256); i++) {
                if(rbuf[i] != (i & 0xff)){
                    printf("loop_cnt:%d, data err[%d]=0x%02x\n", loop_cnt, i, rbuf[i]);
		    udelay(timeout);
		    /* if slave no data readly, master read data is all 0x0. */
		    for (i = 0; i < frame_head[2] + (frame_head[3]*256); i++) {
			if (rbuf[i] == 0x0) {
			    if (i == ((frame_head[2] + (frame_head[3]*256)-1))) {
				printk("slave not data ready\n");
				goto rety;
			    }
			    continue;
			} else
				break;
		    }
                    break;
                }
            }
#if 0
            printk("receive:");
            for (i = 0; i < frame_head[2] + (frame_head[3]*256); i++)
		printk("0x%x ", rbuf[i]);
            printk("\n");
#endif
        } else {
	    spi_slave_ready(timeout);
            hal_spi_write(port, tbuf, frame_head[2] + (frame_head[3]*256));
            printk("MASTER WRITE: %d\n", frame_head[2] + (frame_head[3]*256));
	    udelay(timeout_late);
        }
    }
    gettimeofday(&tm_end, NULL);
    printf("start_time %ld s, %ld us\n", tm_start.tv_sec, tm_start.tv_usec);
    printf("end_time %ld s, %ld us\n", tm_end.tv_sec, tm_end.tv_usec);
    timersub(&tm_end, &tm_start, &tm_run);
    printf("tm_run sec[%ld], usec[%ld]\n", tm_run.tv_sec, tm_run.tv_usec);
    hal_spi_uninit(port);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_spi_master, __cmd_spi_master, hal spi test code);
