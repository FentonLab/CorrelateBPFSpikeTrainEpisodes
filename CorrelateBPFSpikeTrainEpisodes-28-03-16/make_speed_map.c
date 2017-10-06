
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "speed.h"


void	make_speed_map(sess)
SESSION	*sess;
{
	si4		i, j, n_recs, **speed_counts;
	si4		area, x_size, y_size, sum_cnt, *sca;
	sf8		sum;
	RECORD		*recs;
	sf4		**sm, *sma, thresh;
	ui1		min, max, x, y, speed, **am, PositionRec;
	extern void	*gmem();

        area = sess->params->area;
        x_size = sess->params->x_size;
        y_size = sess->params->y_size;
	PositionRec = sess->PositionRec;

	speed_counts = (si4 **) gmem((size_t) y_size, sizeof(si4 *));
	speed_counts[0] = (si4 *) gmem((size_t) area, sizeof(si4));
	for (i = 1, j = x_size; i < y_size; ++i, j += x_size)	
		speed_counts[i] = speed_counts[0] + j;

	n_recs = sess->num_recs;
	recs = sess->recs;
	sm = sess->speed->map;
	am = sess->apparatus->map;

	min = MAXIMUM_SPEED;	/* note MAXIMUM_SPEED is used to indicate a bad speed (i.e. from a non-detect, etc.) */
	max = 0;
	sum = 0.0;
	sum_cnt = 0;
	for (i = 0; i < n_recs; ++i)
		if (recs[i].type == PositionRec) {
			speed = recs[i].running_speed;
			if (speed == MAXIMUM_SPEED)
				continue;

			if(sess->opts->bpf_room_file){
				x = recs[i].RoomX;
				y = recs[i].RoomY;
			}else if (sess->opts->bpf_arena_file){
				x = recs[i].ArenaX;
				y = recs[i].ArenaY;
			}else{
				x = recs[i].red_x;
				y = recs[i].red_y;
			}

			if (!am[y][x])
				continue;
			sm[y][x] += (sf4) speed;
			++speed_counts[y][x];
			if (speed > max)
				max = speed;
			else if (speed < min)
				min = speed;
			sum += (sf8) speed;
			++sum_cnt;
		}
	
	sess->speed->average = sum / (sf8) sum_cnt;
	sess->speed->max = max;
	sess->speed->min = min;

	sma = sm[0];
	sca = speed_counts[0];
	thresh = (sf4) sess->params->global_speed_thresh;
	for (i = 0; i < area; ++i) {
		if (sca[i]) {
			sma[i] /= (sf4) sca[i];
			if (sma[i] < thresh)
				sma[i] = 0.0;
		} else
			sma[i] = NO_SPEED;
	}

	free((void *) speed_counts[0]);
	free((void *) speed_counts);


	return;
}
