

#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"

#define POS_INTERVALS_TO_CHECK	1000	/* this is set to be a reasonable number */
					/* position sample intervals such that it is very */
					/* unlikely that there would be an interruption */
					/* of the session prior to this point. If for */
					/* example the position sampling rate is 50 Hz */
					/* this constitutes 20 seconds of recording -> */
					/* nobody interrupts a recording before 20 seconds. */
					/* Yet this is more than an adequate number of samples */
					/* to get the sampling frequency (which is presumed */
					/* to be an integral number of Hz) */

#define INIT_POS_SAMPS_TO_SKIP	100	/* for whatever reason, things don't seem to be perfectly */
					/* timed at the beginning of a session on some systems, */
					/* so skip some position samples in the beginning of the file */

void	get_pos_samp_freq(sess)
SESSION	*sess;
{
	si1	check_pos_sampling();

	if (sess->opts->no_pos_samp) {
		sess->params->position_samp_freq = (sf4) TIME_STAMPS_PER_SECOND;
		return;
	}

	if (sess->opts->ts_file) {
		// sess->params->position_samp_freq was set in get_recs_from_ts()
		return;
	}

	if (sess->opts->bpf_room_file || sess->opts->bpf_arena_file) {
		if(check_pos_sampling(sess)){
			(void) fprintf(stderr, "%c\n\tError in the determination of Position sampling frequency. Terminating program.\n\n", 7);
			exit(1);
		}
			
	}else{ // UFF files
		if(check_pos_sampling(sess)){
			(void) fprintf(stderr, "%c\n\tTimestamp interval error exceeds acceptable levels. Terminating program.\n\n", 7);
			exit(1);
		}
	}

	return;
}
					
si1	check_pos_sampling(SESSION *sess){
	RECORD	*recs;
	si4	i, pitc, n_recs, init_recs;
	ui4	min, max, last_time, time_diff;
	sf8	sum, mean, range1, range2;
	ui1	PositionRec;

	recs = sess->recs;
	n_recs = sess->num_recs;
	PositionRec = sess->PositionRec;

	max = 0;
	min = ~max;
	sum = 0.0;

	last_time = 0;
	for (pitc = 0, i = 10; i < n_recs; ++i){	// skip the first few samples as they can often be problematic
		if (recs[i].type == PositionRec){
			if(last_time == 0){
				last_time = recs[i].time_stamp;
				continue;
			}	

			time_diff = recs[i].time_stamp - last_time;
			last_time = recs[i].time_stamp;
			if (time_diff < min){
				min = time_diff;
			} else if (time_diff > max){
				max = time_diff;
			}
			sum += (sf8) time_diff;
			if (++pitc == POS_INTERVALS_TO_CHECK)
				break;
		}
	}
	if (pitc < POS_INTERVALS_TO_CHECK)
		return(-1);

	mean = sum / (sf8) pitc;
	// sess->params->position_samp_freq = (sf4) ((si4) (((sf8) TIME_STAMPS_PER_SECOND / mean) + 0.5));
	sess->params->position_samp_freq = (sf4) ((sf8) TIME_STAMPS_PER_SECOND / mean);

	if(sess->opts->bpf_room_file || sess->opts->bpf_arena_file)
		return(0);

	mean = (sf8) TIME_STAMPS_PER_SECOND / (sf8) sess->params->position_samp_freq;
	range1 = (mean - (sf8) min) / mean;
	range2 = ((sf8) max - mean) / mean;

	return (range1 > MAX_TIME_STAMP_ERROR || range2 > MAX_TIME_STAMP_ERROR); 
}
