
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_show_speed_map(sess)
SESSION	*sess;
{
	si4		i, j, area;
	sf4		*spm, *sort;
	SPEED		*sp;
	ui1		*sm;
	extern si4	get_breakpoints(), get_medians();
	extern void	*gmem();
	si4	ascending_sf4_sort();

        area = sess->params->area;

	sp = sess->speed;
	spm = sp->map[0];

	sort = (sf4 *) gmem((size_t) area, sizeof(sf4));
	(void) memcpy((void *) sort, (void *) spm, (size_t) area * sizeof(sf4));
	qsort((void *) sort, (size_t) area, sizeof(sf4), ascending_sf4_sort);

	sp->bkpts[0] = (sf4) sp->threshold;
	sp->num_bkpts = get_breakpoints(sort, sp->bkpts, N_BKPTS, area);
	sp->num_bkpts = get_medians(sort, sp->bkpts, sp->num_bkpts, sp->medns, area);

	free((void *) sort);

	sm = sp->show_map[0];
	for (i = 0; i < area; i++) {
		for (j = 0; j < sp->num_bkpts; ++j)
			if (spm[i] < sp->bkpts[j])
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
