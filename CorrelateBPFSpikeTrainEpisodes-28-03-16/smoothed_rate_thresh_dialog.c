
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	smoothed_rate_thresh_dialog(sess)
SESSION	*sess;
{
	si4		i, err, n_units, cell_num;
	si1		response[32];
	sf4		thresh;
	CELL		*cp;
	extern void	pause();

	n_units = sess->num_cells + sess->num_probes;

	while (TRUE) {

		(void) system("clear");
		(void) fprintf(stderr, "\nThe units to be analyzed have the following smoothed rate thresholds:\n\n");
		(void) fprintf(stderr, "Cell/Probe\tName\t\tThreshold\n\n");
		
		for (i = 0; i < n_units; ++i) {
			cp = sess->cells[i];
			(void) fprintf(stderr, "%d\t\t%s\t\t%f\n", i, cp->name_str, cp->smoothed_rate_thresh);
		}

		(void) fprintf(stderr, "\nTo change a threshold enter its cell/probe number and a\n");
		(void) fprintf(stderr, "new threshold, and press return. To exit, press return with no entries.\n\n");

		(void) fgets(response, 32, stdin);
		if (response[0] == '\n') 
			break;
		err = sscanf(response, "%d%f", &cell_num, &thresh);
		if (err != 2) {
			(void) fprintf(stderr, "%c\nTwo numbers are expected as input. Go ahead and give it another shot.\n", 7);
			pause(3.0);
			continue;
		}
		if (cell_num >= n_units || cell_num < 0) {
			(void) fprintf(stderr, "%c\nCell number %d does not exist.\n", 7, i);
			pause(3.0);
			continue;
		}
		if (thresh <= 0.0) {
			(void) fprintf(stderr, "%c\nThe threshold must be greater than zero.\n", 7);
			pause(3.0);
			continue;
		}

		sess->cells[cell_num]->smoothed_rate_thresh = thresh;
		sess->secondary_cells[cell_num]->smoothed_rate_thresh = thresh;
	}

	(void) system("clear");
	(void) fprintf(stderr, "\nThe units will be analyzed with the following smoothed rate thresholds:\n\n");
	(void) fprintf(stderr, "Cell/Probe\tName\t\tThreshold\n\n");
		
	for (i = 0; i < n_units; ++i) {
		cp = sess->cells[i];
		(void) fprintf(stderr, "%d\t\t%s\t\t%f\n", i, cp->name_str, cp->smoothed_rate_thresh);
	}

	(void) fprintf(stderr, "\n");

	return;
}
