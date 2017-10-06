
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "theta.h"


void	norm_filt(n_samps, filt, crits)
register sf8	*filt;
ui4		n_samps;
register ui1	*crits;
{
	int	i, j, end, first, last;
	sf8	m, b;

	end = (int) (n_samps - 1);

	i = 0;
	while (i != end) {
		if (crits[i] == MN) {
			first = i;
			for (; crits[i] != MX; ++i);
			last = i;
			m = 2000.0 / (filt[last] - filt[first]);
			b = 1000.0 - (m * filt[last]);
			for (j = first + 1; j < last; ++j)
				filt[j] = (m * filt[j]) + b;
		}
		if (i == end)
			break;
		if (crits[i] == MX) {
			first = i;
			for (; crits[i] != MN; ++i);
			last = i;
			m = 2000.0 / (filt[first] - filt[last]);
			b = 1000.0 - (m * filt[first]);
			for (j = first + 1; j < last; ++j)
				filt[j] = (m * filt[j]) + b;
		}
	}	

	for (i = 0; i < n_samps; ++i) {
		if (crits[i] == MN)
			filt[i] = -1000.0;
		else if (crits[i] == MX)
			filt[i] = 1000.0;
	}

	return;	
}
