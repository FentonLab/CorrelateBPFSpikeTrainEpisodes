
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"


sf8	z2p(z)
sf8	z;
{
	sf8	fna, fnb, fnc, t, p;


	if (z < 0.0)
		z = -z;

	t = 1.0 / (1.0 + (0.2316419 * z));
	fna = -1.82125598 + (t * 1.33027443);
	fnb = 0.3989423 * t * exp(-z * z / 2.0);
	fnc = fnb * (0.31938153 + (t * (-0.35656378 + (t * (1.78147794 + (t * fna))))));
	p = fnc * 2.0;

	return(p);
}

