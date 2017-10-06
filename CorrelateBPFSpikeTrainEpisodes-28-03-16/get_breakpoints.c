
/* n.b. in this scheme a breakpoint represents the minimum of a color category */
/* breakpoint 0 is expected to be filled, everything greater than or equal to */
/* it is used to determine the breakpoints */

#include <stdlib.h> 
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


si4	get_breakpoints(sort, bkpts, n_bkpts, area)
sf4	*sort, *bkpts;
si4	n_bkpts, area;
{
	si4		i, j, pix_in_cat, n_bkpts_found, pix_remaining;
	sf4		m, sum;


	for (i = 0; i < area; ++i)
		if (sort[i] >= bkpts[0])
			break;

	if (i == area)
		return(1);

	n_bkpts_found = 1;

	while (TRUE) {

		sum = m = 1.0;
		for (j = n_bkpts - n_bkpts_found; j--;) {
			m *= RATIO;
			sum += m;
		}

		pix_remaining = area - i;
		pix_in_cat = (int) (((sf4) pix_remaining / sum) + 0.5);
		i += pix_in_cat;

		while (i < area) {
			if (sort[i] > sort[i - 1])
				break;
			++i;
		}
		if (i == area)
			return(n_bkpts_found);

		bkpts[n_bkpts_found++] = sort[i];
	}
}

