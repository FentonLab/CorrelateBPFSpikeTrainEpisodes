
#include <stdlib.h> 
#include <stdio.h> 
#include "size_types.h"
#include "sess_anal.h"


void	fill_app_holes(sess)
SESSION	*sess;
{
	si4		i, j, stack_sz, *i_stack, *j_stack, size;
	si4		x_size, y_size, area;
	ui1		**am, neg_val;
	APPARATUS	*app;
	extern void	*gmem();

        x_size = sess->params->x_size;
        y_size = sess->params->y_size;
        area = sess->params->area;

	i_stack = (si4 *) gmem((size_t) area, sizeof(si4));
	j_stack = (si4 *) gmem((size_t) area, sizeof(si4));

	app = sess->apparatus;
	am = app->map;

	i_stack[0] = j_stack[0] = i = j = 0;

	stack_sz = 1;
	neg_val = TRUE + 1;
	am[i][j] = neg_val;

	--x_size;
	--y_size;

	do {
		if (i)
			if (!am[i - 1][j] && am[i - 1][j] != neg_val) {
				i_stack[stack_sz] = i - 1;
				j_stack[stack_sz] = j;
				am[i - 1][j] = neg_val;
				++stack_sz;
			}
		if (i < y_size)
			if (!am[i + 1][j] && am[i + 1][j] != neg_val) {
				i_stack[stack_sz] = i + 1;
				j_stack[stack_sz] = j;
				am[i + 1][j] = neg_val;
				++stack_sz;
			}
		if (j)
			if (!am[i][j - 1] && am[i][j - 1] != neg_val) {
				i_stack[stack_sz] = i;
				j_stack[stack_sz] = j - 1;
				am[i][j - 1] = neg_val;
				++stack_sz;
			}
		if (j < x_size)
			if (!am[i][j + 1] && am[i][j + 1] != neg_val) {
				i_stack[stack_sz] = i;
				j_stack[stack_sz] = j + 1;
				am[i][j + 1] = neg_val;
				++stack_sz;
			}

		--stack_sz;
		i = i_stack[stack_sz];
		j = j_stack[stack_sz];

	} while (stack_sz);

	free((void * ) i_stack);
	free((void * ) j_stack);

	++x_size;
	++y_size;

	for (i = 0; i < y_size; ++i) {
		for (j = 0; j < x_size; ++j) {
			if (am[i][j] == neg_val)
				am[i][j] = FALSE;
			else if (!am[i][j])
				am[i][j] = TRUE;
		}
	}
	

	return;
}
