
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"
#include "bpf.h"

void	rotate_pixels(sess)	
SESSION	*sess;
{
	si4	i, n_recs, bits;
	ui4	duration, start_time;
	sf8	polar_x, polar_y, center_x, center_y, angle, radius, rotated_x, rotated_y;
	sf8	period, ang_displacement, two_pi, rads_per_time_stamp;
	ui1	RotateRoom, RotateArena;
	RECORD	*rec;

	period = sess->params->pixel_room_rotate_rpm;
	if(period == 0.0){
		period = sess->params->pixel_arena_rotate_rpm;
		if(period == 0.0){
			return;
		}
		RotateArena = 1;
	}else{
		RotateRoom = 1;
	}

	rec = sess->recs;
	n_recs = sess->num_recs;

	// RotateRoom = 0;
	// RotateArena = 1;

	period = 1.0; 
	period *= TIME_STAMPS_PER_SECOND * 60.0;
	rads_per_time_stamp = two_pi / period;

	center_x = CENTER_X;
	center_y = CENTER_Y;
	two_pi = 2.0 * M_PI;
	
	start_time = rec[0].time_stamp;
	
	for (i = 0; i < n_recs; ++i, rec++) {
		duration = rec->time_stamp - start_time;
		ang_displacement = (sf8)(duration * rads_per_time_stamp);

		if(RotateRoom){
			if(!(rec->RoomX) && !(rec->RoomY))
				continue;
			polar_x = (sf8)(rec->RoomX - center_x);
			polar_y = (sf8)(center_y - rec->RoomY);
			angle = atan2(polar_y , polar_x);
			radius = hypot(polar_y, polar_x);
			angle += ang_displacement;
			rotated_x = radius * cos(angle); 
			rotated_y = radius * sin(angle); 
			rotated_x = center_x + rotated_x;
			rotated_y = center_y - rotated_y;
			rec->RoomX = (ui1) (rotated_x + 0.5);
			rec->RoomY = (ui1) (rotated_y + 0.5);

			polar_x = (sf8)(rec->red_x - center_x);
			polar_y = (sf8)(center_y - rec->red_y);
			angle = atan2(polar_y , polar_x);
			radius = hypot(polar_y, polar_x);
			angle += ang_displacement;
			rotated_x = radius * cos(angle); 
			rotated_y = radius * sin(angle); 
			rotated_x = center_x + rotated_x;
			rotated_y = center_y - rotated_y;
			rec->red_x = (ui1) (rotated_x + 0.5);
			rec->red_y = (ui1) (rotated_y + 0.5);
		}

		if(RotateArena){
			if(!(rec->ArenaX) && !(rec->ArenaY))
				continue;
			polar_x = (sf8)(rec->ArenaX - center_x);
			polar_y = (sf8)(center_y - rec->ArenaY);
			angle = atan2(polar_y , polar_x);
			radius = hypot(polar_y, polar_x);
			angle += ang_displacement;
			rotated_x = radius * cos(angle); 
			rotated_y = radius * sin(angle); 
			rotated_x = center_x + rotated_x;
			rotated_y = center_y - rotated_y;
			rec->ArenaX = (ui1) (rotated_x + 0.5);
			rec->ArenaY = (ui1) (rotated_y + 0.5);

			polar_x = (sf8)(rec->green_x - center_x);
			polar_y = (sf8)(center_y - rec->green_y);
			angle = atan2(polar_y , polar_x);
			radius = hypot(polar_y, polar_x);
			angle += ang_displacement;
			rotated_x = radius * cos(angle); 
			rotated_y = radius * sin(angle); 
			rotated_x = center_x + rotated_x;
			rotated_y = center_y - rotated_y;
			rec->green_x = (ui1) (rotated_x + 0.5);
			rec->green_y = (ui1) (rotated_y + 0.5);
		}
	}

	return;	
}
