#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <init.h>
#include <log.h>
#include <sound/snd_core.h>

extern struct snd_codec sun8iw19_codec;
extern struct snd_codec dummy_codec;

int sunxi_soundcards_register(void)
{
	int ret = 0;
	ret = snd_card_register("audiocodec", &sun8iw19_codec, SND_PLATFORM_TYPE_CPUDAI);
	if (ret == 0) {
		__log("soundcards: audiocodec register success!");
	} else {
		return ret;
	}

	ret = snd_card_register("snddaudio0", &dummy_codec, SND_PLATFORM_TYPE_DAUDIO0);
	if (ret == 0) {
		__log("soundcards: snddaudio0 register success!");
	} else {
		return ret;
	}

	return 0;
}

late_initcall(sunxi_soundcards_register);
