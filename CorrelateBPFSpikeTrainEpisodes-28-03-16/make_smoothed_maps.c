
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_smoothed_maps(sess)
SESSION	*sess;
{
	si4	X, Y, x, y, delta, min_y, min_x, max_y, max_x, origin_x, origin_y;
	si4	n_units, x_size, y_size, y_tracker_resolution, x_tracker_resolution;
	CELL	*cp, *cp2;	
	sf4	**ssm, **ssm2;
	ui2	**sm, **sm2;
	ui1	PositionRec;
	sf4	**tm, **tm2, **stm, **stm2, **srm, **srm2, thresh, thresh2, pcf, samps_per_sec, **weight, distance, sum;
	si4	i, n_recs, c_num, p_num;
	si1	**c_map, *p_map;
	RECORD	*rec;

	n_recs = sess->num_recs;
	c_map = sess->cell_map;
	p_map = sess->probe_map;
	tm = sess->time->fine_map;
	tm2 = sess->secondary_time->fine_map;
	PositionRec = sess->PositionRec;

	// fill in the fine resolution time and spike maps
	rec = sess->recs;
	for (i = 0; i < n_recs; ++i) {
		if (is_spike(*sess, rec->type)) {
			p_num = p_map[rec->probe];
			if (p_num != NOT_CUT) {
				sm = sess->probes[p_num]->fine_spike_map;
				sm2 = sess->secondary_probes[p_num]->fine_spike_map;
				if(sess->opts->bpf_room_file){
					++sm[rec->RoomY][rec->RoomX];		
					++sm2[rec->ArenaY][rec->ArenaX];		
				}else if(sess->opts->bpf_arena_file){
					++sm[rec->ArenaY][rec->ArenaX];		
					++sm2[rec->RoomY][rec->RoomX];		
				}else{
					++sm[rec->red_y][rec->red_x];		
					++sm2[rec->green_y][rec->green_x];		
				}
			}
			c_num = c_map[rec->probe][rec->clust];
			if (c_num != NOT_CUT) {
				sm = sess->cells[c_num]->fine_spike_map;
				sm2 = sess->secondary_cells[c_num]->fine_spike_map;
				if(sess->opts->bpf_room_file){
					++sm[rec->RoomY][rec->RoomX];		
					++sm2[rec->ArenaY][rec->ArenaX];		
				}
				else if(sess->opts->bpf_arena_file){
					++sm[rec->ArenaY][rec->ArenaX];		
					++sm2[rec->RoomY][rec->RoomX];		
				}else{
					++sm[rec->red_y][rec->red_x];		
					++sm2[rec->green_y][rec->green_x];		
				}
			}
		}else if (rec->type == PositionRec){
			if(sess->opts->bpf_room_file){
				++tm[rec->RoomY][rec->RoomX];		
				++tm2[rec->ArenaY][rec->ArenaX];		
			}else if(sess->opts->bpf_arena_file){
				++tm[rec->ArenaY][rec->ArenaX];		
				++tm2[rec->RoomY][rec->RoomX];		
			}else{
				++tm[rec->red_y][rec->red_x];		
				++tm2[rec->green_y][rec->green_x];		
			}
		}
		++rec;
	}	

        x_size = sess->params->x_size - 1;
        y_size = sess->params->y_size - 1;
        pcf = sess->params->pixel_collapse_factor;
        delta = (si4)ceil(pcf * sess->params->smoothing_factor);
	y_tracker_resolution = sess->params->y_tracker_resolution;
	x_tracker_resolution = sess->params->x_tracker_resolution;
	samps_per_sec = sess->params->position_samp_freq;

	n_units = sess->num_cells + sess->num_probes;
	stm = sess->time->smoothed_map;
	stm2 = sess->secondary_time->smoothed_map;

	// set weight matrix for smoothing
	weight = (sf4 **) gmem((size_t) delta + 1, sizeof(sf4 *));
	for(y=0; y < delta + 1; y++){
		weight[y] = (sf4 *) gmem((size_t) delta + 1, sizeof(sf4));
	}

	sum = 0.0;
	for(y=0; y <= delta; y++){
		for(x=0; x <= delta; x++){
			distance = (sf4)hypot(y, x) + 0.5;
			if(distance > delta){
				weight[y][x] = 0.0;
				continue;
			}
			weight[y][x] = 1.0;
			// weight[y][x] = 1.0 / distance;
			sum += weight[y][x]; // changed 6_28_04
		}
	}
/*
	for(y=0; y < delta; y++){
		for(x=0; x < delta; x++){
			weight[y][x] /= sum;
			// printf("y=%d x=%d %f\n", y, x, weight[y][x]);
		}
	}
*/

	// create smoothed time map
	for(Y=0; Y < y_size; Y++){
		for(X=0; X < x_size; X++){
			origin_y = (Y * pcf);
			min_y = origin_y - delta;
			if(min_y < 0)
				min_y = 0;
			max_y = origin_y + delta;
			if(max_y > y_tracker_resolution)
				max_y = y_tracker_resolution;
			for(y = min_y; y < max_y; y++){
				origin_x = (X * pcf);
				min_x = origin_x - delta;
				if(min_x < 0)
					min_x = 0;
				max_x = origin_x + delta;
				if(max_x > x_tracker_resolution)
					max_x = x_tracker_resolution;

				for(x = min_x; x < max_x; x++){
					if(!x && !y)
						continue;
					stm[Y][X] += (tm[y][x] * weight[abs(y - origin_y)][abs(x - origin_x)]);
					stm2[Y][X] += (tm2[y][x] * weight[abs(y - origin_y)][abs(x - origin_x)]);
				}
			}
		}
	}


	// create smoothed map for each cell
	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];
		cp2 = sess->secondary_cells[i];
		ssm = cp->smoothed_spike_map;
		ssm2 = cp2->smoothed_spike_map;
		sm = cp->fine_spike_map;
		sm2 = cp2->fine_spike_map;
		srm = cp->smoothed_rate_map;
		srm2 = cp2->smoothed_rate_map;
		thresh = cp->smoothed_rate_thresh;
		thresh2 = cp2->smoothed_rate_thresh;

		for(Y=0; Y < y_size; Y++){
			for(X=0; X < x_size; X++){
				origin_y = (Y * pcf);
				min_y = origin_y - delta;
				if(min_y < 0)
					min_y = 0;
				max_y = origin_y + delta;
				if(max_y > y_tracker_resolution)
					max_y = y_tracker_resolution;
				for(y = min_y; y < max_y; y++){
					origin_x = (X * pcf);
					min_x = origin_x - delta;
					if(min_x < 0)
						min_x = 0;
					max_x = origin_x + delta;
					if(max_x > x_tracker_resolution)
						max_x = x_tracker_resolution;

					for(x = min_x; x < max_x; x++){
						if(!x && !y)
							continue;
						ssm[Y][X] += (sm[y][x] * weight[abs(y - origin_y)][abs(x - origin_x)]);
						ssm2[Y][X] += (sm2[y][x] * weight[abs(y - origin_y)][abs(x - origin_x)]);
					}
				}
			}
		}
		for(Y=0; Y < y_size; Y++){
			for(X=0; X < x_size; X++){
				if(stm[Y][X] > 0.0){
					srm[Y][X] = ssm[Y][X] / stm[Y][X] * samps_per_sec;
					srm2[Y][X] = ssm2[Y][X] / stm2[Y][X] * samps_per_sec;
				}else{
					srm[Y][X] = NO_RATE;
					srm2[Y][X] = NO_RATE;
				}
			}
		}
				
	}

	return;
}
