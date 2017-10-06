
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	name_cells(sess)
SESSION	*sess;
{
	si4		i, j, cell_num;
	si1		**cm, *pm;
	CELL		*cp;
	OPTIONS		*opts;

	cm = sess->cell_map;
	pm = sess->probe_map;
	opts = sess->opts;
	
	for (i = 0; i < MAX_PROBES; ++i) {
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
			if (cm[i][j] == NOT_CUT)
				continue;
			cell_num = cm[i][j];
			cp = sess->cells[cell_num];
			cp->probe = i;
			cp->clust = j;
			if (opts->km_file)
				(void) sprintf(cp->name_str, "Channel %d", j);
			else
				(void) sprintf(cp->name_str, "Probe %d, Cluster %d", i, j);
		}
		if (pm[i] == NOT_CUT)
			continue;
		cell_num = pm[i];
		cp = sess->probes[cell_num];
		cp->probe = i;
		cp->clust = MAX_CLUSTS_PER_PROBE;
		(void) sprintf(cp->name_str, "Probe %d", i);
	}

	return;
}

