
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_rate_maps(sess)
SESSION	*sess;
{
	si4	i, j, n_units, area;
	CELL	*cp;	
	ui2	*sm;
	sf4	*tm, *rm, thresh;

        area = sess->params->area;

	n_units = sess->num_cells + sess->num_probes;
	tm = sess->time->map[0];

	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];
		sm = cp->spike_map[0];
		rm = cp->rate_map[0];
		thresh = cp->rate_thresh;
		for (j = 0; j < area; ++j) {
			if (tm[j] > 0.0) {
				rm[j] = (sf4) sm[j] / tm[j];
				if (rm[j] < thresh)
					rm[j] = 0.0;
			} else
				rm[j] = NO_RATE;
		}
	}

	tm = sess->secondary_time->map[0];

	for (i = 0; i < n_units; ++i) {
		cp = sess->secondary_cells[i];
		sm = cp->spike_map[0];
		rm = cp->rate_map[0];
		thresh = cp->rate_thresh;
		for (j = 0; j < area; ++j) {
			if (tm[j] > 0.0) {
				rm[j] = (sf4) sm[j] / tm[j];
				if (rm[j] < thresh)
					rm[j] = 0.0;
			} else
				rm[j] = NO_RATE;
		}
	}
	return;
}
