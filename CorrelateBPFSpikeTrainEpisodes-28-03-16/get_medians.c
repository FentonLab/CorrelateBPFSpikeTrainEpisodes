
/* n.b. a breakpoint is the minimum value of a category */

#include <stdlib.h> 
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


si4	get_medians(sort, bkpts, n_bkpts, medns, area)
sf4	*sort, *bkpts, *medns;
si4	n_bkpts, area;
{
	si4	i, j, k, mid, cnt;

	for (j = 0; j < area; ++j)
		if (sort[j] >= bkpts[0])
			break;

	for (i = 1, k = 0; i < n_bkpts; ++i, ++k) {
		for (cnt = 0; j < area; ++cnt, ++j)
			if (sort[j] >= bkpts[i])
				break;
		mid = j - (cnt / 2) - 1;
		if (cnt % 2)
			medns[k] = sort[mid];
		else {
			if (cnt == 0)
				medns[k] = bkpts[i - 1];
			else {
				medns[k] = sort[mid];
				medns[k] += sort[mid + 1];
				medns[k] /= 2.0;
			}
		}
		if (j == area)  /* this can happen if the breakpoints were generated from another file */
			return(i);
	}

	cnt = area - j;
	mid = area - (cnt / 2) - 1;
	if (cnt % 2)
		medns[k] = sort[mid];
	else {
		medns[k] = sort[mid];
		medns[k] += sort[mid + 1];
		medns[k] /= 2.0;
	}


	return(n_bkpts);
}

