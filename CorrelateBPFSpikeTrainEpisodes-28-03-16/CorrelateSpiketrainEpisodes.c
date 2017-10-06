// 28.03.16 Added ui8 type to size_types.h
// Necessary to accomodate very large MaxBins and NumPairs in CorrelateSpikeTrains() and CorrelateSpikeTrainsNoPos() and functions in Kendall.c
//

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "size_types.h"
#include "sess_anal.h"

#define	DEFAULT_BIN_SIZE	250
#define	DEFAULT_ANALYSIS_INTERVAL 0


si4	main(argc, argv)
si4	argc;
si1	**argv;
{
	SESSION		sess;
	ui4		file_len;
	si4		i, j;
	extern void	allocate_memory();
	extern void	allocate_memory_for_smoothing();
	extern void	calculate_theta();
	extern void	calculate_direction();
	extern void	calculate_mean_waveforms();
	extern void	calculate_speed();
	extern void	clean_spike_maps();
	extern void	clean_smoothed_maps();
	extern void	clean_time_map();
	extern void	collapse_pixels();
	extern void	CorrelateSpikeTrains();
	extern void	CorrelateSpikeTrainsNoPos();
	extern void	date_time_str();
	extern void	extract_eeg();
	extern void	fill_xy_fields();
	extern void	filter_recs_for_events();
	extern void	filter_recs_for_non_theta();
	extern void	filter_recs_for_stillness();
	extern void	filter_recs_for_theta();
	extern void	filter_recs_for_walking();
	extern void	get_app_map();
	extern void	get_cell_data();
	extern void	get_data();
	extern void	get_pos_samp_freq();
	extern void	get_records();
	extern void	get_recs_from_km();
	extern void	get_recs_from_ts();
	extern void	get_recs_from_tts();
	extern void	get_recs_from_ts_files();
	extern void	get_recs_from_tts_files();
	extern	si1	is_bpf(), is_spike();
	extern void	name_cells();
	extern void	make_cell_map();
	extern void	make_directional_subdivisions();
	extern void	make_field_maps();
	extern void	make_phase_subdivisions();
	extern void	make_show_field_maps();
	extern void	make_show_rate_maps();
	extern void	make_show_smoothed_rate_maps();
	extern void	make_show_speed_map();
	extern void	make_show_time_map();
	extern void	make_rate_maps();
	extern void	make_rotational_dir_subdivisions();
	extern void	make_smoothed_maps();
	extern void	make_speed_map();
	extern void	make_spike_maps();
	extern void	make_temporal_subdivisions();
	extern void	make_time_map();
	extern void	ps_print_cell_report();
	extern void	ps_print_dir_div_maps();
	extern void	ps_print_maps();
	extern void	ps_print_phase_div_maps();
	extern void	ps_print_plots();
	extern void	ps_print_time_div_maps();
	extern void	rotate_pixels();
	extern void	set_opts_and_params();
	extern void	set_thresholds();
	extern void	uff_to_km();
	extern void	write_smoothed_maps();
	extern void	write_maps();
	extern void	write_waveforms();

#ifndef BIG_ENDIAN_MACHINE
#ifndef LITTLE_ENDIAN_MACHINE
	(void) fprintf(stderr, "%c\n\tYou must define either \"BIG_ENDIAN_MACHINE\" or \"LITTLE_ENDIAN_MACHINE\"\nin \"size_types.h\" and recompile\n\n", 7);
	return(0);
#endif 
#endif 

	date_time_str(sess.anal_time);
	set_opts_and_params(argc, argv, &sess);

	get_data(&sess);

        if(sess.opts->ts_file){
                get_recs_from_ts_files(&sess);
        }else if(sess.opts->tts_file){
                get_recs_from_tts_files(&sess);
        }else if(is_bpf(sess)){
                get_recs_from_bpf(&sess);
	}

	get_pos_samp_freq(&sess);

	fill_xy_fields(&sess);

	make_cell_map(&sess);

	allocate_memory_for_smoothing(&sess);		// do this before calling allocate_memory because the high level structures are allocated here

	if (sess.directives->make_smoothed_maps) {	// do this before collapsing the position resolution
		make_smoothed_maps(&sess);
	//	if (sess.opts->smoothed_rate_maps)
	//		make_show_smoothed_rate_maps(&sess);
	}

	collapse_pixels(&sess);

	allocate_memory(&sess);

	free((void *) sess.data);

	name_cells(&sess);

	set_thresholds(&sess);

	if (sess.directives->make_time_map) {
		make_time_map(&sess);
		get_app_map(&sess);
		if (sess.directives->make_smoothed_maps) {		// needs the collapsed resolution app map
			clean_smoothed_maps(&sess);
	//		if (sess.opts->smoothed_rate_maps)
	//			make_show_smoothed_rate_maps(&sess);
		}
	}

	if (sess.directives->make_time_map) {
		clean_time_map(&sess);
		if (sess.opts->time_map)
			make_show_time_map(&sess);
	}

	if (sess.directives->make_rate_maps) {
		make_spike_maps(&sess);
		clean_spike_maps(&sess);
		make_rate_maps(&sess);
		if (sess.directives->make_show_rate_maps)
			make_show_rate_maps(&sess);
	}
	if (sess.directives->make_field_maps) {
		make_field_maps(&sess);
		if (sess.opts->field_maps)
			make_show_field_maps(&sess);
	}
	// calculate cell-specific spatial info
	// get_field_data(&sess);
	   get_cell_data(&sess);

	if(sess.opts->write_maps)
		write_maps(&sess);
	if(sess.opts->bpf_acute_file)
		CorrelateSpikeTrainsNoPos(&sess);
	else
		CorrelateSpikeTrains(&sess);

	// Correlation stuff done
	return(0);
}

si1	is_spike(sess, type)
SESSION	sess;
ui1	type;
{
	if(sess.opts->bpf_room_file || sess.opts->bpf_arena_file || sess.opts->bpf_acute_file)
		return (type == SINGLE_BPF_REC_TYPE || type == STEREO_BPF_REC_TYPE || type == TETRODE_BPF_REC_TYPE);
	else
		return (type == SINGLE_ELECTRODE_SPIKE_REC || type == STEREOTRODE_SPIKE_REC || type == TETRODE_SPIKE_REC); 
}

si1	is_bpf(sess)
SESSION	sess;
{
	return (sess.opts->bpf_room_file || sess.opts->bpf_arena_file || sess.opts->bpf_acute_file);
}
