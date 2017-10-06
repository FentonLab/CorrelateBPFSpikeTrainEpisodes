
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_show_smoothed_rate_maps(sess)
SESSION	*sess;
{
	si4		i, j, k, n_units, area;
	sf4		*srm, *sort;
	CELL		*cp;
	ui1		*sm;
	extern si4	get_breakpoints(), get_medians();
	extern void	*gmem();
	si4		ascending_sf4_sort();


        area = sess->params->area;

	n_units = sess->num_cells + sess->num_probes;
	sort = (sf4 *) gmem((size_t) area, sizeof(sf4));

	for (i = 0; i < n_units; ++i) {

		cp = sess->cells[i];
		srm = cp->smoothed_rate_map[0];

		(void) memcpy((void *) sort, (void *) srm, (size_t) area * sizeof(sf4));	
		qsort((void *) sort, (size_t) area, sizeof(sf4), ascending_sf4_sort);

		/* this is a little convoluted because we want to guarantee that the */
		/* "yellow" pixels are only those below the rate threshold */

		cp->smoothed_rate_bkpts[0] = 0.0;
		cp->smoothed_rate_bkpts[1] = cp->smoothed_rate_thresh;
		cp->num_smoothed_rate_bkpts = get_breakpoints(sort, cp->smoothed_rate_bkpts + 1, N_BKPTS - 1, area);
		cp->num_smoothed_rate_bkpts += 1;

		cp->num_smoothed_rate_bkpts = get_medians(sort, cp->smoothed_rate_bkpts, cp->num_smoothed_rate_bkpts, cp->smoothed_rate_medns, area);

		sm = cp->show_smoothed_rate_map[0];
		for (j = 0; j < area; j++) {
			for (k = 0; k < cp->num_smoothed_rate_bkpts; ++k)
				if (srm[j] < cp->smoothed_rate_bkpts[k])
					break;
			sm[j] =  k;
		}
	}

	free((void *) sort);

	return;
}


/* defined in make_show_rate_maps.c

si4      ascending_sf4_sort(a, b)
sf4             *a, *b;
{
        if (*a > *b)
                return(1);
        if (*a < *b)
                return(-1);
        return(0);
}
*/ 
