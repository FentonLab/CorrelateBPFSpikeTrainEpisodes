
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	set_thresholds(sess)
SESSION	*sess;
{
	si4		i, n_units;
	extern void	rate_thresh_dialog(), smoothed_rate_thresh_dialog();

	n_units = sess->num_cells + sess->num_probes;

	sess->time->threshold = sess->params->global_time_thresh;
	sess->secondary_time->threshold = sess->params->global_time_thresh;
	if (sess->directives->calc_speed){
		sess->speed->threshold = sess->params->global_speed_thresh;
		sess->secondary_speed->threshold = sess->params->global_speed_thresh;
	}

	for (i = 0; i < n_units; ++i) {
		sess->cells[i]->rate_thresh = sess->params->global_rate_thresh;
		sess->secondary_cells[i]->rate_thresh = sess->params->global_rate_thresh;
		sess->cells[i]->smoothed_rate_thresh = sess->params->global_smoothed_rate_thresh;
		sess->secondary_cells[i]->smoothed_rate_thresh = sess->params->global_smoothed_rate_thresh;
	}
	
	if (sess->opts->change_rate_thresh)
		rate_thresh_dialog(sess);
	if (sess->opts->change_smoothed_rate_thresh)
		smoothed_rate_thresh_dialog(sess);
		
	return;
}
