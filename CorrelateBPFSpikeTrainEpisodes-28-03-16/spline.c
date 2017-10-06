
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
 

void spline(x, y, n, d2y, u)
sf8	*x, *y, *d2y, *u;
si4	n;
{
	register si4	i;
	register sf8	p, sig;

	d2y[0] = u[0] = 0.0;

	for (i = 1; i < n - 1; i++) {
		sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
		p = sig * d2y[i - 1] + 2.0;
		d2y[i] = (sig - 1.0) / p;
		u[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
		u[i] = (6.0 * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
	}

	d2y[n - 1] = 0.0;

	for (i = n - 1; i--; )
		d2y[i] = d2y[i] * d2y[i + 1] + u[i];

	return;
}


