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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sound/snd_core.h>
#include <sound/snd_pcm.h>
#include <sound/snd_misc.h>
#include <errno.h>
#include <aw-alsa-lib/pcm_config.h>
#include <sound/pcm_common.h>
#include <hal_cache.h>
#include <semaphore.h>
#include <hal_atomic.h>

//#define XRUN_DEBUG
#ifdef XRUN_DEBUG
#define xrun_debug(fmt, args...) \
	printf(SNDRV_LOG_COLOR_BLUE "[XRUN_DEBUG][%s:%d]" fmt \
		SNDRV_LOG_COLOR_NONE, __FUNCTION__, __LINE__, ##args)
#else
#define xrun_debug(fmt,arg...)
#endif

static void xrun(struct snd_pcm_substream *substream);

static int snd_pcm_hw_constrains_init(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_hw_constrains *constrains = &runtime->hw_constrains;
	int i;

	for (i = SND_PCM_HW_PARAM_FIRST_MASK; i <= SND_PCM_HW_PARAM_LAST_MASK; i++) {
		constrains->intervals[i].mask = 0xff;
	}
	for (i = SND_PCM_HW_PARAM_FIRST_INTERVAL; i <= SND_PCM_HW_PARAM_LAST_INTERVAL; i++) {
		constrains->intervals[i].range.min = 0;
		constrains->intervals[i].range.max = UINT_MAX;
	}

	return 0;
}

static int snd_pcm_open_substream(struct snd_pcm *pcm, int stream)
{
	int ret = 0;
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;

	snd_print("\n");
	substream = pcm->streams[stream];

	runtime = snd_malloc(sizeof(struct snd_pcm_runtime));
	if (!runtime) {
		snd_err("no memory\n");
		return -ENOMEM;
	}

	runtime->tsleep = snd_schd_init();
	if (runtime->tsleep == NULL) {
		snd_err("semaphore tsleep create failed\n");
		ret = -1;
		goto err1;
	}

	runtime->sleep = snd_schd_init();
	if (runtime->sleep == NULL) {
		snd_err("semaphore sleep create failed\n");
		ret = -1;
		goto err1;
	}

	runtime->dsleep = snd_schd_init();
	if (runtime->dsleep == NULL) {
		snd_err("semaphore dsleep create failed\n");
		ret = -1;
		goto err1;
	}

	memset(runtime->dsleep_list, 0, sizeof(runtime->dsleep_list));

	runtime->status = snd_malloc(sizeof(struct snd_pcm_mmap_status));
	if (!runtime->status) {
		snd_err("no memory\n");
		ret = -ENOMEM;
		goto err1;
	}
	runtime->control = snd_malloc(sizeof(struct snd_pcm_mmap_control));
	if (!runtime) {
		snd_err("no memory\n");
		ret = -ENOMEM;
		goto err2;
	}

	runtime->status->state = SNDRV_PCM_STATE_OPEN;
	runtime->pcm_mutex = snd_mutex_init();

	substream->runtime = runtime;
	snd_pcm_hw_constrains_init(substream);

	ret = substream->ops->open(substream);
	if (ret < 0)
		goto err3;

	return ret;
err3:
	snd_mutex_destroy(runtime->pcm_mutex);
	snd_free(runtime->control);
err2:
	snd_free(runtime->status);
err1:
	snd_free(runtime);
	substream->runtime = NULL;
	return ret;
}

static int snd_pcm_release_substream(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;

	snd_print("\n");
	substream = pcm->streams[stream];

	substream->ref_count--;
	if (substream->ref_count > 0)
		return 0;

	if (substream->ops->hw_free &&
		substream->runtime->status->state != SNDRV_PCM_STATE_OPEN)
		substream->ops->hw_free(substream);
	substream->ops->close(substream);
	substream->ref_count = 0;

	runtime = substream->runtime;
	snd_free(runtime->status);
	snd_free(runtime->control);
	snd_mutex_destroy(runtime->pcm_mutex);
	snd_schd_destroy(runtime->tsleep);
	snd_schd_destroy(runtime->sleep);
	snd_schd_destroy(runtime->dsleep);
	snd_free(runtime);

	return 0;
}


static inline void snd_pcm_set_state(struct snd_pcm_substream *substream, int state)
{
	uint32_t __cpsr;

	snd_print("set state:%u\n", state);
        __cpsr = hal_spin_lock_irqsave();
	substream->runtime->status->state = state;
        hal_spin_unlock_irqrestore(__cpsr);
}

static void snd_pcm_dump_params(struct snd_pcm_runtime *runtime)
{
	snd_print("HW params:--------------\n");
	snd_print("format:            %u\n", runtime->format);
	snd_print("rate:              %d\n", runtime->rate);
	snd_print("channels:          %u\n", runtime->channels);
	snd_print("period_size:       %lu\n", runtime->period_size);
	snd_print("periods:           %u\n", runtime->periods);
	snd_print("buffer_size:       %lu\n", runtime->buffer_size);
	snd_print("frame_bits:        %u\n", runtime->frame_bits);
	snd_print("sample_bits:       %u\n", runtime->sample_bits);
	snd_print("SW params:--------------\n");
	snd_print("avail_min:         %lu\n", runtime->control->avail_min);
	snd_print("start_threshold:   %lu\n", runtime->start_threshold);
	snd_print("stop_threshold:    %lu\n", runtime->stop_threshold);
	snd_print("silence_size:      %lu\n", runtime->silence_size);
	snd_print("boundary:          %lu\n", runtime->boundary);
	snd_print("------------------------\n");
	return ;
}


static inline int snd_pcm_running(struct snd_pcm_substream *substream)
{
	return (substream->runtime->status->state == SNDRV_PCM_STATE_RUNNING ||
		(substream->runtime->status->state == SNDRV_PCM_STATE_DRAINING &&
		substream->stream == SNDRV_PCM_STREAM_PLAYBACK));
}

static void snd_pcm_playback_silence(struct snd_pcm_substream *substream, snd_pcm_uframes_t new_hw_ptr)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t frames, ofs, transfer;

	if (runtime->silence_size < runtime->boundary) {
		snd_err("silence_size:%lu, boundary:%lu,"
			" not support silence_size < boundary!\n",
			runtime->silence_size, runtime->boundary);
		return;
	} else {
		if (new_hw_ptr == ULONG_MAX) {
			/* initialization, filled silence data into whole ringbuffer */
			snd_pcm_sframes_t avail = snd_pcm_playback_hw_avail(runtime);

			if (avail > runtime->buffer_size)
				avail = runtime->buffer_size;
			runtime->silence_filled = avail > 0 ? avail : 0;
			runtime->silence_start = (runtime->status->hw_ptr +
						runtime->silence_filled) %
						runtime->boundary;
		} else {
			ofs = runtime->status->hw_ptr;
			frames = new_hw_ptr - ofs;
			/* cross boundary */
			if ((snd_pcm_sframes_t)frames < 0)
				frames += runtime->boundary;
			runtime->silence_filled -= frames;
			if ((snd_pcm_sframes_t)runtime->silence_filled < 0) {
				runtime->silence_filled = 0;
				runtime->silence_start = new_hw_ptr;
			} else {
				runtime->silence_start = ofs;
			}
		}
		frames = runtime->buffer_size - runtime->silence_filled;
	}
	if (frames == 0)
		return;
	ofs = runtime->silence_start % runtime->buffer_size;
	while (frames > 0) {
		char *hwbuf = runtime->dma_addr + frames_to_bytes(runtime, ofs);
		/* seperate twice to fill silence if cross ringbuffer */
		transfer = ofs + frames > runtime->buffer_size ? runtime->buffer_size - ofs : frames;
		if (runtime->access != SND_PCM_ACCESS_RW_INTERLEAVED &&
				runtime->access != SND_PCM_ACCESS_MMAP_INTERLEAVED) {
			snd_err("unsupport access:%d\n", runtime->access);
			return ;
		}

		snd_pcm_format_set_silence(runtime->format, hwbuf, transfer * runtime->channels);
		/* flush cache */
		awos_arch_mems_clean_flush_dcache_region((uintptr_t)hwbuf, frames_to_bytes(runtime, transfer));
		runtime->silence_filled += transfer;
		frames -= transfer;
		ofs = 0;
	}

	return;
}

static int snd_pcm_stop(struct snd_pcm_substream *substream, int state);
static int snd_pcm_update_state(struct snd_pcm_substream *substream,
			struct snd_pcm_runtime *runtime)
{
	snd_pcm_uframes_t avail;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		avail = snd_pcm_playback_avail(runtime);
	else
		avail = snd_pcm_capture_avail(runtime);

	if (runtime->status->state == SNDRV_PCM_STATE_DRAINING) {
		snd_print("\n");
		if (avail >= runtime->buffer_size) {
			snd_pcm_stop(substream, SNDRV_PCM_STATE_SETUP);
			return -EPIPE;
		}
	} else {
		snd_print("\n");
		/*
		 * available data larger than stop_thread(buffer_size)
		 * Playback: means pcm data in ringbuffer
		 * Capture:  pcm data full of ringbuffer
		 */
		if (avail >= runtime->stop_threshold) {
			xrun(substream);
			return -EPIPE;
		}
	}
	if (runtime->twake) {
		if (avail >= runtime->twake)
			snd_schd_wakeup(runtime->tsleep);
	} else if (avail >= runtime->control->avail_min) {
		snd_schd_wakeup(runtime->sleep);
	}

	return 0;
}

static int snd_pcm_update_hw_ptr0(struct snd_pcm_substream *substream,
					unsigned int in_interrupt)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t pos;
	snd_pcm_uframes_t old_hw_ptr, new_hw_ptr, hw_base;
	snd_pcm_sframes_t delta;

	snd_print("\n");
	old_hw_ptr = runtime->status->hw_ptr;
	pos = substream->ops->pointer(substream);

	if (pos >= runtime->buffer_size) {
		snd_err("invalid position: pos = %ld, buffer size = %ld, period size = %ld\n",
			pos, runtime->buffer_size, runtime->period_size);
		pos = 0;
	}
	xrun_debug("hw_ptr:0x%lx, appl_ptr:0x%lx, pos:0x%lx\n",
			runtime->status->hw_ptr, runtime->control->appl_ptr, pos);
	/* frame align */
	pos -= pos % runtime->min_align;
	hw_base = runtime->hw_ptr_base;
	new_hw_ptr = hw_base + pos;

	/*
	 * new_hw_ptr might be lower than old_hw_ptr in case
	 * when pointer cross the end of the ring buffer
	 */
	xrun_debug("in_interrupt:%u, new_hw_ptr:0x%lx, old_hw_ptr:0x%lx, hw_base:0x%lx\n",
			in_interrupt, new_hw_ptr, old_hw_ptr, hw_base);
	if (new_hw_ptr < old_hw_ptr) {
		hw_base += runtime->buffer_size;
		if (hw_base >= runtime->boundary) {
			hw_base = 0;
		}
		new_hw_ptr = hw_base + pos;
	}
	xrun_debug("new_hw_ptr:0x%lx, old_hw_ptr:0x%lx, hw_base:0x%lx\n", new_hw_ptr, old_hw_ptr, hw_base);
__delta:
	delta = new_hw_ptr - old_hw_ptr;

	/* it means cross the end of boundary */
	if (delta < 0)
		delta += runtime->boundary;
	if (runtime->no_period_wakeup) {
		/* usually period wakeup(10s) */
	}

	/* something must be really wrong */
	if (delta >= runtime->buffer_size + runtime->period_size) {
		snd_err("Unexpected hw_ptr, stream:%d, pos:%ld, new_hw_ptr:%ld,old_hw_ptr:%ld\n",
			substream->stream, (long)pos, (long)new_hw_ptr, (long)old_hw_ptr);
		return 0;
	}

	if (delta > runtime->period_size + runtime->period_size / 2) {
		xrun_debug("Lost interrupts? stream:%d, pos:%ld, new_hw_ptr:%ld,old_hw_ptr:%ld\n",
			substream->stream, (long)pos, (long)new_hw_ptr, (long)old_hw_ptr);
	}

	xrun_debug("\n");
	if (runtime->status->hw_ptr == new_hw_ptr)
		return 0;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK &&
		runtime->silence_size > 0)
		snd_pcm_playback_silence(substream, new_hw_ptr);

	runtime->hw_ptr_base = hw_base;
	runtime->status->hw_ptr = new_hw_ptr;
	xrun_debug("hw_ptr_base:0x%lx, hw_ptr:0x%lx\n", hw_base, new_hw_ptr);

	return snd_pcm_update_state(substream, runtime);
}


/* CAUTION: call int with irq disabled */
static int snd_pcm_update_hw_ptr(struct snd_pcm_substream *substream)
{
	return snd_pcm_update_hw_ptr0(substream, 0);
}

static int snd_pcm_paused(struct snd_pcm_substream *substream, int push)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	snd_print("\n");
	if (push) {
		if (runtime->status->state != SNDRV_PCM_STATE_RUNNING) {
			snd_err("unsupport state transform.\n");
			return -EBADFD;
		}
		snd_pcm_update_hw_ptr(substream);
		substream->ops->trigger(substream, SNDRV_PCM_TRIGGER_PAUSE_PUSH);
		runtime->status->state = SNDRV_PCM_STATE_PAUSED;
		/* ensure perform without irq */
		snd_schd_wakeup(runtime->sleep);
		snd_schd_wakeup(runtime->tsleep);
	} else {
		if (runtime->status->state != SNDRV_PCM_STATE_PAUSED) {
			snd_err("unsupport state transform.\n");
			return -EBADFD;
		}
		substream->ops->trigger(substream, SNDRV_PCM_TRIGGER_PAUSE_RELEASE);
		runtime->status->state = SNDRV_PCM_STATE_RUNNING;
	}

	return 0;
}



static int snd_pcm_do_reset(struct snd_pcm_substream *substream, int state)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	uint32_t __cpsr;

        __cpsr = hal_spin_lock_irqsave();
	if (snd_pcm_running(substream) &&
		snd_pcm_update_hw_ptr(substream) >= 0) {
		runtime->status->hw_ptr %= runtime->buffer_size;
	} else {
		runtime->status->hw_ptr = 0;
	}
        hal_spin_unlock_irqrestore(__cpsr);

	runtime->hw_ptr_base = 0;
	/* hw_ptr_interrupt 中断处理中的update_hw_ptr0会用到?*/

	return 0;
}

/* can't lock in this function */
static int snd_pcm_do_start(struct snd_pcm_substream *substream, int state)
{
	int ret = 0;
	struct snd_pcm_runtime *runtime = substream->runtime;

	snd_print("\n");
	if (runtime->status->state != SNDRV_PCM_STATE_PREPARED) {
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK &&
		!snd_pcm_playback_data(substream))
		return -EPIPE;
	snd_print("\n");
	if (substream->ops->trigger) {
		ret = substream->ops->trigger(substream, SNDRV_PCM_TRIGGER_START);
		if (ret < 0) {
			substream->ops->trigger(substream, SNDRV_PCM_TRIGGER_STOP);
			snd_err("trigger failed.\n");
			return ret;
		}
	}
	/* can't lock */
	/*snd_pcm_set_state(substream, state);*/
	substream->runtime->status->state = state;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK &&
		runtime->silence_size > 0)
		snd_pcm_playback_silence(substream, ULONG_MAX);
	/* timer start */

	return ret;
}

/* can't lock in this function */
static int snd_pcm_stop(struct snd_pcm_substream *substream, int state)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	if (runtime->status->state == SNDRV_PCM_STATE_OPEN) {
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}
	if (snd_pcm_running(substream))
		substream->ops->trigger(substream, SNDRV_PCM_TRIGGER_STOP);
	if (runtime->status->state != state) {
		/* can't lock */
		/*snd_pcm_set_state(substream, state);*/
		substream->runtime->status->state = state;
	}
	snd_schd_wakeup(runtime->sleep);
	snd_schd_wakeup(runtime->tsleep);

	return 0;
}

static void xrun(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	runtime->xrun_cnt++;
	snd_err("%s occure!!! xrun_cnt=%u\n",
			substream->stream == SNDRV_PCM_STREAM_PLAYBACK ?
			"Underrun" : "Overrun",
			runtime->xrun_cnt);
	snd_pcm_stop(substream, SNDRV_PCM_STATE_XRUN);
}

/*TODO*/
static void snd_schd_wakeup_all(struct snd_pcm_runtime *runtime)
{
	int i;

	snd_schd_wakeup(runtime->dsleep);
	for (i = 0; i < ARRAY_SIZE(runtime->dsleep_list); i++) {
		if (runtime->dsleep_list[i] != NULL)
			snd_schd_wakeup(runtime->dsleep_list[i]);
	}
}

void snd_pcm_period_elapsed(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime;
	uint32_t __cpsr;

	snd_print("\n");
	runtime = substream->runtime;
        __cpsr = hal_spin_lock_irqsave();
	if (!snd_pcm_running(substream) ||
		snd_pcm_update_hw_ptr0(substream, 1) < 0) {
		goto _end;
	}
	/* TODO:timer interrupt for dmix */
	snd_schd_wakeup_all(runtime);
_end:
	/* kill_fasync(&runtime->fasync, SIGIO, POLL_IN) */
        hal_spin_unlock_irqrestore(__cpsr);
	return;
}

static int wait_for_avail(struct snd_pcm_substream *substream,
				snd_pcm_uframes_t *availp)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int is_playback = substream->stream == SNDRV_PCM_STREAM_PLAYBACK;
	int err = 0;
	snd_pcm_uframes_t avail = 0;
	long wait_time, tout = 0;
	int ret = 0;
	uint32_t __cpsr;

	/* usually period wakeup(10s) */
	if (runtime->no_period_wakeup) {
		wait_time = LONG_MAX;
	} else {
		/* 10ms at lease */
		wait_time = 10;
		if (runtime->rate) {
			long t = runtime->period_size * 2 / runtime->rate;
			wait_time = max(t, wait_time);
		}
	}
	for (;;) {
		/* TODO: break by signal */
		if (is_playback) {
			avail = snd_pcm_playback_avail(runtime);
		} else {
			avail = snd_pcm_capture_avail(runtime);
		}
		if (avail >= runtime->twake)
			break;
		snd_lock_debug("\n");
		/* lock by write/read process */
		tout = snd_schd_timeout(runtime->tsleep, wait_time*100);
		snd_lock_debug("\n");
		switch (runtime->status->state) {
		case SNDRV_PCM_STATE_SUSPENDED:
			err = -ESTRPIPE;
			goto _endloop;
		case SNDRV_PCM_STATE_XRUN:
			err = -EPIPE;
			goto _endloop;
		case SNDRV_PCM_STATE_DRAINING:
			if (is_playback)
				err = -EPIPE;
			else
				avail = 0;
			goto _endloop;
		case SNDRV_PCM_STATE_OPEN:
		case SNDRV_PCM_STATE_SETUP:
		case SNDRV_PCM_STATE_DISCONNECTED:
			snd_err("unsupport state transform.\n");
			err = -EBADFD;
			goto _endloop;
		case SNDRV_PCM_STATE_PAUSED:
			continue;
		default:
		    break;
		}
		if (tout < 0) {
			snd_err("%s write error (DMA transfer data error?) tout %ld\n",
				is_playback ? "Playback" : "Capture", tout);
			err = -EIO;
			break;
		}
	}
_endloop:
	*availp = avail;
	return err;
}

/*
 * hwoff:  appl_ptr % buffer_size
 * data:   app data ptr
 * off:    app data offset
 * frames: write frames data
 *
 */
static int snd_pcm_lib_write_transfer(struct snd_pcm_substream *substream,
				unsigned int hwoff,
				unsigned long data, unsigned int off,
				snd_pcm_uframes_t frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret = 0;
	char *buf = (char *)data + frames_to_bytes(runtime, off);

	snd_print("\n");
	if (substream->ops->copy) {
		if ((ret = substream->ops->copy(substream, -1, hwoff, buf, frames)) < 0)
			return ret;
	} else {
		char *hwbuf = runtime->dma_addr + frames_to_bytes(runtime, hwoff);
		snd_print("hwbuf:%p, bytes:%lu, frames:%lu\n",
				hwbuf,
				frames_to_bytes(runtime, frames),
				frames);
		memcpy(hwbuf, buf, frames_to_bytes(runtime, frames));
		snd_print("start:0x%x, end:0x%x, bytes:0x%x\n",
			hwbuf, hwbuf+frames_to_bytes(runtime, frames),
			frames_to_bytes(runtime, frames));
		/* flush cache */
		awos_arch_mems_clean_flush_dcache_region((uintptr_t)hwbuf, frames_to_bytes(runtime, frames));
	}
	return 0;
}

typedef int (*transfer_f)(struct snd_pcm_substream *substream, unsigned int hwoff,
			unsigned long data, unsigned int off,
			snd_pcm_uframes_t size);

static snd_pcm_sframes_t snd_pcm_lib_write(struct snd_pcm_substream *substream,
					unsigned long data,
					snd_pcm_uframes_t size,
					transfer_f transfer)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t xfer = 0;
	snd_pcm_uframes_t offset = 0;
	snd_pcm_uframes_t avail;
	int err = 0;
//	uint32_t __cpsr;

	if (size ==0)
		return 0;

	snd_lock_debug("\n");
//        __cpsr = hal_spin_lock_irqsave();
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_PREPARED:
	case SNDRV_PCM_STATE_RUNNING:
	case SNDRV_PCM_STATE_PAUSED:
		break;
	case SNDRV_PCM_STATE_XRUN:
		err = -EPIPE;
		goto _end_unlock;
	case SNDRV_PCM_STATE_SUSPENDED:
		err = -ESTRPIPE;
		goto _end_unlock;
	default:
		err = -EBADFD;
		snd_err("unsupport state transform.\n");
		goto _end_unlock;
	}
	runtime->twake = runtime->control->avail_min ? : 1;
	if (runtime->status->state == SNDRV_PCM_STATE_RUNNING)
		snd_pcm_update_hw_ptr(substream);
	avail = snd_pcm_playback_avail(runtime);
	while (size > 0) {
		snd_pcm_uframes_t frames , appl_ptr, appl_ofs;
		snd_pcm_uframes_t cont;
		if (!avail) {
			/* ringbuffer is full with data, wait consume */
			runtime->twake = min(size, runtime->twake);
			snd_print("avail=0x%lx(%lu)\n", avail, avail);
			err = wait_for_avail(substream, &avail);
			if (err < 0) {
				goto _end_unlock;
			}
		}
		frames = size > avail ? avail : size;
		/* transfer frame can not cross the end of ring buffer, so separate */
		cont = runtime->buffer_size - runtime->control->appl_ptr % runtime->buffer_size;
		snd_print("frames:%lu, cont:%lu\n", frames, cont);
		if (frames > cont)
			frames = cont;
		appl_ptr = runtime->control->appl_ptr;
		appl_ofs = appl_ptr % runtime->buffer_size;
		snd_print("\n");
//		hal_spin_unlock_irqrestore(__cpsr);
		err = transfer(substream, appl_ofs, data, offset, frames);
		snd_lock_debug("\n");
//		__cpsr = hal_spin_lock_irqsave();
		if (err < 0)
			goto _end_unlock;
		switch (runtime->status->state) {
		case SNDRV_PCM_STATE_XRUN:
			err = -EPIPE;
			goto _end_unlock;
		case SNDRV_PCM_STATE_SUSPENDED:
			err = -ESTRPIPE;
			goto _end_unlock;
		default:
		    break;
		}
		appl_ptr += frames;
		if (appl_ptr >= runtime->boundary)
			appl_ptr -= runtime->boundary;
		runtime->control->appl_ptr = appl_ptr;
		snd_print("appl_ptr:0x%x\n", appl_ptr);

		offset += frames;
		size -= frames;
		xfer += frames;
		avail -= frames;
		/* data in ringbuffer >=  start_threshld(buffer_size), pcm start */
		if (runtime->status->state == SNDRV_PCM_STATE_PREPARED &&
			snd_pcm_playback_hw_avail(runtime) >= (snd_pcm_sframes_t)runtime->start_threshold) {
			err = snd_pcm_do_start(substream, SNDRV_PCM_STATE_RUNNING);
			if (err < 0) {
				goto _end_unlock;
			}
		}
	}

_end_unlock:
	runtime->twake = 0;
	if (xfer > 0 && err >= 0)
		snd_pcm_update_state(substream, runtime);
//	hal_spin_unlock_irqrestore(__cpsr);
	return xfer > 0 ? (snd_pcm_sframes_t)xfer : err;
}



static int snd_pcm_lib_read_transfer(struct snd_pcm_substream *substream,
					unsigned int hwoff,
					unsigned long data, unsigned int off,
					snd_pcm_uframes_t frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;
	char *buf = (char *)data + frames_to_bytes(runtime, off);

	snd_print("\n");
	if (substream->ops->copy) {
		err = substream->ops->copy(substream, -1, hwoff, buf, frames);
		if (err < 0)
			return err;
	} else {
		char *hwbuf = runtime->dma_addr + frames_to_bytes(runtime, hwoff);
		snd_print("hwbuf:%p, bytes:%lu, frames:%lu\n",
				hwbuf,
				frames_to_bytes(runtime, frames),
				frames);
		awos_arch_mems_flush_dcache_region((uintptr_t)hwbuf, frames_to_bytes(runtime, frames));
		memcpy(buf, hwbuf, frames_to_bytes(runtime, frames));
	}

	return 0;
}

static snd_pcm_sframes_t snd_pcm_lib_read(struct snd_pcm_substream *substream,
					unsigned long data,
					snd_pcm_uframes_t size,
					transfer_f transfer)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t xfer = 0;
	snd_pcm_uframes_t offset = 0;
	snd_pcm_uframes_t avail;
	int err = 0;
//	uint32_t __cpsr;

	if (size ==0)
		return 0;

	snd_print("\n");
//        __cpsr = hal_spin_lock_irqsave();
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_PREPARED:
		if (size >= runtime->start_threshold) {
			err = snd_pcm_do_start(substream, SNDRV_PCM_STATE_RUNNING);
			if (err < 0)
				goto _end_unlock;
		}
		break;
	case SNDRV_PCM_STATE_DRAINING:
	case SNDRV_PCM_STATE_RUNNING:
	case SNDRV_PCM_STATE_PAUSED:
		break;
	case SNDRV_PCM_STATE_XRUN:
		err = -EPIPE;
		goto _end_unlock;
	case SNDRV_PCM_STATE_SUSPENDED:
		err = -ESTRPIPE;
		goto _end_unlock;
	default:
		err = -EBADFD;
		snd_err("unsupport state transform.\n");
		goto _end_unlock;
	}

	runtime->twake = runtime->control->avail_min ? : 1;
	if (runtime->status->state == SNDRV_PCM_STATE_RUNNING)
		snd_pcm_update_hw_ptr(substream);
	avail = snd_pcm_capture_avail(runtime);
	while (size > 0) {
		snd_pcm_uframes_t frames , appl_ptr, appl_ofs;
		snd_pcm_uframes_t cont;
		if (!avail) {
			if (runtime->status->state == SNDRV_PCM_STATE_DRAINING) {
				err = snd_pcm_stop(substream, SNDRV_PCM_STATE_SETUP);
				goto _end_unlock;
			}
			/* ringbuffer is empty, wait produce */
			runtime->twake = min(size, runtime->twake);
			snd_print("avail=0x%lx(%lu)\n", avail, avail);
			err = wait_for_avail(substream, &avail);
			snd_print("-----exit wait_for_avail\n");
			if (err < 0)
				goto _end_unlock;
			if (!avail)
				continue;
		}
		frames = size > avail ? avail : size;
		/* transfer frame can not cross the end of ring buffer, so separate */
		cont = runtime->buffer_size - runtime->control->appl_ptr % runtime->buffer_size;
		snd_print("frames:%lu, cont:%lu\n", frames, cont);
		if (frames > cont)
			frames = cont;
		if (!frames) {
			snd_err("frames is 0..\n");
			runtime->twake = 0;
//			hal_spin_unlock_irqrestore(__cpsr);
			return -EINVAL;
		}
		appl_ptr = runtime->control->appl_ptr;
		appl_ofs = appl_ptr % runtime->buffer_size;
		snd_print("\n");
//		hal_spin_unlock_irqrestore(__cpsr);
		err = transfer(substream, appl_ofs, data, offset, frames);
//		__cpsr = hal_spin_lock_irqsave();
		if (err < 0)
			goto _end_unlock;
		switch (runtime->status->state) {
		case SNDRV_PCM_STATE_XRUN:
			err = -EPIPE;
			goto _end_unlock;
		case SNDRV_PCM_STATE_SUSPENDED:
			err = -ESTRPIPE;
			goto _end_unlock;
		default:
		    break;
		}
		appl_ptr += frames;
		if (appl_ptr >= runtime->boundary)
			appl_ptr -= runtime->boundary;
		runtime->control->appl_ptr = appl_ptr;

		offset += frames;
		size -= frames;
		xfer += frames;
		avail -= frames;
	}
_end_unlock:
	runtime->twake = 0;
	if (xfer > 0 && err >= 0)
		snd_pcm_update_state(substream, runtime);
//	hal_spin_unlock_irqrestore(__cpsr);
	return xfer > 0 ? (snd_pcm_sframes_t)xfer : err;
}



/* aw-alsa kernel interface */

int ksnd_card_index(const char *name)
{
	struct snd_card *card;

	card = snd_card_find_by_name(name);
	if (!card)
		return -1;
	return card->num;
}

const char *ksnd_card_name(int index)
{
	struct snd_card *card;

	card = snd_card_find_by_num(index);
	if (!card)
		return NULL;
	return (const char *)card->name;
}

int ksnd_card_num(void)
{
	return snd_card_get_number();
}

int ksnd_pcm_open(int card_num, int device_num, int stream, int mode, void **substream_handle)
{
	int ret;
	struct snd_card *card;
	struct snd_pcm *pcm;
	struct snd_pcm_substream *substream;

	snd_print("\n");
	card = snd_card_find_by_num(card_num);
	if (!card) {
		snd_err("no card %d\n", card_num);
		return -1;
	}
	pcm = snd_card_find_pcm(card, device_num);
	if (!pcm) {
		snd_err("no pcm deice %d\n", device_num);
		return -1;
	}

	if (mode & SND_PCM_APPEND) {
		snd_mutex_lock(pcm->open_mutex);
		substream = pcm->streams[stream];
		substream->ref_count++;
		*substream_handle = substream;
		snd_mutex_unlock(pcm->open_mutex);
		return 0;
	}

	/* detect already open or not */
	if (pcm->streams[stream]->ref_count != 0)
		return -EBUSY;

	snd_mutex_lock(pcm->open_mutex);
	ret = snd_pcm_open_substream(pcm, stream);
	if (ret < 0) {
		snd_err("open substream failed,%d\n", ret);
		snd_mutex_unlock(pcm->open_mutex);
		return ret;
	}
	substream = pcm->streams[stream];
	substream->ref_count++;
	*substream_handle = substream;
	snd_mutex_unlock(pcm->open_mutex);

	return ret;
}

int ksnd_pcm_release(int card_num, int device_num, int stream)
{
	int ret = 0;
	struct snd_card *card;
	struct snd_pcm *pcm;

	snd_print("\n");
	card = snd_card_find_by_num(card_num);
	if (!card) {
		snd_err("no card %d\n", card_num);
		return -1;
	}
	pcm = snd_card_find_pcm(card, device_num);
	if (!pcm) {
		snd_err("no pcm deice %d\n", device_num);
		return -1;
	}
	snd_mutex_lock(pcm->open_mutex);
	snd_pcm_release_substream(pcm, stream);
	snd_mutex_unlock(pcm->open_mutex);

	return ret;
}

int ksnd_pcm_hw_refine(void *substream_handle, void *params_wrapper)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_hw_params *params = params_wrapper;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_hw_constrains *cons = &runtime->hw_constrains;

	snd_print("\n");
	memcpy(params, cons, sizeof(struct snd_pcm_hw_params));

	return 0;
}

#define is_range_of_hw_constrains(param, cons, type) \
((param->intervals[type].range.min >= cons->intervals[type].range.min) && \
(param->intervals[type].range.min <= cons->intervals[type].range.max))

static int snd_hw_constrains_check(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_pcm_hw_constrains *cons = &substream->runtime->hw_constrains;
	int i;

	if (!(params->intervals[SND_PCM_HW_PARAM_FORMAT].mask &
		cons->intervals[SND_PCM_HW_PARAM_FORMAT].mask)) {
		snd_err("hw_params format invalid."
			"params mask:0x%lx, hw_cons mask:0x%lx\n",
			params->intervals[SND_PCM_HW_PARAM_FORMAT].mask,
			cons->intervals[SND_PCM_HW_PARAM_FORMAT].mask);
		return -1;
	}

	params->intervals[SND_PCM_HW_PARAM_FRAME_BITS].range.min =
		params->intervals[SND_PCM_HW_PARAM_SAMPLE_BITS].range.min *
		params->intervals[SND_PCM_HW_PARAM_CHANNELS].range.min;
	snd_info("sample bits:%u, frame bits:%u\n",
		params->intervals[SND_PCM_HW_PARAM_SAMPLE_BITS].range.min,
		params->intervals[SND_PCM_HW_PARAM_FRAME_BITS].range.min);
	/*
	 * update period_time,period_size,period_bytes
	 * priority:
	 * period_time
	 * period_size
	 * period_bytes
	 */
	snd_info("period_time:%u, period_size:%u, period_bytes:%u\n",
		params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min,
		params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min,
		params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min);
	if (params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min != 0) {
		/* period_size */
		params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min =
			params->intervals[SND_PCM_HW_PARAM_RATE].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min / 1000000;
		/* period_bytes */
		params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min *
			params->intervals[SND_PCM_HW_PARAM_FRAME_BITS].range.min / 8;
	} else if (params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min != 0) {
		/* period_time */
		params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min *
			100000 / params->intervals[SND_PCM_HW_PARAM_RATE].range.min;
		/* period_bytes */
		params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min *
			params->intervals[SND_PCM_HW_PARAM_FRAME_BITS].range.min / 8;
	} else {
		/* period_size */
		params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min * 8 /
			params->intervals[SND_PCM_HW_PARAM_FRAME_BITS].range.min;
		/* period_time */
		params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min =
			1000000 * params->intervals[SND_PCM_HW_PARAM_RATE].range.min /
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min;
	}
	snd_info("period_time:%u, period_size:%u, period_bytes:%u\n",
		params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min,
		params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min,
		params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min);

	/*
	 * update periods,buffer_time,buffer_size,buffer_bytes
	 * priority:
	 * buffer_time
	 * buffer_size
	 * periods
	 */
	snd_info("periods:%u, buffer_time:%u, buffer_size:%u, buffer_bytes:%u\n",
		params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min,
		params->intervals[SND_PCM_HW_PARAM_BUFFER_TIME].range.min,
		params->intervals[SND_PCM_HW_PARAM_BUFFER_SIZE].range.min,
		params->intervals[SND_PCM_HW_PARAM_BUFFER_BYTES].range.min);
	if (params->intervals[SND_PCM_HW_PARAM_BUFFER_TIME].range.min != 0) {
		/* periods */
		params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min =
			params->intervals[SND_PCM_HW_PARAM_BUFFER_TIME].range.min /
			params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min;
		/* buffer_time */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_TIME].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min;
		/* buffer_size */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_SIZE].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min;
		/* buffer_bytes */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_BYTES].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min;
	} else if (params->intervals[SND_PCM_HW_PARAM_BUFFER_SIZE].range.min != 0) {
		/* periods */
		params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min =
			params->intervals[SND_PCM_HW_PARAM_BUFFER_SIZE].range.min /
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min;
		/* buffer_time */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_TIME].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min;
		/* buffer_size */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_SIZE].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min;
		/* buffer_bytes */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_BYTES].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min;
	} else {
		/* use periods 4 */
		if (params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min == 0)
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min = 4;
		/* buffer_time */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_TIME].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_TIME].range.min;
		/* buffer_size */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_SIZE].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_SIZE].range.min;
		/* buffer_bytes */
		params->intervals[SND_PCM_HW_PARAM_BUFFER_BYTES].range.min =
			params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min *
			params->intervals[SND_PCM_HW_PARAM_PERIOD_BYTES].range.min;
	}
	snd_info("periods:%u, buffer_time:%u, buffer_size:%u, buffer_bytes:%u\n",
		params->intervals[SND_PCM_HW_PARAM_PERIODS].range.min,
		params->intervals[SND_PCM_HW_PARAM_BUFFER_TIME].range.min,
		params->intervals[SND_PCM_HW_PARAM_BUFFER_SIZE].range.min,
		params->intervals[SND_PCM_HW_PARAM_BUFFER_BYTES].range.min);
	for (i = SND_PCM_HW_PARAM_CHANNELS; i <= SND_PCM_HW_PARAM_LAST_INTERVAL; i++) {
		if (!is_range_of_hw_constrains(params, cons, i)) {
			snd_err("hw_params %d type invalid."
				"params min:%lu,max:%lu, hw_cons min:%lu,max:%lu\n",
				i, params->intervals[i].range.min,
				params->intervals[i].range.max,
				cons->intervals[i].range.min,
				cons->intervals[i].range.max);
			return -1;
		}
	}

	return 0;
}

int ksnd_pcm_hw_params(void *substream_handle, void *params_wrapper)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_hw_params *params = params_wrapper;
	unsigned int bits;
	uint32_t __cpsr;

	snd_lock_debug("\n");
        __cpsr = hal_spin_lock_irqsave();
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_OPEN:
	case SNDRV_PCM_STATE_SETUP:
	case SNDRV_PCM_STATE_PREPARED:
		break;
	default:
		hal_spin_unlock_irqrestore(__cpsr);
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}
        hal_spin_unlock_irqrestore(__cpsr);

	ret = snd_hw_constrains_check(substream, params);
	if (ret < 0) {
		snd_err("hw params invalid\n");
		goto err1;
	}

	if (substream->ops->hw_params) {
		ret = substream->ops->hw_params(substream, params);
		if (ret < 0)
			goto err1;
	}

	runtime->access = params_access(params);
	runtime->format = params_format(params);
	runtime->rate = params_rate(params);
	runtime->channels = params_channels(params);
	runtime->period_size = params_period_size(params);
	runtime->periods = params_periods(params);
	runtime->buffer_size = params_buffer_size(params);
	runtime->can_paused = params->can_paused;

	bits = snd_pcm_format_physical_width(runtime->format);
	runtime->sample_bits = bits;
	bits *= runtime->channels;
	runtime->frame_bits = bits;
	/* bits should be 8bit align */
	runtime->min_align = bits/8;

	/* default sw params */
	runtime->control->avail_min = runtime->period_size;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		runtime->start_threshold = runtime->buffer_size;
	else
		runtime->start_threshold = 1;
	runtime->stop_threshold = runtime->buffer_size;
	runtime->silence_size = 0;
	runtime->boundary = runtime->buffer_size;
	while (runtime->boundary * 2 <= LONG_MAX - runtime->buffer_size)
		runtime->boundary *= 2;

	snd_pcm_dump_params(runtime);

	snd_pcm_set_state(substream, SNDRV_PCM_STATE_SETUP);

	return 0;
err1:
	snd_pcm_set_state(substream, SNDRV_PCM_STATE_OPEN);
	if (substream->ops->hw_free)
		substream->ops->hw_free(substream);

	return ret;
}

int ksnd_pcm_sw_params(void *substream_handle, void *params_wrapper)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_sw_params *params = params_wrapper;
	uint32_t __cpsr;

	snd_lock_debug("\n");
        __cpsr = hal_spin_lock_irqsave();
	if (runtime->status->state == SNDRV_PCM_STATE_OPEN) {
		hal_spin_unlock_irqrestore(__cpsr);
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}
	hal_spin_unlock_irqrestore(__cpsr);

	if (params->avail_min == 0)
		return -EINVAL;
	if (params->silence_size != 0 &&
		params->silence_size != runtime->boundary) {
		snd_err("silence_size only support 0 or boundary.\n");
		return -EINVAL;
	}

	snd_lock_debug("\n");
        __cpsr = hal_spin_lock_irqsave();
	runtime->control->avail_min = params->avail_min;
	if (params->start_threshold > runtime->buffer_size)
		params->start_threshold = runtime->buffer_size;
	runtime->start_threshold = params->start_threshold;
	runtime->stop_threshold = params->stop_threshold;
	runtime->silence_size = params->silence_size;
	params->boundary = runtime->boundary;
	if (snd_pcm_running(substream)) {
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK &&
			runtime->silence_size > 0)
			snd_pcm_playback_silence(substream, ULONG_MAX);
		ret = snd_pcm_update_state(substream, runtime);
	}
	snd_pcm_dump_params(runtime);
	hal_spin_unlock_irqrestore(__cpsr);

	return ret;
}

int ksnd_pcm_prepare(void *substream_handle)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;

	if (runtime->status->state == SNDRV_PCM_STATE_OPEN ||
		runtime->status->state == SNDRV_PCM_STATE_DISCONNECTED) {
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}
	if (snd_pcm_running(substream))
		return -EBUSY;

	ret = substream->ops->prepare(substream);
	if (ret < 0)
		return ret;
	ret = snd_pcm_do_reset(substream, 0);
	if (ret < 0)
		return ret;

	runtime->control->appl_ptr = runtime->status->hw_ptr;
	snd_pcm_set_state(substream, SNDRV_PCM_STATE_PREPARED);
	return ret;
}

int ksnd_pcm_hw_free(void *substream_handle)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	uint32_t __cpsr;

        __cpsr = hal_spin_lock_irqsave();
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_SETUP:
	case SNDRV_PCM_STATE_PREPARED:
		break;
	default:
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}
        hal_spin_unlock_irqrestore(__cpsr);
	if (substream->ops->hw_free)
		ret = substream->ops->hw_free(substream);
	snd_pcm_set_state(substream, SNDRV_PCM_STATE_OPEN);

	return ret;
}

snd_pcm_sframes_t ksnd_pcm_writei(void *substream_handle, const void *buffer, snd_pcm_uframes_t size)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;

	if (runtime->status->state == SNDRV_PCM_STATE_OPEN) {
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}

	snd_print("\n");
	return snd_pcm_lib_write(substream, (unsigned long)buffer,
				size, snd_pcm_lib_write_transfer);
}

snd_pcm_sframes_t ksnd_pcm_readi(void *substream_handle, void *buffer, snd_pcm_uframes_t size)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;

	if (runtime->status->state == SNDRV_PCM_STATE_OPEN) {
		snd_err("unsupport state transform.\n");
		return -EBADFD;
	}

	snd_print("\n");
	return snd_pcm_lib_read(substream, (unsigned long)buffer,
				size, snd_pcm_lib_read_transfer);
}

int ksnd_pcm_start(void *substream_handle)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	uint32_t __cpsr;

        __cpsr = hal_spin_lock_irqsave();
	ret = snd_pcm_do_start(substream, SNDRV_PCM_STATE_RUNNING);
        hal_spin_unlock_irqrestore(__cpsr);

	return ret;
}

int ksnd_pcm_drop(void *substream_handle)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	uint32_t __cpsr;

	snd_print("\n");
	if (runtime->status->state == SNDRV_PCM_STATE_OPEN ||
            runtime->status->state == SNDRV_PCM_STATE_DISCONNECTED ||
            runtime->status->state == SNDRV_PCM_STATE_SUSPENDED) {
		snd_err("unsupport state transform.\n");
                return -EBADFD;
	}
	snd_lock_debug("\n");
        __cpsr = hal_spin_lock_irqsave();
	if (runtime->status->state == SNDRV_PCM_STATE_PAUSED)
		snd_pcm_paused(substream, 0);
	ret = snd_pcm_stop(substream, SNDRV_PCM_STATE_SETUP);
        hal_spin_unlock_irqrestore(__cpsr);

	return ret;
}

int ksnd_pcm_pause(void *substream_handle, int enable)
{
	int ret;
	uint32_t __cpsr;

	snd_lock_debug("\n");
        __cpsr = hal_spin_lock_irqsave();
	ret = snd_pcm_paused(substream_handle, enable);
        hal_spin_unlock_irqrestore(__cpsr);

	return ret;
}

int ksnd_pcm_drain(void *substream_handle)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	uint32_t __cpsr;

	snd_lock_debug("\n");
        __cpsr = hal_spin_lock_irqsave();
	if (runtime->status->state == SNDRV_PCM_STATE_PAUSED)
		snd_pcm_paused(substream, 0);
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_OPEN:
	case SNDRV_PCM_STATE_DISCONNECTED:
	case SNDRV_PCM_STATE_SUSPENDED:
		snd_err("unsupport state transform.\n");
		hal_spin_unlock_irqrestore(__cpsr);
		return -EBADFD;
	default:
	    break;
	}
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		switch (runtime->status->state) {
		case SNDRV_PCM_STATE_PREPARED:
			if (!snd_pcm_playback_empty(substream)) {
				/* ringbuffer has data */
				snd_pcm_do_start(substream, SNDRV_PCM_STATE_DRAINING);
			} else {
				/* ringbuffer empty */
				/* can't lock */
				runtime->status->state = SNDRV_PCM_STATE_SETUP;
			}
			break;
		case SNDRV_PCM_STATE_RUNNING:
			runtime->status->state = SNDRV_PCM_STATE_DRAINING;
			break;
		case SNDRV_PCM_STATE_XRUN:
			runtime->status->state = SNDRV_PCM_STATE_SETUP;
			break;
		default:
		    break;
		}
	} else {
		if (runtime->status->state == SNDRV_PCM_STATE_RUNNING) {
			int new_state = snd_pcm_capture_avail(runtime) > 0 ?
				SNDRV_PCM_STATE_DRAINING : SNDRV_PCM_STATE_SETUP;
			ret = snd_pcm_stop(substream, new_state);
		}
	}

	for (;;) {
		long tout = 10;
		snd_print("during drain process, state:%u\n", runtime->status->state);
		if (substream->stream != SNDRV_PCM_STREAM_PLAYBACK)
			break;
		if (runtime->status->state != SNDRV_PCM_STATE_DRAINING )
			break;
		if (runtime->rate) {
			long t = runtime->period_size * 2 / runtime->rate;
			tout = max(t, tout);
		}
		hal_spin_unlock_irqrestore(__cpsr);
		tout = snd_schd_timeout(runtime->sleep, tout*100);
		snd_lock_debug("\n");
	        __cpsr = hal_spin_lock_irqsave();
		snd_print("after snd_schd_timeout, tout=%ld\n", tout);
		if (tout != 0) {
			snd_err("playback drain error (DMA transfer error?) tout %ld\n", tout);
			ret = -EIO;
			break;
		}
	}

	hal_spin_unlock_irqrestore(__cpsr);

	return ret;
}

static int snd_pcm_hw_sync(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret = 0;
	uint32_t __cpsr;

        __cpsr = hal_spin_lock_irqsave();
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_DRAINING:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			goto __badfd;
		/* Fall through */
	case SNDRV_PCM_STATE_RUNNING:
		if ((ret = snd_pcm_update_hw_ptr(substream)) < 0)
			break;
		/* Fall through */
	case SNDRV_PCM_STATE_PREPARED:
		ret = 0;
		break;
	case SNDRV_PCM_STATE_SUSPENDED:
		ret = -ESTRPIPE;
		break;
	case SNDRV_PCM_STATE_XRUN:
		ret = -EPIPE;
		break;
	default:
	      __badfd:
		ret = -EBADFD;
		break;
	}
        hal_spin_unlock_irqrestore(__cpsr);

	return ret;
}

int ksnd_pcm_sync_ptr(void *substream_handle,
			void *status_wrap,
			void *control_wrap,
			unsigned int flags)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	volatile struct snd_pcm_mmap_status *status = status_wrap;
	volatile struct snd_pcm_mmap_control *control = control_wrap;
	uint32_t __cpsr;

	snd_print("\n");
	if (!substream) {
		snd_err("substream is NULL\n");
		return -EINVAL;
	}
	runtime = substream->runtime;
	if (!runtime) {
		snd_err("runtime is NULL\n");
		return -EINVAL;
	}

	if (flags & SNDRV_PCM_SYNC_PTR_HWSYNC) {
		ret = snd_pcm_hw_sync(substream);
		if (ret < 0)
			return ret;
	}

        __cpsr = hal_spin_lock_irqsave();
	if (!(flags & SNDRV_PCM_SYNC_PTR_APPL))
		runtime->control->appl_ptr = control->appl_ptr;
	else
		control->appl_ptr = runtime->control->appl_ptr;
	if (!(flags & SNDRV_PCM_SYNC_PTR_AVAIL_MIN))
		runtime->control->avail_min = control->avail_min;
	else
		control->avail_min = runtime->control->avail_min;

	status->state = runtime->status->state;
	status->hw_ptr = runtime->status->hw_ptr;
        hal_spin_unlock_irqrestore(__cpsr);

	return ret;
}

int ksnd_pcm_delay(void *substream_handle, void *delay_wrap)
{
	int ret = 0;
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_sframes_t n = 0, *delay = delay_wrap;
	uint32_t __cpsr;

        __cpsr = hal_spin_lock_irqsave();
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_DRAINING:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			goto __badfd;
		/* Fall through */
	case SNDRV_PCM_STATE_RUNNING:
		if ((ret = snd_pcm_update_hw_ptr(substream)) < 0)
			break;
		/* Fall through */
	case SNDRV_PCM_STATE_PREPARED:
	case SNDRV_PCM_STATE_SUSPENDED:
		ret = 0;
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			n = snd_pcm_playback_hw_avail(runtime);
		else
			n = snd_pcm_capture_avail(runtime);
		break;
	case SNDRV_PCM_STATE_XRUN:
		ret = -EPIPE;
		break;
	default:
__badfd:
		ret = -EBADFD;
		break;
	}

        hal_spin_unlock_irqrestore(__cpsr);
	if (!ret)
		*delay = n;

	return ret;
}

int ksnd_pcm_channel_info(void *substream_handle, void *info_wrapper)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_channel_info_t *info = info_wrapper;
	int width;

	if (!runtime->dma_buffer_p->addr) {
		snd_err("dma_buffer_p->addr is NULL");
		return -1;
	}
	if (!runtime->dma_addr) {
		snd_err("dma_addr is NULL");
		return -1;
	}
	info->addr = runtime->dma_addr;

	width = snd_pcm_format_physical_width(runtime->format);
	switch (runtime->access) {
	case SND_PCM_ACCESS_MMAP_INTERLEAVED:
	case SND_PCM_ACCESS_RW_INTERLEAVED:
		info->first = info->channel * width;
		info->step = runtime->channels * width;
		break;
	case SND_PCM_ACCESS_MMAP_NONINTERLEAVED:
	case SND_PCM_ACCESS_RW_NONINTERLEAVED:
	{
		size_t size = runtime->dma_bytes / runtime->channels;
		info->first = info->channel * size * 8;
		info->step = width;
		break;
	}
	default:
		snd_err("not supported access type\n");
		return -EINVAL;
	}
	return 0;
}

void ksnd_pcm_hw_mmap_dcache_update(void *substream_handle, snd_pcm_uframes_t offset,
		snd_pcm_uframes_t size)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;

	char *hwbuf = runtime->dma_addr + frames_to_bytes(runtime, offset);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		awos_arch_mems_clean_flush_dcache_region((uintptr_t)hwbuf, frames_to_bytes(runtime, size));
	} else {
		awos_arch_mems_flush_dcache_region((uintptr_t)hwbuf, frames_to_bytes(runtime, size));
	}
}

int ksnd_pcm_wait(void *substream_handle, int index, int timeout)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int is_playback = substream->stream == SNDRV_PCM_STREAM_PLAYBACK;
	long wait_time, tout = 0;
	int wait_infinite = 0;
	int err = 0;

	wait_infinite = timeout < 0 ? 1 : 0;

	/* 10ms at lease */
	wait_time = 10;
	if (runtime->rate) {
		long t = runtime->period_size * 2 / runtime->rate;
		wait_time = max(t, wait_time);
	}

	do {
		if (!index) {
			tout = snd_schd_timeout(runtime->dsleep, wait_time*100);
		} else {
			tout = snd_schd_timeout(runtime->dsleep_list[index-1], wait_time*100);
		}
		if (tout == 0) {
			break;
		} else if (tout < 0) {
			if (wait_infinite) {
				continue;
			} else {
				snd_err("%s write error (DMA transfer data error?)\n",
					is_playback ? "Playback" : "Capture");
				err = -EIO;
				break;
			}
		}
	} while (wait_infinite);

	return err;
}
int ksnd_pcm_rewind(snd_pcm_substream_t *substream, snd_pcm_uframes_t *frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_sframes_t appl_ptr;
	snd_pcm_sframes_t ret;
	snd_pcm_sframes_t hw_avail;
	uint32_t __cpsr;

	if (*frames == 0)
		return 0;

        __cpsr = hal_spin_lock_irqsave();
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_PREPARED:
		if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			break;
		/* Fall through */
	case SNDRV_PCM_STATE_DRAINING:
		if(substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			break;
		/* Fall through */
	case SNDRV_PCM_STATE_RUNNING:
		if (snd_pcm_update_hw_ptr(substream) >= 0)
			break;
		/* Fall through */
	case SNDRV_PCM_STATE_XRUN:
		ret = -EPIPE;
		goto __end;
	case SNDRV_PCM_STATE_SUSPENDED:
		ret = -ESTRPIPE;
		goto __end;
	default:
		ret = -EBADFD;
		goto __end;
	}
	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		hw_avail = snd_pcm_playback_hw_avail(runtime);
	else
		hw_avail = snd_pcm_capture_hw_avail(runtime);
	if (hw_avail <= 0) {
		ret = 0;
		goto __end;
	}
	if (*frames > (snd_pcm_uframes_t)hw_avail)
		*frames = hw_avail;
	appl_ptr = runtime->control->appl_ptr - *frames;
	if (appl_ptr < 0)
		appl_ptr += runtime->boundary;
	runtime->control->appl_ptr = appl_ptr;
	ret = *frames;
 __end:
        hal_spin_unlock_irqrestore(__cpsr);
	return ret;
}


int ksnd_pcm_dsleep_init(void *substream_handle)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int i, index = -1;
	int ret;
	uint32_t __cpsr;

        __cpsr = hal_spin_lock_irqsave();
	for (i = 0; i < ARRAY_SIZE(runtime->dsleep_list); i++) {
		/*printf("i=%d, ptr=%p\n", i, runtime->dsleep_list[i]);*/
		if (!runtime->dsleep_list[i]) {
			runtime->dsleep_list[i] = snd_schd_init();
			index = i + 1;
			break;
		}
	}
        hal_spin_unlock_irqrestore(__cpsr);

	return index;
}

int ksnd_pcm_dsleep_release(void *substream_handle, int index)
{
	struct snd_pcm_substream *substream = substream_handle;
	struct snd_pcm_runtime *runtime = substream->runtime;
	uint32_t __cpsr;

	if (index < 0 || index > ARRAY_SIZE(runtime->dsleep_list))
		return -1;
        __cpsr = hal_spin_lock_irqsave();
	if (runtime->dsleep_list[index - 1] != NULL) {
		snd_schd_destroy(runtime->dsleep_list[index - 1]);
		runtime->dsleep_list[index - 1] = NULL;
	}
        hal_spin_unlock_irqrestore(__cpsr);

	return 0;
}

