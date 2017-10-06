
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "uff.h"
#include "sess_anal.h"


void	get_recs_from_km(sess)
SESSION	*sess;
{
	si4		i, j, k, l;
	ui4		total_recs, n_pos_recs, file_len;
	ui4		curr_time_stamp, spike_counters[4];
	si1		sess_path[256], *km_dir;
	ui1		*data, *dp;
	sf8		pos_samp_interval, curr_time;
	extern void	*gmem();
	extern si1	*get_env_var();
	RECORD		*rp;
	FILE		*fp;


	(void) fprintf(stderr, "%cNote: The position sampling frequency is assumed to be %0.1f Hz in the KM file.\n", 7, POSITION_SAMPLING_FREQUENCY);

	strcpy(sess->date_str, "not available");
	strcpy(sess->time_str, "not available");
	strcpy(sess->desc_str, "not available");

	file_len = sess->file_len;
	if (file_len % 6) {
		(void) fprintf(stderr, "%c\n\tError in length of file \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	data = sess->data;

	for (i = 0; i < 4; ++i)
		spike_counters[i] = 0;
	n_pos_recs = file_len / 6;
	for (i = 0, dp = data; i < n_pos_recs; ++i, dp += 6) {
		spike_counters[0] += (ui4) (dp[4] & 15);
		spike_counters[1] += (ui4) (dp[4] >> 4);
		spike_counters[2] += (ui4) (dp[5] & 15);
		spike_counters[3] += (ui4) (dp[5] >> 4);
	}

	total_recs = n_pos_recs;
	for (i = 0; i < 4; ++i)
		total_recs += spike_counters[i];

	rp = sess->recs = (RECORD *) gmem((size_t) total_recs, sizeof(RECORD));
	sess->num_recs = total_recs;

	pos_samp_interval = (sf8) TIME_STAMPS_PER_SECOND / (sf8) POSITION_SAMPLING_FREQUENCY;
	curr_time = 0.0;

	for (i = l = 0, dp = data; i < n_pos_recs; ++i, dp += 6) {
		curr_time += pos_samp_interval;
		curr_time_stamp = (ui4) (curr_time + 0.5);
		spike_counters[0] = dp[4] & 15;
		spike_counters[1] = dp[4] >> 4;
		spike_counters[2] = dp[5] & 15;
		spike_counters[3] = dp[5] >> 4;
		for (j = 0; j < 4; ++j)
			for (k = 0; k < spike_counters[j]; ++k) {
				rp[l].type = SINGLE_ELECTRODE_SPIKE_REC;
				rp[l].probe = 0;
				rp[l].clust = j + 1;
				rp[l].time_stamp = curr_time_stamp - 1;
				++l;
			}
		rp[l].type = POSITION_REC;
		rp[l].time_stamp = curr_time_stamp;
		rp[l].red_x = dp[0];
		rp[l].red_y = dp[1];
		rp[l].green_x = dp[2];
		rp[l].green_y = dp[3];
		++l;
	}


	return;
}	

