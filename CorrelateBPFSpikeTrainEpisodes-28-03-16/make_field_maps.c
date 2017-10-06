
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_field_maps(sess)
SESSION	*sess;
{
	si4		i, j, k, n_units, x_size, y_size, area;
	CELL		*cp;
	 void	get_field_map(), prune_field_map();	
	extern void	*gmem();

	n_units = sess->num_cells + sess->num_probes;
	x_size = sess->params->x_size;
	y_size = sess->params->y_size;
	area = sess->params->area;
	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];

                cp->field_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
                cp->field_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));

                for (j = 1, k = x_size; j < y_size; ++j, k += x_size)
                        cp->field_map[j] = cp->field_map[0] + k;

		get_field_map(sess, i, 1);
		prune_field_map(sess, i, 1);
	}
	for (i = 0; i < n_units; ++i) {
		cp = sess->secondary_cells[i];

                cp->field_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
                cp->field_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));

                for (j = 1, k = x_size; j < y_size; ++j, k += x_size)
                        cp->field_map[j] = cp->field_map[0] + k;

		get_field_map(sess, i, 2);
		prune_field_map(sess, i, 2);
	}

	return;
}


 void	get_field_map(sess, cell_num, frame)
SESSION		*sess;
si4		cell_num;
si4		frame;
{
	si4		i, j, field_cnt, stack_sz;
	si4		x_size, y_size, area, *i_stack, *j_stack;
	ui1		**f_map;
	sf4		**r_map, thr;
	CELL		*cp;
	extern void	*gmem();

	
	x_size = sess->params->x_size - 1;
	y_size = sess->params->y_size - 1;
	area = sess->params->area;

	i_stack = (si4 *) gmem((size_t) area, sizeof(si4));
	j_stack = (si4 *) gmem((size_t) area, sizeof(si4));

	if(frame == 1)
		cp = sess->cells[cell_num];
	else
		cp = sess->secondary_cells[cell_num];
		
	thr = cp->rate_thresh;
	f_map = cp->field_map;
	r_map = cp->rate_map;

	for (field_cnt = i = 0; i <= y_size; ++i) {
		for (j = 0; j <= x_size; ++j) {
			if (r_map[i][j] > thr && f_map[i][j] == 0) {
				i_stack[0] = i;
				j_stack[0] = j;
				stack_sz = 1;
				++field_cnt;
				f_map[i][j] = field_cnt;
				do {
					if (i)
						if (r_map[i - 1][j] > thr && f_map[i - 1][j] == 0) {
							i_stack[stack_sz] = i - 1;
							j_stack[stack_sz] = j;
							f_map[i - 1][j] = field_cnt;
							++stack_sz;
						}
					if (i < y_size)
						if (r_map[i + 1][j] > thr && f_map[i + 1][j] == 0) {
							i_stack[stack_sz] = i + 1;
							j_stack[stack_sz] = j;
							f_map[i + 1][j] = field_cnt;
							++stack_sz;
						}
					if (j)
						if (r_map[i][j - 1] > thr && f_map[i][j - 1] == 0) {
							i_stack[stack_sz] = i;
							j_stack[stack_sz] = j - 1;
							f_map[i][j - 1] = field_cnt;
							++stack_sz;
						}
					if (j < x_size)
						if (r_map[i][j + 1] > thr && f_map[i][j + 1] == 0) {
							i_stack[stack_sz] = i;
							j_stack[stack_sz] = j + 1;
							f_map[i][j + 1] = field_cnt;
							++stack_sz;
						}
					--stack_sz;
					i = i_stack[stack_sz];
					j = j_stack[stack_sz];
				} while (stack_sz);
			}
		}
	}

	cp->num_fields = field_cnt;


/*
	cp = sess->secondary_cells[cell_num];
	thr = cp->rate_thresh;
	f_map = cp->field_map;
	r_map = cp->rate_map;

	for (field_cnt = i = 0; i <= y_size; ++i) {
		for (j = 0; j <= x_size; ++j) {
			if (r_map[i][j] > thr && f_map[i][j] == 0) {
				i_stack[0] = i;
				j_stack[0] = j;
				stack_sz = 1;
				++field_cnt;
				f_map[i][j] = field_cnt;
				do {
					if (i)
						if (r_map[i - 1][j] > thr && f_map[i - 1][j] == 0) {
							i_stack[stack_sz] = i - 1;
							j_stack[stack_sz] = j;
							f_map[i - 1][j] = field_cnt;
							++stack_sz;
						}
					if (i < y_size)
						if (r_map[i + 1][j] > thr && f_map[i + 1][j] == 0) {
							i_stack[stack_sz] = i + 1;
							j_stack[stack_sz] = j;
							f_map[i + 1][j] = field_cnt;
							++stack_sz;
						}
					if (j)
						if (r_map[i][j - 1] > thr && f_map[i][j - 1] == 0) {
							i_stack[stack_sz] = i;
							j_stack[stack_sz] = j - 1;
							f_map[i][j - 1] = field_cnt;
							++stack_sz;
						}
					if (j < x_size)
						if (r_map[i][j + 1] > thr && f_map[i][j + 1] == 0) {
							i_stack[stack_sz] = i;
							j_stack[stack_sz] = j + 1;
							f_map[i][j + 1] = field_cnt;
							++stack_sz;
						}
					--stack_sz;
					i = i_stack[stack_sz];
					j = j_stack[stack_sz];
				} while (stack_sz);
			}
		}
	}
*/

	free((void *) i_stack);
	free((void *) j_stack);

	return;
}


 void	prune_field_map(sess, cell_num, frame)
SESSION		*sess;
si4		cell_num, frame;
{
	si4		i, j, max, max_idx, nf;
	si4		area, *field_sizes;
	ui1		**f_map, *fma, *field_ranks;
	CELL		*cp;
	extern void	*gmem();


	area = sess->params->area;
	
	if(frame == 1)
		cp = sess->cells[cell_num];
	else
		cp = sess->secondary_cells[cell_num];
	f_map = cp->field_map;
	fma = f_map[0];

	field_sizes = (si4 *) gmem((size_t) (cp->num_fields + 1), sizeof(si4));
	field_ranks = (ui1 *) gmem((size_t) (cp->num_fields + 1), sizeof(ui1));

	for (i = 0; i < area; ++i)
		if (fma[i])
			++field_sizes[fma[i]];

	for (i = 1; i <= cp->num_fields; ++i) {
		for (max = max_idx = j = 1; j <= cp->num_fields; ++j) {
			if (field_sizes[j] >= max && field_ranks[j] == 0) {
				max = field_sizes[j];
				max_idx = j;
			}
		}
		field_ranks[max_idx] = i;
	}

	for (nf = 0, i = 1; i <= cp->num_fields; ++i) {
		if (field_sizes[i] < MIN_FIELD_SIZE)
			field_ranks[i] = 0;
		else
			++nf;
	}

	cp->num_fields = nf;

	for (i = 0; i < area; ++i)
		fma[i] = field_ranks[fma[i]];

	free((void *) field_sizes);
	free((void *) field_ranks);

	return;
}
