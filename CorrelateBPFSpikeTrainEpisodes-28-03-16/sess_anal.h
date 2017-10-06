
#ifndef SESS_ANAL_IN
#define SESS_ANAL_IN


#include "size_types.h"
#include "uff.h"
#include "bpf.h"
#include "postscript.h"

#define	MyStdErr	stderr

#define DEBUG(x)	fprintf(stdout,"HERE %d\n",x); fflush(stdout);
#define DEBUGi(x)	fprintf(stdout,"HERE %d\n",x); fflush(stdout);
#define DEBUGf(x)	fprintf(stdout,"HERE %f\n",x); fflush(stdout);
#define DEBUGs(x)	fprintf(stdout,"HERE %s\n",x); fflush(stdout);

#define TRUE		1
#define FALSE		0

#define	NO_FRAME	0
#define	ROOM_FRAME	1
#define	ARENA_FRAME	2
#define	ROOM_FRAME_STR	"ROOM"
#define	ARENA_FRAME_STR	"ARENA"

/* default values for options */
#define AUTOCORR_PLOTS_DFLT			FALSE
#define ANAL_PROBES_DFLT			FALSE
#define ANAL_ZERO_CLUSTS_DFLT			FALSE
#define BETWEEN_EVENT_FLAGS_ONLY_DFLT		FALSE
#define BPF_FILE_DFLT				FALSE
#define BPF_ROOM_FILE_DFLT                      TRUE
#define BPF_ARENA_FILE_DFLT                     FALSE
#define BPF_ACUTE_FILE_DFLT                     FALSE
#define CELL_REPORT_DFLT			TRUE
#define CHANGE_RATE_THRESH_DFLT			FALSE
#define CHANGE_SMOOTHED_RATE_THRESH_DFLT	FALSE
#define COLOR_MAPS_DFLT				FALSE
#define CROSS_CORR_PLOTS_DFLT			FALSE
#define DIR_DIV_RATE_MAPS_DFLT			FALSE
#define DIR_PLOTS_DFLT				FALSE
#define DO_NOT_CUT_DFLT				FALSE
#define ELIM_CLUSTS_DFLT			FALSE
#define ERR_BARS_ON_WAVEFORMS_DFLT		FALSE
#define EXTRACT_EEG_DFLT			FALSE
#define FIELD_MAPS_DFLT				FALSE
#define FILL_APP_HOLES_DFLT			TRUE
#define IGNORE_ZEROS_KENDALL_TAU_DFLT		FALSE
#define INTERSPIKE_INT_PLOTS_DFLT		FALSE
#define IRR_POS_SAMP_DFLT			FALSE
#define KEEP_CLUSTS_DFLT			FALSE
#define KM_FILE_DFLT				FALSE
#define MEAN_WAVEFORM_PLOTS_DFLT		FALSE
#define NON_THETA_ONLY_DFLT			FALSE
#define NO_POS_SAMP_DFLT			FALSE
#define OUTPUT_TO_PRINTER_DFLT			FALSE
#define OUTPUT_TO_PS_FILES_DFLT			FALSE
#define PHASE_DIV_RATE_MAPS_DFLT		FALSE
#define PHASE_PLOTS_DFLT			FALSE
#define RATE_MAPS_DFLT				FALSE
#define READ_APP_MAP_DFLT			FALSE
#define READ_APP_TMAP_DFLT			FALSE
#define READ_BREAKPOINTS_DFLT			FALSE
#define ROT_DIR_DIV_RATE_MAPS_DFLT		FALSE
#define SMOOTHED_RATE_MAPS_DFLT			FALSE
#define SPEED_MAP_DFLT				FALSE
#define STILL_ONLY_DFLT				FALSE
#define THETA_ONLY_DFLT				FALSE
#define TIME_DIV_RATE_MAPS_DFLT			FALSE
#define TIME_MAP_DFLT				FALSE
#define TS_FILE_DFLT				FALSE
#define UFF_FILE_DFLT				FALSE
#define UFF_TO_KM_DFLT				FALSE
#define WALKING_ONLY_DFLT			FALSE
#define WRITE_APP_MAP_DFLT			FALSE
#define WRITE_BREAKPOINTS_DFLT			FALSE
#define WRITE_SMOOTHED_MAPS_DFLT		FALSE
#define WRITE_MAPS_DFLT				FALSE
#define WRITE_WCP_POSITION_DFLT			FALSE
#define WRITE_TS_DFLT				FALSE
#define WRITE_TTS_DFLT				FALSE
#define WRITE_WAVEFORMS_DFLT			FALSE
#define REVERSE_TS_BINARY_DFLT			FALSE

/* default values for parameters */			/* Note: parameters differ from options in that they tend to */
#define AUTOCORR_BINS_DFLT			100	/* govern how a particular type of processing is done, while */
#define AUTOCORR_END_MS_DFLT			500.0	/* options tend to govern which type of processing is done */
#define AUTOCORR_START_MS_DFLT			0.0
#define ANALYSIS_INTERVAL_SEC_DFLT              300
#define BIN_SIZE_MS_DFLT                        250
#define CROSS_CORR_BINS_DFLT			100
#define CROSS_CORR_END_MS_DFLT			500.0
#define CROSS_CORR_START_MS_DFLT		0.0
#define EVENT_BEGIN_FLAG_DFLT			'b'	
#define EVENT_END_FLAG_DFLT			'e'
#define GLOBAL_RATE_THRESH_DFLT			1.0e-6
#define GLOBAL_SMOOTHED_RATE_THRESH_DFLT	1.0e-1
#define GLOBAL_TIME_THRESH_DFLT			1.0e-6
#define INTERSPIKE_INT_BINS_DFLT		100
#define INTERSPIKE_INT_END_MS_DFLT		50.0
#define INTERSPIKE_INT_START_MS_DFLT		0.0
#define KENDALL_RATE_THRESH_DFLT                100.0   // this means to use (100 % of grand_rate)
#define LIST_OF_TS_FILES_DFLT                   ""
#define LIST_OF_TTS_FILES_DFLT                  ""
#define MAP_DIR_BINS_DFLT			8
#define MAP_PHASE_BINS_DFLT			8
#define MAX_INTER_LED_DIST_DFLT			10.0	/* this is real distance in cm */
#define MIN_INTER_LED_DIST_DFLT			2.5	/* this is real distance in cm */
#define PIXEL_COLLAPSE_FACTOR_DFLT		8.0
#define POS_FILTER_MAX_DT_DFLT			1.0	/* time in seconds */
#define POS_FILTER_MAX_DIST_DFLT		2.0	/* distance in pixels */
#define PIXEL_ARENA_ROTATE_RPM_DFLT		0.0
#define PIXEL_ROOM_ROTATE_RPM_DFLT		0.0
#define PLOT_DIR_BINS_DFLT			36
#define PLOT_PHASE_BINS_DFLT			36
#define READ_APP_MAP_FILE_DFLT			""
#define READ_APP_TMAP_FILE_DFLT			""
#define READ_BKPTS_FILE_DFLT			""
#define SMOOTHING_FACTOR_DFLT			1.25	/* 0.0 or greater 0.0 is no_smoothing. The factor is the radius to smooth over as a factor of the collapsed pixels */
#define TIME_DIV_DURATION_DFLT			5.0	/* (float) minutes in session time divisions */ 
#define TIME_END_DFLT				0.0	/* analyse to the last data record */
#define TIME_START_DFLT				0.0	/* analyse from the first data record */
#define WRITE_APP_MAP_FILE_DFLT			""
#define WRITE_BKPTS_FILE_DFLT			""

/* various other constants - of the type rarely needing modification */
#define	MIN_FIELD_SIZE_DFLT		9
#define	MIN_FIELD_SIZE			9
#define X_TRACKING_RESOLUTION		256	/* range of values stored in the raw data, usually 8 bits - hence 256 */
#define Y_TRACKING_RESOLUTION		256	/* range of values stored in the raw data, usually 8 bits - hence 256 */
#define PIXEL_SIZE			0.625	/* (floating point) constant to convert (tracking resolution) pixels to cm */
#define POSITION_SAMPLING_FREQUENCY     60.0    /* in Hz, 50.0 for Europe, 60.0 for U.S. - used only for getting data from KM files */
#define MAX_TIME_STAMP_ERROR		3.0	/* proportion of "true" position sampling interval that a sampling interval can */
						/* vary -> 0.075 recommended */
#define POSITION_SAMPLING_FOR_SPEED     20
#define	RATIO				0.8
#define COLOR_CATS			7
#define N_BKPTS				6

#define MAX_MULTITRODE_CHANNELS		4 // for a tetrode
#define MAX_SPIKE_AD_VALS		96 // for the Axona BPF file

#define	MAX_BURST_ISI			(TIME_STAMPS_PER_SECOND / 100) // 10 ms

#define PROPORTION_WAVEFORM_PEAK_FOR_WIDTH 0.3 // calculate waveform width at 30%  peak amplitude

#define	TIME_P_MAP_TOLERANCE		0.9

/* internal constants and flags for program execution */
#define NO_DIRECTION	-1
#define NO_RATE		-1.0
#define CUT		1
#define NOT_CUT		127
#define FILE_NAME_CHARS 128

#define CLOCKWISE		0
#define COUNTERCLOCKWISE	1
#define NUM_ROT_DIRS		2

#define WHITE		0
#define YELLOW		1
#define ORANGE		2
#define RED		3
#define GREEN		4
#define BLUE		5
#define PURPLE		6
#define GRAY		7
#define BLACK		8


typedef struct {
	ui1	autocorr_plots;
	ui1	anal_probes;
	ui1	anal_zero_clusts;
	ui1	between_event_flags_only;
	ui1	bpf_acute_file;
	ui1	bpf_arena_file;
	ui1	bpf_room_file;
	ui1	bpf_2frame_file;
	ui1	cell_report;
	ui1	change_rate_thresh;
	ui1	change_smoothed_rate_thresh;
	ui1	color_maps;
	ui1	cross_corr_plots;
	ui1	do_not_cut;
	ui1	dir_div_rate_maps;
	ui1	dir_plots;
	ui1	elim_clusts;
	ui1	error_bars_on_waveforms;
	ui1	extract_eeg;
	ui1	field_maps;
	ui1	fill_app_holes;
	ui1     ignore_zeros;
	ui1	interspike_int_plots;
	ui1	irregular_pos_samp;
	ui1	keep_clusts;
	ui1	km_file;
	ui1	mean_waveform_plots;
	ui1	no_pos_samp;
	ui1	non_theta_only;
	ui1	output_to_printer;
	ui1	output_to_ps_files;
	ui1	phase_div_rate_maps;
	ui1	phase_plots;
	ui1	rate_maps;
	ui1	read_app_map;
	ui1	read_app_tmap;
	ui1	read_breakpoints;
	ui1     reverse_ts_binary;
	ui1	rot_dir_div_rate_maps;
	ui1	smoothed_rate_maps;
	ui1	speed_map;
	ui1	still_only;
	ui1	theta_only;
	ui1	time_div_rate_maps;
	ui1	time_map;
	ui1	ts_file;
	ui1	tts_file;
	ui1	uff_file;
	ui1	uff_to_km;
	ui1	walking_only;
	ui1	write_app_map;
	ui1	write_breakpoints;
	ui1	write_smoothed_maps;
	ui1	write_maps;
	ui1	write_wcp_positions;
	ui1	write_ts;
	ui1	write_tts;
	ui1	write_waveforms;
} OPTIONS;

typedef struct {
	ui4     analysis_interval_sec;
	si4	area;
	si4	autocorr_bins;
	sf4	autocorr_end_ms;
	sf4	autocorr_start_ms;
	si4     bin_size_ms;
	si4	cross_corr_bins;
	sf4	cross_corr_end_ms;
	sf4	cross_corr_start_ms;
	ui1	event_begin_flag;
	ui1	event_end_flag;
	sf4	global_rate_thresh;
	sf4	global_smoothed_rate_thresh;
	ui1	global_speed_thresh;
	sf4	global_time_thresh;
	si4	interspike_int_bins;
	sf4	interspike_int_end_ms;
	sf4	interspike_int_start_ms;
        sf4     kendall_rate_threshold;
        si1     *ListOfTSFiles;
        si1     *ListOfTTSFiles;
	si4	map_dir_bins;
	si4	map_phase_bins;
	sf4	map_pixel_size;
	ui1     max_clust;
	sf8	max_inter_led_dist;
	ui1	max_non_theta_score;
	ui1     max_probe;
	ui1	max_still_speed;
	sf8	min_inter_led_dist;
	ui4	min_field_size;
	ui1	min_theta_score;
	ui1	min_walking_speed;
	sf8	position_filter_max_dt;
	sf8	position_filter_max_dx;
	sf8	pixel_collapse_factor;
	sf8	pixel_arena_rotate_rpm;
	sf8	pixel_room_rotate_rpm;
	si4	plot_dir_bins;
	si4	plot_phase_bins;
	sf4	position_samp_freq;
	si1	*read_app_map_file;
	si1	*read_app_tmap_file;
	si1	*read_bkpts_file;
	sf8	smoothing_factor;
	si4	time_bins;
	sf8	time_division_duration;
	sf4	TimeEndAnalysis;
	sf4	TimeStartAnalysis;
	si1	*write_app_map_file;
	si1	*write_bkpts_file;
	si4	x_size;
	si4	y_size;
	si4	y_tracker_resolution;
	si4	x_tracker_resolution;
} PARAMETERS;

typedef struct {			/* Directives are a mechanism for the program to decide which */
	ui1	calc_direction;		/* processing steps should be taken given the set of options */
	ui1	calc_mean_waves;	/* selected. The user has no interaction with the fields in */
	ui1	calc_speed;		/* this structure. */
	ui1	calc_theta;
	ui1	generate_ps_maps;
	ui1	generate_ps_plots;
	ui1	make_field_maps;
	ui1	make_rate_maps;
	ui1	make_show_rate_maps;
	ui1	make_smoothed_maps;
	ui1	make_speed_map;
	ui1	make_time_map;
} DIRECTIVES;

typedef struct {
	sf8		center_rate;
	si4		center_x;
	si4		center_y;
	sf8		grand_rate;
	si4		number;
	si4		total_pixels;
	si4		total_spikes;
	sf8		total_time;
} FIELD_DATA;

typedef struct {
	ui1		clust;
	FIELD_DATA	**field_data;
	ui1		**field_map;
	sf8		grand_rate;
	sf8		p_active_pixels;
	sf8		mean_waveform[MAX_MULTITRODE_CHANNELS][MAX_SPIKE_AD_VALS];
	sf8		mean_waveform_sds[MAX_MULTITRODE_CHANNELS][MAX_SPIKE_AD_VALS];
	sf8		mean_waveform_cv[MAX_MULTITRODE_CHANNELS];
	sf8		mean_waveform_width[MAX_MULTITRODE_CHANNELS];
	si1		name_str[32];
	si4		num_fields;
	si4		num_rate_bkpts;
	si4		num_smoothed_rate_bkpts;
	FIELD_DATA	*out_of_field_data;
	sf4		p_burst_spikes;
	sf4		peak_isi;
	ui1		probe;
	sf8		rate_coher_p;
	sf8		rate_coher_r;
	sf8		rate_coher_z;
	sf8		rate_info_content;
	sf8		rate_info_content_z;
	sf4		rate_bkpts[N_BKPTS];
	sf4		**rate_map;
	sf4		rate_medns[N_BKPTS];
	sf4		rate_thresh;
	sf8		rate_time_p;
	sf8		rate_time_r;
	sf8		rate_time_z;
	ui1		**show_field_map;
	ui1		**show_rate_map;
	ui1		**show_smoothed_rate_map;
	ui2		**spike_map;
	ui2		**fine_spike_map;
	sf4		smoothed_rate_bkpts[N_BKPTS];
	sf4		**smoothed_rate_map;
	sf4		**smoothed_spike_map;
	sf4		smoothed_rate_medns[N_BKPTS];
	sf4		smoothed_rate_thresh;
	sf8		spike_width_in_samps;
	si4		total_in_apparatus_spikes;
	si4		total_spikes;
	sf8		waveform_correlation;
} CELL;

typedef struct {
	sf8		average;
	sf4		bkpts[N_BKPTS];
	ui1		**show_map;
	sf4		**map;
	ui1		max;
	sf4		medns[N_BKPTS];
	ui1		min;
	si4		num_bkpts;
	ui1		threshold;
} SPEED;

typedef struct {
	sf4		bkpts[N_BKPTS];
	ui1		**show_map;
	sf4		**map;
	sf4		**fine_map;
	sf4		**smoothed_map;
	sf4		medns[N_BKPTS];
	si4		num_bkpts;
	sf4		threshold;
	sf8		total_in_apparatus_secs;
	sf8		total_secs;
} TIME;

typedef struct {
	ui1		clust;
	ui1		*data_offset;
	si2		direction;
	si1             filename[FILE_NAME_CHARS];
	ui1		probe;
	ui1		running_speed;
	ui1		theta_state;
	si2		theta_phase;
	ui4		time_stamp;
	ui1		type;
	ui1		red_x;
	ui1		red_y;
	ui1		green_x;
	ui1		green_y;
	ui1		RoomX;
	ui1		RoomY;
	ui2		RoomAng;
	ui1		ArenaX;
	ui1		ArenaY;
	ui2		ArenaAng;
} RECORD;

typedef struct {
	si4		center_x;
	si4		center_y;
	si4		bottom;
	si4		left;
	ui1		**map;
	sf4		**p_map;
	sf4		**t_map;
	ui1		**smoothed_map;
	si4		right;
	si4		top;
	si4		total_pixels;
} APPARATUS;

typedef struct SESSION {
	si1		anal_time[64];
	APPARATUS	*apparatus;
	APPARATUS	*secondary_apparatus;
	ui1		PositionRec;
	ui1		*bpf_data;
	ui1		OffsetRoomX1;
	ui1		OffsetRoomY1;
	ui1		OffsetRoomZ1;
	ui1		OffsetRoomAngX;
	ui1		OffsetRoomAngY;
	ui1		OffsetRoomAng;
	ui1		OffsetArenaX1;
	ui1		OffsetArenaY1;
	ui1		OffsetArenaZ1;
	ui1		OffsetArenaAngX;
	ui1		OffsetArenaAngY;
	ui1		OffsetArenaAng;
	ui1		BPFRecordTypes[256];
	ui4		BPFRecordSizes[256];
	ui4		BPFRecordTypeNumbers[256];
	ui4		BPFRecordVoltageSamples[256];
	ui4		*BPFEEGChannels;
	si1		**cell_map;
	CELL		**cells;
	CELL		**secondary_cells;
	ui1		*data;
	si1		DataFormat[64];
	si1 		date_str[64];
	si1		desc_str[UFF_FILE_DESC_LEN];
	struct SESSION	**directional_subdivisions;
	DIRECTIVES	*directives;
	ui4		file_len;
	ui1		Frame;
	ui1             SecondaryFrame;
	si1		FrameStr[64];
	si1 		name_str[64];
	si4		num_cells;
	si4		num_probes;
	si4		num_recs;
	OPTIONS		*opts;
	PARAMETERS	*params;
	struct SESSION	**phase_subdivisions;
	si1		*probe_map;
	CELL		**probes;
	CELL		**secondary_probes;
	RECORD		*recs;
	struct SESSION	**rotational_dir_subdivisions;
	SPEED		*speed;
	SPEED		*secondary_speed;
	TIME		*time;
	TIME		*secondary_time;
	struct SESSION	**temporal_subdivisions;
	si1 		time_str[64];
} SESSION;


#endif

