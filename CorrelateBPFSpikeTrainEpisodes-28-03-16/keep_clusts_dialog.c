
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	keep_clusts_dialog(cell_map)
si1	**cell_map;
{
	si4		i, j, probe, clust, err, n_clusts;
	si1		new_cell_map[MAX_PROBES][MAX_CLUSTS_PER_PROBE];
	si1		response[32];
	extern void	pause();

	for (i = 0; i < MAX_PROBES; i++)
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; j++)
			new_cell_map[i][j] = NOT_CUT;

	while (TRUE) {

		(void) system("clear");
		(void) fprintf(stderr, "\nThe following clusters will be eliminated:\n\n");
		(void) fprintf(stderr, "Probe\tCluster\n\n");

		n_clusts = 0;
		for (i = 0; i < MAX_PROBES; i++)
			for (j = 0; j < MAX_CLUSTS_PER_PROBE; j++)
				if (cell_map[i][j] == CUT && new_cell_map[i][j] == NOT_CUT) {
					(void) fprintf(stderr, "%d\t%d\n", i, j);
					++n_clusts;
				}

		if (n_clusts == 0) {
			(void) fprintf(stderr, "%c\n\tAll selected clusters for analysis. Terminating dialog.\n\n", 7);
			pause(3.0);
			break;
		}

		(void) fprintf(stderr, "\nTo include analysis of a cluster, enter its probe and cluster\n");
		(void) fprintf(stderr, "numbers and press return. To exit, press return with no entries.\n\n");

		(void) fgets(response, 32, stdin);
		if (response[0] == '\n') 
			break;
		err = sscanf(response, "%d%d", &probe, &clust);
		if (err != 2) {
			(void) fprintf(stderr, "%c\nTwo numbers are expected as input. Go ahead and give it another shot.\n", 7);
			pause(3.0);
			continue;
		}
		if (cell_map[probe][clust] == NOT_CUT) {
			(void) fprintf(stderr, "%c\nProbe %d cluster %d is not cut.\n", 7, probe, clust);
			pause(3.0);
			continue;
		}

		new_cell_map[probe][clust] = CUT;
	}

	(void) system("clear");

	(void) fprintf(stderr, "The following clusters will be analyzed:\n\n");
	(void) fprintf(stderr, "Probe\tCluster\n\n");

	for (i = 0; i < MAX_PROBES; i++)
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; j++)
			if((cell_map[i][j] = new_cell_map[i][j]) == CUT)
				(void) fprintf(stderr, "%d\t%d\n", i, j);

	(void) fprintf(stderr, "\n");

	return;
}
