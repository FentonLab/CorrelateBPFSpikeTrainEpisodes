
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"
#include "postscript.h"


void	ps_print_cell_report(sess)
SESSION	*sess;
{
	si4		i, j, page, n_pages, cells_on_page, cells_per_page;
	sf4		x, y, y2;
	si1		command[256], out_file[256], name_str[256], *printer, *ps_dir;
	FIELD_DATA	*fdp;
	OPTIONS		*opts;
	CELL		*cp;
	TIME		*time;
	SPEED		*speed;
	PARAMETERS	*params;
	APPARATUS	*app;
	FILE		*fp;
	extern si1	*get_env_var();
	void		new_report_page();
	si1	*bool2ascii();

	if(sess->opts->no_pos_samp == TRUE){
		cells_per_page = NO_POS_CELLS_PER_PAGE;
	}else{
		cells_per_page = CELLS_PER_PAGE;
	}
	n_pages = (sess->num_cells + 1) / cells_per_page;

	if ((sess->num_cells + 1) % cells_per_page)
		++n_pages;

	/* open postcript file */

	if (sess->opts->output_to_ps_files) {
		ps_dir = get_env_var("PS_REPORT_DIR");
		if(sess->opts->bpf_room_file){
			(void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
		}else if (sess->opts->bpf_arena_file){
			(void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
		}else{
			(void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
		}
	} else
		(void) strcpy(out_file, "/usr/tmp/junk2");
	fp = fopen(out_file, "w");

	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCould not open the file \"%s\". Terminating program.\n\n", 7, out_file);
		exit(1);
	}
	/* start postscript file with this stuff */
	(void) fprintf(fp, "%%!PS\n");
	(void) fprintf(fp, "/center {dup stringwidth pop 2 div neg 0 rmoveto} bind def\n");

	app = sess->apparatus;
	opts = sess->opts;
	params = sess->params;
	time = sess->time;
	speed = sess->speed;

	sprintf(name_str,"%s%s", sess->FrameStr, sess->name_str);
	y = PAGE_Y_START;
	page = 1;
	new_report_page(name_str, sess->anal_time, page, n_pages, fp);
	x = COL_1;

	(void) fprintf(fp, "/Times-Roman 10.0 selectfont\n");
	(void) fprintf(fp, "%f %f moveto\n(General Information:) show\n", x, y);
	(void) fprintf(fp, "/Times-Roman 7.0 selectfont\n");
	x = COL_1; y -= LINE_SMALL_Y_DEC; y -= LINE_SMALL_Y_DEC;
	(void) fprintf(fp, "%f %f moveto\n(Data Format:  %s) show\n", x, y, sess->DataFormat);
	x = COL_1; y -= LINE_SMALL_Y_DEC; y -= LINE_SMALL_Y_DEC;
	(void) fprintf(fp, "%f %f moveto\n(Session Date:  %s) show\n", x, y, sess->date_str);
	x = COL_1; y -= LINE_SMALL_Y_DEC;
	(void) fprintf(fp, "%f %f moveto\n(Session Time:  %s) show\n", x, y, sess->time_str);
	x = COL_1; y -= LINE_SMALL_Y_DEC;
	(void) fprintf(fp, "%f %f moveto\n(Session Description:  %s) show\n", x, y, sess->desc_str);
	x = COL_1; y -= LINE_SMALL_Y_DEC; y -= LINE_SMALL_Y_DEC;
	(void) fprintf(fp, "%f %f moveto\n(Cells Analyzed:  %d) show\n", x, y, sess->num_cells);

	if((opts->ts_file == FALSE) && (opts->km_file == FALSE)){
		x = COL_1; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Probes Analyzed:  %d) show\n", x, y, sess->num_probes);
	}

	x = COL_1; y -= LINE_SMALL_Y_DEC; y -= LINE_SMALL_Y_DEC;
	(void) fprintf(fp, "%f %f moveto\n(Session Duration \\(minutes\\):  %0.1lf) show\n", x, y, time->total_secs / 60.0);

	if(opts->no_pos_samp == FALSE){
		x = COL_1; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Detected Time \\(minutes\\):  %0.1lf) show\n", x, y, time->total_in_apparatus_secs / 60.0);
		x = COL_1; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Good Detects \\(%%\\):  %0.1lf) show\n", x, y, time->total_in_apparatus_secs * 100.0 / time->total_secs);
		x = COL_1; y -= LINE_SMALL_Y_DEC; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Apparatus Size \\(pixels\\):  %d) show\n", x, y, app->total_pixels);
		x = COL_1; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Apparatus Center \\(x, y\\):  \\(%d, %d\\)) show\n", x, y, app->center_x, app->center_y);
		x = COL_1; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Apparatus Borders:) show\n", x, y);
		x = COL_2;
		(void) fprintf(fp, "%f %f moveto\n(Left:  %d\t\t\t\t\t\tRight:  %d) show\n", x, y, app->left, app->right);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Top:  %d\t\t\t\t\t\tBottom:  %d) show\n", x, y, app->top, app->bottom);
	}
	x = COL_1; y -= LINE_SMALL_Y_DEC; y -= LINE_SMALL_Y_DEC;
	(void) fprintf(fp, "%f %f moveto\n(Options:) show\n", x, y);
	x = COL_2;

	if((opts->ts_file == FALSE) && (opts->km_file == FALSE)){
		(void) fprintf(fp, "%f %f moveto\n(Filter Data for Time Between Event Flags Only:  %s) show\n", x, y, bool2ascii(opts->between_event_flags_only));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Filter Data for Non-theta Only:  %s) show\n", x, y, bool2ascii(opts->non_theta_only));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Filter Data for Theta Only:  %s) show\n", x, y, bool2ascii(opts->theta_only));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
	}

	if(opts->no_pos_samp == FALSE){
		(void) fprintf(fp, "%f %f moveto\n(Filter Data for Non-movement Only:  %s) show\n", x, y, bool2ascii(opts->still_only));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Filter Data for Walking Only:  %s) show\n", x, y, bool2ascii(opts->walking_only));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Read Breakpoints from a File:  %s) show\n", x, y, bool2ascii(opts->read_breakpoints));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Write Breakpoints to a File:  %s) show\n", x, y, bool2ascii(opts->write_breakpoints));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Read Apparatus Map from a File:  %s) show\n", x, y, bool2ascii(opts->read_app_map));
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Write Apparatus Map to a File:  %s) show\n", x, y, bool2ascii(opts->write_app_map));
		x = COL_1; y -= LINE_SMALL_Y_DEC; y -= LINE_SMALL_Y_DEC;
	}

	(void) fprintf(fp, "%f %f moveto\n(Parameters:) show\n", x, y);
	x = COL_2;

	if(opts->no_pos_samp == FALSE){
		(void) fprintf(fp, "%f %f moveto\n(Time Threshold \\(seconds\\):  %f) show\n", x, y, time->threshold);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Speed Threshold \\(cm/second\\):  %d) show\n", x, y, speed->threshold);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Position Sampling Frequency \\(Hz\\):  %0.1f) show\n", x, y, sess->params->position_samp_freq);
	}

	if((opts->ts_file == FALSE) && (opts->km_file == FALSE)){
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Maximum Non-theta Score:  %d) show\n", x, y, params->max_non_theta_score);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Minimum Theta Score:  %d) show\n", x, y, params->min_theta_score);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
	}

	if(opts->no_pos_samp == FALSE){
		(void) fprintf(fp, "%f %f moveto\n(Maximum Still Speed \\(cm/second\\):  %d) show\n", x, y, params->max_still_speed);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Minimum Walking Speed \\(cm/second\\):  %d) show\n", x, y, params->min_walking_speed);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Pixel Collapse Factor:  %0.1lf) show\n", x, y, params->pixel_collapse_factor);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Breakpoints Read from File:  %s) show\n", x, y, params->read_bkpts_file);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Breakpoints Written to File:  %s) show\n", x, y, params->write_bkpts_file);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Apparatus Map Read from File:  %s) show\n", x, y, params->read_app_map_file);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Apparatus Map Written to File:  %s) show\n", x, y, params->write_app_map_file);
	}

	if((opts->ts_file == FALSE) && (opts->km_file == FALSE)){
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Event Begin Flag:  %c) show\n", x, y, params->event_begin_flag);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Event End Flag:  %c) show\n", x, y, params->event_end_flag);
	}
	
	if(opts->no_pos_samp == FALSE){
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Minimum Inter-LED distance \\(cm\\):  %0.1lf) show\n", x, y, params->min_inter_led_dist);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Maximum Inter-LED distance \\(cm\\):  %0.1lf) show\n", x, y, params->max_inter_led_dist);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Tracker Pixel Collapse Factor:  %0.1lf) show\n", x, y, params->pixel_collapse_factor);
		x = COL_2; y -= LINE_SMALL_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Smoothed Map Smoothing Factor:  %0.3lf) show\n", x, y, params->smoothing_factor);
	}

	cells_on_page = 1; // to account for the space for the General Info
	y-= LINE_Y_DEC; y -= LINE_Y_DEC; y -= LINE_Y_DEC; y -= LINE_Y_DEC;
	for (i = 0; i < sess->num_cells; ++i) {
		x = COL_1;
		cp = sess->cells[i];
		(void) fprintf(fp, "/Times-Roman 12.0 selectfont\n");
		(void) fprintf(fp, "%f %f moveto\n(%s) show\n", x, y, cp->name_str);
		(void) fprintf(fp, "/Times-Roman 9.0 selectfont\n");
		x = COL_1; y -= LINE_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Largest Spike's Width in samples (0..0) %0.2lf) show\n", x, y, cp->spike_width_in_samps);
		x = COL_1; y -= LINE_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Correlation between two largest waveforms %0.4f) show\n", x, y, cp->waveform_correlation);
		x = COL_1; y -= LINE_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Cluster Grand Rate \\(spikes/second\\):  %0.3lf) show\n", x, y, cp->grand_rate);
		x = COL_1; y -= LINE_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Proportion Active Pixels:  %0.3lf) show\n", x, y, cp->p_active_pixels);
		
		if(opts->no_pos_samp == FALSE){
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Rate Threshold \\(spikes/second\\):  %f) show\n", x, y, cp->rate_thresh);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Smoothed Rate Threshold \\(spikes/second\\):  %f) show\n", x, y, cp->smoothed_rate_thresh);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Rate/Time Map Correlation:  %0.3lf) show\n", x, y, cp->rate_time_r);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Rate/Time Map Correlation Statistics:  z = %0.3lf, p = %0.3lf) show\n", x, y, cp->rate_time_z, cp->rate_time_p);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Rate Map Coherence:  %0.3lf) show\n", x, y, cp->rate_coher_r);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Rate Map Coherence Statistics:  z = %0.3lf, p = %0.3lf) show\n", x, y, cp->rate_coher_z, cp->rate_coher_p);
			y -= LINE_Y_DEC;
		}

		x = COL_1; y -= LINE_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Total Spikes:  %d) show\n", x, y, cp->total_spikes);
		x = COL_1; y -= LINE_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Proportion of spikes in bursts (<=%d ms): %0.4f) show\n", x, y, MAX_BURST_ISI * 1000 /TIME_STAMPS_PER_SECOND, cp->p_burst_spikes);
		x = COL_1; y -= LINE_Y_DEC;
		(void) fprintf(fp, "%f %f moveto\n(Total Time (sec):  %0.3lf) show\n", x, y, time->total_secs);

		if(opts->no_pos_samp == FALSE){
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Detected Spikes:  %d) show\n", x, y, cp->total_in_apparatus_spikes);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Good Spikes \\(%%\\):  %0.1lf) show\n", x, y, cp->total_in_apparatus_spikes * 100.0 / cp->total_spikes);
			x = COL_1; y -= LINE_Y_DEC; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Number of Fields:  %d) show\n", x, y, cp->num_fields);
			x = COL_1; y -= LINE_Y_DEC;
			y2 = y;	
			(void) fprintf(fp, "%f %f moveto\n(Field Number:) show\n", x, y);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Field Grand Rate \\(spikes/second\\):) show\n", x, y);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Center Rate \\(spikes/second\\):) show\n", x, y);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Center \\(x, y\\):) show\n", x, y);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Total Pixels:) show\n", x, y);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Total Spikes:) show\n", x, y);
			x = COL_1; y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Total Time \\(minutes\\):) show\n", x, y);
			for (j = 0; j < cp->num_fields && j < 3; ++j) {
				fdp = cp->field_data[j];
				y = y2;
				switch (j) {
					case 0:	x = COL_2; break;
					case 1:	x = COL_3; break;
					case 2:	x = COL_4; break;
				}
				(void) fprintf(fp, "%f %f moveto\n(%d) show\n", x, y, fdp->number);
				y -= LINE_Y_DEC;
				(void) fprintf(fp, "%f %f moveto\n(%0.1lf) show\n", x, y, fdp->grand_rate);
				y -= LINE_Y_DEC;
				(void) fprintf(fp, "%f %f moveto\n(%0.1lf) show\n", x, y, fdp->center_rate);
				y -= LINE_Y_DEC;
				(void) fprintf(fp, "%f %f moveto\n(\\(%d, %d\\)) show\n", x, y, fdp->center_x, fdp->center_y);
				y -= LINE_Y_DEC;
				(void) fprintf(fp, "%f %f moveto\n(%d) show\n", x, y, fdp->total_pixels);
				y -= LINE_Y_DEC;
				(void) fprintf(fp, "%f %f moveto\n(%d) show\n", x, y, fdp->total_spikes);
				y -= LINE_Y_DEC;
				(void) fprintf(fp, "%f %f moveto\n(%0.1lf) show\n", x, y, fdp->total_time / 60.0);
			}
			x = COL_1; y -= LINE_Y_DEC; y-= LINE_Y_DEC;
			fdp = cp->out_of_field_data;
			(void) fprintf(fp, "%f %f moveto\n(Out of Field Rate \\(spikes/second\\):  %0.3lf) show\n", x, y, fdp->grand_rate);
			y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Out of Field Pixels:  %d) show\n", x, y, fdp->total_pixels);
			y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Out of Field Spikes:  %d) show\n", x, y, fdp->total_spikes);
			y -= LINE_Y_DEC;
			(void) fprintf(fp, "%f %f moveto\n(Out of Field Time \\(minutes\\):  %0.1lf) show\n", x, y, fdp->total_time / 60.0);
		}

		if (++cells_on_page == cells_per_page) {
			if (i != sess->num_cells - 1) {
				++page;
				new_report_page(sess->name_str, sess->anal_time, page, n_pages, fp);
				cells_on_page = 0;
				y = PAGE_Y_START;
			}
		} else {
			y -= LINE_Y_DEC; y -= LINE_Y_DEC;
		}
	}
				
	
	/* end file with showpage and EOF character */

	(void) fprintf(fp, "showpage\n%c", 4);
	(void) fflush(fp);
	(void) fclose(fp);


	/* send file to printer */
	
	if (sess->opts->output_to_printer) {
		printer = get_env_var("GRAY_PRINTER");
		(void) sprintf(command, "lpr -P %s %s", printer, out_file);
		(void) system(command);
	}

	return;
}


void	new_report_page(sess_name, anal_time, page, n_pages, fp)
si1		*sess_name, *anal_time;
si4		page, n_pages;
FILE		*fp;
{
	if (page != 1)
		(void) fprintf(fp, "showpage\n");

	(void) fprintf(fp, "gsave\n");
	(void) fprintf(fp, "%f 0.0 translate\n90.0 rotate\n", PS_X_DIM);
	(void) fprintf(fp, "/Times-Roman 5.0 selectfont\n");
	(void) fprintf(fp, "%f %f moveto\n", PS_Y_MIN, PS_X_MAX - 3.0);
	(void) fprintf(fp, "(%s) show\n", anal_time);
	(void) fprintf(fp, "grestore\n");

	(void) fprintf(fp, "/Times-Roman 12.0 selectfont\n");
	(void) fprintf(fp, "%f %f moveto\n", PS_X_CENT, PAGE_Y_START);
/*
	if (n_pages == 1)
		(void) fprintf(fp, "(Session %s)", sess_name);
	else
		(void) fprintf(fp, "(Session %s   \\(page %d of %d\\))",  sess_name, page, n_pages); 
*/
	(void) fprintf(fp, "(Session %s   \\(page %d of %d\\))",  sess_name, page, n_pages); 
	(void) fprintf(fp, " center show\n");

	return;
}


si1	*bool2ascii(bool)
ui1		bool;
{
	if (bool)
		return("true");
	return("false");
}
