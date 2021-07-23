#include <common.h>
#include <arch/spinor.h>
#include <u-boot/zlib.h>
#include <part_efi_nor.h>

#define O_RDONLY    (00000000)
#define O_WRONLY    (00000001)
#define O_RDWR      (00000002)

static uint env_start_sector = 0;
static uint env_start_sector_2 = 0;
static uint env_sectors = 0;
static uint env_sectors_2 = 0;
static uint cur_envsize = 0;

static int have_redund_env = 1;
static unsigned char active_flag = 1;
/* obsolete_flag must be 0 to efficiently set it on NOR flash without erasing */
static unsigned char obsolete_flag = 0;
static unsigned long usable_envsize;
#define ENV_SIZE      usable_envsize

static int dev_current;

struct env_image_single {
	uint32_t crc;		/* CRC32 over data bytes    */
	char data[];
};

struct env_image_redundant {
	uint32_t crc;		/* CRC32 over data bytes    */
	unsigned char flags;	/* active or obsolete */
	char data[];
};

enum flag_scheme {
	FLAG_NONE,
	FLAG_BOOLEAN,
	FLAG_INCREMENTAL,
};

struct environment {
	void *image;
	uint32_t *crc;
	unsigned char *flags;
	char *data;
	enum flag_scheme flag_scheme;
};

static struct environment environment = {
	.flag_scheme = FLAG_NONE,
};

static int flash_io(int mode);

/*
 * s1 is either a simple 'name', or a 'name=value' pair.
 * s2 is a 'name=value' pair.
 * If the names match, return the value of s2, else NULL.
 */
static char *envmatch(char *s1, char *s2)
{
	if (s1 == NULL || s2 == NULL)
		return NULL;

	while (*s1 == *s2++)
		if (*s1++ == '=')
			return s2;
	if (*s1 == '\0' && *(s2 - 1) == '=')
		return s2;
	return NULL;
}

/**
 * Search the environment for a variable.
 * Return the value, if found, or NULL, if not found.
 */
char *fw_getenv(char *name)
{
	char *env, *nxt;

	for (env = environment.data; *env; env = nxt + 1) {
		char *val;

		for (nxt = env; *nxt; ++nxt) {
			if (nxt >= &environment.data[ENV_SIZE]) {
				printf("## Error: "
						"environment not terminated\n");
				return NULL;
			}
		}
		val = envmatch(name, env);
		if (!val)
			continue;
		return val;
	}
	return NULL;
}

/*
 * Set/Clear a single variable in the environment.
 * This is called in sequence to update the environment
 * in RAM without updating the copy in flash after each set
 */
int fw_env_write(char *name, char *value)
{
	int len;
	char *env, *nxt;
	char *oldval = NULL;
	int deleting, creating, overwriting;

	/*
	 * search if variable with this name already exists
	 */
	for (nxt = env = environment.data; *env; env = nxt + 1) {
		for (nxt = env; *nxt; ++nxt) {
			if (nxt >= &environment.data[ENV_SIZE]) {
				printf("## Error: "
					"environment not terminated\n");
				/* errno = EINVAL; */
				return -1;
			}
		}
		oldval = envmatch(name, env);
		if (oldval)
			break;
	}

	deleting = (oldval && !(value && strlen(value)));
	creating = (!oldval && (value && strlen(value)));
	overwriting = (oldval && (value && strlen(value)));

	if (deleting) {
		printf("env: delting\n");
	} else if (overwriting) {
		printf("env: overwriting\n");
	} else if (creating) {
		printf("env: creating\n");
	} else {
		printf("env: nothing\n");
		return 0;
	}

	if (deleting || overwriting) {
		if (*++nxt == '\0') {
			*env = '\0';
		} else {
			for (;;) {
				*env = *nxt++;
				if ((*env == '\0') && (*nxt == '\0'))
					break;
				++env;
			}
		}
		*++env = '\0';
	}

	/* Delete only ? */
	if (!value || !strlen(value))
		return 0;

	/*
	 * Append new definition at the end
	 */
	for (env = environment.data; *env || *(env + 1); ++env)
		;
	if (env > environment.data)
		++env;
	/*
	 * Overflow when:
	 * "name" + "=" + "val" +"\0\0"  > cur_envsize - (env-environment)
	 */
	len = strlen(name) + 2;
	/* add '=' for first arg, ' ' for all others */
	len += strlen(value) + 1;

	if (len > (&environment.data[ENV_SIZE] - env)) {
		printf("Error: environment overflow, \"%s\" deleted\n", name);
		return -1;
	}

	while ((*env = *name++) != '\0')
		env++;
	*env = '=';
	while ((*++env = *value++) != '\0')
		;

	/* end is marked with double '\0' */
	*++env = '\0';

	return 0;
}

int fw_env_flush(void)
{
	/*
	 * Update CRC
	 */
#ifdef CFG_USE_DCACHE
	dcache_enable();
#endif
	*environment.crc = crc32(0, (uint8_t *) environment.data, ENV_SIZE);
#ifdef CFG_USE_DCACHE
	dcache_disable();
#endif

	/* write environment back to flash */
	if (flash_io(O_RDWR)) {
		printf("Error: can't write fw_env to flash\n");
		return -1;
	}

	return 0;
}

static int flash_env_init(void)
{
	get_part_info_by_name(CFG_SPINOR_ENV_NAME, &env_start_sector, &env_sectors);
	get_part_info_by_name(CFG_SPINOR_ENV_NAME_2, &env_start_sector_2, &env_sectors_2);
	if( env_start_sector == 0 && env_start_sector_2 == 0 ) {
		printf("%s and %s part info all can't find\n",
		 CFG_SPINOR_ENV_NAME, CFG_SPINOR_ENV_NAME_2);
		return -1;
	}
	if( env_sectors == env_sectors_2 && env_sectors != 0) {
#if 0
		cur_envsize = env_sectors * 512;
#else
		/* for env 4k is enough, but partition size may not only 4k (maybe align to 32k or 64k) */
		cur_envsize = 4 * 1024;
#endif
	} else {
		printf("%s and %s part size diff\n", CFG_SPINOR_ENV_NAME, CFG_SPINOR_ENV_NAME_2);
		return -1;
	}
	return 0;
}

static int flash_env_read(void *buf, size_t size)
{
	int start_sector;
	if (dev_current == 0)
		start_sector = env_start_sector;
	else if (dev_current == 1)
		start_sector = env_start_sector_2;
	else
		return -1;

	if (spinor_init(0)) {
		printf("spinor init fail\n");
		return -1;
	}
	if (spinor_read(start_sector, size/512, buf)) {
		printf("read spinor fail\n");
		return -1;
	}

	return 0;
}

/*
 * Set obsolete flag at offset - NOR flash only
 */
static int flash_flag_obsolete(off_t offset)
{
	int rc;
	rc = spinor_random_write(offset, sizeof(obsolete_flag), &obsolete_flag);
	if (rc < 0)
		printf("Could not set obsolete flag");

	return rc;
}

static int flash_env_write(void *buf, size_t size)
{
	int next_env_sector;
	int current_env_sector;
	if (dev_current == 0) {
		current_env_sector = env_start_sector;
		next_env_sector = env_start_sector_2;
	} else if (dev_current == 1) {
		current_env_sector = env_start_sector_2;
		next_env_sector = env_start_sector;
	} else {
		return -1;
	}

	if (spinor_init(0)) {
		printf("spinor init fail\n");
		return -1;
	}

	switch (environment.flag_scheme) {
	case FLAG_NONE:
		break;
	case FLAG_INCREMENTAL:
		(*environment.flags)++;
		break;
	case FLAG_BOOLEAN:
		*environment.flags = active_flag;
		break;
	default:
		printf("Unimplemented flash scheme %d\n",
			environment.flag_scheme);
		return -1;
	}

	if (spinor_erase(next_env_sector, size/512)) {
		printf("erase spinor fail\n");
		return -1;
	}

	if (spinor_write(next_env_sector, size/512, buf)) {
		printf("write spinor fail\n");
		return -1;
	}

	if (environment.flag_scheme == FLAG_BOOLEAN) {
		/* Have to set obsolete flag */
		int offset = current_env_sector * 512 +
			offsetof(struct env_image_redundant, flags);

		flash_flag_obsolete(offset);
	}

	return 0;
}

static int flash_io(int mode)
{
	void *buf;
	size_t size;
	int rc;

	if (mode == O_RDWR) {
		buf = environment.image;
		size = cur_envsize;
		rc = flash_env_write(buf, size);
	} else {
		buf = environment.image;
		size = cur_envsize;
		rc = flash_env_read(buf, size);
	}

	return rc;
}

/*
 * Prevent confusion if running from erased flash memory
 */
int fw_env_open(void)
{
	int crc0, crc0_ok;
	unsigned char flag0;
	void *addr0 = NULL;

	int crc1, crc1_ok;
	unsigned char flag1;
	void *addr1 = NULL;

	int ret;

	if (flash_env_init()){
		printf("env init fail\n");
		return -1;
	}

	struct env_image_single *single;
	struct env_image_redundant *redundant;

	addr0 = malloc(cur_envsize);
	if (addr0 == NULL) {
		printf("Not enough memory for environment (%ld bytes)\n",
				cur_envsize);
		ret = -1;
		goto open_cleanup;
	}


	/* read environment from FLASH to local buffer */
	environment.image = addr0;

	if (have_redund_env) {
		redundant = addr0;
		environment.crc = &redundant->crc;
		environment.flags = &redundant->flags;
		environment.data = redundant->data;
	} else {
		single = addr0;
		environment.crc = &single->crc;
		environment.flags = NULL;
		environment.data = single->data;
	}

	usable_envsize = cur_envsize - sizeof(uint32_t);
	if (have_redund_env)
		usable_envsize -= sizeof(char);

	dev_current = 0;
	if (flash_io(O_RDONLY)) {
		ret = -1;
		goto open_cleanup;
	}

#ifdef CFG_USE_DCACHE
	dcache_enable();
#endif
	crc0 = crc32(0, (uint8_t *)environment.data, ENV_SIZE);
#ifdef CFG_USE_DCACHE
	dcache_disable();
#endif

	crc0_ok = (crc0 == *environment.crc);
	if (!have_redund_env) {
		if (!crc0_ok) {
			printf("Warning: Bad CRC\n");
			return -1;
		}
	} else {
		flag0 = *environment.flags;

		dev_current = 1;
		addr1 = malloc(cur_envsize);
		if (addr1 == NULL) {
			printf("Not enough memory for environment (%ld bytes)\n",
					cur_envsize);
			ret = -1;
			goto open_cleanup;
		}
		redundant = addr1;

		/*
		 * have to set environment.image for flash_read(), careful -
		 * other pointers in environment still point inside addr0
		 */
		environment.image = addr1;
		if (flash_io(O_RDONLY)) {
			ret = -1;
			goto open_cleanup;
		}

		/* Check flag scheme compatibility */
		/* Hardcode for nor */
		environment.flag_scheme = FLAG_INCREMENTAL;

#ifdef CFG_USE_DCACHE
		dcache_enable();
#endif
		crc1 = crc32(0, (uint8_t *)redundant->data, ENV_SIZE);
#ifdef CFG_USE_DCACHE
		dcache_disable();
#endif

		crc1_ok = (crc1 == redundant->crc);
		flag1 = redundant->flags;

		printf("crc0_ok:%d, crc1_ok:%d flag0:%d flag1:%d\n", crc0_ok, crc1_ok, flag0, flag1);
		if (crc0_ok && !crc1_ok) {
			dev_current = 0;
		} else if (!crc0_ok && crc1_ok) {
			dev_current = 1;
		} else if (!crc0_ok && !crc1_ok) {
			printf("Warning: Bad CRC\n");
			return -1;
		} else {
			switch (environment.flag_scheme) {
				case FLAG_BOOLEAN:
					if (flag0 == active_flag &&
							flag1 == obsolete_flag) {
						dev_current = 0;
					} else if (flag0 == obsolete_flag &&
							flag1 == active_flag) {
						dev_current = 1;
					} else if (flag0 == flag1) {
						dev_current = 0;
					} else if (flag0 == 0xFF) {
						dev_current = 0;
					} else if (flag1 == 0xFF) {
						dev_current = 1;
					} else {
						dev_current = 0;
					}
					break;
				case FLAG_INCREMENTAL:
					if (flag0 == 255 && flag1 == 0)
						dev_current = 1;
					else if ((flag1 == 255 && flag0 == 0) ||
							flag0 >= flag1)
						dev_current = 0;
					else	/* flag1 > flag0 */
						dev_current = 1;
					break;
				default:
					printf("Unknown flag scheme %d\n",
							environment.flag_scheme);
					return -1;
			}
		}

		/*
		 * If we are reading, we don't need the flag and the CRC any
		 * more, if we are writing, we will re-calculate CRC and update
		 * flags before writing out
		 */
		if (dev_current) {
			environment.image = addr1;
			environment.crc = &redundant->crc;
			environment.flags = &redundant->flags;
			environment.data = redundant->data;
			free(addr0);
		} else {
			environment.image = addr0;
			/* Other pointers are already set */
			free(addr1);
		}
		printf("dev_current:%d\n", dev_current);
	}
	return 0;

open_cleanup:
	if (addr0)
		free(addr0);

	if (addr1)
		free(addr1);

	return ret;
}

/*
 * Simply free allocated buffer with environment
 */
int fw_env_close(void)
{
	if (environment.image)
		free(environment.image);

	environment.image = NULL;

	return 0;
}


