
# macro definitions

PROG = /usr/local/bin/CorrelateSpiketrainEpisodes
LIBS = -lm
OPTIM = -O2

OBJS =\
allocate_memory.o\
calc_corrs.o\
calculate_direction.o\
calculate_mean_waveforms.o\
calculate_speed.o\
calculate_theta.o\
clean_spike_maps.o\
clean_time_map.o\
coher.o\
collapse_pixels.o\
correl.o\
CorrelateSpiketrainEpisodes.o\
CorrelateSpikeTrains.o\
CorrelateSpikeTrainsNoPos.o\
date_time_str.o\
elim_clusts_dialog.o\
extract_eeg.o\
fill_theta_fields.o\
fill_app_holes.o\
fill_xy_fields.o\
filter.o\
filter_recs_for_events.o\
filter_recs_for_non_theta.o\
filter_recs_for_stillness.o\
filter_recs_for_theta.o\
filter_recs_for_walking.o\
find_crits.o\
get_app_map.o\
get_breakpoints.o\
get_cell_data.o\
get_data.o\
get_date_time.o\
get_eeg.o\
get_env_var.o\
get_field_data.o\
get_medians.o\
get_p_burst_spikes.o\
get_phase.o\
get_pos_samp_freq.o\
get_records.o\
get_recs_from_bpf.o\
get_recs_from_km.o\
get_recs_from_ts.o\
get_recs_from_tts.o\
gmem.o\
instruct.o\
Kendall.o\
keep_clusts_dialog.o\
lin_regress.o\
make_cell_map.o\
make_directional_subdivisions.o\
make_field_maps.o\
make_phase_subdivisions.o\
make_rate_maps.o\
make_rotational_dir_subdivisions.o\
make_smoothed_maps.o\
make_show_field_maps.o\
make_show_rate_maps.o\
make_show_smoothed_rate_maps.o\
make_show_speed_map.o\
make_show_time_map.o\
make_speed_map.o\
make_spike_maps.o\
make_temporal_subdivisions.o\
make_time_map.o\
median_smooth.o\
name_cells.o\
norm_filt.o\
pause.o\
peak_time.o\
plot_building_functions.o\
ps_print_cell_report.o\
ps_print_maps.o\
ps_print_plots.o\
rate_thresh_dialog.o\
reverse.o\
ReadBPFHeader.o\
read_write_app_map.o\
read_write_bkpts.o\
rotate_pixels.o\
sscanfLine.o\
set_opts_and_params.o\
set_state.o\
set_thresholds.o\
smoothed_rate_thresh_dialog.o\
spline.o\
splope.o\
uff_to_km.o\
valley_time.o\
write_maps.o\
write_ts.o\
write_waveforms.o\
z2p.o


# program dependencies

$(PROG):$(OBJS)
	cc $(OPTIM) -o $(PROG) $(OBJS) $(LIBS)


# inference rule

.c.o:
	cc -c $(OPTIM) $*.c
	chmod a+rw $*.o


# function dependencies

allocate_memory.o:			sess_anal.h size_types.h
calc_corrs.o:				size_types.h
calculate_direction.o:			sess_anal.h size_types.h
calculate_mean_waveforms.o:		sess_anal.h size_types.h uff.h
calculate_speed.o:			sess_anal.h size_types.h speed.h uff.h
calculate_theta.o:			filter.h sess_anal.h size_types.h theta.h
clean_spike_maps.o:			sess_anal.h size_types.h
clean_time_map.o:			sess_anal.h size_types.h
coher.o:				sess_anal.h size_types.h
rollapse_pixels.o:			sess_anal.h size_types.h
correl.o:				size_types.h
date_time_str.o:			size_types.h
elim_clusts_dialog.o:			sess_anal.h size_types.h uff.h
extract_eeg.o:				sess_anal.h size_types.h theta.h uff.h
fill_theta_fields.o:			sess_anal.h size_types.h theta.h
fill_app_holes.o:			sess_anal.h size_types.h
fill_xy_fields.o:			sess_anal.h size_types.h uff.h
filter.o:				filter.h size_types.h
filter_recs_for_events.o:		sess_anal.h size_types.h uff.h
filter_recs_for_non_theta.o:		sess_anal.h size_types.h theta.h
filter_recs_for_stillness.o:		sess_anal.h size_types.h
filter_recs_for_theta.o:		sess_anal.h size_types.h theta.h
filter_recs_for_walking.o:		sess_anal.h size_types.h
find_crits.o:				size_types.h theta.h
get_app_map.o:				sess_anal.h size_types.h
get_breakpoints.o:			sess_anal.h size_types.h
get_cell_data.o:			sess_anal.h size_types.h
get_data.o:				sess_anal.h size_types.h
get_date_time.o:			sess_anal.h size_types.h uff.h
get_field_data.o:			sess_anal.h size_types.h
get_eeg.o:				sess_anal.h size_types.h theta.h uff.h
get_env_var.o:				size_types.h
get_medians.o:				sess_anal.h size_types.h
get_phase.o:				size_types.h theta.h
get_p_burst_spikes.o:			sess_anal.h size_types.h
get_pos_samp_freq.o:			sess_anal.h size_types.h uff.h
get_records.o:				sess_anal.h size_types.h uff.h
get_recs_from_bpf.o:			sess_anal.h size_types.h bpf.h
get_recs_from_km.o:			sess_anal.h size_types.h uff.h bpf.h
get_recs_from_ts.o:			sess_anal.h size_types.h uff.h bpf.h
get_recs_from_tts.o:			sess_anal.h size_types.h uff.h bpf.h
instruct.o:				postscript.h sess_anal.h size_types.h speed.h theta.h
keep_clusts_dialog.o:			sess_anal.h size_types.h uff.h
kendall.o:				size_types.h
lin_regress.o:				size_types.h
make_cell_map.o:			sess_anal.h size_types.h bpf.h uff.h
make_directional_subdivisions.o:	sess_anal.h size_types.h
make_field_maps.o:			sess_anal.h size_types.h
make_phase_subdivisions.o:		sess_anal.h size_types.h theta.h
make_rate_maps.o:			sess_anal.h size_types.h
make_rotational_dir_subdivisions.o:	sess_anal.h size_types.h
make_smoothed_maps.o:			sess_anal.h size_types.h
make_show_field_maps.o:			sess_anal.h size_types.h
make_show_rate_maps.o:			sess_anal.h size_types.h
make_show_smoothed_rate_maps.o:		sess_anal.h size_types.h
make_show_speed_map.o:			sess_anal.h size_types.h
make_show_time_map.o:			sess_anal.h size_types.h
make_speed_map.o:			sess_anal.h size_types.h speed.h
make_spike_maps.o:			sess_anal.h size_types.h uff.h
make_temporal_subdivisions.o:		sess_anal.h size_types.h uff.h
make_time_map.o:			sess_anal.h size_types.h
median_smooth.o:			size_types.h
name_cells.o:				sess_anal.h size_types.h
norm_filt.o:				size_types.h theta.h
pause.o:				size_types.h
peak_time.o:				size_types.h
plot_building_functions.o:		postscript.h sess_anal.h size_types.h theta.h uff.h
ps_print_cell_report.o:			postscript.h sess_anal.h size_types.h uff.h
ps_print_maps.o:			postscript.h sess_anal.h size_types.h uff.h
ps_print_plots.o:			postscript.h sess_anal.h size_types.h uff.h
rate_thresh_dialog.o:			sess_anal.h size_types.h
ReadBPFHeader.o:			sess_anal.h size_types.h bpf.h
read_write_app_map.o:			sess_anal.h size_types.h
read_write_bkpts.o:			sess_anal.h size_types.h
reverse.o:				size_types.h
rotate_pixels.o:			bpf.h sess_anal.h size_types.h 
sscanLine.o:				size_types.h 
CorrelateSpiketrainEpisodes.o:		bpf.h sess_anal.h size_types.h 
CorrelateSpikeTrains.o:			bpf.h sess_anal.h size_types.h 
CorrelateSpikeTrainsNoPos.o:			bpf.h sess_anal.h size_types.h 
set_opts_and_params.o:			postscript.h sess_anal.h size_types.h speed.h theta.h
set_state.o:				sess_anal.h size_types.h theta.h
set_thresholds.o:			sess_anal.h size_types.h
smoothed_rate_thresh_dialog.o:		sess_anal.h size_types.h
spline.o:				size_types.h
splope.o:				size_types.h
uff_to_km.o:				sess_anal.h size_types.h uff.h
valley_time.o:				size_types.h
write_maps.o:				sess_anal.h size_types.h
write_ts.o:				sess_anal.h size_types.h uff.h bpf.h ts.h
write_waveforms.o:			sess_anal.h size_types.h uff.h
z2p.0:					size_types.h

