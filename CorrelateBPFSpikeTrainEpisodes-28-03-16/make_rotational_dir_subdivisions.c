
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
// #include <values.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_rotational_dir_subdivisions(sess)
SESSION	*sess;
{
	si4		i, j, n_rot_dirs, n_units;
	SESSION		*dvsn;
	extern void	allocate_memory();
	extern void	clean_spike_maps();
	extern void	clean_time_map();
	extern void	*gmem();
	extern void	name_cells();
	extern void	make_show_rate_maps();
	extern void	make_rate_maps();
	extern void	make_spike_maps();
	extern void	make_time_map();
	 void	separate_rot_dir_recs();
	 void	set_rot_dir_opts_and_params();


	n_rot_dirs = NUM_ROT_DIRS;
	n_units = sess->num_cells + sess->num_probes;

	sess->rotational_dir_subdivisions = (SESSION **) gmem((size_t) n_rot_dirs, sizeof(SESSION *));
	for (i = 0; i < n_rot_dirs; ++i) {
		sess->rotational_dir_subdivisions[i] = (SESSION *) gmem((size_t) 1, sizeof(SESSION));
		*(sess->rotational_dir_subdivisions[i]) = *sess;
	}

	separate_rot_dir_recs(sess);

	for (i = 0; i < n_rot_dirs; ++i) {

		dvsn = sess->rotational_dir_subdivisions[i];

		set_rot_dir_opts_and_params(sess, dvsn);

		allocate_memory(dvsn);

		name_cells(dvsn);

		dvsn->time->threshold = sess->time->threshold;
		for (j = 0; j < n_units; ++j)
			dvsn->cells[j]->rate_thresh = sess->cells[j]->rate_thresh;

		make_time_map(dvsn);

		dvsn->apparatus->map = sess->apparatus->map;
	
		clean_time_map(dvsn);

		make_spike_maps(dvsn);

		clean_spike_maps(dvsn);

		make_rate_maps(dvsn);

		make_show_rate_maps(dvsn);
	}

	return;
}


 void	separate_rot_dir_recs(sess)
SESSION		*sess;
{
	si4		i, j, n_recs, *n_new_recs;
	RECORD		*recs, **new_recs;
	sf8		ang, cent_x, cent_y, y, x;
	si2		dir;
	extern void	*gmem();
	
	/* this should really be done before the pixels are collapsed */
	/* but as of this writing I don't know if it will be worth the */
	/* extra programming */

	n_recs = sess->num_recs;
	recs = sess->recs;
	n_new_recs = (si4 *) gmem((size_t) NUM_ROT_DIRS, sizeof(si4));
	cent_x = (sf8) sess->apparatus->center_x;
	cent_y = (sf8) sess->apparatus->center_y;
	
	for (i = 0; i < n_recs; ++i) {
		if ((dir = recs[i].direction) == NO_DIRECTION)
			continue;
		if(sess->opts->bpf_room_file){
			x = (sf8) recs[i].RoomX - cent_x;
			y = (sf8) recs[i].RoomY - cent_y;
		}else if(sess->opts->bpf_arena_file){
			x = (sf8) recs[i].ArenaX - cent_x;
			y = (sf8) recs[i].ArenaY - cent_y;
		}else{
			x = (sf8) recs[i].red_x - cent_x;
			y = (sf8) recs[i].red_y - cent_y;
		}
		if (x == 00.0 && y == 0.0)
			continue;
		ang = atan2(y, x) * 180.0 / M_PI;
		ang = (sf8) dir - ang;
		if (ang < 0.0)
			ang += 360.0;
		else if (ang >= 360.0)
			ang -= 360.0;
		if (ang < 180.0)
			++n_new_recs[COUNTERCLOCKWISE];	
		else
			++n_new_recs[CLOCKWISE];	
	}
	
	new_recs = (RECORD **) gmem((size_t) NUM_ROT_DIRS, sizeof(RECORD *));
	for (i = 0; i < NUM_ROT_DIRS; ++i) {
		new_recs[i] = sess->rotational_dir_subdivisions[i]->recs = (RECORD *) gmem((size_t) n_new_recs[i], sizeof(RECORD));
		sess->rotational_dir_subdivisions[i]->num_recs = n_new_recs[i];
		n_new_recs[i] = 0;
	}

	for (i = 0; i < n_recs; ++i) {
		if ((dir = recs[i].direction) == NO_DIRECTION)
			continue;
		if(sess->opts->bpf_room_file){
                        x = (sf8) recs[i].RoomX - cent_x;
                        y = (sf8) recs[i].RoomY - cent_y;
                }else if(sess->opts->bpf_arena_file){
                        x = (sf8) recs[i].ArenaX - cent_x;
                        y = (sf8) recs[i].ArenaY - cent_y;
                }else{  
			x = (sf8) recs[i].red_x - cent_x;
			y = (sf8) recs[i].red_y - cent_y;
		}
		if (x == 00.0 && y == 0.0)
			continue;
		ang = atan2(y, x) * 180.0 / M_PI;
		ang = (sf8) dir - ang;
		if (ang < 0.0)
			ang += 360.0;
		else if (ang >= 360.0)
			ang -= 360.0;
		if (ang < 180.0)
			new_recs[COUNTERCLOCKWISE][n_new_recs[COUNTERCLOCKWISE]++] = recs[i];	
		else
			new_recs[CLOCKWISE][n_new_recs[CLOCKWISE]++] = recs[i];	
	}

	free((void *) n_new_recs);
	free((void *) new_recs);


	return;
}


 void	set_rot_dir_opts_and_params(sess, dvsn)
SESSION		*sess, *dvsn;
{
	OPTIONS		*opts;
	extern void	*gmem();


	opts = dvsn->opts = (OPTIONS *) gmem((size_t) 1, sizeof(OPTIONS));
	*opts = *sess->opts;
	 
	opts->write_breakpoints = FALSE;
	opts->read_breakpoints = TRUE;

	dvsn->params = (PARAMETERS *) gmem((size_t) 1, sizeof(PARAMETERS));
	*dvsn->params = *sess->params;

	dvsn->params->read_bkpts_file = sess->params->write_bkpts_file;
	dvsn->params->write_bkpts_file = "";

	
	return;
}

