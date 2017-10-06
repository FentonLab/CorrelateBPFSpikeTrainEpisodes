
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_smoothed_rate_maps(sess)
SESSION	*sess;
{
	si4	i, j, k, l, m, x, y, wx, wy;
	si4	n_units, x_size, y_size;
	CELL	*cp, *cp2;	
	ui2	**sm, **sm2;
	ui1	**am, **am2;
	sf4	**tm, **srm, thresh, **tm2, **srm2, thresh2;
	sf8	surr_spks, surr_tm, rad_2, weight_grid[3][3];
	sf8	cent_spks, cent_tm, sf, n_pix, tot_spks, tot_tm, range;

        x_size = sess->params->x_size - 1;
        y_size = sess->params->y_size - 1;
        sf = sess->params->smoothing_factor;
	rad_2 = 1.0 / sqrt(2.0);

	weight_grid[0][0] = weight_grid[0][2] = weight_grid[2][0] = weight_grid[2][2] = rad_2;
	weight_grid[0][1] = weight_grid[1][0] = weight_grid[1][2] = weight_grid[2][1] = 1.0;
	weight_grid[1][1] = 0.0;

	n_units = sess->num_cells + sess->num_probes;
	tm = sess->time->map;
	tm2 = sess->secondary_time->map;
	am = sess->apparatus->map;
	am2 = sess->secondary_apparatus->map;

	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];
		cp2 = sess->secondary_cells[i];
		sm = cp->spike_map;
		sm2 = cp2->spike_map;
		srm = cp->smoothed_rate_map;
		srm2 = cp2->smoothed_rate_map;
		thresh = cp->smoothed_rate_thresh;
		thresh2 = cp2->smoothed_rate_thresh;
		for (j = 1; j < y_size; ++j) {
			for (k = 1; k < x_size; ++k) {
				if (!am[j][k]) {
					srm[j][k] = NO_RATE;
					continue;
				}
				surr_spks = surr_tm = n_pix = 0.0;
				for (l = -1; l <= 1; ++l) {
					y = j + l;
					wy = l + 1;
					for (m = -1; m <= 1; ++m) {
						x = k + m;
						if (!am[y][x])
							continue;
						wx = m + 1;
						surr_spks += (sf8) sm[y][x] * weight_grid[wy][wx];
						surr_tm += (sf8) tm[y][x] * weight_grid[wy][wx];
						n_pix += weight_grid[wy][wx];
					}
				}
				if (!tm[j][k]) {
					srm[j][k] = NO_RATE;
					continue;
				}
				range = (1.0 + ((1.0 - sf) * n_pix)) / (n_pix + 1.0);
				cent_spks = (sf8) sm[j][k] * range;
				cent_tm = (sf8) tm[j][k] * range;
				surr_spks *= (1.0 - range);
				surr_tm *= (1.0 - range);
				tot_spks = cent_spks + surr_spks;
				tot_tm = cent_tm + surr_tm;
				if (tot_tm) {
					srm[j][k] = (sf4) (tot_spks / tot_tm);
					if (srm[j][k] < thresh)
						srm[j][k] = 0.0;
				} else
					srm[j][k] = NO_RATE;
			}	
		}
		for (j = 1; j < y_size; ++j) {
			for (k = 1; k < x_size; ++k) {
				if (!am2[j][k]) {
					srm2[j][k] = NO_RATE;
					continue;
				}
				surr_spks = surr_tm = n_pix = 0.0;
				for (l = -1; l <= 1; ++l) {
					y = j + l;
					wy = l + 1;
					for (m = -1; m <= 1; ++m) {
						x = k + m;
						if (!am2[y][x])
							continue;
						wx = m + 1;
						surr_spks += (sf8) sm2[y][x] * weight_grid[wy][wx];
						surr_tm += (sf8) tm2[y][x] * weight_grid[wy][wx];
						n_pix += weight_grid[wy][wx];
					}
				}
				if (!tm2[j][k]) {
					srm2[j][k] = NO_RATE;
					continue;
				}
				range = (1.0 + ((1.0 - sf) * n_pix)) / (n_pix + 1.0);
				cent_spks = (sf8) sm2[j][k] * range;
				cent_tm = (sf8) tm2[j][k] * range;
				surr_spks *= (1.0 - range);
				surr_tm *= (1.0 - range);
				tot_spks = cent_spks + surr_spks;
				tot_tm = cent_tm + surr_tm;
				if (tot_tm) {
					srm2[j][k] = (sf4) (tot_spks / tot_tm);
					if (srm2[j][k] < thresh2)
						srm2[j][k] = 0.0;
				} else
					srm2[j][k] = NO_RATE;
			}	
		}
	}

	return;
}

