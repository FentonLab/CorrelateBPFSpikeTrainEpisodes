
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void	fill_xy_fields(sess)
SESSION	*sess;
{
	si4	i, j, n_recs, beginning, end;
	ui1	next_red_x, next_red_y, next_green_x, next_green_y, PositionRec;
	ui1	last_red_x, last_red_y, last_green_x, last_green_y;
	ui1	next_room_x, next_room_y, next_arena_x, next_arena_y;
	ui1	last_room_x, last_room_y, last_arena_x, last_arena_y;
	ui2	next_room_ang, next_arena_ang;
	ui2	last_room_ang, last_arena_ang;
	ui4	next_pos_time, last_pos_time, ltd, ntd;
	RECORD 	*recs;


	n_recs = sess->num_recs;
	recs = sess->recs;
	PositionRec = sess->PositionRec;
	
	if (sess->opts->no_pos_samp){
		/* fill in all pos samps with (128, 128) */
		for (i = 0; i < n_recs; ++i) {
			recs[i].red_x = recs[i].red_y = recs[i].green_x = recs[i].green_y = recs[i].RoomX = recs[i].RoomY = recs[i].ArenaX = recs[i].ArenaY = 128;
		}
		return;
	}

	/* fill in actual sampled positions */

	/* take care of end */
	for (i = n_recs - 1; i >= 0; --i)
		if (recs[i].type == PositionRec)
			break;
	end = i;
	last_red_x = recs[i].red_x;
	last_red_y = recs[i].red_y;
	last_green_x = recs[i].green_x;
	last_green_y = recs[i].green_y;
	last_room_x = recs[i].RoomX;
	last_room_y = recs[i].RoomY;
	last_room_ang = recs[i].RoomAng;
	last_arena_x = recs[i].ArenaX;
	last_arena_y = recs[i].ArenaY;
	last_arena_ang = recs[i].ArenaAng;
	
	for (i = n_recs - 1; i > end; --i) {
		recs[i].red_x = last_red_x;
		recs[i].red_y = last_red_y;
		recs[i].green_x = last_green_x;
		recs[i].green_y = last_green_y;
		recs[i].RoomX = last_room_x;
		recs[i].RoomY = last_room_y;
		recs[i].RoomAng = last_room_ang;
		recs[i].ArenaX = last_arena_x;
		recs[i].ArenaY = last_arena_y;
		recs[i].ArenaAng = last_arena_ang;
	}

	/* take care of beginning */
	for (i = 0; i < n_recs; ++i)
		if (recs[i].type == PositionRec)
			break;
	beginning = i;
	next_red_x = recs[i].red_x;
	next_red_y = recs[i].red_y;
	next_green_x = recs[i].green_x;
	next_green_y = recs[i].green_y;
	next_room_x = recs[i].RoomX;
	next_room_y = recs[i].RoomY;
	next_room_ang = recs[i].RoomAng;
	next_arena_x = recs[i].ArenaX;
	next_arena_y = recs[i].ArenaY;
	next_arena_ang = recs[i].ArenaAng;
	
	for (i = 0; i < beginning; ++i) {
		recs[i].red_x = next_red_x;
		recs[i].red_y = next_red_y;
		recs[i].green_x = next_green_x;
		recs[i].green_y = next_green_y;
		recs[i].RoomX = next_room_x;
		recs[i].RoomY = next_room_y;
		recs[i].RoomAng = next_room_ang;
		recs[i].ArenaX = next_arena_x;
		recs[i].ArenaY = next_arena_y;
		recs[i].ArenaAng = next_arena_ang;
	}

	/* setup to fill in rest */
	last_red_x = next_red_x;
	last_red_y = next_red_y;
	last_green_x = next_green_x;
	last_green_y = next_green_y;
	last_room_x = next_room_x;
	last_room_y = next_room_y;
	last_room_ang = next_room_ang;
	last_arena_x = next_arena_x;
	last_arena_y = next_arena_y;
	last_arena_ang = next_arena_ang;

	last_pos_time = recs[i].time_stamp;
	for (i = beginning + 1; i < n_recs; ++i)
		if (recs[i].type == PositionRec)
			break;
	next_red_x = recs[i].red_x;
	next_red_y = recs[i].red_y;
	next_green_x = recs[i].green_x;
	next_green_y = recs[i].green_y;
	next_room_x = recs[i].RoomX;
	next_room_y = recs[i].RoomY;
	next_room_ang = recs[i].RoomAng;
	next_arena_x = recs[i].ArenaX;
	next_arena_y = recs[i].ArenaY;
	next_arena_ang = recs[i].ArenaAng;
	next_pos_time = recs[i].time_stamp;

	/* fill in rest */
	for (i = beginning + 1; i < end; ++i) {
		if (recs[i].type == PositionRec) {
			last_red_x = next_red_x;
			last_red_y = next_red_y;
			last_green_x = next_green_x;
			last_green_y = next_green_y;
			last_room_x = next_room_x;
			last_room_y = next_room_y;
			last_room_ang = next_room_ang;
			last_arena_x = next_arena_x;
			last_arena_y = next_arena_y;
			last_arena_ang = next_arena_ang;

			last_pos_time = next_pos_time;
			for (j = i + 1; j <= end; ++j)
				if (recs[j].type == PositionRec)
					break;
			next_red_x = recs[j].red_x;
			next_red_y = recs[j].red_y;
			next_green_x = recs[j].green_x;
			next_green_y = recs[j].green_y;
			next_room_x = recs[j].RoomX;
			next_room_y = recs[j].RoomY;
			next_room_ang = recs[j].RoomAng;
			next_arena_x = recs[j].ArenaX;
			next_arena_y = recs[j].ArenaY;
			next_arena_ang = recs[j].ArenaAng;

			next_pos_time = recs[j].time_stamp;
		} else {
			ltd = recs[i].time_stamp - last_pos_time;
			ntd = next_pos_time - recs[i].time_stamp;
			if (ltd < ntd) {
				recs[i].red_x = last_red_x;
				recs[i].red_y = last_red_y;
				recs[i].green_x = last_green_x;
				recs[i].green_y = last_green_y;
				recs[i].RoomX = last_room_x;
				recs[i].RoomY = last_room_y;
				recs[i].RoomAng = last_room_ang;
				recs[i].ArenaX = last_arena_x;
				recs[i].ArenaY = last_arena_y;
				recs[i].ArenaAng = last_arena_ang;
			} else {
				recs[i].red_x = next_red_x;
				recs[i].red_y = next_red_y;
				recs[i].green_x = next_green_x;
				recs[i].green_y = next_green_y;
				recs[i].RoomX = next_room_x;
				recs[i].RoomY = next_room_y;
				recs[i].RoomAng = next_room_ang;
				recs[i].ArenaX = next_arena_x;
				recs[i].ArenaY = next_arena_y;
				recs[i].ArenaAng = next_arena_ang;
			}
		}
	}


	return;
}
