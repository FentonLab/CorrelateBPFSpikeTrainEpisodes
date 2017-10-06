
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
// #include <values.h>
#include "size_types.h"
#include "theta.h"


/* this routine might seem like overkill, you may think whoever 
programmed this was the king of anal retention, but unless the times
of the peaks and valleys are interpolated large asymmetries will
appear in the 90 and 270 degree bins when the phase data are binned */


void	get_phase(filt, n_samps, phase, crits)
register sf8	*filt;
ui4		n_samps;
register si2	*phase;
ui1		*crits;
{
	si4		i, j, k;
	si4		last_min, last_max, longest_stretch;
	si4		n_mins, n_maxs, stretch, current_time;
	sf8		*x, *y, *d2y, *u;
	sf8		*min_times, *max_times;
	sf8		deg_per_time, phase_offset;
	extern sf8	peak_time(), valley_time();
	extern void	spline(), *gmem();
	
	/* find out how much memory will be necessary */

	for (i = 0; crits[i] != MX; ++i);
	last_max = i;
	for (i = 0; crits[i] != MN; ++i);
	last_min = i;

	longest_stretch = i = 0;
	n_mins = n_maxs = 0;
	while (i < n_samps) {
		if (crits[i] == MX) {
			stretch = (i - last_max) + 1;
			if (stretch > longest_stretch)
				longest_stretch = stretch;
			last_max = i;
			++n_maxs;
		}
		else if (crits[i] == MN) {
			stretch = (i - last_min) + 1;
			if (stretch > longest_stretch)
				longest_stretch = stretch;
			last_min = i;
			++n_mins;
		}
		++i;
	}

	/* allocate memory */

	x = (sf8 *) gmem((size_t) longest_stretch, sizeof(sf8));
	y = (sf8 *) gmem((size_t) longest_stretch, sizeof(sf8));
	u = (sf8 *) gmem((size_t) longest_stretch, sizeof(sf8));
	d2y = (sf8 *) gmem((size_t) longest_stretch, sizeof(sf8));
	min_times = (sf8 *) gmem((size_t) n_mins, sizeof(sf8));
	max_times = (sf8 *) gmem((size_t) n_maxs, sizeof(sf8));

	/* use spline to get more accurate estimates of the min & max times */

	for (i = 0; i < longest_stretch; ++i)
		x[i] = (sf8) i;

	for (i = 1; crits[i] != MX; ++i);
	last_max = i;
	for (i = 1; crits[i] != MN; ++i);
	last_min = i;

	n_mins = n_maxs = 0;
	if (last_max > last_min) {
		i = last_max;
		last_max = 0;
		max_times[n_maxs] = 0.0;
		++n_maxs;
	} else {
		i = last_min;
		last_min = 0;
		min_times[n_mins] = 0.0;
		++n_mins;
	}

	while (i < n_samps) {
		if (crits[i] == MN) {
			stretch = (i - last_min) + 1;
			for (j = last_min, k = 0; j <= i; ++j, ++k)
				y[k] = (sf8) filt[j];
			spline(x, y, stretch, d2y, u);
			max_times[n_maxs] = peak_time(x, y, d2y, stretch);
			max_times[n_maxs] += (sf8) last_min;
			++n_maxs;
			last_min = i;
		}
		else if (crits[i] == MX) {
			stretch = (i - last_max) + 1;
			for (j = last_max, k = 0; j <= i; ++j, ++k)
				y[k] = (sf8) filt[j];
			spline(x, y, stretch, d2y, u);
			min_times[n_mins] = valley_time(x, y, d2y, stretch);
			min_times[n_mins] += (sf8) last_max;
			++n_mins;
			last_max = i;
		}
		++i;
	}

	free((void *) x);
	free((void *) y);
	free((void *) d2y);
	free((void *) u);

	/* use interpolated times to assign phases */
				
	current_time = 0;
	while ((sf8) current_time < min_times[0] || (sf8) current_time < max_times[0])
		++current_time;

	i = j = 0;
	while (1) {
		if (max_times[i] < min_times[j]) {
			deg_per_time = 180.0 / (min_times[j] - max_times[i]);
			phase_offset = (((sf8) current_time - max_times[i]) * deg_per_time) + 90.0;
			k = 0;
			while ((sf8) current_time < min_times[j]) {
				phase[current_time] = (si2) ((((sf8) k * deg_per_time) + phase_offset) + 0.5);
				++k;
				++current_time;
			}
			if (++i == n_maxs)
				break;
		}
		if (min_times[j] < max_times[i]) {
			deg_per_time = 180.0 / (max_times[i] - min_times[j]);
			phase_offset = (((sf8) current_time - min_times[j]) * deg_per_time) + 270.0;
			k = 0;
			while ((sf8) current_time < max_times[i]) {
				phase[current_time] = (si2) ((((sf8) k * deg_per_time) + phase_offset) + 0.5);
				if (phase[current_time] >= 360)
					phase[current_time] -= 360;
				++k;
				++current_time;
			}
			if (++j == n_mins)
				break;
		}
	}			

	if (i < n_maxs) {
		--j;
		deg_per_time = 180.0 / (max_times[i] - min_times[j]);
		phase_offset = (((sf8) current_time - min_times[j]) * deg_per_time) + 270.0;
		k = 0;
		while ((sf8) current_time < max_times[i]) {
			phase[current_time] = (si2) ((((sf8) k * deg_per_time) + phase_offset) + 0.5);
			if (phase[current_time] >= 360)
				phase[current_time] -= 360;
			++k;
			++current_time;
		}
	}
	else if (j < n_mins) {
		--i;
		deg_per_time = 180.0 / (min_times[j] - max_times[i]);
		phase_offset = (((sf8) current_time - max_times[i]) * deg_per_time) + 90.0;
		k = 0;
		while ((sf8) current_time < min_times[j]) {
			phase[current_time] = (si2) ((((sf8) k * deg_per_time) + phase_offset) + 0.5);
			++k;
			++current_time;
		}
	}

	free((void *) min_times);
	free((void *) max_times);

	return;
}
