
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	allocate_memory(sess)
SESSION	*sess;
{
	si4		i, j, k, n_units, x_size, y_size, area; 
	TIME		*tp, *tp2;
	SPEED		*sp, *sp2;
	CELL		*cp, *cp2;
	APPARATUS	*ap, *ap2;
	extern void	*gmem();
	ui1		**test;

	x_size = sess->params->x_size;
	y_size = sess->params->y_size;
	area = sess->params->area;
	
	/* do cells and probes */
	/* note the trick used here is to put the probes at the end of the cells */
	/* whole set can be done as one array - this will be used in later functions as well */

	n_units = sess->num_cells + sess->num_probes;

	// this memory was  already allocated in allocate_for_smoothing
	// sess->cells = (CELL **) gmem((size_t) n_units, sizeof(CELL *));
	// sess->cells[0] = (CELL *) gmem((size_t) n_units, sizeof(CELL));
	// for (i = 1; i < n_units; i++)
	//	sess->cells[i] = sess->cells[0] + i;
	// sess->probes = sess->cells + sess->num_cells;
	// sess->time = (TIME *) gmem((size_t) 1, sizeof(TIME));
	// sess->apparatus = (APPARATUS *) gmem((size_t) 1, sizeof(APPARATUS));

	for (i = 0; i < n_units; i++) {
		cp = sess->cells[i];
		cp2 = sess->secondary_cells[i];
		if (sess->directives->make_rate_maps) {
			cp->rate_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
			cp2->rate_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
			cp->rate_map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
			cp2->rate_map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
			cp->spike_map = (ui2 **) gmem((size_t) y_size, sizeof(ui2 *));
			cp2->spike_map = (ui2 **) gmem((size_t) y_size, sizeof(ui2 *));
			cp->spike_map[0] = (ui2 *) gmem((size_t) area, sizeof(ui2));
			cp2->spike_map[0] = (ui2 *) gmem((size_t) area, sizeof(ui2));
			for (j = 1, k = x_size; j < y_size; ++j, k += x_size) {	
				cp->rate_map[j] = cp->rate_map[0] + k;
				cp2->rate_map[j] = cp2->rate_map[0] + k;
				cp->spike_map[j] = cp->spike_map[0] + k;
				cp2->spike_map[j] = cp2->spike_map[0] + k;
			}
		}
		if (sess->directives->make_show_rate_maps) {
			cp->show_rate_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
			cp2->show_rate_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
			cp->show_rate_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
			cp2->show_rate_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
			for (j = 1, k = x_size; j < y_size; ++j, k += x_size){
				cp->show_rate_map[j] = cp->show_rate_map[0] + k;
				cp2->show_rate_map[j] = cp2->show_rate_map[0] + k;
			}
		}
	}


	/* do time */
	if (sess->directives->make_time_map) {
		tp = sess->time;
		tp2 = sess->secondary_time;
		tp->map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
		tp2->map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
		tp->map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
		tp2->map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
		for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
			tp->map[i] = tp->map[0] + j;
			tp2->map[i] = tp2->map[0] + j;
		}

		if (sess->opts->time_map) {
			tp->show_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
			tp2->show_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
			tp->show_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
			tp2->show_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
			for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
				tp->show_map[i] = tp->show_map[0] + j;
				tp2->show_map[i] = tp2->show_map[0] + j;
			}
		}
	}

	/* do apparatus */
	ap = sess->apparatus;
	ap2 = sess->secondary_apparatus;
	ap->map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
	ap2->map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
	ap->map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
	ap2->map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
	for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
		ap->map[i] = ap->map[0] + j;
		ap2->map[i] = ap2->map[0] + j;
	}

	/* do speed */
	if (sess->directives->calc_speed) {
		sp = sess->speed = (SPEED *) gmem((size_t) 1, sizeof(SPEED));
		sp2 = sess->secondary_speed = (SPEED *) gmem((size_t) 1, sizeof(SPEED));
		if (sess->directives->make_speed_map) {
			sp->map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
			sp2->map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
			sp->map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
			sp2->map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
			for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
				sp->map[i] = sp->map[0] + j;
				sp2->map[i] = sp2->map[0] + j;
			}
			if (sess->opts->speed_map) {
				sp->show_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
				sp2->show_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
				sp->show_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
				sp2->show_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
				for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
					sp->show_map[i] = sp->show_map[0] + j;
					sp2->show_map[i] = sp->show_map[0] + j;
				}
			}
		}
	}

	return;
}

void	allocate_memory_for_smoothing(sess)
SESSION	*sess;
{
	si4		i, j, k, n_units, x_size, y_size, area;
	TIME		*tp, *tp2;
	SPEED		*sp, *sp2;
	CELL		*cp, *cp2;
	APPARATUS	*ap, *ap2;
	extern void	*gmem();

	/* do cells and probes */
	/* note the trick used here is to put the probes at the end of the cells */
	/* whole set can be done as one array - this will be used in later functions as well */

	// allocate the memory for structures that contain the maps
	n_units = sess->num_cells + sess->num_probes;
	sess->cells = (CELL **) gmem((size_t) n_units, sizeof(CELL *));
	sess->secondary_cells = (CELL **) gmem((size_t) n_units, sizeof(CELL *)); // second reference frame
	sess->cells[0] = (CELL *) gmem((size_t) n_units, sizeof(CELL));
	sess->secondary_cells[0] = (CELL *) gmem((size_t) n_units, sizeof(CELL));
	for (i = 1; i < n_units; i++){
		sess->cells[i] = sess->cells[0] + i;
		sess->secondary_cells[i] = sess->secondary_cells[0] + i;
	}
	sess->probes = sess->cells + sess->num_cells;
	sess->secondary_probes = sess->secondary_cells + sess->num_cells;
	sess->time = (TIME *) gmem((size_t) 1, sizeof(TIME));
	sess->secondary_time = (TIME *) gmem((size_t) 1, sizeof(TIME));
	sess->apparatus = (APPARATUS *) gmem((size_t) 1, sizeof(APPARATUS));
	sess->secondary_apparatus = (APPARATUS *) gmem((size_t) 1, sizeof(APPARATUS));

	if (!sess->directives->make_smoothed_maps) 
		return;

	// the following allocations are only for smoothed maps

	for (i = 0; i < n_units; i++) {
		cp = sess->cells[i];
		cp2 = sess->secondary_cells[i];

		// coarse resolution maps
		y_size = sess->params->y_size;
		x_size = sess->params->x_size;
		area = y_size * x_size;

		cp->smoothed_spike_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
		cp2->smoothed_spike_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
		cp->smoothed_spike_map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
		cp2->smoothed_spike_map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
		for (j = 1, k = x_size; j < y_size; ++j, k += x_size){
			cp->smoothed_spike_map[j] = cp->smoothed_spike_map[0] + k;
			cp2->smoothed_spike_map[j] = cp2->smoothed_spike_map[0] + k;
		}
		cp->smoothed_rate_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
		cp2->smoothed_rate_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
		cp->smoothed_rate_map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
		cp2->smoothed_rate_map[0] = (sf4 *) gmem((size_t) area, sizeof(sf4));
		for (j = 1, k = x_size; j < y_size; ++j, k += x_size){
			cp->smoothed_rate_map[j] = cp->smoothed_rate_map[0] + k;
			cp2->smoothed_rate_map[j] = cp2->smoothed_rate_map[0] + k;
		}
		if (sess->opts->smoothed_rate_maps) {
			cp->show_smoothed_rate_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
			cp2->show_smoothed_rate_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
			cp->show_smoothed_rate_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
			cp2->show_smoothed_rate_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
			for (j = 1, k = x_size; j < y_size; ++j, k += x_size){
				cp->show_smoothed_rate_map[j] = cp->show_smoothed_rate_map[0] + k;
				cp2->show_smoothed_rate_map[j] = cp2->show_smoothed_rate_map[0] + k;
			}
		}

		// maps for fine resolution smoothing
		y_size = sess->params->y_tracker_resolution;
		x_size = sess->params->x_tracker_resolution;
		area = y_size * x_size;

		cp->fine_spike_map = (ui2 **) gmem((size_t) y_size, sizeof(ui2 *));
		cp2->fine_spike_map = (ui2 **) gmem((size_t) y_size, sizeof(ui2 *));
		cp->fine_spike_map[0] = (ui2 *) gmem((size_t) area, sizeof(ui2));
		cp2->fine_spike_map[0] = (ui2 *) gmem((size_t) area, sizeof(ui2));
		for (j = 1, k = x_size; j < y_size; ++j, k += x_size){
			cp->fine_spike_map[j] = cp->fine_spike_map[0] + k;
			cp2->fine_spike_map[j] = cp2->fine_spike_map[0] + k;
		}
	}

	/* do time */
	tp = sess->time;
	tp2 = sess->secondary_time;
	// coarse resolution maps
	y_size = sess->params->y_size;
	x_size = sess->params->x_size;
	area = y_size * x_size;

	tp->smoothed_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
	tp2->smoothed_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
	tp->smoothed_map[0] = (sf4 *) gmem((size_t) area,  sizeof(sf4));
	tp2->smoothed_map[0] = (sf4 *) gmem((size_t) area,  sizeof(sf4));
	for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
		tp->smoothed_map[i] = tp->smoothed_map[0] + j;
		tp2->smoothed_map[i] = tp2->smoothed_map[0] + j;
	}
	

	// maps for fine resolution smoothing
	y_size = sess->params->y_tracker_resolution;
	x_size = sess->params->x_tracker_resolution;
	area = y_size * x_size;

	tp->fine_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
	tp2->fine_map = (sf4 **) gmem((size_t) y_size, sizeof(sf4 *));
	tp->fine_map[0] = (sf4 *) gmem((size_t) area,  sizeof(sf4));
	tp2->fine_map[0] = (sf4 *) gmem((size_t) area,  sizeof(sf4));
	for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
		tp->fine_map[i] = tp->fine_map[0] + j;
		tp2->fine_map[i] = tp2->fine_map[0] + j;
	}
	
	/* do apparatus */
	ap = sess->apparatus;
	ap2 = sess->secondary_apparatus;

	// coarse resolution maps
	y_size = sess->params->y_size;
	x_size = sess->params->x_size;
	area = y_size * x_size;

	ap->smoothed_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
	ap2->smoothed_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
	ap->smoothed_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
	ap2->smoothed_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));
	for (i = 1, j = x_size; i < y_size; ++i, j += x_size){
		ap2->smoothed_map[i] = ap2->smoothed_map[0] + j;
	}

	return;
}
