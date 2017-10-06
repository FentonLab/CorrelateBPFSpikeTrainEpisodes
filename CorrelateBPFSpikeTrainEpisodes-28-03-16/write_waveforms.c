
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void     write_waveforms(sess)
SESSION *sess;
{
        si4             i, j, num_ad_vals, n_recs, n_cells, cell_num, num_probes;
        si2             temp_si2;
        RECORD          *recs;
        ui1             type, *lbt, *hbt, *lbd, *hbd, probe, clust, chan;
        si1             **cm, *c, spk_file[256];
        CELL            *cp;
        extern void     *gmem();
	extern si1      *get_env_var();
	FILE		*fp[MAX_PROBES][MAX_CLUSTS_PER_PROBE][TET_CHANNELS];

	if(is_bpf(*sess))
		num_ad_vals = sess->BPFRecordVoltageSamples['T'];
		// num_ad_vals = BPF_SPIKE_SIZE;
	else
		num_ad_vals = SU_SPIKE_SIZE;

        n_recs = sess->num_recs;
        recs = sess->recs;
        cm = sess->cell_map;
        n_cells = sess->num_cells;
	num_probes = sess->num_probes;

	(void) sprintf(spk_file, "%s%s", get_env_var("WAVES_DIR"), sess->name_str);
	c = spk_file;
	while(*c++ != '.')
		if(*c == '\0'){
			*c++ = '.';
			break;
		}
        for(i=0; i < MAX_PROBES; i++){
                for(j=0; j < MAX_CLUSTS_PER_PROBE; j++){
                	if (sess->cell_map[i][j] == NOT_CUT)
				continue;
                	for(chan=0; chan < TET_CHANNELS; chan++){
				sprintf(c,"P%d%d%d",i,j,chan);
				fp[i][j][chan] = fopen(spk_file,"w");
				if (fp[i][j][chan] == NULL) {
                       			(void) fprintf(stderr, "%c\n\tCould not open the waveform file \"%s\". Terminating program.\n\n", 7, spk_file);
                       			exit(1);
                		}
			}
		}
	}

	/* prepare to handle the byte order of the 2 byte shorts */
#ifdef BIG_ENDIAN_MACHINE
        hbt = (ui1 *) &temp_si2;
        lbt = hbt + 1;
#else
        lbt = (ui1 *) &temp_si2;
        hbt = lbt + 1;
#endif
        for (i = 0; i < n_recs; ++i) {
                type = recs[i].type;
		if (!(is_spike(*sess, type)))
			continue;
	
		probe = recs[i].probe;
		clust = recs[i].clust;

                cell_num = cm[probe][clust];
                if (cell_num == NOT_CUT)
                        continue;

		if(is_bpf(*sess)){
                	switch (recs[i].type){
			case  SINGLE_BPF_REC_TYPE:
                		lbd = recs[i].data_offset + BPF_SPIKE_OFFSET - 2;
                		hbd = lbd + 1;
                		for (j = 0; j < num_ad_vals; ++j) {
                       		 	*lbt = *(lbd += 2);
                       		 	*hbt = *(hbd += 2);
                       		 	(void) fprintf(fp[probe][clust][0], "%d ", temp_si2);
                		}
				fprintf(fp[probe][clust][0],"\n");
				continue;
			case  STEREO_BPF_REC_TYPE:
               		 	lbd = recs[i].data_offset + BPF_SPIKE_OFFSET - 2;
               		 	hbd = lbd + 1;
               		 	for (j = 0; j < num_ad_vals; ++j) {
               		         	*lbt = *(lbd += (2 * STEREO_CHANNELS));
               		         	*hbt = *(hbd += (2 * STEREO_CHANNELS));
               		 		for(chan=0; chan < STEREO_CHANNELS; chan++){
               		         		*lbt = *(lbd + (2 * chan));
               		         		*hbt = *(hbd + (2 * chan));
               		         		(void) fprintf(fp[probe][clust][chan], "%d ", temp_si2);
					}
               		 	}
				fprintf(fp[probe][clust][0],"\n");
               		        continue;
			case  TETRODE_BPF_REC_TYPE:
                		lbd = recs[i].data_offset + BPF_SPIKE_OFFSET - 2;
                		hbd = lbd + 1;
                		for (j = 0; j < num_ad_vals; ++j) {
                        		*lbt = *(lbd += (2 * TET_CHANNELS));
                        		*hbt = *(hbd += (2 * TET_CHANNELS));
                			for(chan=0; chan < TET_CHANNELS; chan++){
                        			*lbt = *(lbd + (2 * chan));
                        			*hbt = *(hbd + (2 * chan));
                        			(void) fprintf(fp[probe][clust][chan], "%d ", temp_si2);
					}
                		}
				fprintf(fp[probe][clust][0],"\n");
                        	continue;
			default:
				continue;
			}

                	lbd = recs[i].data_offset + BPF_SPIKE_OFFSET - 2;
                	hbd = lbd + 1;
		}else{
                	switch (recs[i].type){
			case  SINGLE_ELECTRODE_SPIKE_REC:
       		         	lbd = recs[i].data_offset + SU_SE_SPIKE_OFFSET - 2;
       		         	hbd = lbd + 1;
       		         	for (j = 0; j < SU_SE_SPIKE_SIZE; ++j) {
               		         	*lbt = *(lbd += 2);
               		         	*hbt = *(hbd += 2);
               		         	(void) fprintf(fp[probe][clust][0], "%d ", temp_si2);
               		 	}
				fprintf(fp[probe][clust][0],"\n");
				continue;
			case  STEREOTRODE_SPIKE_REC:
       		         	lbd = recs[i].data_offset + SU_QT_SPIKE_OFFSET - (2 * STEREO_CHANNELS);
       		         	hbd = lbd + 1;
       		         	for (j = 0; j < SU_SE_SPIKE_SIZE; ++j) {
       		                 	*lbt = *(lbd += (2 * STEREO_CHANNELS));
       		                 	*hbt = *(hbd += (2 * STEREO_CHANNELS));
       		         		for(chan=0; chan < STEREO_CHANNELS; chan++){
       		                 		*lbt = *(lbd + (2 * chan));
       		                 		*hbt = *(hbd + (2 * chan));
       		                 		(void) fprintf(fp[probe][clust][chan], "%d ", temp_si2);
					}
       		         	}
				fprintf(fp[probe][clust][0],"\n");
                       		continue;
			case  TETRODE_SPIKE_REC:
                		lbd = recs[i].data_offset + SU_QT_SPIKE_OFFSET - (2 * TET_CHANNELS);
                		hbd = lbd + 1;
                		for (j = 0; j < SU_SE_SPIKE_SIZE; ++j) {
                        		*lbt = *(lbd += (2 * TET_CHANNELS));
                        		*hbt = *(hbd += (2 * TET_CHANNELS));
                			for(chan=0; chan < TET_CHANNELS; chan++){
                        			*lbt = *(lbd + (2 * chan));
                        			*hbt = *(hbd + (2 * chan));
                        			(void) fprintf(fp[probe][clust][chan], "%d ", temp_si2);
					}
                		}
				fprintf(fp[probe][clust][0],"\n");
                        	continue;
			default:
				continue;
			}
		}
        }
        for(i=0; i < num_probes; i++)
                for(j=0; j < MAX_CLUSTS_PER_PROBE; j++)
                	for(chan=0; chan < TET_CHANNELS; chan++){
                		cell_num = cm[recs[i].probe][recs[i].clust];
                		if (cell_num != NOT_CUT)
					fclose(fp[i][j][chan]);
			}

	return;
}
