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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <aw-alsa-lib/console.h>
#include <aw-alsa-lib/pcm.h>
#include <aw-alsa-lib/control.h>
#include "common.h"
#include "wav_parser.h"

unsigned int g_capture_start = 0;
unsigned int g_capture_loop_enable = 0;
static char *g_pcm_name;
extern unsigned int g_verbose;
extern int aplay(const char *card_name, snd_pcm_format_t format, unsigned int rate,
			unsigned int channels, const char *data, unsigned int datalen);
extern void sunxi_audiocodec_reg_dump(void);
static int pcm_read(snd_pcm_t *handle, const char *data, snd_pcm_uframes_t frames_total, unsigned int frame_bytes)
{
	int ret = 0;
	snd_pcm_sframes_t size;
	snd_pcm_uframes_t frames_loop = 400;
	snd_pcm_uframes_t frames_count = 0;
	snd_pcm_uframes_t frames = 0;
	unsigned int offset = 0;

	while (1) {
		if ((frames_total - frames_count) < frames_loop)
			frames = frames_total - frames_count;
		if (frames == 0)
			frames = frames_loop;
		/*usleep(500000);*/
		/*printf("snd_pcm_readi %ld frames\n", frames);*/
		size = snd_pcm_readi(handle, (void *)(data + offset), frames);
		if (size < 0)
			printf("snd_pcm_readi return %ld\n", size);
		if (size == -EAGAIN) {
			/* retry */
			usleep(10000);
			continue;
		} else if (size == -EPIPE) {
			xrun(handle);
			continue;
		} else if (size == -ESTRPIPE) {

			continue;
		} else if (size < 0) {
			printf("-----snd_pcm_readi failed!!, return %ld\n", size);
			ret = (int)size;
			goto err;
		}
		offset += (size * frame_bytes);
		frames_count += size;
		frames -= size;
		if (frames_total == frames_count)
			break;
		/*printf("frames_count = %ld, frames_total = %ld\n", frames_count, frames_total);*/
	}
err:
	return frames_count > 0 ? frames_count : ret;
}

/*
 * arg0: arecord
 * arg1: card
 * arg2: format
 * arg3: rate
 * arg4: channels
 * arg5: data
 * arg6: len
 */
static int arecord(const char *card_name, snd_pcm_format_t format, unsigned int rate,
			unsigned int channels, const void *data, unsigned int datalen)
{
	int ret = 0;
	snd_pcm_t *handle;
	int mode = 0;
	snd_pcm_hw_params_t *params;
	snd_pcm_sw_params_t *sw_params;
	snd_pcm_uframes_t period_frames = 1024, buffer_frames = 4096;

	printf("dump args:\n");
	printf("card:      %s\n", card_name);
	printf("format:    %u\n", format);
	printf("rate:      %u\n", rate);
	printf("channels:  %u\n", channels);
	printf("data:      %p\n", data);
	printf("datalen:   %u\n", datalen);

	/* open card */
	ret = snd_pcm_open(&handle, card_name, SND_PCM_STREAM_CAPTURE, mode);
	if (ret < 0) {
		printf("audio open error:%d\n", ret);
		return -1;
	}

	ret = set_param(handle, format, rate, channels, period_frames, buffer_frames);
	if (ret < 0)
		goto err1;
	do {
		printf("pcm_read start...\n");
		ret = pcm_read(handle, data,
			snd_pcm_bytes_to_frames(handle, datalen),
			snd_pcm_frames_to_bytes(handle, 1));
		if (ret < 0) {
			printf("capture error:%d\n", ret);
			goto err1;
		}
		if (g_capture_start) {
			g_capture_start = 0;
			break;
		}
	} while (g_capture_loop_enable);

	ret = snd_pcm_drain(handle);
	if (ret < 0)
		printf("stop failed!, return %d\n", ret);

err1:
	/* close card */
	ret = snd_pcm_close(handle);
	if (ret < 0) {
		printf("audio close error:%d\n", ret);
		return ret;
	}

	return 0;
}

static int capture_then_play(audio_mgr_t *audio_mgr)
{
	char *capture_data = NULL;
	unsigned int len = 0;

	if (audio_mgr->capture_duration == 0)
		audio_mgr->capture_duration = 5;

	len = snd_pcm_format_size(audio_mgr->format,
			audio_mgr->capture_duration * audio_mgr->rate * audio_mgr->channels);
	capture_data = malloc(len);
	if (!capture_data) {
		printf("no memory\n");
		return -1;
	}

	do {
		memset(capture_data, 0, len);

		printf("arecord start...\n");
		arecord(g_pcm_name, audio_mgr->format, audio_mgr->rate,
					audio_mgr->channels, capture_data, len);
#if 0
		printf("aplay start...\n");
		/*snd_ctl_set("audiocodec", "LINEOUT volume", 0x1f);*/
                aplay("audiocodec", audio_mgr->format, audio_mgr->rate,
					audio_mgr->channels, capture_data, len);
#endif
	} while (g_capture_loop_enable);

	free(capture_data);
	capture_data = NULL;

	return 0;
}

#define WRITE_WAV_DATA_TO_FS_ONCE

int capture_fs_wav(audio_mgr_t *mgr, const char *path)
{
	unsigned int len = 0;
	int ret = 0, fd = 0;
	wav_header_t header;
	unsigned int written = 0;
	long rest = -1, c = 0;
	char *audiobuf = NULL;
	unsigned int chunk_bytes, frame_bytes = 0;
	struct stat statbuf;
	char *temp_wav_file = NULL;
	unsigned int temp_wav_file_off = 0;

	printf("card:		%s\n", g_pcm_name);
	printf("format:		%d\n", mgr->format);
	printf("rate:		%d\n", mgr->rate);
	printf("channels:	%d\n", mgr->channels);
	printf("period_size:	%ld\n", mgr->period_size);
	printf("buffer_size:	%ld\n", mgr->buffer_size);

	/* open card */
	ret = snd_pcm_open(&mgr->handle, g_pcm_name, SND_PCM_STREAM_CAPTURE, 0);
	if (ret < 0) {
		printf("audio open error:%d\n", ret);
		return -1;
	}

	ret = set_param(mgr->handle, mgr->format, mgr->rate, mgr->channels,
			mgr->period_size, mgr->buffer_size);
	if (ret < 0)
		goto err;

	frame_bytes = snd_pcm_frames_to_bytes(mgr->handle, 1);
	chunk_bytes = snd_pcm_frames_to_bytes(mgr->handle, mgr->period_size);
	if (mgr->capture_duration > 0)
		rest = mgr->capture_duration * snd_pcm_frames_to_bytes(mgr->handle, mgr->rate);

	create_wav(&header, mgr->format, mgr->rate, mgr->channels);
	if (!stat(path, &statbuf)) {
		if (S_ISREG(statbuf.st_mode))
			remove(path);
	}
	fd = open(path, O_RDWR | O_CREAT, 0644);
	if (fd < 0) {
		printf("create wav file failed\n");
		goto err;
	}
	write(fd, &header, sizeof(header));

	audiobuf = malloc(chunk_bytes);
	if (!audiobuf) {
		printf("no memory...\n");
		goto err;
	}

#ifdef WRITE_WAV_DATA_TO_FS_ONCE
	if (rest < 0) {
		printf("please set capture duration..\n");
		goto err;
	}
	temp_wav_file = malloc(rest);
	if (!temp_wav_file) {
		printf("no memory for temp_wav_file\n");
		goto err;
	}
#endif
	while (rest > 0 && !mgr->in_aborting) {
		int f = mgr->period_size;
		if (rest <= chunk_bytes)
			c = rest;
		else
			c = chunk_bytes;
		f = pcm_read(mgr->handle, audiobuf, f, frame_bytes);
		if (f < 0) {
			printf("pcm read error, return %d\n", f);
			break;
		}
#ifdef WRITE_WAV_DATA_TO_FS_ONCE
		memcpy(temp_wav_file + temp_wav_file_off, audiobuf, c);
		temp_wav_file_off += c;
#else
		if (write(fd, audiobuf, c) != c) {
			printf("write file error\n");
			break;
		}
#endif
		if (rest > 0)
			rest -= c;
		written += c;
	}

	ret = snd_pcm_drain(mgr->handle);
	if (ret < 0)
		printf("stop failed!, return %d\n", ret);

err:
	/* close card */
	ret = snd_pcm_close(mgr->handle);
	if (ret < 0) {
		printf("audio close error:%d\n", ret);
		return ret;
	}

#ifdef WRITE_WAV_DATA_TO_FS_ONCE
	printf("please wait...writing data(%u bytes) into %s\n", temp_wav_file_off, path);
	if (write(fd, temp_wav_file, temp_wav_file_off) != temp_wav_file_off) {
		printf("write temp_wav_file failed\n");
		goto err1;
	}
	printf("write finish...\n");
#endif
	resize_wav(&header, written);
	lseek(fd, 0, SEEK_SET);
	write(fd, &header, sizeof(header));

#ifdef WRITE_WAV_DATA_TO_FS_ONCE
err1:
	if (temp_wav_file)
	{
		free(temp_wav_file);
	}
#endif

	if (fd > 0)
		close(fd);
	if (audiobuf)
		free(audiobuf);

	return ret;
}

static void usage(void)
{
	printf("Usage: arecord [option]\n");
	printf("-D,          pcm device name\n");
	printf("-r,          sample rate\n");
	printf("-f,          sample bits\n");
	printf("-c,          channels\n");
	printf("-p,          period size\n");
	printf("-b,          buffer size\n");
	printf("-t,          capture time(second)\n");
	printf("\n");
}

int cmd_arecord(int argc, char ** argv)
{
	int i = 0, c;
	unsigned int bits = 16;
	audio_mgr_t *audio_mgr = NULL;
	g_pcm_name = "default";
	g_verbose = 0;

	audio_mgr = audio_mgr_create();
	if (!audio_mgr)
		return -1;
	optind = 0;
	while ((c = getopt(argc, argv, "D:r:f:c:p:b:t:khlsv")) != -1) {
		switch (c) {
		case 'D':
			g_pcm_name = optarg;
			break;
		case 'r':
			audio_mgr->rate = atoi(optarg);
			break;
		case 'f':
			bits = atoi(optarg);
			break;
		case 'c':
			audio_mgr->channels = atoi(optarg);
			break;
		case 'p':
			audio_mgr->period_size = atoi(optarg);
			break;
		case 'b':
			audio_mgr->buffer_size = atoi(optarg);
			break;
		case 't':
			audio_mgr->capture_duration = atoi(optarg);
			break;
		case 'l':
			g_capture_loop_enable = 1;
			break;
		case 's':
			g_capture_start = 1;
			free(audio_mgr);
			return 0;
		case 'v':
			g_verbose = 1;
			break;
		default:
			usage();
			goto err;
		}
	}

	switch (bits) {
	case 16:
		audio_mgr->format = SND_PCM_FORMAT_S16_LE;
		break;
	case 24:
		audio_mgr->format = SND_PCM_FORMAT_S24_LE;
		break;
	default:
		printf("%u bits not supprot\n", bits);
		return -1;
	}

	if (optind < argc) {
		capture_fs_wav(audio_mgr, argv[optind]);
	} else {
		usage();
//		capture_then_play(audio_mgr);
	}

err:
	audio_mgr_release(audio_mgr);

	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_arecord, arecord, Record voice);
