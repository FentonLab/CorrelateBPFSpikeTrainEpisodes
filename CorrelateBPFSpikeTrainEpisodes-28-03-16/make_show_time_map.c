
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_show_time_map(sess)
SESSION	*sess;
{
	si4		i, j, area;
	sf4		*tm, *sort;
	TIME		*tp;
	ui1		*sm;
	extern si4	get_breakpoints(), get_medians();
	extern void	*gmem();
	si4	ascending_sf4_sort();

        area = sess->params->area;

	tp = sess->time;
	tm = tp->map[0];

	sort = (sf4 *) gmem((size_t) area, sizeof(sf4));
	(void) memcpy((void *) sort, (void *) tm, (size_t) area * sizeof(sf4));
	qsort((void *) sort, (size_t) area, sizeof(sf4), ascending_sf4_sort);

	tp->bkpts[0] = tp->threshold;
	tp->num_bkpts = get_breakpoints(sort, tp->bkpts, N_BKPTS, area);
	tp->num_bkpts = get_medians(sort, tp->bkpts, tp->num_bkpts, tp->medns, area);

	// free((void *) sort);

	sm = tp->show_map[0];
	for (i = 0; i < area; i++) {
		for (j = 0; j < tp->num_bkpts; ++j)
			if (tm[i] < tp->bkpts[j])
				break;
		sm[i] = j;
	}
	tp = sess->secondary_time;
	tm = tp->map[0];

	// sort = (sf4 *) gmem((size_t) area, sizeof(sf4));
	(void) memcpy((void *) sort, (void *) tm, (size_t) area * sizeof(sf4));
	qsort((void *) sort, (size_t) area, sizeof(sf4), ascending_sf4_sort);

	tp->bkpts[0] = tp->threshold;
	tp->num_bkpts = get_breakpoints(sort, tp->bkpts, N_BKPTS, area);
	tp->num_bkpts = get_medians(sort, tp->bkpts, tp->num_bkpts, tp->medns, area);

	free((void *) sort);

	sm = tp->show_map[0];
	for (i = 0; i < area; i++) {
		for (j = 0; j < tp->num_bkpts; ++j)
			if (tm[i] < tp->bkpts[j])
				break;
		sm[i] = j;
	}

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
