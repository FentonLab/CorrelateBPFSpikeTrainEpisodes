
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"
#include "postscript.h"


void	ps_print_plots(sess)
SESSION	*sess;
{
	si4		i, j, k, n_plots, n_cells;
	si4		n_pages, page, probe_num, cell_num, cell2_num;
	sf4		plot_x, plot_y;
	si1		command[256], out_file[256], name_str[256];
	si1		*ps_dir, *printer;
	PLOT		*plots;
	FILE		*fp;
	void	draw_plot(), new_plot_page();
	extern void	*gmem();
	extern void	mean_waveform_plot(), head_direction_plot();
	extern void	theta_phase_plot(), interspike_int_plot();
	extern void	cross_correlation_plot(), autocorrelation_plot();
	extern si1	*get_env_var();


	n_plots = 0;
	if (sess->opts->phase_plots)
		n_plots += sess->num_cells;
	if (sess->opts->interspike_int_plots)
		n_plots += sess->num_cells;
	if (sess->opts->mean_waveform_plots)
		n_plots += sess->num_cells;
	if (sess->opts->dir_plots)
		n_plots += sess->num_cells;
	if (sess->opts->autocorr_plots)
		n_plots += sess->num_cells;
	if (sess->opts->cross_corr_plots){
		for (i = 0; i < MAX_PROBES; ++i) {
			for (j = 0, n_cells = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
				cell_num = sess->cell_map[i][j];
				if (cell_num != NOT_CUT)
					n_cells++;	
			}
			while(n_cells)
				n_plots += (--n_cells);
		}
	}

	/* defaults set here are assumed by the plot building and drawing functions */

	plots = (PLOT *) gmem((size_t) n_plots, sizeof(PLOT));
	for (i = 0; i < n_plots; ++i) {
		for (j = 0; j < MAX_VALS; ++j)
			plots[i].val_names[j] = (si1 *) NULL;
		plots[i].x_units = plots[i].y_units = plots[i].type = 0;
		*plots[i].name = (si1)NULL;
		for (j = 0; j < MAX_POINT_SETS; ++j) {
			plots[i].point_set_labels[j] = (si1 *) NULL;
			plots[i].num_points[j] = 0;	
			plots[i].draw_errs[j] = plots[i].draw_lines[j] = plots[i].draw_points[j] = plots[i].draw_bars[j] = FALSE;	
		}
	}

	/* make array of plots to be printed */

	n_plots = 0;
	for (i = 0; i < MAX_PROBES; ++i) {
		probe_num = sess->probe_map[i];
		if (probe_num != NOT_CUT) {
			/* if you want any plots for probes, list them here - make sure you add them */
			/* to the plot count above */
		}
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
			cell_num = sess->cell_map[i][j];
			if (cell_num != NOT_CUT) {
				if (sess->opts->phase_plots) {
					theta_phase_plot(plots + n_plots, sess->cells[cell_num], sess);
					++n_plots;
				}
				if (sess->opts->interspike_int_plots) {
					interspike_int_plot(plots + n_plots, sess->cells[cell_num], sess);
					++n_plots;
				}
				if (sess->opts->mean_waveform_plots) {
					if(is_bpf(*sess))
						mean_waveform_plot(plots + n_plots, sess->cells[cell_num], sess, sess->BPFRecordVoltageSamples['T'], MAX_MULTITRODE_CHANNELS);
						// mean_waveform_plot(plots + n_plots, sess->cells[cell_num], sess, BPF_SPIKE_SIZE, MAX_MULTITRODE_CHANNELS);
					else
						mean_waveform_plot(plots + n_plots, sess->cells[cell_num], sess, SU_SPIKE_SIZE, MAX_MULTITRODE_CHANNELS);
					++n_plots;
				}
				if (sess->opts->dir_plots) {
					head_direction_plot(plots + n_plots, sess->cells[cell_num], sess);
					++n_plots;
				}
				if (sess->opts->autocorr_plots) {
					autocorrelation_plot(plots + n_plots, sess->cells[cell_num], sess);
					++n_plots;
				}
				if (sess->opts->cross_corr_plots) {
					for (k = j+1; k < MAX_CLUSTS_PER_PROBE; ++k) {
						cell2_num = sess->cell_map[i][k];
						if (cell2_num != NOT_CUT) {
							cross_correlation_plot(plots + n_plots, sess->cells[cell_num], sess->cells[cell2_num], sess);
							++n_plots;
						}
					}
				}
			}
		}
	}

	/* open postcript file */

	if (sess->opts->output_to_ps_files) {
		ps_dir = get_env_var("PS_PLOT_DIR");
		if(sess->opts->bpf_room_file)
                        (void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
                else if (sess->opts->bpf_arena_file)
                        (void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
                else
			(void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
	} else
		(void) strcpy(out_file, "/usr/tmp/junk4");
	fp = fopen(out_file, "w");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCould not open the file \"%s\". Terminating program.\n\n", 7, out_file);
		exit(1);
	}

	/* start postscript file with this stuff */
	(void) fprintf(fp, "%%!PS\n");
	(void) fprintf(fp, "/center {dup stringwidth pop 2 div neg 0 rmoveto} bind def\n");
	(void) fprintf(fp, "/right {dup stringwidth pop neg 0 rmoveto} bind def\n");
	(void) fprintf(fp, "/ml {moveto 2 pop lineto} bind def\n");
	(void) fprintf(fp, "/pt {%0.2f %0.2f rectfill} bind def\n", PLOT_POINT_SIZE, PLOT_POINT_SIZE);
	
	n_pages = n_plots / MAPS_PER_PAGE;
	if (n_plots % MAPS_PER_PAGE)
		++n_pages;

	/* loop on plots */

	sprintf(name_str,"%s%s", sess->FrameStr, sess->name_str);

	page = 1;
	for (i = 0; i < n_plots; ++i) {
		if (!(i % MAPS_PER_PAGE)) {
			plot_y = PLOT_Y_START + PLOT_Y_DEC; /* Y_INCREMENT WILL BE ADDED BELOW */
			new_plot_page(name_str, sess->anal_time, page, n_pages, fp);
			++page;
		}
		if (!(i % MAPS_PER_ROW)) {
			plot_x = PLOT_X_START;
			plot_y -= PLOT_Y_DEC;
		}
		draw_plot(plots + i, plot_x, plot_y, fp);
		plot_x += PLOT_X_INC;
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

	for (i = 0; i < n_plots; ++i)
		for (j = 0; j < MAX_POINT_SETS; ++j)
			if (plots[i].num_points[j]) {
				free((void *) plots[i].xs[j]);
				free((void *) plots[i].ys[j]);
			}
	free((void *) plots);


	return;
}


void	new_plot_page(sess_name, anal_time, page, n_pages, fp)
si1		*sess_name, *anal_time;
si4		page, n_pages;
FILE		*fp;
{
	if (page != 1)
		(void) fprintf(fp, "showpage\n");

	(void) fprintf(fp, "%f 0.0 translate\n90.0 rotate\n", PS_X_DIM);
	/* (0,0) is bottom left corner, note that this switches the x and y dimensions */

	(void) fprintf(fp, "/Times-Roman 5.0 selectfont\n");
	(void) fprintf(fp, "%f %f moveto\n", PS_Y_MIN, PS_X_MAX - 3.0);
	(void) fprintf(fp, "(%s) show\n", anal_time);

	(void) fprintf(fp, "/Times-Roman 18.0 selectfont\n");
	(void) fprintf(fp, "%f %f moveto\n", PS_Y_CENT, PS_X_MAX - 12.0);
	if (n_pages == 1)
		(void) fprintf(fp, "(Session %s)", sess_name);
	else
		(void) fprintf(fp, "(Session %s   \\(page %d of %d\\))",  sess_name, page, n_pages); 
	(void) fprintf(fp, " center show\n");

	return;
}


 void	draw_plot(plot, plot_x, plot_y, fp)
PLOT		*plot;
sf4		plot_x, plot_y;
FILE		*fp;
{
	si4		i, j, point_sets;
	sf4		x, y, x_tick_offset, y_tick_offset;
	sf4		label, label_inc, grey_lev, grey_dec;	
	si1		val_str[256], val_str2[256], *vp;
	 void	draw_lines(), draw_errs(), draw_points(), draw_bars();
	

	/* draw name and type */

	(void) fprintf(fp, "/Times-Roman 10.0 selectfont\n0.0 0.0 0.0 setrgbcolor\n");
	if (plot->type != (si1 *) NULL) {
		y = plot_y - PLOT_TYPE_OFFSET;
		x = plot_x + (PLOT_X_SIZE / 2.0);
		(void) fprintf(fp, "%0.2f %0.2f moveto\n(%s) center show\n", x, y, plot->type);
	}
	if (plot->name != (si1 *) NULL) {
		y = plot_y - PLOT_NAME_OFFSET;
		x = plot_x + (PLOT_X_SIZE / 2.0);
		(void) fprintf(fp, "%0.2f %0.2f moveto\n(%s) center show\n", x, y, plot->name);
	}

	/* write error message if limits set incorrectly */

	point_sets = 0;
	for (i = 0; i < MAX_POINT_SETS; ++i)
		if (plot->num_points[i])
			++point_sets;
	for (i = 0; i < point_sets; ++i){
		 if (plot->x_min[i] >= plot->x_max[i] || plot->y_min[i] >= plot->y_max[i]) {
			(void) fprintf(stderr, "%cBad axis values. Skipping Plot.\n", 7);
			(void) fprintf(fp, "/Times-Roman 12.0 selectfont\n");
			x = plot_x + PLOT_X_SIZE / 2.0; y = plot_y - PLOT_Y_SIZE / 2.0;
			(void) fprintf(fp, "%f %f moveto\n(PLOT AXIS ERROR) center show\n", x, y);
			return;
		}
	}

	/* make axes */
	
	(void) fprintf(fp, "0.25 setlinewidth\n/Times-Roman 6.0 selectfont\n");

	x = plot_x; y = plot_y;
	(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\nstroke\n", x, y - PLOT_Y_SIZE, x, y);
	(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\nstroke\n", x, y - PLOT_Y_SIZE, x + PLOT_X_SIZE, y - PLOT_Y_SIZE);
	
	x = plot_x + PLOT_ZERO_TICK_OFFSET;
	y = plot_y - PLOT_Y_SIZE;
	x_tick_offset = (PLOT_X_SIZE - PLOT_ZERO_TICK_OFFSET) / (sf4) (PLOT_X_TICKS - 1);
	label = plot->x_min[0];
	label_inc = (plot->x_max[0] - plot->x_min[0]) / (sf4) (PLOT_X_TICKS - 1);
	for (i = 0; i < PLOT_X_TICKS; ++i) {
		(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\nstroke\n", x, y - PLOT_TICK_SIZE, x, y);
		(void) fprintf(fp, "%0.2f %0.2f moveto\n(%0.1f) center show\n", x, y - PLOT_TICK_SIZE - X_LABEL_Y_OFFSET, label);
		x += x_tick_offset;
		label += label_inc;
	}
		
	x = plot_x;
	y = plot_y;
	label = plot->y_max[0];
	label_inc = (plot->y_max[0] - plot->y_min[0]) / (sf4) (PLOT_Y_TICKS - 1);
	y_tick_offset = (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET) / (sf4) (PLOT_Y_TICKS - 1);
	for (i = 0; i < PLOT_Y_TICKS; ++i) {
		(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\nstroke\n", x - PLOT_TICK_SIZE, y, x, y);
		(void) fprintf(fp, "%0.2f %0.2f moveto\n(%0.1f) right show\n", x - PLOT_TICK_SIZE - Y_LABEL_X_OFFSET, y - Y_LABEL_Y_OFFSET, label);
		y -= y_tick_offset;
		label -= label_inc;
	}


	/* do axis labels */

	(void) fprintf(fp, "/Times-Roman 8.0 selectfont\n");

	if (plot->x_units != (si1 *) NULL) {
		y = plot_y - PLOT_Y_SIZE - X_UNITS_Y_OFFSET;
		x = plot_x + (PLOT_X_SIZE / 2.0);
		(void) fprintf(fp, "%0.2f %0.2f moveto\n(%s) center show\n", x, y, plot->x_units);
	}

	if (plot->y_units != (si1 *) NULL) {
		(void) fprintf(fp, "gsave\n");
		y = plot_y - (PLOT_Y_SIZE / 2.0);
		x = plot_x;
		(void) fprintf(fp, "%0.2f %0.2f translate\n90.0 rotate\n", x, y);
		(void) fprintf(fp, "0.0 %0.2f moveto\n(%s) center show\n", Y_UNITS_Y_OFFSET, plot->y_units);
		(void) fprintf(fp, "grestore\n");
	}

	
	/* do vals, if they exist */

	for (i = 0; i < MAX_VALS; ++i)
		if (plot->val_names[i] != (si1 *) NULL)
			break;

	if (i != MAX_VALS) {
		val_str[0] = '\0';
		val_str2[0] = '\0';
		for (i = 0; i < MAX_VALS; ++i){
			if (plot->val_names[i] != (si1 *) NULL) {
				if(i >= 4)
					vp = val_str + strlen(val_str);
				else
					vp = val_str2 + strlen(val_str2);
				(void) sprintf(vp, "%s = %0.3f, ", plot->val_names[i], plot->vals[i]);
			} 
		}
				
				
		vp = val_str + strlen(val_str) - 2; /* remove tailing comma and space */
		*vp = '\0';
		x = plot_x + PLOT_X_SIZE / 2.0;
		y = plot_y - PLOT_VAL_OFFSET;
		(void) fprintf(fp, "/Times-Roman 5.0 selectfont\n");
		(void) fprintf(fp, "%0.2f %0.2f moveto\n", x, y);
		(void) fprintf(fp, "(\\(%s\\)) center show\n", val_str);

		if(*val_str2 != NULL){
			vp = val_str2 + strlen(val_str2) - 2; /* remove tailing comma and space */
			*vp = '\0';
			x = plot_x + PLOT_X_SIZE / 2.0;
			y = plot_y - PLOT_VAL_OFFSET - 5;
			(void) fprintf(fp, "%0.2f %0.2f moveto\n", x, y);
			(void) fprintf(fp, "(\\(%s\\)) center show\n", val_str2);
		}
	}


	/* do the plotting */

	grey_dec = 0.9 / (sf4) point_sets;

	for (i = j = 0; i < MAX_POINT_SETS; ++i) {
		if (!plot->num_points[i])
			continue;
		grey_lev = (sf4) j++ * grey_dec;
		switch(j){
		case 0:
		case 1:
			(void) fprintf(fp, "%0.2f %0.2f %0.2f setrgbcolor\n", grey_lev, grey_lev, grey_lev);
			break;
		case 2:
		(void) fprintf(fp, "%0.2f %0.2f %0.2f setrgbcolor\n", 1.0, 0.0, 0.0 );
			break;
		case 3:
		(void) fprintf(fp, "%0.2f %0.2f %0.2f setrgbcolor\n", 0.0, 1.0, 0.0 );
			break;
		case 4:
		(void) fprintf(fp, "%0.2f %0.2f %0.2f setrgbcolor\n", 0.0, 0.0, 1.0 );
			break;
		default:
			(void) fprintf(fp, "%0.2f %0.2f %0.2f setrgbcolor\n", grey_lev, grey_lev, grey_lev);
			break;
		}

		if (plot->draw_lines[i])	
			draw_lines(plot, i, plot_x, plot_y, fp);
		if (plot->draw_errs[i])	
			draw_errs(plot, i, plot_x, plot_y, fp);
		if (plot->draw_points[i])	
			draw_points(plot, i, plot_x, plot_y, fp);
		if (plot->draw_bars[i])	
			draw_bars(plot, i, plot_x, plot_y, fp);

		/* do point set labels, if any */

		if (plot->point_set_labels[i] != (si1 *) NULL) {
			(void) fprintf(fp, "/Times-Roman 5.0 selectfont\n");
			x = plot_x + PLOT_X_SIZE;
			y = plot_y - (sf4) j * 5.0;
			(void) fprintf(fp, "%0.2f %0.2f moveto\n", x, y);
			(void) fprintf(fp, "(%s) right show\n", plot->point_set_labels[i]);
		}
	}


	return;
}


 void	draw_lines(plot, idx, plot_x, plot_y, fp)
PLOT		*plot;
si4		idx;
sf4		plot_x, plot_y;
FILE		*fp;
{
	si4	i;
	sf4	x_scale, y_scale, x, y, x_2, y_2;

	x_scale = (PLOT_X_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->x_max[idx] - plot->x_min[idx]);
	y_scale = (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->y_max[idx] - plot->y_min[idx]);

	plot_x += PLOT_ZERO_TICK_OFFSET;
	plot_y -= (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET);

	(void) fprintf(fp, "0.5 setlinewidth\n");
	for (i = 1; i < plot->num_points[idx]; ++i) {
		x = plot_x + ((plot->xs[idx][i - 1] - plot->x_min[idx]) * x_scale);
		y = plot_y + ((plot->ys[idx][i - 1] - plot->y_min[idx]) * y_scale);
		x_2 = plot_x + ((plot->xs[idx][i] - plot->x_min[idx]) * x_scale);
		y_2 = plot_y  + ((plot->ys[idx][i] - plot->y_min[idx]) * y_scale);
		(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\n", x, y, x_2, y_2);
	}
	(void) fprintf(fp, "stroke\n");

	
	return;
}


 void	draw_errs(plot, idx, plot_x, plot_y, fp)
PLOT		*plot;
si4		idx;
sf4		plot_x, plot_y;
FILE		*fp;
{
	si4	i;
	sf4	x_scale, y_scale, x, y, x_2, y_2;


	x_scale = (PLOT_X_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->x_max[idx] - plot->x_min[idx]);
	y_scale = (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->y_max[idx] - plot->y_min[idx]);

	plot_x += PLOT_ZERO_TICK_OFFSET;
	plot_y -= (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET);

	(void) fprintf(fp, "0.25 setlinewidth\n");
	for (i = 0; i < plot->num_points[idx]; ++i) {
		x = plot_x + ((plot->xs[idx][i] - plot->x_min[idx]) * x_scale);
		y = plot_y + ((plot->ys[idx][i] - plot->errs[idx][i] - plot->y_min[idx]) * y_scale);
		y_2 = plot_y + ((plot->ys[idx][i] + plot->errs[idx][i] - plot->y_min[idx]) * y_scale);
		(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\n", x, y, x, y_2);
		x_2 = x + PLOT_ERR_TICK_SIZE;
		x -= PLOT_ERR_TICK_SIZE;
		(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\n", x, y, x_2, y);
		(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f ml\n", x, y_2, x_2, y_2);
	}
	(void) fprintf(fp, "stroke\n");


	return;
}


 void	draw_points(plot, idx, plot_x, plot_y, fp)
PLOT		*plot;
si4		idx;
sf4		plot_x, plot_y;
FILE		*fp;
{
	si4	i;
	sf4	x_scale, y_scale, x, y, half_point_size;


	x_scale = (PLOT_X_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->x_max[idx] - plot->x_min[idx]);
	y_scale = (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->y_max[idx] - plot->y_min[idx]);

	half_point_size = PLOT_POINT_SIZE / 2.0;
	plot_x += (PLOT_ZERO_TICK_OFFSET - half_point_size);
	plot_y -= (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET + half_point_size);

	for (i = 0; i < plot->num_points[idx]; ++i) {
		x = plot_x + ((plot->xs[idx][i] - plot->x_min[idx]) * x_scale);
		y = plot_y + ((plot->ys[idx][i] - plot->y_min[idx]) * y_scale);
		(void) fprintf(fp, "%0.2f %0.2f pt\n", x, y);
	}


	return;
}


 void	draw_bars(plot, idx, plot_x, plot_y, fp)
PLOT		*plot;
si4		idx;
sf4		plot_x, plot_y;
FILE		*fp;
{
	si4	i;
	sf4	x_scale, y_scale, x, y;
	sf4	height, width;


	x_scale = (PLOT_X_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->x_max[idx] - plot->x_min[idx]);
	y_scale = (PLOT_Y_SIZE - PLOT_ZERO_TICK_OFFSET) / (plot->y_max[idx] - plot->y_min[idx]);

	plot_x += PLOT_ZERO_TICK_OFFSET;

	width = (plot->xs[idx][1] - plot->xs[idx][0]) * x_scale;
	y = plot_y - PLOT_Y_SIZE;

	for (i = 0; i < plot->num_points[idx]; ++i) {
		x = plot_x + ((plot->xs[idx][i] - plot->x_min[idx]) * x_scale);
		height = (plot->ys[idx][i] - plot->y_min[idx]) * y_scale + PLOT_ZERO_TICK_OFFSET;
		(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f rectfill\n", x, y, width, height);
	}


	return;
}
