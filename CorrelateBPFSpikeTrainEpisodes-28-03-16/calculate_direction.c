
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
// #include <values.h>
#include "size_types.h"
#include "sess_anal.h"


void	calculate_direction(sess)	
SESSION	*sess;
{
	si4	i, n_recs;
	sf8	y_diff, x_diff, angle, two_pi, dist;
	sf8	rad_to_deg, min_dist, max_dist;
	RECORD	*rec;


	n_recs = sess->num_recs;
	rec = sess->recs;

	two_pi = 2.0 * M_PI;
	rad_to_deg = 180.0 / M_PI;
	min_dist = sess->params->min_inter_led_dist;
	max_dist = sess->params->max_inter_led_dist;

	for (i = 0; i < n_recs; ++i, ++rec) {
		if(sess->opts->bpf_room_file){
			rec->direction = rec->RoomAng;
			continue;
		}else if (sess->opts->bpf_arena_file){
			rec->direction = rec->ArenaAng;
			continue;
		}
			
		if (!(rec->red_x || rec->red_y) || !(rec->green_x || rec->green_y)) {
			rec->direction = NO_DIRECTION;
			continue;
		}
		y_diff = (sf8) rec->green_y - (sf8) rec->red_y; /* tracker coordinates are inverted in the y dimmension */
		x_diff = (sf8) rec->red_x - (sf8) rec->green_x; /* so this inversion corrects for it */
		dist = hypot(y_diff, x_diff) * PIXEL_SIZE;
		if (dist < min_dist || dist > max_dist) {
			rec->direction = NO_DIRECTION;
			continue;
		}
		angle = atan2(y_diff, x_diff);
		if (angle < 0.0)
			angle += two_pi;
		rec->direction = (si2) ((angle * rad_to_deg) + 0.5);

		rec->direction %= 360;
	}

	return;
}

