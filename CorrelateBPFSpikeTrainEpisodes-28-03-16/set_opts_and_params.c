
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sess_anal.h"
#include "size_types.h"
#include "theta.h"
#include "speed.h"
#include "postscript.h"


void	set_opts_and_params(argc, argv, sess)
si4	argc;
si1	**argv;
SESSION	*sess;
{
	si4		index;
	ui1		sense;
	OPTIONS		*opts;
	PARAMETERS	*params;
	 void	set_defaults(), bad_opt(), set_directives();
	 ui1	boolean_opt(), get_param();
	extern void	instruct(), pause();


	if ((index = 1) > --argc)
		instruct(argv[0]);
	
	set_defaults(sess);
	opts = sess->opts;
	params = sess->params;

	while (index < argc) {

		if (argv[index][0] != '-')
			instruct(argv[0]);
		++argv[index];

		sense = TRUE;
negative_sense:

		if (argv[index][0] == '\0')
			instruct(argv[0]);

		switch (argv[index][0]) {
			case 'A':
				if (get_param(&params->analysis_interval_sec, argv, argc, &index, "AnIn", "ui4")) break;
			//	if (boolean_opt(&opts->anal_probes, sense, argv[index], "AnPr")) break;
				if (boolean_opt(&opts->anal_zero_clusts, sense, argv[index], "AnZCl")) break;
			//	if (get_param(&params->autocorr_bins, argv, argc, &index, "AcBn", "si4")) break;
			//	if (get_param(&params->autocorr_end_ms, argv, argc, &index, "AcEnTm", "sf4")) break;
			//	if (boolean_opt(&opts->autocorr_plots, sense, argv[index], "AcPlt")) break;
			//	if (get_param(&params->autocorr_start_ms, argv, argc, &index, "AcStTm", "sf4")) break;
				bad_opt(argv[index], sense, argv[0]);
			case 'B':
			//	if (boolean_opt(&opts->between_event_flags_only, sense, argv[index], "BtEvFlO")) break;
				if (get_param(&params->bin_size_ms, argv, argc, &index, "BnSz", "si4")) break;
				if (boolean_opt(&opts->bpf_acute_file, sense, argv[index], "BPFacute")){
					opts->bpf_room_file = FALSE;
					opts->bpf_arena_file = FALSE;
					 break;
				}
				if (boolean_opt(&opts->bpf_arena_file, sense, argv[index], "BPFarena")){
					opts->bpf_room_file = FALSE;
					opts->bpf_acute_file = FALSE;
					break;
				}
				if (boolean_opt(&opts->bpf_room_file, sense, argv[index], "BPFroom")){
					opts->bpf_arena_file = FALSE;
					opts->bpf_acute_file = FALSE;
					break;
				}
				bad_opt(argv[index], sense, argv[0]);
			case 'C':
			//	if (get_param(&params->cross_corr_bins, argv, argc, &index, "CcBn", "si4")) break;
			//	if (get_param(&params->cross_corr_end_ms, argv, argc, &index, "CcEnTm", "sf4")) break;
			//	if (boolean_opt(&opts->cross_corr_plots, sense, argv[index], "CcPlt")) break;
			//	if (get_param(&params->cross_corr_start_ms, argv, argc, &index, "CcStTm", "sf4")) break;
				if (boolean_opt(&opts->change_rate_thresh, sense, argv[index], "ChRtThr")) break;
				if (boolean_opt(&opts->change_smoothed_rate_thresh, sense, argv[index], "ChSmRtThr")) break;
			//	if (boolean_opt(&opts->cell_report, sense, argv[index], "ClRpt")) break;
			//	if (boolean_opt(&opts->color_maps, sense, argv[index], "CMp")) break;
				bad_opt(argv[index], sense, argv[0]);
			case 'D':
			//	if (boolean_opt(&opts->dir_div_rate_maps, sense, argv[index], "DrDvRtMp")) break;
			//	if (boolean_opt(&opts->dir_plots, sense, argv[index], "DrPlt")) break;
				bad_opt(argv[index], sense, argv[0]);	
			case 'E':
				if (boolean_opt(&opts->elim_clusts, sense, argv[index], "ElCl")) break;
			//	if (boolean_opt(&opts->error_bars_on_waveforms, sense, argv[index], "ErBrWv")) break;
			//	if (get_param(&params->event_begin_flag, argv, argc, &index, "EvBgFl", "ui1")) break;
			//	if (get_param(&params->event_end_flag, argv, argc, &index, "EvEnFl", "ui1")) break;
				bad_opt(argv[index], sense, argv[0]);	
			case 'F':
			//	if (boolean_opt(&opts->fill_app_holes, sense, argv[index], "FlApHl")) break;
			//	if (boolean_opt(&opts->field_maps, sense, argv[index], "FldMp")) break;
				bad_opt(argv[index], sense, argv[0]);	
			case 'G':
				if (get_param(&params->global_rate_thresh, argv, argc, &index, "GlRtThr", "sf4")) break;
				if (get_param(&params->global_smoothed_rate_thresh, argv, argc, &index, "GlSmRtThr", "sf4")) break;
			//	if (get_param(&params->global_speed_thresh, argv, argc, &index, "GlSpThr", "ui1")) break;
				if (get_param(&params->global_time_thresh, argv, argc, &index, "GlTmThr", "sf4")) break;
				bad_opt(argv[index], sense, argv[0]);
			case 'I':
				if (boolean_opt(&opts->ignore_zeros, sense, argv[index], "IgZer")) break;
			//	if (get_param(&params->interspike_int_bins, argv, argc, &index, "ISIBn", "si4")) break;
			//	if (get_param(&params->interspike_int_end_ms, argv, argc, &index, "ISIEnTm", "sf4")) break;
			//	if (boolean_opt(&opts->interspike_int_plots, sense, argv[index], "ISIPlt")) break;
			//	if (get_param(&params->interspike_int_start_ms, argv, argc, &index, "ISIStTm", "sf4")) break;
				bad_opt(argv[index], sense, argv[0]);
			case 'K':
			//	if (boolean_opt(&opts->km_file, sense, argv[index], "KMF")) break;
				if (boolean_opt(&opts->keep_clusts, sense, argv[index], "KpCl")) break;
				if (get_param(&params->kendall_rate_threshold, argv, argc, &index, "KRtThr", "sf4")) break;
				bad_opt(argv[index], sense, argv[0]);	
			case 'M':
			//	if (get_param(&params->min_inter_led_dist, argv, argc, &index, "MnLtDst", "sf8")) break;
			//	if (get_param(&params->min_theta_score, argv, argc, &index, "MnThSc", "ui1")) break;
			//	if (get_param(&params->min_walking_speed, argv, argc, &index, "MnWkSp", "ui1")) break;
			//	if (boolean_opt(&opts->mean_waveform_plots, sense, argv[index], "MnWvPlt")) break;
			//	if (get_param(&params->map_dir_bins, argv, argc, &index, "MpDrBn", "si4")) break;
			//	if (get_param(&params->map_phase_bins, argv, argc, &index, "MpPhBn", "si4")) break;
			//	if (get_param(&params->map_pixel_size, argv, argc, &index, "MpPxSz", "sf4")) break;
			//	if (get_param(&params->max_inter_led_dist, argv, argc, &index, "MxLtDst", "sf8")) break;
			//	if (get_param(&params->max_non_theta_score, argv, argc, &index, "MxNThSc", "ui1")) break;
			//	if (get_param(&params->max_still_speed, argv, argc, &index, "MxStSp", "ui1")) break;
				if (get_param(&params->max_clust, argv, argc, &index, "MxCl", "ui1")){ 
					opts->do_not_cut = TRUE;
					break;
				}
				if (get_param(&params->max_probe, argv, argc, &index, "MxPr", "ui1")){
					opts->do_not_cut = TRUE;
					 break;
				}
				bad_opt(argv[index], sense, argv[0]);
			case 'N':
				if (argv[index][0] == 'N' && argv[index][1] == 'o') {
					argv[index] += 2; sense = FALSE; goto negative_sense; }	
			//	if (boolean_opt(&opts->non_theta_only, sense, argv[index], "NThO")) break;
			//	if (boolean_opt(&opts->no_pos_samp, sense, argv[index], "NPos")) break;
				bad_opt(argv[index], sense, argv[0]);	
			case 'O':
			//	if (boolean_opt(&opts->output_to_printer, sense, argv[index], "OPr")) break;
			//	if (boolean_opt(&opts->output_to_ps_files, sense, argv[index], "OPs")) break;
				bad_opt(argv[index], sense, argv[0]);	
			case 'P':
			//	if (boolean_opt(&opts->phase_div_rate_maps, sense, argv[index], "PhDvRtMp")) break;
			//	if (boolean_opt(&opts->phase_plots, sense, argv[index], "PhPlt")) break;
			//	if (get_param(&params->plot_dir_bins, argv, argc, &index, "PltDrBn", "si4")) break;
			//	if (get_param(&params->plot_phase_bins, argv, argc, &index, "PltPhBn", "si4")) break;
				if (get_param(&params->pixel_collapse_factor, argv, argc, &index, "PxClF", "sf8")) break;
			//	if (get_param(&params->pixel_arena_rotate_rpm, argv, argc, &index, "PxArRpm", "sf8")) break;
			//	if (get_param(&params->pixel_room_rotate_rpm, argv, argc, &index, "PxRmRpm", "sf8")) break;
				bad_opt(argv[index], sense, argv[0]);
			case 'R':
				if (boolean_opt(&opts->read_app_map, sense, argv[index], "RdApMp")) {
					if (opts->read_app_map)
						(void) get_param(&params->read_app_map_file, argv, argc, &index, "RdApMp", "si1 *");
					break;
				}
			//	if (boolean_opt(&opts->read_breakpoints, sense, argv[index], "RdBkpt")) {
			//		if (opts->read_breakpoints)
			//			(void) get_param(&params->read_bkpts_file, argv, argc, &index, "RdBkpt", "si1 *");
			//		break;
			//	}
			//	if (boolean_opt(&opts->rot_dir_div_rate_maps, sense, argv[index], "RtDrDvRtMp")) break;
			//	if (boolean_opt(&opts->rate_maps, sense, argv[index], "RtMp")) break;
				if (boolean_opt(&opts->reverse_ts_binary, sense, argv[index], "RBin")) break;
				bad_opt(argv[index], sense, argv[0]);
			case 'S':
				if (get_param(&params->smoothing_factor, argv, argc, &index, "SmF", "sf8")) break;
				if (boolean_opt(&opts->smoothed_rate_maps, sense, argv[index], "SmRtMp")) break;
			//	if (boolean_opt(&opts->speed_map, sense, argv[index], "SpMp")) break;
			//	if (boolean_opt(&opts->still_only, sense, argv[index], "StO")) break;
				bad_opt(argv[index], sense, argv[0]);
			case 'T':
			//	if (boolean_opt(&opts->theta_only, sense, argv[index], "ThO")) break;
			//	if (get_param(&params->time_division_duration, argv, argc, &index, "TmDvDur", "sf8")) break;
			//	if (boolean_opt(&opts->time_div_rate_maps, sense, argv[index], "TmDvRtMp")) break;
			//	if (boolean_opt(&opts->time_map, sense, argv[index], "TmMp")) break;
				if (boolean_opt(&opts->ts_file, sense, argv[index], "TSF")) {
					if (opts->ts_file)
						(void) get_param(&params->ListOfTSFiles, argv, argc, &index, "TSF", "si1 *");
					break;
				}
				if (boolean_opt(&opts->tts_file, sense, argv[index], "TTSF")) {
					if (opts->tts_file)
						(void) get_param(&params->ListOfTTSFiles, argv, argc, &index, "TTSF", "si1 *");
					break;
				}
				bad_opt(argv[index], sense, argv[0]);
			case 'U':
			//	if (boolean_opt(&opts->uff_to_km, sense, argv[index], "U2KM")) break;
			//	bad_opt(argv[index], sense, argv[0]);
			case 'W':
			//	if (boolean_opt(&opts->walking_only, sense, argv[index], "WkO")) break;
			//	if (boolean_opt(&opts->extract_eeg, sense, argv[index], "WtEeg")) break;
				if (boolean_opt(&opts->write_smoothed_maps, sense, argv[index], "WtSmMaps")) break;
				if (boolean_opt(&opts->write_maps, sense, argv[index], "WtMaps")) break;
			//	if (boolean_opt(&opts->write_ts, sense, argv[index], "WtTs")) break;
			//	if (boolean_opt(&opts->write_waveforms, sense, argv[index], "WtWf")) break;
			//	if (boolean_opt(&opts->write_app_map, sense, argv[index], "WtApMp")) {
			//		if (opts->write_app_map)
			//			(void) get_param(&params->write_app_map_file, argv, argc, &index, "WtApMp", "si1 *");
			//		break;
			//	}
			//	if (boolean_opt(&opts->write_breakpoints, sense, argv[index], "WtBkpt")) {
			//		if (opts->write_breakpoints)
			//			(void) get_param(&params->write_bkpts_file, argv, argc, &index, "WtBkpt", "si1 *");
			//		break;
			//	}
				bad_opt(argv[index], sense, argv[0]);
			default:
				bad_opt(argv[index], sense, argv[0]);
		}
		++index;
	}
	
	if (argv[index][0] == '-') {
		(void) fprintf(stderr, "%c\n\tA data file must be the last argument on the command line.\n\n", 7);
		pause(3.0);
		instruct(argv[0]);
	}

	(void) strcpy(sess->name_str, argv[index]);

	set_directives(sess);

	if(opts->bpf_room_file && opts->bpf_arena_file){
		(void) fprintf(stderr, "%c\n\tYou may only analyse the Room frame or the Arena frame firing, but not both.\n\n", 7);
		exit(-1);
	}

	if(opts->bpf_room_file){
		sess->PositionRec = ROOM_POSITION_BPF_REC_TYPE;
		sess->Frame = ROOM_FRAME;
		sprintf(sess->FrameStr, "%s", "ROOM/");
	}else if(opts->bpf_arena_file){
		sess->PositionRec = ARENA_POSITION_BPF_REC_TYPE;
		sess->Frame = ARENA_FRAME;
		sprintf(sess->FrameStr, "%s", "ARENA/");
	}else if(opts->bpf_acute_file){
		opts->no_pos_samp = TRUE;
		sess->Frame = NO_FRAME;
		opts->write_smoothed_maps = FALSE;
        	opts->write_maps = FALSE;
		sprintf(sess->FrameStr, "%s", "");
	}else {
		sess->PositionRec = POSITION_REC;
		sess->Frame = NO_FRAME;
		sprintf(sess->FrameStr, "");
	}
		
	if(opts->km_file)
		strcpy(sess->DataFormat,"KM file");
	else if (opts->ts_file)
		strcpy(sess->DataFormat,"TS file");
	else if (opts->tts_file)
		strcpy(sess->DataFormat,"TTS file");
	else if (opts->bpf_room_file)
		strcpy(sess->DataFormat,"BPF file (Room frame positions)");
	else if (opts->bpf_arena_file)
		strcpy(sess->DataFormat,"BPF file (Arena frame positions)");
	else if (opts->bpf_acute_file)
		strcpy(sess->DataFormat,"BPF file (No position sampling)");
	else
		strcpy(sess->DataFormat,"UFF file");
		
	if((opts->no_pos_samp) && (!opts->bpf_acute_file))
		strcat(sess->DataFormat," (No position sampling)");
	return;
}


 ui1	get_param(param, argv, argc, index, param_str, p_type)
void		*param;
si1		**argv, *param_str, *p_type;
si4		argc, *index;
{
	extern void	pause(), instruct();
	ui1		temp_ui1;
	si1		temp_si1;
	ui2		temp_ui2;
	si2		temp_si2;
	ui4		temp_ui4;
	si4		temp_si4;
	sf4		temp_sf4;
	sf8		temp_sf8;


	if (strcmp(argv[*index], param_str) == 0) {
		++*index;
		if (*index == argc || argv[*index][0] == '-') {
			(void) fprintf(stderr, "%c\n\tThe parameter \"%s\" requires an argument.\n\n", 7, param_str);
			pause(3.0);
			instruct(argv[0]);
		}
		if (strcmp(p_type, "ui1") == 0) {
			temp_ui1 = (ui1) atoi(argv[*index]);
			if (*((ui1 *) param) == temp_ui1)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %d.\n", 7, param_str, temp_ui1);
			*((ui1 *) param) = temp_ui1; 
			return(TRUE);
		}
		if (strcmp(p_type, "si1") == 0) {
			temp_si1 = (si1) atoi(argv[*index]);
			if (*((si1 *) param) == temp_si1)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %d.\n", 7, param_str, temp_si1);
			*((si1 *) param) = temp_si1; 
			return(TRUE);
		}
		if (strcmp(p_type, "si1 *") == 0) {
			if (strcmp(*((si1 **) param), argv[*index]) == 0)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %s.\n", 7, param_str, (si1 *)
param);
			*((si1 **) param) = argv[*index]; 
			return(TRUE);
		}
		if (strcmp(p_type, "ui2") == 0) {
			temp_ui2 = (ui2) atoi(argv[*index]);
			if (*((ui2 *) param) == temp_ui2)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %d.\n", 7, param_str, temp_ui2);
			*((ui2 *) param) = temp_ui2; 
			return(TRUE);
		}
		if (strcmp(p_type, "si2") == 0) {
			temp_si2 = (si2) atoi(argv[*index]);
			if (*((si2 *) param) == temp_si2)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %d.\n", 7, param_str, temp_si2);
			*((si2 *) param) = temp_si2; 
			return(TRUE);
		}
		if (strcmp(p_type, "ui4") == 0) {
			temp_ui4 = (ui4) atoi(argv[*index]);
			if (*((ui4 *) param) == temp_ui4)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %d.\n", 7, param_str, temp_ui4);
			*((ui4 *) param) = temp_ui4; 
			return(TRUE);
		}
		if (strcmp(p_type, "si4") == 0) {
			temp_si4 = (si4) atoi(argv[*index]);
			if (*((si4 *) param) == temp_si4)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %d.\n", 7, param_str, temp_si4);
			*((si4 *) param) = temp_si4; 
			return(TRUE);
		}
		if (strcmp(p_type, "sf4") == 0) {
			temp_sf4 = (sf4) atof(argv[*index]);
			if (*((sf4 *) param) == temp_sf4)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %f.\n", 7, param_str, temp_sf4);
			*((sf4 *) param) = temp_sf4; 
			return(TRUE);
		}
		if (strcmp(p_type, "sf8") == 0) {
			temp_sf8 = (sf8) atof(argv[*index]);
			if (*((sf8 *) param) == temp_sf8)
				(void) fprintf(stderr, "%cPlease note: The parameter \"%s\" has the default value of %lf.\n", 7, param_str, temp_sf8);
			*((sf8 *) param) = temp_sf8; 
			return(TRUE);
		}
		(void) fprintf(stderr, "%c\n\tThe parameter type \"%s\" is unknown. Terminating program.\n\n", 7, p_type);
		exit(1);
	}
	return(FALSE);
}


 void	bad_opt(opt_str, sense, prog_name)
si1		*opt_str, *prog_name;
ui1		sense;
{
	extern void	instruct();
	extern void	pause();
	char		*s;

	if (sense)
		s = "";
	else
		s = "No";
	
	(void) fprintf(stderr, "%c\n\t\"%s%s\" is not a recognized option\n\n", 7, s, opt_str);

	pause(3.0);

	instruct(prog_name);
}


 ui1	boolean_opt(opt, sense, cmd_str, opt_str)
ui1		*opt, sense;
si1		*cmd_str, *opt_str;
{
	si1	*s;

	if (strcmp(cmd_str, opt_str) == 0) {	
		if (*opt == sense) {
			if (sense)
				s = "";
			else
				s = "No";
			(void) fprintf(stderr, "%cPlease note: The option \"%s%s\" is the default.\n", 7, s, opt_str);
		}
		*opt = sense;
		return(TRUE);
	}
	
	return(FALSE);
}


 void	set_defaults(sess)
SESSION		*sess;
{
	OPTIONS		*opts;
	PARAMETERS	*params;
	extern void	*gmem();


	opts = sess->opts = (OPTIONS *) gmem((size_t) 1, sizeof(OPTIONS));
	 
	opts->anal_probes = ANAL_PROBES_DFLT;
	opts->anal_zero_clusts = ANAL_ZERO_CLUSTS_DFLT;
	opts->autocorr_plots = AUTOCORR_PLOTS_DFLT;
	opts->between_event_flags_only = BETWEEN_EVENT_FLAGS_ONLY_DFLT;
	opts->bpf_room_file = BPF_ROOM_FILE_DFLT;
	opts->bpf_arena_file = BPF_ARENA_FILE_DFLT;
	opts->bpf_acute_file = BPF_ACUTE_FILE_DFLT;
	opts->cell_report = CELL_REPORT_DFLT;
	opts->change_rate_thresh = CHANGE_RATE_THRESH_DFLT;
	opts->change_smoothed_rate_thresh = CHANGE_SMOOTHED_RATE_THRESH_DFLT;
	opts->color_maps = COLOR_MAPS_DFLT;
	opts->do_not_cut = DO_NOT_CUT_DFLT;
	opts->dir_div_rate_maps = DIR_DIV_RATE_MAPS_DFLT;
	opts->dir_plots = DIR_PLOTS_DFLT;
	opts->elim_clusts = ELIM_CLUSTS_DFLT;
	opts->error_bars_on_waveforms = ERR_BARS_ON_WAVEFORMS_DFLT;
	opts->field_maps = FIELD_MAPS_DFLT;
	opts->fill_app_holes = FILL_APP_HOLES_DFLT;
	opts->interspike_int_plots = INTERSPIKE_INT_PLOTS_DFLT;
	opts->keep_clusts = KEEP_CLUSTS_DFLT;
	opts->km_file = KM_FILE_DFLT;
	opts->ignore_zeros = IGNORE_ZEROS_KENDALL_TAU_DFLT;
	opts->mean_waveform_plots = MEAN_WAVEFORM_PLOTS_DFLT;
	opts->no_pos_samp = NO_POS_SAMP_DFLT;
	opts->non_theta_only = NON_THETA_ONLY_DFLT;
	opts->output_to_printer = OUTPUT_TO_PRINTER_DFLT;
	opts->output_to_ps_files = OUTPUT_TO_PS_FILES_DFLT;
	opts->phase_div_rate_maps = PHASE_DIV_RATE_MAPS_DFLT;
	opts->phase_plots = PHASE_PLOTS_DFLT;
	opts->rate_maps = RATE_MAPS_DFLT;
	opts->read_app_map = READ_APP_MAP_DFLT;
	opts->read_breakpoints = READ_BREAKPOINTS_DFLT;
	opts->reverse_ts_binary = REVERSE_TS_BINARY_DFLT;
	opts->rot_dir_div_rate_maps = ROT_DIR_DIV_RATE_MAPS_DFLT;
	opts->smoothed_rate_maps = SMOOTHED_RATE_MAPS_DFLT;
	opts->speed_map = SPEED_MAP_DFLT;
	opts->still_only = STILL_ONLY_DFLT;
	opts->theta_only = THETA_ONLY_DFLT;
	opts->time_div_rate_maps = TIME_DIV_RATE_MAPS_DFLT;
	opts->time_map = TIME_MAP_DFLT;
	opts->ts_file = TS_FILE_DFLT;
	opts->tts_file = TS_FILE_DFLT;
	opts->uff_to_km = UFF_TO_KM_DFLT;
	opts->walking_only = WALKING_ONLY_DFLT;
	opts->write_app_map = WRITE_APP_MAP_DFLT;
	opts->write_breakpoints = WRITE_BREAKPOINTS_DFLT;
	opts->write_smoothed_maps = WRITE_SMOOTHED_MAPS_DFLT;
	opts->write_maps = WRITE_MAPS_DFLT;
	opts->write_ts = WRITE_TS_DFLT;
	opts->write_tts = WRITE_TTS_DFLT;
	opts->write_waveforms = WRITE_WAVEFORMS_DFLT;

	params = sess->params = (PARAMETERS *) gmem((size_t) 1, sizeof(PARAMETERS));

	params->analysis_interval_sec = ANALYSIS_INTERVAL_SEC_DFLT;
	params->autocorr_bins = AUTOCORR_BINS_DFLT;
	params->autocorr_end_ms = AUTOCORR_END_MS_DFLT;
	params->autocorr_start_ms = AUTOCORR_START_MS_DFLT;
	params->bin_size_ms = BIN_SIZE_MS_DFLT;
	params->event_begin_flag = EVENT_BEGIN_FLAG_DFLT;
	params->event_end_flag = EVENT_END_FLAG_DFLT;
	params->global_rate_thresh = GLOBAL_RATE_THRESH_DFLT;
	params->global_smoothed_rate_thresh = GLOBAL_SMOOTHED_RATE_THRESH_DFLT;
	params->global_speed_thresh = GLOBAL_SPEED_THRESH_DFLT;
	params->global_time_thresh = GLOBAL_TIME_THRESH_DFLT;
	params->interspike_int_bins = INTERSPIKE_INT_BINS_DFLT;
	params->interspike_int_end_ms = INTERSPIKE_INT_END_MS_DFLT;
	params->interspike_int_start_ms = INTERSPIKE_INT_START_MS_DFLT;
	params->kendall_rate_threshold = KENDALL_RATE_THRESH_DFLT;
	params->ListOfTSFiles = LIST_OF_TS_FILES_DFLT;
	params->ListOfTTSFiles = LIST_OF_TTS_FILES_DFLT;
	params->map_dir_bins = MAP_DIR_BINS_DFLT;
	params->map_phase_bins = MAP_PHASE_BINS_DFLT;
	params->map_pixel_size = MAP_PIXEL_SIZE_DFLT;
	params->max_inter_led_dist = MAX_INTER_LED_DIST_DFLT;
	params->max_non_theta_score = MAX_NON_THETA_SCORE_DFLT;
	params->max_still_speed = MAX_STILL_SPEED_DFLT;
	params->max_clust = 0;
	params->max_probe = 0;
	params->min_inter_led_dist = MIN_INTER_LED_DIST_DFLT;
	params->min_theta_score = MIN_THETA_SCORE_DFLT;
	params->min_walking_speed = MIN_WALKING_SPEED_DFLT;
	params->pixel_collapse_factor = PIXEL_COLLAPSE_FACTOR_DFLT;
	params->pixel_room_rotate_rpm = PIXEL_ROOM_ROTATE_RPM_DFLT;
	params->pixel_arena_rotate_rpm = PIXEL_ARENA_ROTATE_RPM_DFLT;
	params->plot_dir_bins = PLOT_DIR_BINS_DFLT;
	params->plot_phase_bins = PLOT_PHASE_BINS_DFLT;
	params->read_app_map_file = READ_APP_MAP_FILE_DFLT;
	params->read_bkpts_file = READ_BKPTS_FILE_DFLT;
	params->smoothing_factor = SMOOTHING_FACTOR_DFLT;
	params->time_division_duration = TIME_DIV_DURATION_DFLT;
	params->write_app_map_file = WRITE_APP_MAP_FILE_DFLT;
	params->write_bkpts_file = WRITE_BKPTS_FILE_DFLT;

	sess->PositionRec = POSITION_REC;

	return;
}


 void	set_directives(sess)
SESSION		*sess;
{
	OPTIONS		*opts;
	PARAMETERS	*params;
	DIRECTIVES	*directives;
	extern void	*gmem();
	 void	km_exceptions();
	 void	ts_exceptions();
	 void	tts_exceptions();


	opts = sess->opts;
	params = sess->params;
	directives = sess->directives = (DIRECTIVES *) gmem((size_t) 1, sizeof(DIRECTIVES));

	/* assign value of each directive according to options */

	directives->calc_direction = FALSE;
	if (	opts->dir_div_rate_maps || \
		opts->dir_plots || \
		opts->rot_dir_div_rate_maps)
			directives->calc_direction = TRUE;

	directives->calc_mean_waves = FALSE;
	if (	opts->mean_waveform_plots)
			directives->calc_mean_waves = TRUE;

	directives->calc_speed = FALSE;
	if (	opts->speed_map || \
		opts->still_only || \
		opts->walking_only)
			directives->calc_speed = TRUE;

	directives->calc_theta = FALSE;
	if (	opts->non_theta_only || \
		opts->phase_div_rate_maps || \
		opts->phase_plots || \
		opts->theta_only)
			directives->calc_theta = TRUE;
	
	directives->generate_ps_maps = FALSE;
	if (	!opts->no_pos_samp && \
		opts->field_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->smoothed_rate_maps || \
		opts->speed_map || \
		opts->time_div_rate_maps || \
		opts->time_map)
			directives->generate_ps_maps = TRUE;

	directives->generate_ps_plots = FALSE;
	if (	opts->autocorr_plots || \
		opts->cross_corr_plots || \
		opts->dir_plots || \
		opts->interspike_int_plots || \
		opts->mean_waveform_plots || \
		opts->phase_plots)
			directives->generate_ps_plots = TRUE;

	directives->make_field_maps = FALSE;
	if (	opts->cell_report || \
		opts->field_maps)
			directives->make_field_maps = TRUE;
	
	directives->make_rate_maps = FALSE;
	if (	opts->cell_report || \
		opts->dir_div_rate_maps || \
		opts->field_maps || \
		opts->phase_div_rate_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->smoothed_rate_maps || \
		opts->time_div_rate_maps || \
		opts->write_maps )
			directives->make_rate_maps = TRUE;
	
	directives->make_show_rate_maps = FALSE;
	if (	!opts->no_pos_samp && \
		opts->dir_div_rate_maps || \
		opts->phase_div_rate_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->time_div_rate_maps)
			directives->make_show_rate_maps = TRUE;

	directives->make_smoothed_maps = FALSE;
	if (	opts->smoothed_rate_maps || \
		opts->write_smoothed_maps) 
			directives->make_smoothed_maps = TRUE;
	
	directives->make_speed_map = FALSE;
	if (	opts->speed_map)
			directives->make_speed_map = TRUE;
	
	directives->make_time_map = FALSE;
	if (	opts->cell_report || \
		opts->dir_div_rate_maps || \
		opts->field_maps || \
		opts->phase_div_rate_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->smoothed_rate_maps || \
		opts->time_div_rate_maps || \
		opts->speed_map || \
		opts->time_map || \
		opts->write_maps || \
		opts->write_smoothed_maps )
			directives->make_time_map = TRUE;
	
	/* special conditions */
	
	if (opts->km_file)		
		km_exceptions(sess);

	if (opts->ts_file)		
		ts_exceptions(sess);

	if (opts->tts_file)		
		tts_exceptions(sess);

	if (	opts->time_div_rate_maps || \
		opts->phase_div_rate_maps || \
		opts->dir_div_rate_maps || \
		opts->rot_dir_div_rate_maps) {
			opts->write_breakpoints = TRUE;
			params->write_bkpts_file = "temp";
	}
			
	/* these can only be set once the scaling factor is known */
	params->x_size = (si4) ceil((sf8) X_TRACKING_RESOLUTION / params->pixel_collapse_factor);
	params->y_size = (si4) ceil((sf8) Y_TRACKING_RESOLUTION / params->pixel_collapse_factor);
	params->area = params->x_size * params->y_size;
	params->x_tracker_resolution = (si4) X_TRACKING_RESOLUTION;
	params->y_tracker_resolution = (si4) Y_TRACKING_RESOLUTION;

	return;
}


 void	km_exceptions(sess)		/* KM files have only spike time and location information, */
SESSION	*sess;					/* thus many options do not work with them. This is a little */	
{						/* bit of a pain in the ass here, but after this they behave */
	OPTIONS		*opts;			/* just fine. */
	DIRECTIVES	*directives;


	opts = sess->opts;
	directives = sess->directives;

	directives->generate_ps_maps = FALSE;
	if (directives->calc_theta) {
		directives->calc_theta = FALSE;
		opts->non_theta_only = FALSE;
		opts->phase_div_rate_maps = FALSE;
		opts->phase_plots = FALSE;
		opts->theta_only = FALSE;
	}

	directives->generate_ps_maps = FALSE;
	if (	opts->dir_div_rate_maps || \
		opts->field_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->smoothed_rate_maps || \
		opts->speed_map || \
		opts->time_div_rate_maps || \
		opts->time_map)
			directives->generate_ps_maps = TRUE;

	directives->generate_ps_plots = FALSE;
	if (	opts->interspike_int_plots)
			directives->generate_ps_plots = TRUE;

	if (directives->calc_mean_waves) {
		directives->calc_mean_waves = FALSE;
		opts->mean_waveform_plots = FALSE;
	}

	directives->make_rate_maps = FALSE;
		if (	opts->cell_report || \
		opts->dir_div_rate_maps || \
		opts->field_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->smoothed_rate_maps || \
		opts->time_div_rate_maps)
			directives->make_rate_maps = TRUE;

	directives->make_show_rate_maps = FALSE;
	if (	opts->dir_div_rate_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->time_div_rate_maps)
			directives->make_show_rate_maps = TRUE;

	directives->make_time_map = FALSE;
	if (	opts->cell_report || \
		opts->dir_div_rate_maps || \
		opts->field_maps || \
		opts->rate_maps || \
		opts->rot_dir_div_rate_maps || \
		opts->smoothed_rate_maps || \
		opts->time_div_rate_maps || \
		opts->speed_map || \
		opts->time_map)
			directives->make_time_map = TRUE;

	opts->between_event_flags_only = FALSE;
	opts->anal_probes = FALSE;
	opts->anal_zero_clusts = FALSE;
	opts->uff_to_km = FALSE;

	return;
}

 void	ts_exceptions(sess)		/* TS files have only spike time and location information, */
SESSION	*sess;					/* thus many options do not work with them. This is a little */	
{						/* bit of a pain in the ass here, but after this they behave */
	OPTIONS		*opts;			/* just fine. */
	DIRECTIVES	*directives;


	opts = sess->opts;
	directives = sess->directives;

	directives->calc_mean_waves = FALSE;
	opts->mean_waveform_plots = FALSE;
	opts->between_event_flags_only = FALSE;
	opts->anal_probes = FALSE;
	opts->anal_zero_clusts = FALSE;
	opts->uff_to_km = FALSE;
	opts->bpf_room_file = FALSE;
	opts->bpf_arena_file = FALSE;
	opts->bpf_acute_file = FALSE;
	
	directives->calc_theta = FALSE;
	opts->non_theta_only = FALSE;
	opts->phase_div_rate_maps = FALSE;
	opts->phase_plots = FALSE;
	opts->theta_only = FALSE;

	return;
}

void	tts_exceptions(sess)		/* TTS files have only spike time and location information, */
SESSION	*sess;					/* thus many options do not work with them. This is a little */	
{						/* bit of a pain in the ass here, but after this they behave */
	OPTIONS		*opts;			/* just fine. */
	DIRECTIVES	*directives;


	opts = sess->opts;
	directives = sess->directives;

	directives->calc_mean_waves = FALSE;
	opts->mean_waveform_plots = FALSE;
	opts->between_event_flags_only = FALSE;
	opts->anal_probes = FALSE;
	opts->anal_zero_clusts = FALSE;
	opts->uff_to_km = FALSE;
	opts->bpf_room_file = FALSE;
	opts->bpf_arena_file = FALSE;
	opts->bpf_acute_file = FALSE;
	
	directives->calc_theta = FALSE;
	opts->non_theta_only = FALSE;
	opts->phase_div_rate_maps = FALSE;
	opts->phase_plots = FALSE;
	opts->theta_only = FALSE;

	return;
}
