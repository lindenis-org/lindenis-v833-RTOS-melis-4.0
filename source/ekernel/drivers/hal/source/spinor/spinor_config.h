#ifndef _SPINOR_CONFIG_H
#define _SPINOR_CONFIG_H

extern int get_clock_frequency(int *freq);
extern int get_read_mode(int *read_mode);
extern int get_write_mode(int *write_mode);
extern int get_spi_port(int *port);

#define SPINOR_SINGLE_MODE 1
#define SPINOR_DUAL_MODE   2
#define SPINOR_QUAD_MODE   4

#define DEFAULT_READ_MODE SPINOR_QUAD_MODE
#define DEFAULT_WRITE_MODE SPINOR_QUAD_MODE

#endif
