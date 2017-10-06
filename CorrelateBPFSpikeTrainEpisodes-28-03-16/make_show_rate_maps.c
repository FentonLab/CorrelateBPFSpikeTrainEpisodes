
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_show_rate_maps(sess)
SESSION	*sess;
{
	si4		i, j, k, n_units, area;
	sf4		*rm, *sort;
	sf4		*rm2, *sort2;
	CELL		*cp, *cp2;
	ui1		*sm, *sm2;
	extern si4	get_breakpoints(), get_medians();
	extern void	*gmem();
	extern void	read_bkpts(), write_bkpts();
	si4		ascending_sf4_sort();


        area = sess->params->area;

	n_units = sess->num_cells + sess->num_probes;
	sort = (sf4 *) gmem((size_t) area, sizeof(sf4));
	sort2 = (sf4 *) gmem((size_t) area, sizeof(sf4));

	if (sess->opts->read_breakpoints)
		read_bkpts(sess);

	for (i = 0; i < n_units; ++i) {

		cp = sess->cells[i];
		cp2 = sess->secondary_cells[i];
		rm = cp->rate_map[0];
		rm2 = cp2->rate_map[0];

		(void) memcpy((void *) sort, (void *) rm, (size_t) area * sizeof(sf4));	
		(void) memcpy((void *) sort2, (void *) rm2, (size_t) area * sizeof(sf4));	
		qsort((void *) sort, (size_t) area, sizeof(sf4), ascending_sf4_sort);
		qsort((void *) sort2, (size_t) area, sizeof(sf4), ascending_sf4_sort);

		if (!sess->opts->read_breakpoints) {

			/* this is a little convoluted because we want to guarantee that the */
			/* "yellow" pixels are only those below the rate threshold */

			cp->rate_bkpts[0] = 0.0;
			cp2->rate_bkpts[0] = 0.0;
			cp->rate_bkpts[1] = cp->rate_thresh;
			cp2->rate_bkpts[1] = cp->rate_thresh;
			cp->num_rate_bkpts = get_breakpoints(sort, cp->rate_bkpts + 1, N_BKPTS - 1, area);
			cp2->num_rate_bkpts = get_breakpoints(sort, cp->rate_bkpts + 1, N_BKPTS - 1, area);
			cp->num_rate_bkpts += 1;
			cp2->num_rate_bkpts += 1;
		}

		cp->num_rate_bkpts = get_medians(sort, cp->rate_bkpts, cp->num_rate_bkpts, cp->rate_medns, area);
		cp2->num_rate_bkpts = get_medians(sort, cp2->rate_bkpts, cp2->num_rate_bkpts, cp2->rate_medns, area);

		sm = cp->show_rate_map[0];
		sm2 = cp2->show_rate_map[0];
		for (j = 0; j < area; j++) {
			for (k = 0; k < cp->num_rate_bkpts; ++k)
				if (rm[j] < cp->rate_bkpts[k])
					break;
			sm[j] =  k;
		}
		for (j = 0; j < area; j++) {
			for (k = 0; k < cp2->num_rate_bkpts; ++k)
				if (rm2[j] < cp2->rate_bkpts[k])
					break;
			sm2[j] =  k;
		}
	}

	free((void *) sort);

	if (sess->opts->write_breakpoints)
		write_bkpts(sess);


	return;
}


si4      ascending_sf4_sort(a, b)
sf4             *a, *b;
{
        if (*a > *b)
                return(1);
        if (*a < *b)
                return(-1);
        return(0);
}
 
