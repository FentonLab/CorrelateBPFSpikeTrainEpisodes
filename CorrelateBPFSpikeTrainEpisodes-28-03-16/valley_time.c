
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"


sf8	valley_time(xa, ya, d2y, len)
sf8	*xa, *ya, *d2y;
si4	len;
{
	register si4	lo_pt, hi_pt;
	register sf8	x, dx, s;
	sf8		limit;
	extern sf8	splope();

	limit = -1e-2;
	x = xa[0];
	dx = (xa[len - 1] - x) / 2.0;

search_loop:

	lo_pt = (si4) (x + dx);
	hi_pt = lo_pt + 1;
	s = splope(xa, ya, d2y, x + dx, lo_pt, hi_pt);

	if (s < limit) {
		x += dx;
		dx = (xa[len - 1] - x) / 2.0;
		goto search_loop;
	}

	if (s > 0.0) {
		dx /= 2.0;
		goto search_loop;
	}

	return(x + dx);
}
