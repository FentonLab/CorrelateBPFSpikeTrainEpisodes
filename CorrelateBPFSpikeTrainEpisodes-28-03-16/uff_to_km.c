
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void	uff_to_km(sess)
SESSION	*sess;
{
	si4		i, j, k, n_recs, n_files, cell_num;
	RECORD		*recs;
	ui1		km_rec[6], type, spike_counters[4];
	FILE		*fp;
	si1		**cell_map, km_name[256], base_name[64], *km_dir, *c1, *c2;
	extern si1	*get_env_var();


	km_dir = get_env_var("KM_DIR");

	n_files = sess->num_cells / 4;
	if (sess->num_cells % 4)
		++n_files;

	c1 = sess->name_str;
	c2 = base_name;
	while (*c1 != '.' && *c1 != '\0')
		*c2++ = *c1++;
	*c2 = '\0';

	n_recs = sess->num_recs;
	recs = sess->recs;
	cell_map = sess->cell_map;
	
	for (i = 0; i < n_files; ++i) {
		(void) sprintf(km_name, "%s/%s.km%d", km_dir, base_name, i + 1);
		fp = fopen(km_name, "w");
		if (fp == NULL) {
			(void) fprintf(stderr, "%c\n\tCould not open the file \"%s\". Terminating program.\n\n", 7, km_name);
			exit(1);
		}

		for (j = 0; j < 4; ++j)
			spike_counters[j] = 0;
	
		for (j = 0; j < n_recs; ++j) {
			type = recs[j].type;
			if (type == POSITION_REC) {
				km_rec[0] = recs[j].red_x;
				km_rec[1] = recs[j].red_y;
				km_rec[2] = recs[j].green_x;
				km_rec[3] = recs[j].green_y;
				for (k = 0; k < 4; ++k)
					if (spike_counters[k] & 240) {
						(void) fprintf(stderr, "%cSpike overflow on counter %d in file %d. Ignoring extra spikes.\n", 7, i + 1);
						spike_counters[k] &= 15;
					}
				km_rec[4] = spike_counters[0] | (spike_counters[1] << 4);
				km_rec[5] = spike_counters[2] | (spike_counters[3] << 4);
				(void) fwrite((void *) km_rec, sizeof(ui1), (size_t) 6, fp);
				for (k = 0; k < 4; ++k)
					spike_counters[k] = 0;
				continue;
			}
			if (is_spike(*sess, type)) {
				cell_num = cell_map[recs[j].probe][recs[j].clust];
				if (cell_num == NOT_CUT)
					continue;
				if ((cell_num / 4) != i)
					continue;
				++spike_counters[cell_num % 4];
			}
		}

		(void) fclose(fp);	
	}
			
	return;
}	


