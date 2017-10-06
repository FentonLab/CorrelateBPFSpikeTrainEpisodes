
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"
#include "ts.h"
#include "bpf.h"

/* time_series format:ascii header binary data
n_header_lines
header_line 1
...
header_line n
byte byte byte   word       ...word
x    y    n_spks spk_time-1 ...spk_time-n
...
*/


void	write_ts(sess)
SESSION	*sess;
{
	si4		i, j, k, n, n_files, n_recs, cell_num;
	ui4		 spk_num, max_time_stamp, first_time_stamp, pos_sampling_period;
	RECORD		*recs;
	ui1		type, spike_counters[4], cut_session;
	FILE		*fp[500];
	si1		**cell_map, ts_name[256], base_name[64], *ts_dir, *c1, *c2;
	extern si1	*get_env_var();
	TS_RECORD	ts_rec;

	ts_dir = get_env_var("TS_DIR");

	n_files = sess->num_cells;

	c1 = sess->name_str;
	c2 = base_name;
	while (*c1 != '.' && *c1 != '\0')
		*c2++ = *c1++;
	*c2 = '\0';

	n_recs = sess->num_recs;
	recs = sess->recs;
	cell_map = sess->cell_map;

	
	if (sess->opts->no_pos_samp) {
		first_time_stamp = recs[0].time_stamp;
	}else{
		for (i = 0; i < n_recs; ++i) {	// find the first pos_record
			type = recs[i].type;
			if (type == sess->PositionRec)
				break;
		}
		first_time_stamp = recs[i].time_stamp;
	}

        for (i = 0; i < MAX_PROBES; ++i) {
               	for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
                  	if (cell_map[i][j] != NOT_CUT) {
				if(sess->PositionRec == ROOM_POSITION_BPF_REC_TYPE)
					(void) sprintf(ts_name, "%s/%s.P%dC%dRm", ts_dir, base_name, i, j);
				else if(sess->PositionRec == ARENA_POSITION_BPF_REC_TYPE)
					(void) sprintf(ts_name, "%s/%s.P%dC%dAr", ts_dir, base_name, i, j);
				else if (sess->PositionRec == POSITION_REC)
					(void) sprintf(ts_name, "%s/%s.P%dC%d", ts_dir, base_name, i, j);

				cell_num = cell_map[i][j];
				fp[cell_num] = fopen(ts_name, "w");
				if (fp[cell_num] == NULL) {
					(void) fprintf(stderr, "%c\n\tCould not open the output file \"%s\". Terminating program.\n\n", 7, ts_name);
					exit(1);
				}
				// write header 
				// 6 lines
				(void)fprintf(fp[cell_num],"%d\n", 13);
				(void)fprintf(fp[cell_num],"%%%%BEGIN_HEADER\n");
				(void)fprintf(fp[cell_num],"\t%%SAMPLING_INTERVAL(samps/sec) %f\n", (float)(sess->params->position_samp_freq));
				(void)fprintf(fp[cell_num],"\t%%FIRST_TIMESTAMP(100usUnits) %d\n",first_time_stamp);
				(void)fprintf(fp[cell_num],"\t%%SCALE_Y(RatioTracktoMapPixels) %0.2f\n", 1.0);
				(void)fprintf(fp[cell_num],"\t%%SCALE_X(RatioTracktoMapPixels) %0.2f\n", 1.0);
				// (void)fprintf(fp[cell_num],"\t%%SCALE_Y(RatioTracktoMapPixels) %0.2f\n", sess->params->pixel_collapse_factor);
				// (void)fprintf(fp[cell_num],"\t%%SCALE_X(RatioTracktoMapPixels) %0.2f\n", sess->params->pixel_collapse_factor);

				// 7 lines
				(void)fprintf(fp[cell_num],"TIME_SERIES FORMAT:\n");
				(void)fprintf(fp[cell_num],"Ascii header:\n");
				(void)fprintf(fp[cell_num],"\tNumberOfLinesInHeader\n");
				(void)fprintf(fp[cell_num],"\t(NumberOfLinesInHeader - 1) of Ascii Info.\n");
				(void)fprintf(fp[cell_num],"Binary data format:\n");
				(void)fprintf(fp[cell_num],"\tXposition:1byte Yposition:1byte NspksInSample:1byte TimestampOfSpk-1:4bytes...TimeStampOfSpk-n:4bytes\n");
				(void)fprintf(fp[cell_num],"%%%%ENDHEADER\n");
			}
		}
	}
	for (j = 0; j< MAX_NUMBER_OF_UNITS; ++j)
		ts_rec.n_spikes[j] = 0;
		
	if (sess->opts->no_pos_samp) {
		ts_rec.rx = ts_rec.ry = ts_rec.bx = ts_rec.by = 128;

		pos_sampling_period = NTSC_SAMPS_PER_SEC; 
		max_time_stamp = pos_sampling_period;

		for (i = 0; i < n_recs; ++i) {
			type = recs[i].type;
			if (is_spike(*sess, type)) {
				while(max_time_stamp < recs[i].time_stamp){
					// write out data from last position sample
        				for (j = 0; j < MAX_PROBES; ++j) {
               					for (k = 0; k < MAX_CLUSTS_PER_PROBE; ++k) {
                  					if (cell_map[j][k] != NOT_CUT) {
								cell_num = cell_map[j][k];
								fwrite((void *)&(ts_rec.rx), sizeof(ui1), 1, fp[cell_num]);
								fwrite((void *)&(ts_rec.ry), sizeof(ui1), 1, fp[cell_num]);
								fwrite((void *)&(ts_rec.n_spikes[cell_num]), sizeof(ui1), 1, fp[cell_num]);
								for(spk_num = 0; spk_num < ts_rec.n_spikes[cell_num]; spk_num++){
									#ifdef BIG_ENDIAN_MACHINE
                                                                        	// ts_rec.time[cell_num][spk_num] = reverse_ui4((ui1 *)&(ts_rec.time[cell_num][spk_num]));
										reverse_ui4((ui1 *)&(ts_rec.time[cell_num][spk_num]));
									#endif
									fwrite((void *)&(ts_rec.time[cell_num][spk_num]), sizeof(unsigned long), 1, fp[cell_num]);
								}
								ts_rec.n_spikes[cell_num] = 0;
							}
						}
					}
					max_time_stamp += pos_sampling_period;
				}
				if(recs[i].time_stamp <= max_time_stamp){ //within a time_step
					cell_num = cell_map[recs[i].probe][recs[i].clust];
					if (cell_num == NOT_CUT)
						continue;
					ts_rec.time[cell_num][ts_rec.n_spikes[cell_num]] = recs[i].time_stamp;
					ts_rec.n_spikes[cell_num]++;
				}
			}
		}
	}else{ // use position records to clock the ts file
		for (i = 0; i < n_recs; ++i) {	// find the first pos_record
			type = recs[i].type;
			if (type == sess->PositionRec)
				break;
		}
		if(sess->PositionRec == ROOM_POSITION_BPF_REC_TYPE){
			ts_rec.rx = recs[i].RoomX;
			ts_rec.ry = recs[i].RoomY;
		}else if (sess->PositionRec == ARENA_POSITION_BPF_REC_TYPE){
			ts_rec.rx = recs[i].ArenaX;
			ts_rec.ry = recs[i].ArenaY;
		}else if ( sess->PositionRec == POSITION_REC){
			ts_rec.rx = recs[i].red_x;
			ts_rec.ry = recs[i].red_y;
		}

		for (i += 1; i < n_recs; ++i) {
			type = recs[i].type;
			if (is_spike(*sess, type)) {
				cell_num = cell_map[recs[i].probe][recs[i].clust];
				if (cell_num == NOT_CUT)
					continue;
				ts_rec.time[cell_num][ts_rec.n_spikes[cell_num]] = recs[i].time_stamp;
				ts_rec.n_spikes[cell_num]++;
			}else if (type == sess->PositionRec){
				// write out data from last position sample
        			for (j = 0; j < MAX_PROBES; ++j) {
               				for (k = 0; k < MAX_CLUSTS_PER_PROBE; ++k) {
                  				if (cell_map[j][k] != NOT_CUT) {
							cell_num = cell_map[j][k];
							fwrite((void *)&(ts_rec.rx), sizeof(char), 1, fp[cell_num]);
							fwrite((void *)&(ts_rec.ry), sizeof(char), 1, fp[cell_num]);
							fwrite((void *)&(ts_rec.n_spikes[cell_num]), sizeof(char), 1, fp[cell_num]);
							for(spk_num = 0; spk_num < ts_rec.n_spikes[cell_num]; spk_num++){
								#ifdef BIG_ENDIAN_MACHINE
                                                                        ts_rec.time[cell_num][spk_num] = reverse_ui4((ui1 *)&(ts_rec.time[cell_num][spk_num]));
                                                                #endif
								fwrite((void *)&(ts_rec.time[cell_num][spk_num]), sizeof(unsigned long), 1, fp[cell_num]);
							}
						}
					}
				}

				// record data for the current sample
				for (j = 0; j< MAX_NUMBER_OF_UNITS; ++j)
					ts_rec.n_spikes[j] = 0;
				if(sess->PositionRec == ROOM_POSITION_BPF_REC_TYPE){
					ts_rec.rx = recs[i].RoomX;
					ts_rec.ry = recs[i].RoomY;
				}else if (sess->PositionRec == ARENA_POSITION_BPF_REC_TYPE){
					ts_rec.rx = recs[i].ArenaX;
					ts_rec.ry = recs[i].ArenaY;
				}else if ( sess->PositionRec == POSITION_REC){
					ts_rec.rx = recs[i].red_x;
					ts_rec.ry = recs[i].red_y;
				}
			}
		}
	}

        for (i = 0; i < MAX_PROBES; ++i) {
               	for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
                	if (cell_map[i][j] != NOT_CUT) {
				cell_num = cell_map[i][j];
				(void) fclose(fp[cell_num]);	
			}
		}
	}
	return;			
}	
