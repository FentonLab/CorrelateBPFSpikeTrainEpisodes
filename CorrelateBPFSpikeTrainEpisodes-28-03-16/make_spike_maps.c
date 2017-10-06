
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void	make_spike_maps(sess)
SESSION	*sess;
{
	si4	i, n_recs, c_num, p_num;
	si1	**c_map, *p_map;
	RECORD	*rec;
	ui2	**sm;
	ui2	**sm2;


	n_recs = sess->num_recs;
	c_map = sess->cell_map;
	p_map = sess->probe_map;

	rec = sess->recs;
	for (i = 0; i < n_recs; ++i) {
		if (is_spike(*sess, rec->type)) {
			p_num = p_map[rec->probe];
			if (p_num != NOT_CUT) {
				sm = sess->probes[p_num]->spike_map;
				sm2 = sess->secondary_probes[p_num]->spike_map;
				if(sess->opts->bpf_room_file){
					++sm[rec->RoomY][rec->RoomX];		
					++sm2[rec->ArenaY][rec->ArenaX];		
				}else if(sess->opts->bpf_arena_file){
					++sm[rec->ArenaY][rec->ArenaX];		
					++sm2[rec->RoomY][rec->RoomX];		
				}else{
					++sm[rec->red_y][rec->red_x];		
					++sm2[rec->red_y][rec->red_x];		
				}
			}
			c_num = c_map[rec->probe][rec->clust];
			if (c_num != NOT_CUT) {
				sm = sess->cells[c_num]->spike_map;
				sm2 = sess->secondary_cells[c_num]->spike_map;
				if(sess->opts->bpf_room_file){
					++sm[rec->RoomY][rec->RoomX];		
					++sm2[rec->ArenaY][rec->ArenaX];		
				}else if(sess->opts->bpf_arena_file){
					++sm[rec->ArenaY][rec->ArenaX];		
					++sm2[rec->RoomY][rec->RoomX];		
				}else{
					++sm[rec->red_y][rec->red_x];		
					++sm2[rec->red_y][rec->red_x];		
				}
			}
		}
		++rec;
	}	

	return;
}

