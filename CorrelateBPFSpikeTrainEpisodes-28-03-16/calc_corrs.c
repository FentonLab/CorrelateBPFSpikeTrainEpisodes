
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"


void	calc_corrs(samps, filt, corrs, n_samps, wind)
si2	*samps;
sf8	*filt;
ui4	n_samps, wind;
ui1	*corrs;
{
	sf8	a, aa, b, bb, ab, t1, t2, t3, corr;
	ui4	i, j, k, half_wind;

	a = aa = b = bb = ab = 0.0;
	for (i = 0; i < wind; ++i) {
		t1 = (sf8) samps[i];
		t2 = filt[i];
		a += t1;
		aa += t1 * t1;
		b += t2;
		bb += t2 * t2;
		ab += t1 * t2;
	}

	half_wind = wind / 2;
	for (i = 0; i <= half_wind; ++i)
		corrs[i] = 0;
	for (i = n_samps - half_wind - 1; i < n_samps; ++i)
		corrs[i] = 0;

	for (i = 0, j = half_wind + 1, k = wind; k < n_samps; i++, ++j, ++k) {
		
		t1 = (sf8) samps[i];
		t2 = filt[i];
		a -= t1;
		aa -= t1 * t1;
		b -= t2;
		bb -= t2 * t2;
		ab -= t1 * t2;

		t1 = (sf8) samps[k];
		t2 = filt[k];
		a += t1;
		aa += t1 * t1;
		b += t2;
		bb += t2 * t2;
		ab += t1 * t2;

		t1 = ab - a * b / (sf8) wind;
		t2 = aa - a * a / (sf8) wind;
		t3 = bb - b * b / (sf8) wind;
		t3 *= t2;

		if (t1 > 0.0 && t3 > 0.0) {
			corr = t1 / sqrt(t3);
			corr *= corr;
			corrs[j] = (ui1) ((corr * 100.0) + 0.5);
		} else
			corrs[j] = 0;

	}

	return;
}
