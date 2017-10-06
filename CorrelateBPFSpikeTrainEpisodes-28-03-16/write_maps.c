
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"

void	write_maps(sess)
SESSION	*sess;
{
	FILE	*tfp, *sfp, *rfp, *lfp;
	si4	i, j, k, unit_num, area;
	CELL	*cp;	
	ui2	*sm;
	sf4	*tm, *rm, thresh;
	si1	*c, file_name[256], file[256], base_file[256], spk_file[256], time_file[256], rate_file[256], list_file[256], command[256], FrameStr[10];

	(void) sprintf(file_name, "%s", sess->name_str);
	c = file_name;
	while(*c != '.'){
		if(*c == '\0')
			break;
		c++;
	}
	*c++ = '\0';
	(void) sprintf(base_file, "%s%s", sess->FrameStr, file_name);
	if(!strcmp(sess->FrameStr, "ARENA/"))
		strcpy(FrameStr,"Ar");
	else
		strcpy(FrameStr,"Rm");

	(void) sprintf(time_file, "%s\%s", get_env_var("TIME_DIR"), base_file);
	tfp = fopen(time_file,"w");
	if (tfp == NULL) {
        	(void) fprintf(stderr, "%c\n\tCould not open the time map file \"%s\". Terminating program.\n\n", 7, time_file);
       		exit(-1);
       	}
	(void) sprintf(spk_file, "%s\%s", get_env_var("SPIKE_DIR"), base_file);
	(void) sprintf(rate_file, "%s\%s", get_env_var("RATE_DIR"), base_file);
	(void) sprintf(list_file, "%s\%s.%slist", get_env_var("RATE_DIR"), base_file, FrameStr);

	lfp = fopen(list_file,"w");
	if (lfp == NULL) {
        	(void) fprintf(stderr, "%c\n\tCould not open the list of unit maps file \"%s\". Terminating program.\n\n", 7, list_file);
       		exit(-1);
       	}

        area = sess->params->area;
	tm = sess->time->map[0];

	for (k = 0; k < area; ++k)
		(void)fprintf(tfp,"%0.2f\n", (float)tm[k] /  sess->params->position_samp_freq);
	fclose(tfp);

        for(i=0; i < MAX_PROBES; i++){
                for(j=0; j < MAX_CLUSTS_PER_PROBE; j++){
                	unit_num = sess->cell_map[i][j];
			if(unit_num == NOT_CUT)
				continue;
			sprintf(file, "%s.P%dC%d", spk_file, i, j);
			sfp = fopen(file,"w");
			sprintf(file, "%s.P%dC%d", rate_file, i, j);
			rfp = fopen(file,"w");
			if ((sfp == NULL) || (rfp == NULL)) {
        			(void) fprintf(stderr, "%c\n\tCould not open the spike or rate map file \"%s %s\". Terminating program.\n\n", 7, spk_file, rate_file);
       				exit(-1);
       			}
			fprintf(lfp,"%s.P%dC%d\n", base_file, i, j);

			sm = sess->cells[unit_num]->spike_map[0];
			rm = sess->cells[unit_num]->rate_map[0];

			for (k = 0; k < area; ++k) {
				(void)fprintf(sfp,"%0.2f\n", (float)sm[k]);
				(void)fprintf(rfp,"%0.2f\n", rm[k]);
			}
			fclose(sfp);
			fclose(rfp);
		}
	}
	fclose(lfp);
	
	if(!strcmp((char *)get_env_var("MAPDIR"), (char *)get_env_var("RATE_DIR"))){
		sprintf(command,"cp %s .", list_file);
		system(command);
		sprintf(command,"print_maps -V -t1.0 -O -X%d -Y%d %s.%slist", sess->params->x_size, sess->params->y_size, file_name, FrameStr);
		fprintf(stdout,"Calling print_maps: %s\n", command);
		system(command);
		sprintf(command,"rm %s.%slist", file_name, FrameStr);
		system(command);
		fflush(stdout);
	}else
		fprintf(stderr,"Can't call print_maps becasue env variable MAPDIR not set to RATE_DIR.\n");

	return;
}

void	write_smoothed_maps(sess)
SESSION	*sess;
{
	FILE	*tfp, *sfp, *rfp, *lfp;
	si4	i, j, k, unit_num, area;
	CELL	*cp;	
	sf4	*tm, *sm, *rm, thresh;
	si1	*c, file_name[256], file[256], base_file[256], spk_file[256], time_file[256], rate_file[256], list_file[256], command[256], FrameStr[10];

	(void) sprintf(file_name, "%s", sess->name_str);
	c = file_name;
	while(*c != '.'){
		if(*c == '\0')
			break;
		c++;
	}
	*c++ = '\0';
	(void) sprintf(base_file, "%s%s", sess->FrameStr, file_name);
	if(!strcmp(sess->FrameStr, "ARENA/"))
		strcpy(FrameStr,"ArSm");
	else
		strcpy(FrameStr,"RmSm");

	(void) sprintf(time_file, "%s\%s%s", get_env_var("TIME_DIR"), base_file, FrameStr);
	tfp = fopen(time_file,"w");
	if (tfp == NULL) {
        	(void) fprintf(stderr, "%c\n\tCould not open the time map file \"%s\". Terminating program.\n\n", 7, time_file);
       		exit(-1);
       	}
	(void) sprintf(spk_file, "%s\%s%s", get_env_var("SPIKE_DIR"), base_file, FrameStr);
	(void) sprintf(rate_file, "%s\%s%s", get_env_var("RATE_DIR"), base_file, FrameStr);
	(void) sprintf(list_file, "%s\%s.%slist", get_env_var("RATE_DIR"), base_file, FrameStr);

	lfp = fopen(list_file,"w");
	if (lfp == NULL) {
        	(void) fprintf(stderr, "%c\n\tCould not open the list of unit maps file \"%s\". Terminating program.\n\n", 7, list_file);
       		exit(-1);
       	}

        area = sess->params->area;
	tm = sess->time->smoothed_map[0];

	for (k = 0; k < area; ++k)
		(void)fprintf(tfp,"%0.2f\n", tm[k] /  sess->params->position_samp_freq);
	fclose(tfp);

        for(i=0; i < MAX_PROBES; i++){
                for(j=0; j < MAX_CLUSTS_PER_PROBE; j++){
                	unit_num = sess->cell_map[i][j];
			if(unit_num == NOT_CUT)
				continue;
			sprintf(file, "%s.P%dC%d", spk_file, i, j);
			sfp = fopen(file,"w");
			sprintf(file, "%s.P%dC%d", rate_file, i, j);
			rfp = fopen(file,"w");
			if ((sfp == NULL) || (rfp == NULL)) {
        			(void) fprintf(stderr, "%c\n\tCould not open the spike or rate map file \"%s %s\". Terminating program.\n\n", 7, spk_file, rate_file);
       				exit(-1);
       			}
			fprintf(lfp,"%s%s.P%dC%d\n", base_file, FrameStr, i, j);

			sm = sess->cells[unit_num]->smoothed_spike_map[0];
			rm = sess->cells[unit_num]->smoothed_rate_map[0];

			for (k = 0; k < area; ++k) {
				(void)fprintf(sfp,"%0.2f\n", sm[k]);
				(void)fprintf(rfp,"%0.2f\n", rm[k]);
			}
			fclose(sfp);
			fclose(rfp);
		}
	}
	fclose(lfp);
	
	if(!strcmp((char *)get_env_var("MAPDIR"), (char *)get_env_var("RATE_DIR"))){
		sprintf(command,"cp %s .", list_file);
		system(command);
		sprintf(command,"print_maps -V -t1.0 -O -X%d -Y%d %s.%slist", sess->params->x_size, sess->params->y_size, file_name, FrameStr);
		fprintf(stdout,"Calling print_maps: %s\n", command);
		system(command);
		sprintf(command,"rm %s.%slist", file_name, FrameStr);
		system(command);
		fflush(stdout);
	}else
		fprintf(stderr,"Can't call print_maps becasue env variable MAPDIR not set to RATE_DIR.\n");

	return;
}
