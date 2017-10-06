
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_directional_subdivisions(sess)
SESSION	*sess;
{
	si4		i, j, n_dir_bins, n_units;
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
	 void	separate_direc_recs();
	 void	set_direc_opts_and_params();


	n_dir_bins = sess->params->map_dir_bins;
	n_units = sess->num_cells + sess->num_probes;

	sess->directional_subdivisions = (SESSION **) gmem((size_t) n_dir_bins, sizeof(SESSION *));
	for (i = 0; i < n_dir_bins; ++i) {
		sess->directional_subdivisions[i] = (SESSION *) gmem((size_t) 1, sizeof(SESSION));
		*(sess->directional_subdivisions[i]) = *sess;
	}

	separate_direc_recs(sess);

	for (i = 0; i < n_dir_bins; ++i) {

		dvsn = sess->directional_subdivisions[i];

		set_direc_opts_and_params(sess, dvsn);

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


 void	separate_direc_recs(sess)
SESSION		*sess;
{
	si4		i, j, n_recs, n_bins, *n_new_recs;
	RECORD		*recs, **new_recs;
	sf8		ang_inc, ang_start;
	si2		*max_angs, dir;
	extern void	*gmem();


	n_bins = sess->params->map_dir_bins;

	max_angs = (si2 *) gmem((size_t) n_bins, sizeof(si2));
	ang_inc = 360.0 / (sf8) n_bins;
	ang_start = ang_inc / 2.0;
	for (i = 0; i < n_bins; ++i) {
		max_angs[i] = (si2) ang_start;
		ang_start += ang_inc;
	}

	n_recs = sess->num_recs;
	recs = sess->recs;
	n_new_recs = (si4 *) gmem((size_t) n_bins, sizeof(si4));
	
	for (i = 0; i < n_recs; ++i) {
		if ((dir = recs[i].direction) == NO_DIRECTION)
			continue;
		for (j = 0; j < n_bins; ++j) {
			if (dir <= max_angs[j])
				break;
		}
		++n_new_recs[j % n_bins];	
	}
	
	new_recs = (RECORD **) gmem((size_t) n_bins, sizeof(RECORD *));
	for (i = 0; i < n_bins; ++i) {
		new_recs[i] = sess->directional_subdivisions[i]->recs = (RECORD *) gmem((size_t) n_new_recs[i], sizeof(RECORD));
		sess->directional_subdivisions[i]->num_recs = n_new_recs[i];
		n_new_recs[i] = 0;
	}

	for (i = 0; i < n_recs; ++i) {
		if ((dir = recs[i].direction) == NO_DIRECTION)
			continue;
		for (j = 0; j < n_bins; ++j) {
			if (dir <= max_angs[j])
				break;
		}
		j %= n_bins;	
		new_recs[j][n_new_recs[j]++] = recs[i];
	}

	free((void *) n_new_recs);
	free((void *) new_recs);
	free((void *) max_angs);


	return;
}


 void	set_direc_opts_and_params(sess, dvsn)
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

