
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "size_types.h"
#include "uff.h"
#include "bpf.h"
#include "sess_anal.h"


void	get_recs_from_ts(sess)
SESSION	*sess;
{
	si4		i, j, k, l;
	ui4		total_recs, n_pos_recs, file_len, data_offset;
	ui4		curr_time_stamp, spike_counter;
	ui4		n_header_lines, t;
	ui1		x, y, s;
	si4             rec_time_sort();
	si1		sess_path[256], keyword[256], *km_dir, *string;
	ui1		*data, *dp; 
	sf4		pos_samp_interval;
	sf8		curr_time, scale_x, scale_y;
	ui4		first_time_stamp;
	ui4             reverse_ui4();
	ui4             ts_reverse_ui4();
	extern void	*gmem();
	extern si1	*get_env_var();
	RECORD		*rp;
	FILE		*fp;


	strcpy(sess->date_str, "not available");
	strcpy(sess->time_str, "not available");
	strcpy(sess->desc_str, "not available");

        file_len = sess->file_len;
	data = sess->data;
	data_offset = 0;

        if(sscanf((si1 *)data,"%d", &n_header_lines) != 1){
		(void) fprintf(stderr, "%c\n\tTS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}

	string = NULL;
	if((string = strstr((char *)(data),"%SAMPLING_INTERVAL(samps/sec) ")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%SAMPLING_INTERVAL(samps/sec) ' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	sscanf(string,"%s %f", &keyword, &pos_samp_interval);
	sess->params->position_samp_freq = pos_samp_interval; // it actually the frequency that is in the header
	pos_samp_interval = (TIMESTAMPS_PER_SEC / pos_samp_interval);

	string = NULL;
	if((string = strstr((char *)(data),"%FIRST_TIMESTAMP(100usUnits)")) == NULL){
		(void) fprintf(stderr, "\n\tCan't find '%%FIRST_TIMESTAMP(100usUnits)' assuming the value is 0\n", sess_path); 
		first_time_stamp = 0;
	}
	sscanf(string,"%s %d", &keyword, &first_time_stamp);

	string = NULL; 
	if((string = strstr((char *)(data + data_offset),"%SCALE_Y(RatioTracktoMapPixels) ")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%SCALE_Y(RatioTracktoMapPixels) ' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	sscanf(string,"%s%lf", &keyword, &scale_y);

	string = NULL; 
	if((string = strstr((char *)(data + data_offset),"%SCALE_X(RatioTracktoMapPixels) ")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%SCALE_X(RatioTracktoMapPixels) ' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	sscanf(string,"%s%lf", &keyword, &scale_x);

	if(scale_y != scale_x){
		fprintf(stderr,"pixels in TS file are not square. sess_anal assumes the pixels are square. Terminating.\n");
		exit(1);
	}
	dp = NULL;
	if((dp = (ui1 *)strstr((si1 *)data,"%%ENDHEADER")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%%%ENDHEADER' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	dp += (strlen("%%ENDHEADER") + 1); // +1 to account for the new_line
	data_offset = dp - data;

	// count the number of position records
	n_pos_recs = spike_counter = 0;
	while((dp - data) < file_len){
		x = *dp;
		dp++;
		y = *dp;
		dp++;
		s = *dp;
		dp++;
//              printf("%d\t%d\t%d\t%d",n_pos_recs, x,y,s);

                for(i = 0; i < s; i++){
			dp += sizeof(ui4);
//			printf("\t%d", t);
                }
//		printf("\n");
		n_pos_recs++;
		spike_counter += s;
        }

	total_recs = n_pos_recs + spike_counter;

	rp = sess->recs = (RECORD *) gmem((size_t) total_recs, sizeof(RECORD));
	sess->num_recs = total_recs;

	curr_time = 0.0 + first_time_stamp;
	dp = data + data_offset;

	for (i = l = 0; i < n_pos_recs; ++i) {
		curr_time += pos_samp_interval;
		curr_time_stamp = (ui4) (curr_time + 0.5);
//              printf("%d\t%d\t%d\t%d",i, dp[0], dp[1], dp[2]);
		s = dp[2];
		dp+=3;

		for (j = 0; j < s; ++j){
			rp[l].type = SINGLE_ELECTRODE_SPIKE_REC;
			rp[l].probe = 0;
			rp[l].clust = 1;
			if(sess->opts->reverse_ts_binary)
				rp[l].time_stamp = ts_reverse_ui4(dp);
			else
				rp[l].time_stamp = reverse_ui4(dp);
//			printf("\t%d",rp[l].time_stamp);
			// rp[l].time_stamp = curr_time_stamp - 1;
			++l;
			dp += sizeof(ui4);
		}
//		printf("\n");
		rp[l].type = POSITION_REC;
		rp[l].time_stamp = curr_time_stamp;
		rp[l].red_x = dp[0];
		rp[l].red_y = dp[1];
		rp[l].green_x = 0;
		rp[l].green_y = 0;
		++l;
	}

	// sort the recs into time order
	(void) qsort((void *) sess->recs, (size_t) total_recs, sizeof(RECORD), rec_time_sort);

	return;
}	

void	get_recs_from_ts_files(sess)
SESSION	*sess;
{
	si4		i, j, k, l;
	ui4		total_recs, total_recs_in_key_file, n_pos_recs, file_len, data_offset, key_data_offset;
	ui4		curr_time_stamp, spike_counter;
	ui4             reverse_ui4();
	ui4             ts_reverse_ui4();
	ui4		n_header_lines, t;
	ui1		x, y, s;
	si4             rec_time_sort();
	si1		sess_path[256], keyword[256], filename[FILE_NAME_CHARS], *ts_dir, *string;
	ui1		*data, *dp, probe, clust; 
	ui4		NumFiles, max_files, ts_file_num;
	sf4		pos_samp_interval;
	sf8		curr_time, scale_x, scale_y;
	extern void	*gmem();
	extern si1	*get_env_var();
	RECORD		*rp;
	FILE		*fp, *lfp;
	si1		**file;
        struct stat     stat_struct;
        ui4             n_read, first_time_stamp;
        extern si1      *get_env_var();

	strcpy(sess->date_str, "not available");
	strcpy(sess->time_str, "not available");
	strcpy(sess->desc_str, "not available");


	// Get the TS_DIR
        ts_dir = get_env_var("TS_DIR");

	// Get data from the "key" ts file. This file provides ONLY position data
	// its data was read in by get_data();
       	file_len = sess->file_len;
	data = sess->data;
        if(sscanf((si1 *)data,"%d", &n_header_lines) != 1){
		(void) fprintf(stderr, "%c\n\tTS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}

	data_offset = 0;
	string = NULL;
	if((string = strstr((char *)(data),"%SAMPLING_INTERVAL(samps/sec) ")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%SAMPLING_INTERVAL(samps/sec) ' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	sscanf(string,"%s %f", &keyword, &pos_samp_interval);
	sess->params->position_samp_freq = pos_samp_interval; // it actually the frequency that is in the header
	pos_samp_interval = (TIMESTAMPS_PER_SEC / pos_samp_interval);

	string = NULL; 
        if((string = strstr((char *)(data),"%FIRST_TIMESTAMP(100usUnits)")) == NULL){
                (void) fprintf(stderr, "\n\tCan't find '%%FIRST_TIMESTAMP(100usUnits)' assuming the value is 0\n", sess_path);                                       
                first_time_stamp = 0;
        }
        sscanf(string,"%s %d", &keyword, &first_time_stamp);

	string = NULL; 
	if((string = strstr((char *)(data + data_offset),"%SCALE_Y(RatioTracktoMapPixels) ")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%SCALE_Y(RatioTracktoMapPixels) ' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	sscanf(string,"%s%lf", &keyword, &scale_y);

	string = NULL; 
	if((string = strstr((char *)(data + data_offset),"%SCALE_X(RatioTracktoMapPixels) ")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%SCALE_X(RatioTracktoMapPixels) ' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	sscanf(string,"%s%lf", &keyword, &scale_x);

	if(scale_y != scale_x){
		fprintf(stderr,"pixels in TS file are not square. sess_anal assumes the pixels are square. Terminating.\n");
		exit(1);
	}
	dp = NULL;
	if((dp = (ui1 *)strstr((si1 *)data,"%%ENDHEADER")) == NULL){
		(void) fprintf(stderr, "%c\n\tCan't find '%%%%ENDHEADER' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	dp += (strlen("%%ENDHEADER") + 1); // +1 to account for the new_line
	key_data_offset = dp - data;

	// count the number of position records
	n_pos_recs = spike_counter = 0;
	while((dp - data) < file_len){
		x = *dp;
		dp++;
		y = *dp;
		dp++;
		s = *dp;
		dp++;
//              printf("%d\t%d\t%d\t%d\n",n_pos_recs, x,y,s);

                for(i = 0; i < s; i++){
			dp += sizeof(ui4);
                }
		n_pos_recs++;
		spike_counter += s;
        }
	total_recs = n_pos_recs;

        // Get the additional ts data files which provide ONLY spike data
        lfp = fopen(sess->params->ListOfTSFiles, "r");
        if(lfp == NULL){
                printf("Can't open %s\n", sess->params->ListOfTSFiles);
                exit(-1);
        }

	// loop on the number of additional ts files
	// this is done to figure out how many records to allocate memory for
        while(fscanf(lfp,"%s",filename) == 1){
        	(void) sprintf(sess_path, "%s/%s", ts_dir, filename);
        	if ((fp = fopen(sess_path, "r")) == NULL) {
                	(void) fprintf(stderr, "%c\n\tCould not open file \"%s\". Terminating program.\n\n", 7, sess_path);
                	exit(1);
        	}

		// read data file into memory
        	(void) fstat(fileno(fp), &stat_struct);
        	file_len = (ui4) stat_struct.st_size;
        	data = (ui1 *) gmem((size_t)file_len, sizeof(ui1));
        	n_read = (ui4) fread((void *)data, sizeof(ui1), (size_t)file_len, fp);
        	if (n_read != file_len) {
                	(void) fprintf(stderr, "%c\n\tError reading file \"%s\". Terminating program.\n\n", 7, sess_path);
                	exit(1);
        	}
        	(void) fclose(fp);

		dp = NULL;
		if((dp = (ui1 *)strstr((si1 *)data,"%%ENDHEADER")) == NULL){
			(void) fprintf(stderr, "%c\n\tCan't find '%%%%ENDHEADER' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
			exit(1); 
		}
		dp += (strlen("%%ENDHEADER") + 1); // +1 to account for the new_line
		data_offset = dp - data;

		// count the number of spike records
		spike_counter = 0;
		while((dp - data) < file_len){
			s = dp[2];
			dp+=3;
//			x = *dp;
//			dp++;
//			y = *dp;
//			dp++;
//			s = *dp;
//			dp++;
//              	printf("%d\t%d\t%d\t%d",n_pos_recs, x,y,s);

                	for(i = 0; i < s; i++){
				dp += sizeof(ui4);
                	}
			spike_counter += s;
        	}

		total_recs += spike_counter;
		free(data);
	} // loop for reading the ts files

	rp = sess->recs = (RECORD *) gmem((size_t) total_recs, sizeof(RECORD));
	sess->num_recs = total_recs;

	// read data again this time to make records
	curr_time = first_time_stamp - pos_samp_interval;
	dp = sess->data + key_data_offset;

	for (i = l = 0; i < n_pos_recs; ++i) {
		curr_time += pos_samp_interval;
		curr_time_stamp = (ui4) (curr_time + 0.5);
//              printf("%d\t%d\t%d\t%d",i, dp[0], dp[1], dp[2]);
		s = dp[2];
		dp+=3;

		for (j = 0; j < s; ++j)
			dp += sizeof(ui4);

		rp[l].type = POSITION_REC;
		rp[l].time_stamp = curr_time_stamp;
		rp[l].red_x = dp[0];
		rp[l].red_y = dp[1];
		rp[l].green_x = 0;
		rp[l].green_y = 0;
		++l;
	}

	// loop on the additional ts files
	// this time to store the spike data in records.
	rewind(lfp);
	ts_file_num = 0;
        while(fscanf(lfp,"%s",filename) == 1){
        	(void) sprintf(sess_path, "%s/%s", ts_dir, filename);
        	if ((fp = fopen(sess_path, "r")) == NULL) {
                	(void) fprintf(stderr, "%c\n\tCould not open file \"%s\". Terminating program.\n\n", 7, sess_path);
                	exit(1);
        	}

		probe = (int)(ts_file_num / (MAX_CLUSTS_PER_PROBE - 1));
		clust = 1 + (ts_file_num % (MAX_CLUSTS_PER_PROBE - 1));
		ts_file_num++;


		// read data file into memory
        	(void) fstat(fileno(fp), &stat_struct);
        	file_len = (ui4) stat_struct.st_size;
        	data = (ui1 *) gmem((size_t)file_len, sizeof(ui1));
        	n_read = (ui4) fread((void *)data, sizeof(ui1), (size_t)file_len, fp);
        	if (n_read != file_len) {
                	(void) fprintf(stderr, "%c\n\tError reading file \"%s\". Terminating program.\n\n", 7, sess_path);
                	exit(1);
        	}
        	(void) fclose(fp);

		dp = NULL;
		if((dp = (ui1 *)strstr((si1 *)data,"%%ENDHEADER")) == NULL){
			(void) fprintf(stderr, "%c\n\tCan't find '%%%%ENDHEADER' TS file is corrupt \"%s\". Terminating program.\n\n", 7, sess_path); 
			exit(1); 
		}
		dp += (strlen("%%ENDHEADER") + 1); // +1 to account for the new_line
		data_offset = dp - data;

		// count the number of spike records
		spike_counter = 0;
		while((dp - data) < file_len){
			s = dp[2];
			dp+=3;
			for (j = 0; j < s; ++j){
				rp[l].type = SINGLE_ELECTRODE_SPIKE_REC;
				strncpy(rp[l].filename, filename, FILE_NAME_CHARS);
				rp[l].probe = probe;
				rp[l].clust = clust;
				if(sess->opts->reverse_ts_binary)
					rp[l].time_stamp = ts_reverse_ui4(dp);
				else
					rp[l].time_stamp = reverse_ui4(dp);
				++l;
				dp += sizeof(ui4);
			}
        	}
		free(data);
	}
	if(total_recs != l){
		fprintf(stderr, "Programming error in reading data into records. l=%d total_recs=%d\n", l, total_recs);
		exit(-1);
	}
	fclose(lfp);

	if(ts_file_num < 2){
		fprintf(stderr, "Read less than 2 files in %s. Need at least 2 files to calculate a correlation.\n", sess->params->ListOfTSFiles);
		exit(-1);
	}

	// sort the recs into time order
	(void) qsort((void *) sess->recs, (size_t) total_recs, sizeof(RECORD), rec_time_sort);

	return;

}
