
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"

#define BAD_REG -2.0

/* returns coefficients for y = a + bx */


void	lin_regress(x_s, y_s, len, a, b, r, z, p)
sf4	*x_s, *y_s;
si4	len;
sf8	*a, *b, *r, *z, *p;
{
	si4		i;
	sf8		sx, sxx, sy, syy, sxy, t1, t2, t3, n;
	extern sf8	z2p();


	*a = *b = *r = *p = *z = BAD_REG;

	switch (len) {
		case 0:
			(void) fprintf(stderr, "%cregression error: no points in arrays, returning %0.1f\n", 7, BAD_REG);
			return;
		case 1:
			(void) fprintf(stderr, "%cregression error: regression of a single point is undefined, returning %0.1f\n", 7, BAD_REG);
			return;
		case 2:
			(void) fprintf(stderr, "%cregression warning: regression of only two points is degenerate\n", 7);
			break;
		default:
			break;
	}

	sx = sxx = sy = syy = sxy = 0.0;
	n = (sf8) len;

	for (i = 0; i < len; i++) {
		t1 = (sf8) x_s[i];
		t2 = (sf8) y_s[i];
		sx += t1;
		sxx += t1 * t1;
		sy += t2;
		syy += t2 * t2;
		sxy += t1 * t2;
	}

	t1 = sxy - (sx * sy / n);
	t2 = sxx - (sx * sx / n);
	t3 = syy - (sy * sy / n);
	
	if (t2 == 0.0) {
		(void) fprintf(stderr, "%cregression error: divide by zero, returning %0.1f.\n", 7, BAD_REG);
		return;
	}

	*b = t1 / t2;
	*a = (sy / n) - (*b * sx / n);

	t2 *= t3;

	if (t2 < 0.0) {
		(void) fprintf(stderr, "%cregression error: square root of a negative number, returning %0.1f.\n", 7, BAD_REG);
		return;
	}

	t2 = sqrt(t2);

	if (!t2) {
		(void) fprintf(stderr, "%cregression error: divide by zero, returning %0.1f.\n", 7, BAD_REG);
		return;
	}

	*r = t1 / t2;

	if (len == 3) {
		(void) fprintf(stderr, "%cregression warning: z score of only three correlated points is undefined, returning %0.1f\n", 7, BAD_REG);
		return;
	}

	*z = (0.5 * log((1.0 + *r) / (1.0 - *r))) / sqrt(1.0 / (n - 3.0));
	*p = z2p(*z);


	return;
}

