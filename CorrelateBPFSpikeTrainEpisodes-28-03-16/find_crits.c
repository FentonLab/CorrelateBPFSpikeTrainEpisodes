
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "theta.h"


void	find_crits(n_samps, filt, crits)
register sf8	*filt;
ui4		n_samps;
register ui1	*crits;
{
	si4	i, end;

	end = (si4) (n_samps - 1);

	(void) memset((void *) crits, MD, (size_t) n_samps);
	
	for (i = 0, crits[0] = MN; filt[i] < filt[i + 1]; ++i);
	crits[i] = MX;

	while (i != end) {
		for (; i < end; ++i)
			if (filt[i + 1] > filt[i])
				break;
		crits[i] = MN;
		if (i == end)
			break;
		for (; i < end; ++i)
			if (filt[i + 1] < filt[i])
				break;
		crits[i] = MX;
	}	
	if (filt[i] > filt[i - 1])
		crits[i] = MX;
	else
		crits[i] = MN;

	return;	
}

