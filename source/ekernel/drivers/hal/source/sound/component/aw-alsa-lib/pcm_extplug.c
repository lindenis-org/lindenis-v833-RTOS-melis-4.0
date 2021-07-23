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
#include <aw-alsa-lib/pcm_extplug.h>
#include "pcm_local.h"
#include "pcm_plugin_generic.h"
#include "pcm_ext_parm.h"

typedef struct snd_pcm_extplug_priv {
	snd_pcm_plugin_t plug;
	snd_pcm_extplug_t *data;
	struct snd_ext_parm params[SND_PCM_EXTPLUG_HW_PARAMS];
	struct snd_ext_parm sparams[SND_PCM_EXTPLUG_HW_PARAMS];
} extplug_priv_t;


static const int hw_params_type[SND_PCM_EXTPLUG_HW_PARAMS] = {
	[SND_PCM_EXTPLUG_HW_FORMAT] = SND_PCM_HW_PARAM_FORMAT,
	[SND_PCM_EXTPLUG_HW_CHANNELS] = SND_PCM_HW_PARAM_CHANNELS
};

#define is_mask_type(i) (hw_params_type[i] < SND_PCM_HW_PARAM_FIRST_INTERVAL)

/*
 * set min/max values for the given parameter
 */
int snd_ext_parm_set_minmax(struct snd_ext_parm *parm, unsigned int min, unsigned int max)
{
	parm->num_list = 0;
	free(parm->list);
	parm->list = NULL;
	parm->min = min;
	parm->max = max;
	return 0;
}

/*
 * set the list of available values for the given parameter
 */
static int val_compar(const void *ap, const void *bp)
{
	return *(const unsigned int *)ap - *(const unsigned int *)bp;
}

int snd_ext_parm_set_list(struct snd_ext_parm *parm, unsigned int num_list, const unsigned int *list)
{
	unsigned int *new_list;

	new_list = malloc(sizeof(*new_list) * num_list);
	if (new_list == NULL)
		return -ENOMEM;
	memcpy(new_list, list, sizeof(*new_list) * num_list);
	qsort(new_list, num_list, sizeof(*new_list), val_compar);

	free(parm->list);
	parm->num_list = num_list;
	parm->list = new_list;
	return 0;
}

void snd_ext_parm_clear(struct snd_ext_parm *parm)
{
	free(parm->list);
	memset(parm, 0, sizeof(*parm));
}

int snd_pcm_extplug_set_param_list(snd_pcm_extplug_t *extplug, int type,
		unsigned int num_list, const unsigned int *list)
{
	extplug_priv_t *ext = extplug->pcm->private_data;
	if (type < 0 || type >= SND_PCM_EXTPLUG_HW_PARAMS) {
		awalsa_err("EXTPLUG: invalid parameter type %d\n", type);
		return -EINVAL;
	}
	return snd_ext_parm_set_list(&ext->params[type], num_list, list);
}

int snd_pcm_extplug_set_param_minmax(snd_pcm_extplug_t *extplug, int type,
		unsigned int min, unsigned int max)
{
	extplug_priv_t *ext = extplug->pcm->private_data;
	if (type < 0 || type >= SND_PCM_EXTPLUG_HW_PARAMS) {
		awalsa_err("EXTPLUG: invalid parameter type %d\n", type);
		return -EINVAL;
	}
	if (is_mask_type(type)) {
		awalsa_err("EXTPLUG: invalid parameter type %d\n", type);
		return -EINVAL;
	}
	return snd_ext_parm_set_minmax(&ext->params[type], min, max);
}

int snd_pcm_extplug_set_slave_param_list(snd_pcm_extplug_t *extplug, int type,
		unsigned int num_list, const unsigned int *list)
{
	extplug_priv_t *ext = extplug->pcm->private_data;
	if (type < 0 || type >= SND_PCM_EXTPLUG_HW_PARAMS) {
		awalsa_err("EXTPLUG: invalid parameter type %d\n", type);
		return -EINVAL;
	}
	return snd_ext_parm_set_list(&ext->sparams[type], num_list, list);
}

int snd_pcm_extplug_set_slave_param_minmax(snd_pcm_extplug_t *extplug, int type,
		unsigned int min, unsigned int max)
{
	extplug_priv_t *ext = extplug->pcm->private_data;
	if (type < 0 || type >= SND_PCM_EXTPLUG_HW_PARAMS) {
		awalsa_err("EXTPLUG: invalid parameter type %d\n", type);
		return -EINVAL;
	}
	if (is_mask_type(type)) {
		awalsa_err("EXTPLUG: invalid parameter type %d\n", type);
		return -EINVAL;
	}
	return snd_ext_parm_set_minmax(&ext->sparams[type], min, max);
}

static void clear_ext_params(extplug_priv_t *ext)
{
	int i;
	for (i = 0; i < SND_PCM_EXTPLUG_HW_PARAMS; i++) {
		snd_ext_parm_clear(&ext->params[i]);
		snd_ext_parm_clear(&ext->sparams[i]);
	}
}

void snd_pcm_extplug_params_reset(snd_pcm_extplug_t *extplug)
{
	extplug_priv_t *ext = extplug->pcm->private_data;
	clear_ext_params(ext);
}

/*
 * call init callback
 */
static int snd_pcm_extplug_init(snd_pcm_t *pcm)
{
	awalsa_debug("\n");
	extplug_priv_t *ext = pcm->private_data;
	return ext->data->callback->init(ext->data);
}

/*
 * write_areas skeleton - call transfer callback
 */
static snd_pcm_uframes_t
snd_pcm_extplug_write_areas(snd_pcm_t *pcm,
			    const snd_pcm_channel_area_t *areas,
			    snd_pcm_uframes_t offset,
			    snd_pcm_uframes_t size,
			    const snd_pcm_channel_area_t *slave_areas,
			    snd_pcm_uframes_t slave_offset,
			    snd_pcm_uframes_t *slave_sizep)
{
	extplug_priv_t *ext = pcm->private_data;

	if (size > *slave_sizep)
		size = *slave_sizep;
	size = ext->data->callback->transfer(ext->data, slave_areas, slave_offset,
					     areas, offset, size);
	*slave_sizep = size;
	return size;
}

/*
 * read_areas skeleton - call transfer callback
 */
static snd_pcm_uframes_t
snd_pcm_extplug_read_areas(snd_pcm_t *pcm,
			   const snd_pcm_channel_area_t *areas,
			   snd_pcm_uframes_t offset,
			   snd_pcm_uframes_t size,
			   const snd_pcm_channel_area_t *slave_areas,
			   snd_pcm_uframes_t slave_offset,
			   snd_pcm_uframes_t *slave_sizep)
{
	extplug_priv_t *ext = pcm->private_data;

	if (size > *slave_sizep)
		size = *slave_sizep;
	size = ext->data->callback->transfer(ext->data, areas, offset,
					     slave_areas, slave_offset, size);
	*slave_sizep = size;
	return size;
}

static int snd_pcm_extplug_close(snd_pcm_t *pcm)
{
	extplug_priv_t *ext = pcm->private_data;

	awalsa_debug("\n");
	snd_pcm_close(ext->plug.gen.slave);
	clear_ext_params(ext);
	if (ext->data->callback->close)
		ext->data->callback->close(ext->data);
	free(ext);
	return 0;
}

static int snd_pcm_extplug_hw_refine(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
	extplug_priv_t *ext = pcm->private_data;
	snd_pcm_t *slave = ext->plug.gen.slave;
	int ret;

	awalsa_debug("\n");

	// TODO: how to refine?
	ret = snd_pcm_hw_refine(slave, params);
	if (ret < 0)
		return ret;

	return 0;
}

static int snd_pcm_extplug_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
	extplug_priv_t *ext = pcm->private_data;
	snd_pcm_t *slave = ext->plug.gen.slave;
	int err;
	snd_pcm_hw_params_t *slave_params;

	awalsa_debug("\n");

	// TODO: how to set hw_params?
	snd_pcm_hw_params_alloca(&slave_params);
	memcpy(slave_params, params, sizeof(snd_pcm_hw_params_t));

	snd_pcm_hw_param_change(slave_params, SND_PCM_HW_PARAM_ACCESS);
	err = snd_pcm_hw_params_set_access(slave, slave_params, SND_PCM_ACCESS_MMAP_INTERLEAVED);
	if (err < 0) {
		awalsa_err("snd_pcm_hw_params_set_access failed\n");
		return err;
	}

	err = _snd_pcm_hw_params_internal(slave, slave_params);
	if (err < 0) {
		awalsa_err("failed to set slave hw params\n");
		return err;
	}
	memcpy(params, slave_params, sizeof(snd_pcm_hw_params_t));

	ext->data->slave_format = slave->format;
	ext->data->slave_channels = slave->channels;
	ext->data->rate = slave->rate;
	snd_pcm_hw_params_get_format(params, &ext->data->format);
	snd_pcm_hw_params_get_channels(params, &ext->data->channels);

	if (ext->data->callback->hw_params) {
		err = ext->data->callback->hw_params(ext->data, params);
		if (err < 0)
			return err;
	}
	return 0;
}

static int snd_pcm_extplug_hw_free(snd_pcm_t *pcm)
{
	extplug_priv_t *ext = pcm->private_data;

	awalsa_debug("\n");
	snd_pcm_hw_free(ext->plug.gen.slave);
	if (ext->data->callback->hw_free)
		return ext->data->callback->hw_free(ext->data);
	return 0;
}

static void snd_pcm_extplug_dump(snd_pcm_t *pcm)
{
	extplug_priv_t *ext = pcm->private_data;
	awalsa_debug("\n");

	if (ext->data->callback->dump)
		ext->data->callback->dump(ext->data);
	else {
		if (ext->data->name)
			printf("%s\n", ext->data->name);
		else
			printf("External PCM Plugin\n");
		if (pcm->setup) {
			printf("Its setup is:\n");
			snd_pcm_dump_setup(pcm);
		}
	}
	printf("Slave: ");
	snd_pcm_dump(ext->plug.gen.slave);
}

static const snd_pcm_ops_t snd_pcm_extplug_ops = {
	.close = snd_pcm_extplug_close,
	.hw_refine = snd_pcm_extplug_hw_refine,
	.hw_params = snd_pcm_extplug_hw_params,
	.hw_free = snd_pcm_extplug_hw_free,
	.sw_params = snd_pcm_generic_sw_params,
	.channel_info = snd_pcm_generic_channel_info,
	.dump = snd_pcm_extplug_dump,
	.mmap = snd_pcm_generic_mmap,
	.munmap = snd_pcm_generic_munmap,
	//.query_chmaps = snd_pcm_extplug_query_chmaps,
	//.get_chmap = snd_pcm_extplug_get_chmap,
	//.set_chmap = snd_pcm_extplug_set_chmap,
};

int snd_pcm_extplug_create(snd_pcm_extplug_t *extplug, const char *name,
			   const char *spcm_name, snd_pcm_stream_t stream, int mode)
{
	int ret = 0;
	extplug_priv_t *ext;
	snd_pcm_t *spcm, *pcm;
	const snd_pcm_config_t *sconf = NULL;

	assert(extplug && extplug->callback);
	assert(extplug->callback->transfer);

	sconf = snd_pcm_config_get_config(spcm_name);
	if (!sconf) {
		awalsa_err("can't find pcm slave: %s\n", spcm_name);
		ret = -EINVAL;
		goto err_out;
	}
	ret = snd_pcm_open_config(&spcm, sconf, stream, mode);
	if (ret < 0) {
		awalsa_err("unable to open slave\n");
		goto err_out;
	}

	ext = calloc(1, sizeof(*ext));
	if (!ext) {
		awalsa_err("no memory\n");
		ret = -ENOMEM;
		goto err_close_spcm;
	}

	ext->data = extplug;
	extplug->stream = stream;

	snd_pcm_plugin_init(&ext->plug);
	ext->plug.read = snd_pcm_extplug_read_areas;
	ext->plug.write = snd_pcm_extplug_write_areas;
	ext->plug.undo_read = snd_pcm_plugin_undo_read_generic;
	ext->plug.undo_write = snd_pcm_plugin_undo_write_generic;
	ext->plug.gen.slave = spcm;
	ext->plug.gen.close_slave = 1;
	if (extplug->callback->init)
		ext->plug.init = snd_pcm_extplug_init;

	ret = snd_pcm_new(&pcm, SND_PCM_TYPE_EXTPLUG, name, stream, mode);
	if (ret < 0) {
		goto err_free_ext;
	}

	extplug->pcm = pcm;
	pcm->ops = &snd_pcm_extplug_ops;
	pcm->fast_ops = &snd_pcm_plugin_fast_ops;
	pcm->private_data = ext;
	snd_pcm_set_hw_ptr(pcm, &ext->plug.hw_ptr, -1, 0);
	snd_pcm_set_appl_ptr(pcm, &ext->plug.appl_ptr, -1, 0);

	return 0;

err_free_ext:
	free(ext);
err_close_spcm:
	snd_pcm_close(spcm);
err_out:
	return ret;
}
