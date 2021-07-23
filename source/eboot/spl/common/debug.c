/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 */
#include <common.h>

void u8_to_string_hex( u8 input, char * str )
{
	int i;
	static char base[] = "0123456789abcdef";

	for( i = 1; i >= 0; --i )
	{
		str[i] = base[input&0xf];
		input >>= 4;
	}

	str[2] = '\0';

	return;
}

int snprintf(char *buf, size_t size, const char *fmt, ...);
void ndump(u8 *buf, int count)
{
	char line[50];
	char *tmp = line;
	char *in  = (char *)buf;
	int i;
	printf("inbuf:%x\n", (uint32_t)buf);
	for (i = 0; i < count; i++) {
		snprintf(tmp, 5, "%02x ", *((uint8_t *)in));
		tmp += 3;
		if (((i % 16) == 15)) {
			printf("%s\n", line);
			tmp = line;
		}
		in++;
	}
	if (((i % 16) != 0)) {
		printf("%s\n", line);
		tmp = line;
	}
}

void __assert_fail(const char *assertion, const char *file, unsigned line,
		   const char *function)
{
	/* This will not return */
	printf("%s:%u: %s: Assertion `%s' failed.", file, line, function,
	      assertion);
	while(1);
}
