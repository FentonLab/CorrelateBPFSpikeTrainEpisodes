
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "size_types.h"
#include "uff.h"
#include "sess_anal.h"
#include "speed.h"


void	calculate_speed(sess)
SESSION	*sess;
{
	si4		i, j, k, n_recs, n_pos_recs;
	si4		speed_samp_interval, half_samp_interval;
	RECORD		*recs;
	ui4		*times;
	ui1		*x_s, *y_s, *speeds, temp_ui1, PositionRec;
	sf8		diff_x, diff_y, ds, dt, max_dt, temp_speed;
	sf8		diff_speed, delta, speed_constant;
	extern void	*gmem(), median_smooth();


	speed_samp_interval = (si4) (((sf8) sess->params->position_samp_freq * (sf8) SPEED_TIME_INTERVAL) + 0.5);
	half_samp_interval = speed_samp_interval >> 1;

	n_recs = sess->num_recs;
	recs = sess->recs;
	PositionRec = sess->PositionRec;

	for (i = n_pos_recs = 0; i < n_recs ; ++i)
		if (recs[i].type == PositionRec)
			++n_pos_recs;

	/* set up temporary arrays */
	x_s = (ui1 *) gmem((size_t) n_pos_recs, sizeof(ui1));
	y_s = (ui1 *) gmem((size_t) n_pos_recs, sizeof(ui1));
	speeds = (ui1 *) gmem((size_t) n_pos_recs + half_samp_interval, sizeof(ui1));
	times = (ui4 *) gmem((size_t) n_pos_recs, sizeof(ui4));

	for (i = j = 0; i < n_recs ; ++i) {
		if (recs[i].type == PositionRec) {
			if(sess->opts->bpf_room_file){
				x_s[j] = recs[i].RoomX;
				y_s[j] = recs[i].RoomY;
			}else if(sess->opts->bpf_arena_file){
				x_s[j] = recs[i].ArenaX;
				y_s[j] = recs[i].ArenaY;
			}else{
				x_s[j] = recs[i].red_x;
				y_s[j] = recs[i].red_y;
			}

			times[j] = recs[i].time_stamp;
			++j;
		}
	}

	speed_constant = (sf8) TIME_STAMPS_PER_SECOND * (sf8) PIXEL_SIZE;
	max_dt = (sf8) speed_samp_interval * (((sf8) TIME_STAMPS_PER_SECOND / (sf8) sess->params->position_samp_freq) * (1.0 + MAX_TIME_STAMP_ERROR));

	/* calculate speeds */
	for (i = 0, j = speed_samp_interval; j < n_pos_recs; ++i, ++j) {
		if (!(x_s[i] || y_s[i]) || !(x_s[j] || y_s[j])) {
			speeds[j] = MAXIMUM_SPEED;
			continue;
		}
		diff_x = (sf8) x_s[i] - (sf8) x_s[j];
		diff_y = (sf8) y_s[i] - (sf8) y_s[j];
		ds = hypot(diff_x, diff_y);
		dt = (sf8) times[j] - (sf8) times[i];
		temp_speed = (ds / dt) * speed_constant;
		if (temp_speed <= (sf8) REASONABLE_SPEED && dt <= max_dt)
			speeds[j] = (ui1) (temp_speed + 0.5);
		else
			speeds[j] = MAXIMUM_SPEED;

	}

	/* smooth speeds */
	// median_smooth((ui4) n_pos_recs, (ui4) speed_samp_interval, speeds, 0);

	/* insert into records, shift speeds to center of window */
	temp_ui1 = speeds[n_pos_recs - 1];
 	for (i = n_pos_recs, n_pos_recs += half_samp_interval; i < n_pos_recs; ++i) 
		speeds[i] = temp_ui1;
	for (i = 0, j = half_samp_interval; i < n_recs; ++i) {
		recs[i].running_speed = speeds[j];
		if (recs[i].type == PositionRec && j < n_pos_recs)
			++j;
	}

	free((void *) x_s);
	free((void *) y_s);
	free((void *) speeds);
	free((void *) times);


	return;
}
