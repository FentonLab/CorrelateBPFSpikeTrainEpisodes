
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"
#include "bpf.h"
#include "uff.h"


void	make_cell_map(sess)
SESSION		*sess;
{
	si4		i, j, n_recs, max_clusts;
	RECORD		*recs;
	ui1		type, max_p, max_c;
	si1		**cm, *cma;
	extern void	*gmem(), elim_clusts_dialog(), keep_clusts_dialog();

	
	max_clusts = MAX_PROBES * MAX_CLUSTS_PER_PROBE;
	sess->cell_map = (si1 **) gmem((size_t) MAX_PROBES, sizeof(si1 *));
	sess->cell_map[0] = (si1 *) gmem((size_t) max_clusts, sizeof(si1));
	for (i = 1, j = MAX_CLUSTS_PER_PROBE; i < MAX_PROBES; ++i, j += MAX_CLUSTS_PER_PROBE)
		sess->cell_map[i] = sess->cell_map[0] + j;

	cm = sess->cell_map;
	cma = cm[0];
	(void) memset((void *) cma, NOT_CUT, (size_t) max_clusts);

	n_recs = sess->num_recs;
	recs = sess->recs;

	max_p = max_c = 0;
	for (i = 0; i < n_recs; ++i) {
		type = recs[i].type;
		if (is_spike(*sess, type)){
			cm[recs[i].probe][recs[i].clust] = CUT;
			if(max_p < recs[i].probe) max_p = recs[i].probe;
			if(max_c < recs[i].clust) max_c = recs[i].clust;
		}
	}
//	printf("HERE %d %d\n", max_p, max_c);

	for (i = 0; i < max_clusts; ++i)
		if (cma[i] == CUT)
			goto found_spikes;
	(void) fprintf(stderr, "%c\n\tNo spikes were detected in this session. Terminating program.\n\n", 7);
	exit(1);
	
found_spikes:	

	if (sess->opts->elim_clusts)
		elim_clusts_dialog(cm);
	else if (sess->opts->keep_clusts)
		keep_clusts_dialog(cm);
	else if (!sess->opts->anal_zero_clusts)
		for (i = 0; i < MAX_PROBES; ++i)
			cm[i][0] = NOT_CUT;

	sess->num_cells = 0;
	for (i = 0; i < max_clusts; ++i)
		if (cma[i] == CUT)
			cma[i] = sess->num_cells++;

// printf("HERE cut cells = %d\n",sess->num_cells++);

	if (sess->num_cells == 0) {
		(void) fprintf(stderr, "%c\n\tNo clusters selected for analysis. Terminating program.\n\n", 7);
		exit(1);
	}	

	/* do probe stuff */
	sess->num_probes = 0;
	sess->probe_map = (si1 *) gmem((size_t) MAX_PROBES, sizeof(si1));
	(void) memset((void *) sess->probe_map, NOT_CUT, (size_t) MAX_PROBES);

	/* only analyze a probe if there were cells on it */
	if (sess->opts->anal_probes) {
		for (i = 0; i < MAX_PROBES; ++i) {
			for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j)
				if (cm[i][j] != NOT_CUT)
					break;
			if (j != MAX_CLUSTS_PER_PROBE)	
				sess->probe_map[i] = sess->num_probes++;
		}
	}

	return;
}
