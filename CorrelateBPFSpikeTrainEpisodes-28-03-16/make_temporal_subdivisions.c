
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void	make_temporal_subdivisions(sess)
SESSION	*sess;
{
	si4		i, j, n_time_bins, n_units, minutes;
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
	 void	separate_time_recs();
	 void	set_time_opts_and_params();

	minutes = (si4) ((sess->time->total_secs / 60.0) + 0.5);
	sess->params->time_bins = n_time_bins = (si4) ceil((sf8) minutes / sess->params->time_division_duration);
	n_units = sess->num_cells + sess->num_probes;

	sess->temporal_subdivisions = (SESSION **) gmem((size_t) n_time_bins, sizeof(SESSION *));
	for (i = 0; i < n_time_bins; ++i) {
		sess->temporal_subdivisions[i] = (SESSION *) gmem((size_t) 1, sizeof(SESSION));
		*(sess->temporal_subdivisions[i]) = *sess;
	}

	separate_time_recs(sess);

	for (i = 0; i < n_time_bins; ++i) {

		dvsn = sess->temporal_subdivisions[i];

		set_time_opts_and_params(sess, dvsn);

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


 void	separate_time_recs(sess)
SESSION		*sess;
{
	si4		i, bin, n_recs, n_bins, pos_recs_per_bin, *n_new_recs;
	RECORD		*recs, **new_recs;
	si4		n_pos_recs;
	extern void	*gmem();
	ui1		PositionRec;


	n_recs = sess->num_recs;
	recs = sess->recs;
	n_bins = sess->params->time_bins;
	n_new_recs = (si4 *) gmem((size_t) n_bins, sizeof(si4));
	pos_recs_per_bin = (ui4) (sess->params->time_division_duration * 60.0 * sess->params->position_samp_freq);
	PositionRec = sess->PositionRec;

	n_pos_recs = 0;
	for (i = bin = 0; i < n_recs; ++i) {
		if (recs[i].type == PositionRec)
			if (n_pos_recs++ == pos_recs_per_bin) {
				++bin;
				n_pos_recs = 0;
			}
		++n_new_recs[bin];
	}
	
	new_recs = (RECORD **) gmem((size_t) n_bins, sizeof(RECORD *));
	for (i = 0; i < n_bins; ++i) {
		new_recs[i] = sess->temporal_subdivisions[i]->recs = (RECORD *) gmem((size_t) n_new_recs[i], sizeof(RECORD));
		sess->temporal_subdivisions[i]->num_recs = n_new_recs[i];
		n_new_recs[i] = 0;
	}

	n_pos_recs = 0;
	for (i = bin = 0; i < n_recs; ++i) {
		if (recs[i].type == PositionRec)
			if (n_pos_recs++ == pos_recs_per_bin) {
				++bin;
				n_pos_recs = 0;
			}
		new_recs[bin][n_new_recs[bin]++] = recs[i];
	}
	
	free((void *) n_new_recs);
	free((void *) new_recs);


	return;
}


 void	set_time_opts_and_params(sess, dvsn)
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

