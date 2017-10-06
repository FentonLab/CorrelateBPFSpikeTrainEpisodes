
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"


sf8	splope(xa, ya, d2y, x, lo_pt, hi_pt)
sf8	*xa, *ya, *d2y, x;
si4	lo_pt, hi_pt;
{
	register sf8	a, b, c, d, e, f, g, h;

	a = xa[lo_pt];
	b = xa[hi_pt];
	c = ya[lo_pt];
	d = ya[hi_pt];
	e = d2y[lo_pt];
	f = d2y[hi_pt];
	g = b - a;

	h = (x * (e * b - f * a) + d - c) / g;
	h += (x * x * (f - e) + f * a * a - e * b * b) / (2.0 * g);
	h += g * (e - f) / 6.0;

	return(h);
}
