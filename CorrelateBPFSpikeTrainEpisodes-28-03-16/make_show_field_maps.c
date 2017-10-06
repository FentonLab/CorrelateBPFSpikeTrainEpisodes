
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	make_show_field_maps(sess)
SESSION	*sess;
{
	si4		i, j, k, n_units;
	si4		x_size, y_size, area;
	CELL		*cp;
	ui1		*sm, *fm, *am, n_colors;
	extern void	*gmem();

        x_size = sess->params->x_size;
        y_size = sess->params->y_size;
        area = sess->params->area;

	n_units = sess->num_cells + sess->num_probes;
	am = sess->apparatus->map[0];

	n_colors = (ui1) (COLOR_CATS - 1);

	for (i = 0; i < n_units; ++i) {

		cp = sess->cells[i];
		fm = cp->field_map[0];

		cp->show_field_map = (ui1 **) gmem((size_t) y_size, sizeof(ui1 *));
		cp->show_field_map[0] = (ui1 *) gmem((size_t) area, sizeof(ui1));

		for (j = 1, k = x_size; j < y_size; ++j, k += x_size)
			cp->show_field_map[j] = cp->show_field_map[0] + k;

		sm = cp->show_field_map[0];

		for (j = 0; j < area; ++j) {
			if (!am[j])
				sm[j] = WHITE;
			else if (!fm[j])
				sm[j] = YELLOW;
			else if (fm[j] < n_colors)
				sm[j] = (ui1) COLOR_CATS - fm[j];
			else
				sm[j] = YELLOW;
		}
	}

	return;
}
