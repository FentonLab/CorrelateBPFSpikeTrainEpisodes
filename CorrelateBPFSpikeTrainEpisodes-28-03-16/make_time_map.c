
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_time_map(sess)
SESSION	*sess;
{
	si4	i, n_recs, area, x_size, y_size, start_rec, start_time, stop_time;
	RECORD	*recs;
	sf4	**tm, **tm2, *tma, *tma2, pos_samp_freq;
	ui1	edge_of_app_warning, PositionRec;
	TIME	*tp, *tp2;

        area = sess->params->area;
        x_size = sess->params->x_size;
        y_size = sess->params->y_size;

	n_recs = sess->num_recs;
	recs = sess->recs;
	tp = sess->time;
	tp2 = sess->secondary_time;
	tm = tp->map;
	tm2 = tp2->map;
	tma = tm[0];
	tma2 = tm2[0];
	PositionRec = sess->PositionRec;

	(void) memset((void *) tma, 0, (size_t) area * sizeof(sf4));	/* have to do this because some options cause the */
	(void) memset((void *) tma2, 0, (size_t) area * sizeof(sf4));	/* time map to be built twice, once for the apparatus */
									/* and again after the records have been filtered */
	if (sess->opts->no_pos_samp){
		// recording may not have been continuous in time so use a series of methodes to estimate the recording time.
		// if there are no pos samps then the total time is estimated to be the time of the last record.
		tm[recs[0].red_y][recs[0].red_x] = recs[n_recs - 1].time_stamp;
		tm2[recs[0].red_y][recs[0].red_x] = recs[n_recs - 1].time_stamp;
		tp->total_secs = (sf8)(recs[n_recs - 1].time_stamp - recs[0].time_stamp) / (sf8)TIME_STAMPS_PER_SECOND;
		tp2->total_secs = (sf8)(recs[n_recs - 1].time_stamp - recs[0].time_stamp) / (sf8)TIME_STAMPS_PER_SECOND;

		// If there are EEG records then they provide the best estimate of when the system was in recording mode
		for (i = 0; i < n_recs; ++i){
			if (recs[i].type == EEG_BPF_REC_TYPE){
				start_rec = i;
				start_time = recs[i].time_stamp;
	
				for (; i < n_recs; ++i){
					if (recs[i].type == EEG_BPF_REC_TYPE)
						stop_time = recs[i].time_stamp;
				}

				tp->total_secs = (sf8)(stop_time - start_time) / (sf8)TIME_STAMPS_PER_SECOND;
				tp2->total_secs = (sf8)(stop_time - start_time) / (sf8)TIME_STAMPS_PER_SECOND;
				tm[recs[0].red_y][recs[0].red_x] = stop_time - start_time;
				tm2[recs[0].red_y][recs[0].red_x] = stop_time - start_time;

				return;
			}
		}
		return;
	}

	if (sess->opts->bpf_room_file || sess->opts->bpf_arena_file){
		for (i = 0; i < n_recs; ++i){
			if (recs[i].type == PositionRec){
				start_rec = i;
				start_time = recs[i].time_stamp;
				break;
			}
		}

		for (; i < n_recs; ++i){
			if (recs[i].type == PositionRec){
				if (sess->opts->bpf_room_file){
					tm[recs[start_rec].RoomY][recs[start_rec].RoomX] += (recs[i].time_stamp - start_time);
					tm2[recs[start_rec].ArenaY][recs[start_rec].ArenaX] += (recs[i].time_stamp - start_time);
				}else{
					tm[recs[start_rec].ArenaY][recs[start_rec].ArenaX] += (recs[i].time_stamp - start_time);
					tm2[recs[start_rec].RoomY][recs[start_rec].RoomX] += (recs[i].time_stamp - start_time);
				}

				start_rec = i;
				start_time = recs[i].time_stamp;
			}
		}

		tp->total_secs = 0.0;
		tp2->total_secs = 0.0;
		for (i = 0; i < area; ++i) {
			tma[i] /= TIME_STAMPS_PER_SECOND;
			tma2[i] /= TIME_STAMPS_PER_SECOND;
			tp->total_secs += (sf8) tma[i];
			tp2->total_secs += (sf8) tma2[i];
		}

	} else { // UFF or TS or KM
		for (i = 0; i < n_recs; ++i){
			if (recs[i].type == PositionRec){
				tm[recs[i].red_y][recs[i].red_x] += 1.0;
				tm2[recs[i].red_y][recs[i].red_x] += 1.0;
			}
		}

		pos_samp_freq = sess->params->position_samp_freq;
		tp->total_secs = 0.0;
		tp2->total_secs = 0.0;
		for (i = 0; i < area; ++i) {
			tma[i] /= pos_samp_freq;
			tma2[i] /= pos_samp_freq;
			tp->total_secs += (sf8) tma[i];
			tp2->total_secs += (sf8) tma[i];
		}
	}	


	tm[0][0] = tm2[0][0] = 0.0;	/* remove non-detects, and anything else that got collapsed into */
			/* this pixel or was legitimately detected in this pixel, unfortunately. */
			/* But since the tracker can use all the values between 0 and 255, and */
			/* and since I did not include a mechanism to specifically label detects at
			/* exactly (0,0) before collapsing the pixels, this is the price that must */
			/* be paid. I real life this is not likely to be an issue, however */

	return;
}
