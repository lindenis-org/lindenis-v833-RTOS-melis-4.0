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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <aw-alsa-lib/pcm_plugin.h>
#include <aw-alsa-lib/plugin/pcm_plug.h>
#include <aw-alsa-lib/pcm_config.h>
#include "pcm_local.h"
#include "pcm_generic.h"

enum snd_pcm_plug_route_policy {
        PLUG_ROUTE_POLICY_NONE,
        PLUG_ROUTE_POLICY_DEFAULT,
        PLUG_ROUTE_POLICY_COPY,
        PLUG_ROUTE_POLICY_AVERAGE,
        PLUG_ROUTE_POLICY_DUP,
};

typedef struct {
        snd_pcm_generic_t gen;
        snd_pcm_t *req_slave;
        snd_pcm_format_t sformat;
        int schannels;
        int srate;
        const char *rate_converter;
        enum snd_pcm_plug_route_policy route_policy;
        snd_pcm_route_ttable_entry_t *ttable;
        int ttable_ok;
        unsigned int tt_ssize, tt_cused, tt_sused;
} snd_pcm_plug_t;

static int snd_pcm_plug_close(snd_pcm_t *pcm)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	int err, result = 0;

	awalsa_debug("\n");
	if (plug->ttable != NULL)
		free(plug->ttable);
	if (plug->rate_converter) {
		plug->rate_converter = NULL;
	}
	assert(plug->gen.slave == plug->req_slave);
	if (plug->gen.close_slave) {
		snd_pcm_unlink_hw_ptr(pcm, plug->req_slave);
		snd_pcm_unlink_appl_ptr(pcm, plug->req_slave);
		err = snd_pcm_close(plug->req_slave);
		if (err < 0)
			result = err;
	}
	free(plug);
	return result;
}

static void snd_pcm_plug_clear(snd_pcm_t *pcm)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	snd_pcm_t *slave = plug->req_slave;
	/* Clear old plugins */
	if (plug->gen.slave != slave) {
		snd_pcm_unlink_hw_ptr(pcm, plug->gen.slave);
		snd_pcm_unlink_appl_ptr(pcm, plug->gen.slave);
		snd_pcm_close(plug->gen.slave);
		plug->gen.slave = slave;
		pcm->fast_ops = slave->fast_ops;
		pcm->fast_op_arg = slave->fast_op_arg;
	}
}

typedef struct {
	snd_pcm_access_t access;
	snd_pcm_format_t format;
	unsigned int channels;
	unsigned int rate;
} snd_pcm_plug_params_t;


int snd_pcm_rate_open(snd_pcm_t **pcmp, const char *name,
		      snd_pcm_format_t sformat, unsigned int srate,
		      const char *converter,
		      snd_pcm_t *slave, int close_slave);
static int snd_pcm_plug_change_rate(snd_pcm_t *pcm, snd_pcm_t **new, snd_pcm_plug_params_t *clt, snd_pcm_plug_params_t *slv)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	int err;

	awalsa_debug("\n");
	if (clt->rate == slv->rate)
		return 0;

	awalsa_debug("\n");
	assert(snd_pcm_format_linear(slv->format));
	err = snd_pcm_rate_open(new, NULL, slv->format, slv->rate, plug->rate_converter,
				plug->gen.slave, plug->gen.slave != plug->req_slave);
	if (err < 0)
		return err;
	awalsa_info("insert rate(%s) plugin success\n", plug->rate_converter);
	slv->access = clt->access;
	slv->rate = clt->rate;
	if (snd_pcm_format_linear(clt->format))
		slv->format = clt->format;
	return 1;
}
int snd_pcm_route_open(snd_pcm_t **pcmp, const char *name,
		       snd_pcm_format_t sformat, int schannels,
		       snd_pcm_route_ttable_entry_t *ttable,
		       unsigned int tt_ssize,
		       unsigned int tt_cused, unsigned int tt_sused,
		       snd_pcm_t *slave, int close_slave);
static int snd_pcm_plug_change_channels(snd_pcm_t *pcm, snd_pcm_t **new, snd_pcm_plug_params_t *clt, snd_pcm_plug_params_t *slv)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	unsigned int tt_ssize, tt_cused, tt_sused;
	snd_pcm_route_ttable_entry_t *ttable;
	int err;

	awalsa_debug("\n");
	if (clt->channels == slv->channels &&
	    (!plug->ttable || plug->ttable_ok))
		return 0;
	if (clt->rate != slv->rate &&
	    clt->channels > slv->channels)
		return 0;
	assert(snd_pcm_format_linear(slv->format));
	tt_ssize = slv->channels;
	tt_cused = clt->channels;
	tt_sused = slv->channels;
	ttable = malloc(tt_cused * tt_sused * sizeof(*ttable));
	if (!ttable) {
		printf("no memory...\n");
		return 0;
	}
	if (plug->ttable) {	/* expand or shrink table */
		unsigned int c = 0, s = 0;
		awalsa_debug("\n");
		for (c = 0; c < tt_cused; c++) {
			for (s = 0; s < tt_sused; s++) {
				snd_pcm_route_ttable_entry_t v;
				if (c >= plug->tt_cused)
					v = 0;
				else if (s >= plug->tt_sused)
					v = 0;
				else
					v = plug->ttable[c * plug->tt_ssize + s];
				ttable[c * tt_ssize + s] = v;
			}
		}
		plug->ttable_ok = 1;
	} else {
		unsigned int k;
		unsigned int c = 0, s = 0;
		enum snd_pcm_plug_route_policy rpolicy = plug->route_policy;
		int n;
		awalsa_debug("\n");
		for (k = 0; k < tt_cused * tt_sused; ++k)
			ttable[k] = 0;
		if (rpolicy == PLUG_ROUTE_POLICY_DEFAULT) {
			rpolicy = PLUG_ROUTE_POLICY_COPY;
			/* it's hack for mono conversion */
			if (clt->channels == 1 || slv->channels == 1)
				rpolicy = PLUG_ROUTE_POLICY_AVERAGE;
		}
		switch (rpolicy) {
		case PLUG_ROUTE_POLICY_AVERAGE:
		case PLUG_ROUTE_POLICY_DUP:
			if (clt->channels > slv->channels) {
				n = clt->channels;
			} else {
				n = slv->channels;
			}
			while (n-- > 0) {
				snd_pcm_route_ttable_entry_t v = SND_PCM_PLUGIN_ROUTE_FULL;
				if (rpolicy == PLUG_ROUTE_POLICY_AVERAGE) {
					if (pcm->stream == SND_PCM_STREAM_PLAYBACK &&
					    clt->channels > slv->channels) {
						int srcs = clt->channels / slv->channels;
						if (s < clt->channels % slv->channels)
							srcs++;
						v /= srcs;
					} else if (pcm->stream == SND_PCM_STREAM_CAPTURE &&
						   slv->channels > clt->channels) {
							int srcs = slv->channels / clt->channels;
						if (s < slv->channels % clt->channels)
							srcs++;
						v /= srcs;
					}
				}
				ttable[c * tt_ssize + s] = v;
				if (++c == clt->channels)
					c = 0;
				if (++s == slv->channels)
					s = 0;
			}
			break;
		case PLUG_ROUTE_POLICY_COPY:
			if (clt->channels < slv->channels) {
				n = clt->channels;
			} else {
				n = slv->channels;
			}
			for (c = 0; (int)c < n; c++)
				ttable[c * tt_ssize + c] = SND_PCM_PLUGIN_ROUTE_FULL;
			break;
		default:
			awalsa_err("Invalid route policy\n");
			break;
		}
	}
	awalsa_debug("\n");
	err = snd_pcm_route_open(new, NULL, slv->format, (int) slv->channels, ttable, tt_ssize, tt_cused, tt_sused, plug->gen.slave, plug->gen.slave != plug->req_slave);
	if (err < 0)
		return err;
	free(ttable);
	ttable = NULL;
	awalsa_info("insert route plugin success\n");
	slv->channels = clt->channels;
	slv->access = clt->access;
	if (snd_pcm_format_linear(clt->format))
		slv->format = clt->format;
	return 1;
}

int snd_pcm_linear_open(snd_pcm_t **pcmp, const char *name, snd_pcm_format_t sformat, snd_pcm_t *slave, int close_slave);
static int snd_pcm_plug_change_format(snd_pcm_t *pcm, snd_pcm_t **new, snd_pcm_plug_params_t *clt, snd_pcm_plug_params_t *slv)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	int err;
	snd_pcm_format_t cfmt;
	int (*f)(snd_pcm_t **_pcm, const char *name, snd_pcm_format_t sformat, snd_pcm_t *slave, int close_slave);

	awalsa_debug("\n");
	/* No conversion is needed */
	if (clt->format == slv->format &&
	    clt->rate == slv->rate &&
	    clt->channels == slv->channels &&
	    (!plug->ttable || plug->ttable_ok))
		return 0;
	awalsa_debug("\n");
	/*
	 * ignore:
	 * SND_PCM_FORMAT_MU_LAW:
	 * SND_PCM_FORMAT_A_LAW:
	 * SND_PCM_FORMAT_IMA_ADPCM:
	 * SND_PCM_FORMAT_FLOAT***
	 * */
	if (snd_pcm_format_linear(slv->format)) {
		/* Conversion is done in another plugin */
		if (clt->rate != slv->rate ||
			clt->channels != slv->channels ||
			(plug->ttable && !plug->ttable_ok))
				return 0;
		cfmt = clt->format;

		awalsa_debug("\n");
		f = snd_pcm_linear_open;
	} else {
		awalsa_err("format=%d, unsupport!\n", slv->format);
		return 0;
	}

	awalsa_debug("\n");
	err = f(new, NULL, slv->format, plug->gen.slave, plug->gen.slave != plug->req_slave);
	if (err < 0)
		return err;
	slv->format = cfmt;
	slv->access = clt->access;

	return 1;
}

static int snd_pcm_plug_change_access(snd_pcm_t *pcm, snd_pcm_t **new, snd_pcm_plug_params_t *clt, snd_pcm_plug_params_t *slv)
{
#if 0
	snd_pcm_plug_t *plug = pcm->private_data;
	int err;
	if (clt->access == slv->access)
		return 0;
	err = snd_pcm_copy_open(new, NULL, plug->gen.slave, plug->gen.slave != plug->req_slave);
	if (err < 0)
		return err;
	slv->access = clt->access;
	return 1;
#else
	awalsa_err("access convert not support now!\n");
	return 0;
#endif
}

static int snd_pcm_plug_insert_plugins(snd_pcm_t *pcm,
				       snd_pcm_plug_params_t *client,
				       snd_pcm_plug_params_t *slave)
{
	awalsa_debug("\n");
	snd_pcm_plug_t *plug = pcm->private_data;
	static int (*const funcs[])(snd_pcm_t *_pcm, snd_pcm_t **new, snd_pcm_plug_params_t *s, snd_pcm_plug_params_t *d) = {
		snd_pcm_plug_change_format,
		snd_pcm_plug_change_channels,
		snd_pcm_plug_change_rate,
		snd_pcm_plug_change_channels,
		snd_pcm_plug_change_format,
		snd_pcm_plug_change_access
	};
	snd_pcm_plug_params_t p = *slave;
	unsigned int k = 0;
	plug->ttable_ok = 0;
	while (client->format != p.format ||
	       client->channels != p.channels ||
	       client->rate != p.rate ||
	       client->access != p.access ||
	       (plug->ttable && !plug->ttable_ok)) {
		snd_pcm_t *new;
		int err;
		if (k >= sizeof(funcs)/sizeof(*funcs)) {
			snd_pcm_plug_clear(pcm);
			return -EINVAL;
		}
		err = funcs[k](pcm, &new, client, &p);
		if (err < 0) {
			snd_pcm_plug_clear(pcm);
			return err;
		}
		if (err) {
			plug->gen.slave = new;
			awalsa_debug("access:%d, format:%d, channels:%d, rate:%d\n",
					p.access, p.format,
					p.channels, p.rate);
		}
		k++;
	}
	return 0;

}

static int snd_pcm_plug_hw_refine(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	snd_pcm_t *slave = plug->req_slave;
	int ret;

	awalsa_debug("\n");
	ret = slave->ops->hw_refine(slave->op_arg, params);
	if (ret < 0)
		return ret;

	return ret;
}

static int snd_pcm_plug_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	snd_pcm_t *slave = plug->req_slave;
	snd_pcm_plug_params_t clt_params, slv_params;
	int err;
	snd_pcm_hw_params_t *slave_params, sparams;

	awalsa_debug("\n");

	snd_pcm_hw_params_alloca(&slave_params);
	memcpy(slave_params, params, sizeof(snd_pcm_hw_params_t));
	memcpy(&sparams, params, sizeof(snd_pcm_hw_params_t));
#if 0
	snd_pcm_hw_param_change(slave_params, SND_PCM_HW_PARAM_ACCESS);
	err = snd_pcm_hw_params_set_access(slave, slave_params, SND_PCM_ACCESS_MMAP_INTERLEAVED);
	if (err < 0) {
		awalsa_err("snd_pcm_hw_params_set_access failed\n");
		return err;
	}
#endif

#if 1
	awalsa_debug("sformat=%d, schannels=%d, srate=%d\n",
			plug->sformat, plug->schannels, plug->srate);
	if (plug->sformat >= 0) {
		snd_pcm_hw_param_change(slave_params, SND_PCM_HW_PARAM_FORMAT);
		snd_pcm_hw_param_change(&sparams, SND_PCM_HW_PARAM_FORMAT);

		snd_pcm_hw_param_change(slave_params, SND_PCM_HW_PARAM_SAMPLE_BITS);
		snd_pcm_hw_param_change(&sparams, SND_PCM_HW_PARAM_SAMPLE_BITS);
		snd_pcm_hw_params_set_format(slave, &sparams, plug->sformat);
	}
	if (plug->schannels > 0) {
		snd_pcm_hw_param_change(slave_params, SND_PCM_HW_PARAM_CHANNELS);
		snd_pcm_hw_param_change(&sparams, SND_PCM_HW_PARAM_CHANNELS);
		_snd_pcm_hw_param_set(&sparams, SND_PCM_HW_PARAM_CHANNELS, plug->schannels);
	}
	if (plug->srate > 0) {
		_snd_pcm_hw_param_set(&sparams, SND_PCM_HW_PARAM_RATE, plug->srate);
	}
#endif


#if 0
	err = snd_pcm_plug_hw_refine_sprepare(pcm, &sparams);
	if (err < 0)
		return err;
	err = snd_pcm_plug_hw_refine_schange(pcm, params, &sparams);
	if (err < 0)
		return err;
	err = snd_pcm_hw_refine_soft(slave, &sparams);
	if (err < 0)
		return err;

	INTERNAL(snd_pcm_hw_params_get_access)(params, &clt_params.access);
	INTERNAL(snd_pcm_hw_params_get_format)(params, &clt_params.format);
	INTERNAL(snd_pcm_hw_params_get_channels)(params, &clt_params.channels);
	INTERNAL(snd_pcm_hw_params_get_rate)(params, &clt_params.rate, 0);

	INTERNAL(snd_pcm_hw_params_get_format)(&sparams, &slv_params.format);
	INTERNAL(snd_pcm_hw_params_get_channels)(&sparams, &slv_params.channels);
	INTERNAL(snd_pcm_hw_params_get_rate)(&sparams, &slv_params.rate, 0);
	snd_pcm_plug_clear(pcm);
	if (!(clt_params.format == slv_params.format &&
	      clt_params.channels == slv_params.channels &&
	      clt_params.rate == slv_params.rate &&
	      !plug->ttable &&
	      snd_pcm_hw_params_test_access(slave, &sparams,
					    clt_params.access) >= 0)) {
		INTERNAL(snd_pcm_hw_params_set_access_first)(slave, &sparams, &slv_params.access);
		err = snd_pcm_plug_insert_plugins(pcm, &clt_params, &slv_params);
		if (err < 0)
			return err;
	}
#endif
	snd_pcm_hw_params_get_access(params, &clt_params.access);
	snd_pcm_hw_params_get_format(params, &clt_params.format);
	snd_pcm_hw_params_get_channels(params, &clt_params.channels);
	snd_pcm_hw_params_get_rate(params, &clt_params.rate, 0);

	snd_pcm_hw_params_get_access(params, &slv_params.access);
	snd_pcm_hw_params_get_format(&sparams, &slv_params.format);
	snd_pcm_hw_params_get_channels(&sparams, &slv_params.channels);
	snd_pcm_hw_params_get_rate(&sparams, &slv_params.rate, 0);
	awalsa_info("cur: access:%d, format:%d, channels:%d, rate:%d\n",
				clt_params.access, clt_params.format,
				clt_params.channels, clt_params.rate);
	awalsa_info("slv: access:%d, format:%d, channels:%d, rate:%d\n",
				slv_params.access, slv_params.format,
				slv_params.channels, slv_params.rate);
	snd_pcm_plug_clear(pcm);
	if (!(clt_params.format == slv_params.format &&
		clt_params.channels == slv_params.channels &&
		clt_params.rate == slv_params.rate &&
		!plug->ttable)) {
		awalsa_debug("\n");
		_snd_pcm_hw_param_set(slave_params, SND_PCM_HW_PARAM_ACCESS, slv_params.access);
		err = snd_pcm_plug_insert_plugins(pcm, &clt_params, &slv_params);
		if (err < 0)
			return err;
	}
	awalsa_debug("\n");
	slave = plug->gen.slave;
	err = _snd_pcm_hw_params_internal(slave, slave_params);
	if (err < 0) {
		awalsa_err("failed to set slave hw params\n");
		return err;
	}

	awalsa_debug("\n");
	snd_pcm_unlink_hw_ptr(pcm, plug->req_slave);
	snd_pcm_unlink_appl_ptr(pcm, plug->req_slave);

	pcm->fast_ops = slave->fast_ops;
	pcm->fast_op_arg = slave->fast_op_arg;
	snd_pcm_link_hw_ptr(pcm, slave);
	snd_pcm_link_appl_ptr(pcm, slave);
	return 0;
}

static int snd_pcm_plug_hw_free(snd_pcm_t *pcm)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	snd_pcm_t *slave = plug->gen.slave;
	int err = snd_pcm_hw_free(slave);
	snd_pcm_plug_clear(pcm);
	return err;
}

static void snd_pcm_plug_dump(snd_pcm_t *pcm)
{
	snd_pcm_plug_t *plug = pcm->private_data;
	printf("Plug PCM: ");
	snd_pcm_dump(plug->gen.slave);
}

static const snd_pcm_ops_t snd_pcm_plug_ops = {
	.close = snd_pcm_plug_close,
	/*.info = snd_pcm_plug_info,*/
	.hw_refine = snd_pcm_plug_hw_refine,
	.hw_params = snd_pcm_plug_hw_params,
	.hw_free = snd_pcm_plug_hw_free,
	.sw_params = snd_pcm_generic_sw_params,
	.channel_info = snd_pcm_generic_channel_info,
	.dump = snd_pcm_plug_dump,
	/*.nonblock = snd_pcm_generic_nonblock,*/
	/*.async = snd_pcm_generic_async,*/
	.mmap = snd_pcm_generic_mmap,
	.munmap = snd_pcm_generic_munmap,
	.query_chmaps = snd_pcm_generic_query_chmaps,
	.get_chmap = snd_pcm_generic_get_chmap,
	.set_chmap = snd_pcm_generic_set_chmap,
};

int snd_pcm_plug_open(snd_pcm_t **pcmp,
			const char *name,
			snd_pcm_format_t sformat, int schannels, int srate,
			const char *rate_converter,
			enum snd_pcm_plug_route_policy route_policy,
			snd_pcm_route_ttable_entry_t *ttable,
			unsigned int tt_ssize,
			unsigned int tt_cused, unsigned int tt_sused,
			snd_pcm_t *slave, int close_slave)
{
	int err;
	snd_pcm_plug_t *plug;
	snd_pcm_t *pcm;

	plug = calloc(1, sizeof(snd_pcm_plug_t));
	if (!plug)
		return -ENOMEM;

	plug->sformat = sformat;
	plug->schannels = schannels;
	plug->srate = srate;
	plug->gen.slave = plug->req_slave = slave;
	plug->gen.close_slave = close_slave;
	plug->route_policy = route_policy;
	plug->ttable = ttable;
	plug->tt_ssize = tt_ssize;
	plug->tt_cused = tt_cused;
	plug->tt_sused = tt_sused;

	err = snd_pcm_new(&pcm, SND_PCM_TYPE_PLUG, name, slave->stream, slave->mode);
	if (err < 0) {
		free(plug);
		return err;
	}
	pcm->ops = &snd_pcm_plug_ops;
	pcm->fast_ops = slave->fast_ops;
	pcm->fast_op_arg = slave->fast_op_arg;
	if (rate_converter) {
		plug->rate_converter = rate_converter;
		awalsa_debug("rate_converter=%s\n", plug->rate_converter);
	}
	pcm->private_data = plug;
	pcm->mmap_shadow = 1;
	snd_pcm_link_hw_ptr(pcm, slave);
	snd_pcm_link_appl_ptr(pcm, slave);
	*pcmp = pcm;

	return 0;
}
int snd_pcm_route_determine_ttable(
		const snd_pcm_route_ttable_config_t *tt_config,
		unsigned int *tt_csize, unsigned int *tt_ssize);
int snd_pcm_route_load_ttable(
		const snd_pcm_route_ttable_config_t *tt_config,
		snd_pcm_route_ttable_entry_t *ttable,
		unsigned int tt_csize, unsigned int tt_ssize,
		unsigned int *tt_cused, unsigned int *tt_sused, int schannels);
int _snd_pcm_plug_open(snd_pcm_t **pcmp, const snd_pcm_config_t *pcm_config,
		snd_pcm_stream_t stream, int mode)
{
	int ret;
	const snd_pcm_plug_config_t *plug_config = (const snd_pcm_plug_config_t *)(pcm_config->config);
	const snd_pcm_config_t *sconf = NULL;
	snd_pcm_t *spcm;
	const char *name = pcm_config->name;
	snd_pcm_format_t sformat = SND_PCM_FORMAT_UNKNOWN;
	int schannels = -1, srate = -1;
	const char *rate_converter = "linear";
	enum snd_pcm_plug_route_policy route_policy = PLUG_ROUTE_POLICY_DEFAULT;
	snd_pcm_route_ttable_entry_t *ttable = NULL;
	unsigned int csize = 0, ssize = 0;
	unsigned int cused = 0, sused = 0;

	awalsa_debug("plug config:%p, ttable:%p\n", plug_config, plug_config->ttable);
	if (plug_config->ttable != NULL) {
		route_policy = PLUG_ROUTE_POLICY_NONE;
		ret = snd_pcm_route_determine_ttable(plug_config->ttable, &csize, &ssize);
		if (ret < 0 || !csize || !ssize)
			goto ignore_ttable;
		awalsa_debug("csize=%d, ssize=%d\n", csize, ssize);
		ttable = malloc(csize * ssize * sizeof(snd_pcm_route_ttable_entry_t));
		if (!ttable) {
			awalsa_err("no memory\n");
			return -ENOMEM;
		}
		ret = snd_pcm_route_load_ttable(plug_config->ttable, ttable, csize, ssize,
				&cused, &sused, schannels);
		if (ret < 0) {
			awalsa_err("failed to load ttable\n");
			goto err;
		}
	}
ignore_ttable:
	awalsa_debug("\n");
	if (plug_config->route_policy != NULL) {
		if (ttable != NULL)
			awalsa_err("Table is defined, route policy is ignored\n");
		if (!strcmp(plug_config->route_policy, "default"))
			route_policy = PLUG_ROUTE_POLICY_DEFAULT;
		else if (!strcmp(plug_config->route_policy, "average"))
			route_policy = PLUG_ROUTE_POLICY_AVERAGE;
		else if (!strcmp(plug_config->route_policy, "copy"))
			route_policy = PLUG_ROUTE_POLICY_COPY;
		else if (!strcmp(plug_config->route_policy, "duplicate"))
			route_policy = PLUG_ROUTE_POLICY_DUP;
	}

	awalsa_debug("\n");
	if (plug_config->rate_converter != NULL)
		rate_converter = plug_config->rate_converter;
	if (!plug_config->slave.pcm) {
		awalsa_err("slave is not defined\n");
		ret = -EINVAL;
		goto err;
	}

	sconf = snd_pcm_config_get_config(plug_config->slave.pcm);
	if (!sconf) {
		awalsa_err("can't find plug slave pcm\n");
		ret = -EINVAL;
		goto err;
	}

	awalsa_debug("\n");
	ret = snd_pcm_open_config(&spcm, sconf, stream, mode);
	if (ret < 0)
		goto err;

#if 1
	if (plug_config->slave.format != 0)
		sformat = plug_config->slave.format;
	if (plug_config->slave.channels != 0)
		schannels = plug_config->slave.channels;
	if (plug_config->slave.rate != 0)
		srate = plug_config->slave.rate;
#else
	if (snd_pcm_type(spcm) == SND_PCM_TYPE_DMIX) {

	}
#endif

	awalsa_debug("\n");
	ret = snd_pcm_plug_open(pcmp, name, sformat, schannels, srate, rate_converter,
				route_policy, ttable, ssize, cused, sused, spcm, 1);
	if (ret < 0) {
		snd_pcm_close(spcm);
		goto err;
	}
	awalsa_debug("\n");
	return ret;
err:
	if (ttable)
		free(ttable);
	return ret;
}
