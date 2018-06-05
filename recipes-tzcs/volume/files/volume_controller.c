#include <stdio.h>
#include "vol.h"
#include <string.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

void main(int argc, char *argv[])
{
	int num_of_step;
	double index;
	double cur_vol_w;
	double step;
	double nex_vol_w;
	double nex_vol_db;
	double max_vol_w;
	long vol_set;
        long min_vol_db;
        long max_vol_db;
	long vol_ran_db;
	long vol_range;
	long min_vol;
	long max_vol;
	const char *card = "default";
	const char *selem_name = "Master";

	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, selem_name);

	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &min_vol, &max_vol);
	snd_mixer_selem_ask_playback_vol_dB(elem, min_vol, &min_vol_db);
	snd_mixer_selem_ask_playback_vol_dB(elem, max_vol, &max_vol_db);

	/* Get volume before change */
	cur_vol_w = get_info(GET_CUR_VOL);

	/* Determine volume value of 1% */
	index = max_vol_db/3000;
        max_vol_w = pow(10,index);
	step = max_vol_w/100;

	/* Determine volume after change */
        num_of_step = atoi(argv[1]);
        nex_vol_w = cur_vol_w + num_of_step*step;
        nex_vol_db = pow_to_db(nex_vol_w);
        if (nex_vol_w < step) {
                nex_vol_w = 0;
                nex_vol_db = -62.25;
        }
        if (nex_vol_w >= 1.0) {
                nex_vol_w = 1.0;
        }
        write_info(nex_vol_w);

        /* Determine volume value for setting */
	vol_range = max_vol - min_vol;
	vol_ran_db = max_vol_db - min_vol_db;
	vol_set = (long)(nex_vol_db*100 - min_vol_db);
	vol_set = (vol_set*vol_range)/vol_ran_db;

	/* Set new volume value */
	snd_mixer_selem_set_playback_volume_all(elem, vol_set);
	snd_mixer_close(handle);
}
