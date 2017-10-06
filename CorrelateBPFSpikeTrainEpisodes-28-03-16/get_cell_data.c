
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	get_cell_data(sess)
SESSION	*sess;
{
	si4		i, j, k, k2, n_units, area, active_pixels, active_pixels2;
	ui2		*spike, *spike2;
	CELL		*cp, *cp2;
	sf4		*time, *time2, *rate, *rate2, *arr1, *arr2, *arr21, *arr22;
	sf8		total_secs;
	extern void	*gmem(), correl(), coher();


	area = sess->params->area;
/*	arr1 = (sf4 *) gmem((size_t) area, sizeof(sf4));
	arr2 = (sf4 *) gmem((size_t) area, sizeof(sf4));
	arr21 = (sf4 *) gmem((size_t) area, sizeof(sf4));
	arr22 = (sf4 *) gmem((size_t) area, sizeof(sf4));
*/

	time = sess->time->map[0];
	time2 = sess->secondary_time->map[0];
	total_secs = sess->time->total_secs;

/*
	for (i = j = 0; i < area; ++i){
		if (time[i]){
			arr1[j++] = time[i];
		}
		if (time2[i]){
			arr21[j++] = time2[i];
		}
	}
*/

	n_units = sess->num_cells + sess->num_probes;

	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];
		cp2 = sess->secondary_cells[i];
		cp->grand_rate = (sf8) cp->total_spikes / total_secs;
		cp2->grand_rate = (sf8) cp2->total_spikes / total_secs;
		active_pixels = 0;
		active_pixels2 = 0;

		if(sess->opts->no_pos_samp == FALSE){
			rate = cp->rate_map[0];
			rate2 = cp2->rate_map[0];
			spike = cp->spike_map[0];
			spike2 = cp2->spike_map[0];
			for (j = k = k2 = 0; j < area; ++j){
				if (time[j]){
					// arr2[k++] = rate[j];
					if(rate[j] > 0.0)
						active_pixels++;
				}
				if (time2[j]){
					// arr22[k2++] = rate2[j];
					if(rate2[j] > 0.0)
						active_pixels2++;
				}
			}
			cp->p_active_pixels = (sf8)(active_pixels) / (sf8)(sess->apparatus->total_pixels);
			cp2->p_active_pixels = (sf8)(active_pixels2) / (sf8)(sess->secondary_apparatus->total_pixels);
			// correl(arr1, arr2, k, &cp->rate_time_r, &cp->rate_time_z, &cp->rate_time_p);
			// correl(arr21, arr22, k2, &cp2->rate_time_r, &cp2->rate_time_z, &cp2->rate_time_p);
		}
	}
	// p_burst_spikes(sess);		// calculate the proportion of spikes in bursts

/*
	free((void *) arr1);
	free((void *) arr2);
	free((void *) arr21);
	free((void *) arr22);
*/

	// if(sess->opts->no_pos_samp == FALSE)
	// 	coher(sess);

	return;
}
