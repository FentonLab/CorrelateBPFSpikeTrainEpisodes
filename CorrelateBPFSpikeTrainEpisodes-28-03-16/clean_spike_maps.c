
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	clean_spike_maps(sess)
SESSION	*sess;
{
	si4	i, j, n_units, area;
	CELL	*cp;	
	ui2	*sm;
	ui1	*am;

        area = sess->params->area;

	n_units = sess->num_cells + sess->num_probes;
	am = sess->apparatus->map[0];

	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];
		sm = cp->spike_map[0];
		for (j = 0; j < area; ++j) {
			cp->total_spikes += sm[j];
			if (am[j]){
				cp->total_in_apparatus_spikes += (si4) sm[j];
			}else
				sm[j] = 0;
		}
	}
	for (i = 0; i < n_units; ++i) {
		cp = sess->secondary_cells[i];
		sm = cp->spike_map[0];
		for (j = 0; j < area; ++j) {
			cp->total_spikes += sm[j];
			if (am[j]){
				cp->total_in_apparatus_spikes += (si4) sm[j];
			}else
				sm[j] = 0;
		}
	}


	return;
}

void	clean_smoothed_maps(sess)
SESSION	*sess;
{
	si4	i, j, n_units, area;
	CELL	*cp;	
	sf4	*ssm;
	sf4	*stm, *srm;
	ui1	*am;

        area = sess->params->area;

	n_units = sess->num_cells + sess->num_probes;
	am = sess->apparatus->map[0];
	stm = sess->time->smoothed_map[0];

	for (j = 0; j < area; ++j) {
		if (!am[j]){
			stm[j] = 0.0;	
		}
	}

	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];
		ssm = cp->smoothed_spike_map[0];
		srm = cp->smoothed_rate_map[0];
		for (j = 0; j < area; ++j) {
			if (!am[j]){
				ssm[j] = 0;
				srm[j] = NO_RATE;
			}
		}
	}
	am = sess->secondary_apparatus->map[0];
	stm = sess->secondary_time->smoothed_map[0];

	for (j = 0; j < area; ++j) {
		if (!am[j]){
			stm[j] = 0.0;	
		}
	}

	for (i = 0; i < n_units; ++i) {
		cp = sess->secondary_cells[i];
		ssm = cp->smoothed_spike_map[0];
		srm = cp->smoothed_rate_map[0];
		for (j = 0; j < area; ++j) {
			if (!am[j]){
				ssm[j] = 0;
				srm[j] = NO_RATE;
			}
		}
	}

	return;
}
