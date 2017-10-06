
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "theta.h"


void	fill_theta_fields(sess, eeg_data)
SESSION		*sess;
EEG_DATA	*eeg_data;
{
	si4		i, j, start, end, index;
	si4		before, after, n_recs, n_eeg_samps;
	si2		*eeg_phase;
	ui1		*eeg_state;
	ui4		*eeg_times, t;
	RECORD		*recs;


	n_recs = sess->num_recs;
	n_eeg_samps = eeg_data->n_samps;
	recs = sess->recs;
	eeg_times = eeg_data->times;
	eeg_phase = eeg_data->phases;
	eeg_state = eeg_data->state;

	for (i = 0; recs[i].time_stamp >= eeg_times[0]; ++i) {
		recs[i].theta_state = eeg_state[0];
		recs[i].theta_phase = eeg_phase[0];
	}
	start = i;

	for (i = n_recs - 1; recs[i].time_stamp >= eeg_times[n_eeg_samps - 1]; --i) {
		recs[i].theta_state = eeg_state[n_eeg_samps - 1];
		recs[i].theta_phase = eeg_phase[n_eeg_samps - 1];
	}
	end = i;

	for (i = start, j = 0; i < end; ++i) {
		for (t = recs[i].time_stamp; t > eeg_times[j]; ++j);
		before = (si4) (t - eeg_times[j - 1]);
		after = (si4) (eeg_times[j] - t);
		if (before < after)
			index = j - 1;
		else
			index = j;
		recs[i].theta_state = eeg_state[index];
		recs[i].theta_phase = eeg_phase[index];
	}


	return;
}

