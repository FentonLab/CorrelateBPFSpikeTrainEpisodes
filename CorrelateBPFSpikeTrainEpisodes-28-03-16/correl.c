
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"

#define BAD_CORR -2.0

void	correl(arr1, arr2, len, r, z, p)
sf4	*arr1, *arr2;
si4	len;
sf8	*r, *z, *p;
{
	si4		i;
	sf8		a, aa, b, bb, ab, t1, t2, t3, n;
	extern sf8	z2p();


	*r = *p = *z = BAD_CORR;

	switch (len) {
		case 0:
			(void) fprintf(stderr, "%ccorrelation error: no points in arrays, returning %0.1f\n", 7, BAD_CORR);
			return;
		case 1:
			(void) fprintf(stderr, "%ccorrelation error: correlation of a single point is undefined, returning %0.1f\n", 7, BAD_CORR);
			return;
		case 2:
			(void) fprintf(stderr, "%ccorrelation warning: correlation of only two points is degenerate\n", 7);
			break;
		default:
			break;
	}

	a = aa = b = bb = ab = 0.0;
	n = (sf8) len;

	for (i = 0; i < len; i++) {
		t1 = (sf8) arr1[i];
		t2 = (sf8) arr2[i];
		a += t1;
		aa += t1 * t1;
		b += t2;
		bb += t2 * t2;
		ab += t1 * t2;
// printf("HERE %lf %lf\n", t1, t2);
	}
// getchar();

	t1 = ab - (a * b / n);
	t2 = aa - (a * a / n);
	t3 = bb - (b * b / n);
	t2 *= t3;

	if (t2 < 0.0) {
		(void) fprintf(stderr, "%ccorrelation error: square root of a negative number, returning %0.1f.\n", 7, BAD_CORR);
		return;
	}

	t2 = sqrt(t2);

	if (!t2) {
		(void) fprintf(stderr, "%ccorrelation error: divide by zero, returning %0.1f.\n", 7, BAD_CORR);
		return;
	}

	*r = t1 / t2;


	if (len == 3) {
		(void) fprintf(stderr, "%ccorrelation warning: z score of only three correlated points is undefined, returning %0.1f\n", 7, BAD_CORR);
		return;
	}

	*z = (0.5 * log((1.0 + *r) / (1.0 - *r))) / sqrt(1.0 / (n - 3.0));
	*p = z2p(*z);

	return;
}

