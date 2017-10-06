
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	coher(sess)
SESSION	*sess;
{
	si4		i, j, k, l, m, area, x_size, y_size;
	si4		len, n_units, surr_spks;
	CELL		*cp;
	sf4		**time, **rate, *arr1, *arr2;
	ui2		**spks;
	sf8		surr_time;
	extern void	*gmem(), correl();

	if(sess->opts->no_pos_samp)	/* no point in calculating coherence */
		return;

	area = sess->params->area;
	x_size = sess->params->x_size;
	y_size = sess->params->y_size;

	arr1 = (sf4 *) gmem((size_t) area, sizeof(sf4));
	arr2 = (sf4 *) gmem((size_t) area, sizeof(sf4));

	time = sess->time->map;

	n_units = sess->num_cells + sess->num_probes;

	for (i = 0; i < n_units; ++i) {

		cp = sess->cells[i];
		rate = cp->rate_map;
		spks = cp->spike_map;
		
		len = 0;
		--y_size; --x_size;

		/* inside of map */
		for (j = 1; j < y_size; ++j) {
			for (k = 1; k < x_size; ++k) {
				if (time[j][k]) {
					surr_time = -((sf8) time[j][k]);
					surr_spks = -((si4) spks[j][k]);
					++j; ++k;
					for (l = j - 2; l <= j; ++l) {
						for (m = k - 2; m <= k; ++m) {
							surr_time += (sf8) time[l][m];
							surr_spks += (si4) spks[l][m];
						}
					}
					--j; --k;
					arr1[len] = rate[j][k];
					if(surr_time > 0.0)
						arr2[len] = (sf4) ((sf8) surr_spks / surr_time);
					else
						arr2[len] = 0.0;
					++len;
				}	
			}
		}

		/* top and bottom rows */
		for (k = 1; k < x_size; ++k) {
			if (time[0][k]) {
				surr_time = -((sf8) time[0][k]);
				surr_spks = -((si4) spks[0][k]);
				++k;
				for (l = 0; l <= 1; ++l) {
					for (m = k - 2; m <= k; ++m) {
						surr_time += (sf8) time[l][m];
						surr_spks += (si4) spks[l][m];
					}
				}
				--k;
				arr1[len] = rate[0][k];
				if(surr_time > 0.0)
					arr2[len] = (sf4) ((sf8) surr_spks / surr_time);
				else
					arr2[len] = 0.0;
				++len;
			}	
			if (time[y_size][k]) {
				surr_time = -((sf8) time[y_size][k]);
				surr_spks = -((si4) spks[y_size][k]);
				++k;
				for (l = y_size - 1; l <= y_size; ++l) {
					for (m = k - 2; m <= k; ++m) {
						surr_time += (sf8) time[l][m];
						surr_spks += (si4) spks[l][m];
					}
				}
				--k;
				arr1[len] = rate[0][k];
				if(surr_time > 0.0)
					arr2[len] = (sf4) ((sf8) surr_spks / surr_time);
				else
					arr2[len] = 0.0;
				++len;
			}	
		}


		/* left and right columns */
		for (j = 1; j < y_size; ++j) {
			if (time[j][0]) {
				surr_time = -((sf8) time[j][0]);
				surr_spks = -((si4) spks[j][0]);
				++j;
				for (l = j - 2; l <= j; ++l) {
					for (m = 0; m <= 1; ++m) {
						surr_time += (sf8) time[l][m];
						surr_spks += (si4) spks[l][m];
					}
				}
				--j;
				arr1[len] = rate[j][0];
				if(surr_time > 0.0)
					arr2[len] = (sf4) ((sf8) surr_spks / surr_time);
				else
					arr2[len] = 0.0;
				++len;
			}
			if (time[j][x_size]) {
				surr_time = -((sf8) time[j][x_size]);
				surr_spks = -((si4) spks[j][x_size]);
				++j;
				for (l = j - 2; l <= j; ++l) {
					for (m = x_size - 1; m <= x_size; ++m) {
						surr_time += (sf8) time[l][m];
						surr_spks += (si4) spks[l][m];
					}
				}
				--j;
				arr1[len] = rate[j][x_size];
				if(surr_time > 0.0)
					arr2[len] = (sf4) ((sf8) surr_spks / surr_time);
				else
					arr2[len]= 0.0;
				++len;
			}
		}

		/* corners */
		/* note changing the step size here would make this subsection work for the whole map */
		/* it's a tradeoff between efficiency and code size, I prefer efficiency - hence this */
		/* routine is huge - if this bugs you, just change it */
		for (j = 0; j <= y_size; j += y_size) {
			for (k = 0; k <= x_size; k += x_size) {
				if (time[j][k]) {
					surr_time = -((sf8) time[j][k]);
					surr_spks = -((si4) spks[j][k]);
					++j; ++k;
					for (l = j - 2; l <= j; ++l) {
						if (l < 0 || l > y_size)
							continue;
						for (m = k - 2; m <= k; ++m) {
							if (m < 0 || m > x_size)
								continue;
							surr_time += (sf8) time[l][m];
							surr_spks += (si4) spks[l][m];
						}
					}
					--j; --k;
					arr1[len] = rate[j][k];
					if(surr_time > 0.0)
						arr2[len] = (sf4) ((sf8) surr_spks / surr_time);
					else
						arr2[len] = 0.0;
					++len;
				}	
			}
		}


		++y_size; ++x_size;

		correl(arr1, arr2, len, &cp->rate_coher_r, &cp->rate_coher_z, &cp->rate_coher_p);
	}

	free((void *) arr1);
	free((void *) arr2);

	return;
}
