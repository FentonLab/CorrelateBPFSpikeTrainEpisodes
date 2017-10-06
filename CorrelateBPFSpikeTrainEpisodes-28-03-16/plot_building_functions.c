
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"
#include "postscript.h"
#include "theta.h"
#include "uff.h"


/* Note that all draw flags are set to FALSE when the "PLOT" structure is allocated. */
/* Only those necessary for the particular plot type need to be set in these */
/* functions. Only "point_set_labels" that have been filled will be printed. */
/* All point sets whose "num_points" field is greater than zero will be drawn */
/* and only if any draw flags have been set. Only "vals" whose labels have been filled */
/* will be printed. Only "name", "type", "x_units", and "y_units" labels that have */
/* been filled will be printed */

/* Note that using the function "lin_regress" one can easily draw a scatter plot */
/* with a fitted line. Make one point set a scatter plot and another a two point */
/* line plot with the points calculated from the coefficients returned by "lin_regress" */

/* correlation coefficients, etc. can be obtained from "correl" or "lin_regress", and */
/* printed as "vals" */


void	theta_phase_plot(plot, cell, sess)
PLOT	*plot;
CELL	*cell;
SESSION	*sess;
{
	si4		i, num_bins, bin, n_recs, probe, clust, type;
	sf4		bin_width, min, max;	
	sf8		*spks, *tm;
	RECORD		*recs;
	extern void	*gmem();
	ui1		PositionRec;

	num_bins = sess->params->plot_phase_bins;

	recs = sess->recs;
	n_recs = sess->num_recs;	

	spks = (sf8 *) gmem((size_t) (num_bins + 1), sizeof(sf8));
	tm = (sf8 *) gmem((size_t) (num_bins + 1), sizeof(sf8));

	probe = cell->probe;
	clust = cell->clust;
	PositionRec = sess->PositionRec;

	bin_width = 360.0 / (sf4) num_bins;
	for (i = 0; i < n_recs; ++i) {
		if (recs[i].theta_state != THETA)
			continue;
		type = recs[i].type;
		if (type == PositionRec) {
			bin = (si4) ((sf4) recs[i].theta_phase / bin_width);
			tm[bin] += 1.0;
		} else if (is_spike(*sess, type)) {
			if (recs[i].probe != probe || recs[i].clust != clust)
				continue;
			bin = (si4) ((sf4) recs[i].theta_phase / bin_width);
			spks[bin] += 1.0;
		}
	}

	plot->xs[0] = (sf4 *) gmem((size_t) num_bins, sizeof(sf4));
	plot->ys[0] = (sf4 *) gmem((size_t) num_bins, sizeof(sf4));

	for (i = 0; i < num_bins; ++i) {
		if (tm[i])
			plot->ys[0][i] = (sf4) (spks[i] / (tm[i] / (sf8) sess->params->position_samp_freq));
		else
			plot->ys[0][i] = 0.0;
	}

	free((void *) spks);
	free((void *) tm);
			
	plot->y_min[0] = 0;
	plot->y_max[0] = plot->ys[0][0];
	for (i = 0; i < num_bins; ++i) {
		plot->xs[0][i] = (sf4) i * bin_width;
		if (plot->ys[0][i] > plot->y_max[0])
			plot->y_max[0] = plot->ys[0][i];
	}
	plot->x_min[0] = 0.0;
	plot->x_max[0] = 360.0;

	plot->x_units = "Theta Phase";
	plot->y_units = "Spikes / Second";
	strcpy(plot->name, cell->name_str);
	plot->type = "Theta Phase Spike Histogram";
	plot->num_points[0] = num_bins;
	plot->draw_bars[0] = TRUE;
	

	return;
}


void	head_direction_plot(plot, cell, sess)
PLOT	*plot;
CELL	*cell;
SESSION	*sess;
{
	si4		i, num_bins, bin, n_recs, probe, clust, type;
	sf4		bin_width, min, max;	
	si2		dir;
	sf8		*spks, *tm;
	RECORD		*recs;
	extern void	*gmem();
	ui1		PositionRec;

	num_bins = sess->params->plot_dir_bins;

	recs = sess->recs;
	n_recs = sess->num_recs;	

	spks = (sf8 *) gmem((size_t) (num_bins + 1), sizeof(sf8));
	tm = (sf8 *) gmem((size_t) (num_bins + 1), sizeof(sf8));

	probe = cell->probe;
	clust = cell->clust;
	PositionRec = sess->PositionRec;

	bin_width = 360.0 / (sf4) num_bins;
	for (i = 0; i < n_recs; ++i) {
		if ((dir = recs[i].direction) == NO_DIRECTION)
			continue;
		type = recs[i].type;
		if (type == PositionRec) {
			bin = (si4) ((sf4) recs[i].direction / bin_width);
			tm[bin] += 1.0;
		} else if (is_spike(*sess, type)) {
			if (recs[i].probe != probe || recs[i].clust != clust)
				continue;
			bin = (si4) ((sf4) recs[i].direction / bin_width);
			spks[bin] += 1.0;
		}
	}

	plot->xs[0] = (sf4 *) gmem((size_t) num_bins, sizeof(sf4));
	plot->ys[0] = (sf4 *) gmem((size_t) num_bins, sizeof(sf4));

	for (i = 0; i < num_bins; ++i) {
		if (tm[i])
			plot->ys[0][i] = (sf4) (spks[i] / (tm[i] / (sf8) sess->params->position_samp_freq));
		else
			plot->ys[0][i] = 0.0;
	}

	free((void *) spks);
	free((void *) tm);
			
	plot->y_min[0] = 0;
	plot->y_max[0] = plot->ys[0][0];
	for (i = 0; i < num_bins; ++i) {
		plot->xs[0][i] = (sf4) i * bin_width;
		if (plot->ys[0][i] > plot->y_max[0])
			plot->y_max[0] = plot->ys[0][i];
	}
	plot->x_min[0] = 0.0;
	plot->x_max[0] = 360.0;

	plot->x_units = "Head Direction";
	plot->y_units = "Spikes / Second";
	strcpy(plot->name, cell->name_str);
	plot->type = "Head Direction Spike Histogram";
	plot->num_points[0] = num_bins;
	plot->draw_bars[0] = TRUE;
	

	return;
}


void	interspike_int_plot(plot, cell, sess)
PLOT	*plot;
CELL	*cell;
SESSION	*sess;
{
	si4		i, num_bins, bin, n_recs, probe, clust, type;
	ui4		max_int, min_int, intvl, last_time;
	sf4		bin_width, oom;
	sf8		temp;
	RECORD		*recs;
	extern void	*gmem();

	num_bins = sess->params->interspike_int_bins;

	recs = sess->recs;
	n_recs = sess->num_recs;	

	plot->xs[0] = (sf4 *) gmem((size_t) (num_bins + 1), sizeof(sf4));
	plot->ys[0] = (sf4 *) gmem((size_t) (num_bins + 1), sizeof(sf4));

	temp = (sf8) sess->params->interspike_int_start_ms * ((sf8) TIME_STAMPS_PER_SECOND / 1000.0);
	min_int = (ui4) floor(temp);
	temp = (sf8) sess->params->interspike_int_end_ms * ((sf8) TIME_STAMPS_PER_SECOND / 1000.0);
	max_int = (ui4) ceil(temp);

	probe = cell->probe;
	clust = cell->clust;

	for (i = 0; i < n_recs; ++i) {
		type = recs[i].type;
		if (is_spike(*sess, type)) {
			if (recs[i].probe == probe || recs[i].clust == clust) {
				last_time = recs[i].time_stamp;
				break;
			}
		}
	}			

	bin_width = (sf4) (max_int - min_int) / (sf4) num_bins;
	for (; i < n_recs; ++i) {
		type = recs[i].type;
		if (is_spike(*sess, type)) {
			if (recs[i].probe != probe || recs[i].clust != clust)
				continue;
			intvl = recs[i].time_stamp - last_time;
			last_time = recs[i].time_stamp;
			if (intvl < min_int || intvl > max_int)
				continue;
			bin = (si4) ((sf4) intvl / bin_width);
			plot->ys[0][bin] += 1.0;
		}
	}


	plot->y_min[0] = plot->y_max[0] = plot->ys[0][0];
	for (i = 1; i < num_bins; ++i) {
		plot->xs[0][i] = (sf4) i * bin_width / ((sf8) TIME_STAMPS_PER_SECOND / 1000.0); /* convert to milliseconds */
		if (plot->ys[0][i] < plot->y_min[0])
			plot->y_min[0] = plot->ys[0][i];
		else if (plot->ys[0][i] > plot->y_max[0])
			plot->y_max[0] = plot->ys[0][i];
	}
	if (plot->y_min[0] != 0.0) {
		plot->y_min[0] -= 0.05 * (plot->y_max[0] - plot->y_min[0]);
		if (plot->y_min[0] < 0.0)
			plot->y_min[0] = 0.0;
	}
	plot->x_min[0] = plot->xs[0][0] = sess->params->interspike_int_start_ms;
	plot->x_max[0] = sess->params->interspike_int_end_ms;

	oom = 1.0;
	while (plot->y_max[0] >= 100.0) {
		plot->y_max[0] /= 10.0;
		oom *= 10.0;
	}
	if (oom > 1.0) {
		plot->y_min[0] /= oom;
		for (i = 0; i < num_bins; ++i)
			plot->ys[0][i] /= oom;
	}

	if (oom == 1.0)
		plot->y_units = "Counts";
	else if (oom == 10.0)
		plot->y_units = "Counts (tens)";
	else if (oom == 100.0)
		plot->y_units = "Counts (hundreds)";
	else if (oom == 1000.0)
		plot->y_units = "Counts (thousands)";
	else if (oom == 10000.0)
		plot->y_units = "Counts (ten thousands)";
	else if (oom == 100000.0)
		plot->y_units = "Counts (hundred thousands)";
	else if (oom == 1000000.0)
		plot->y_units = "Counts (millions)";
	
	plot->x_units = "Milliseconds";
	strcpy(plot->name, cell->name_str);
	plot->type = "Interspike Interval Histogram";
	plot->num_points[0] = num_bins;
	plot->draw_bars[0] = TRUE;
	

	return;
}


void	autocorrelation_plot(plot, cell, sess)
PLOT	*plot;
CELL	*cell;
SESSION	*sess;
{
	si4		i, j, num_bins, bin, n_recs, probe, clust, type;
	ui4		max_int, min_int, intvl, last_time;
	sf4		bin_width, oom;	
	sf8		temp;
	RECORD		*recs;
	extern void	*gmem();

	num_bins = sess->params->interspike_int_bins;

	recs = sess->recs;
	n_recs = sess->num_recs;	

	plot->xs[0] = (sf4 *) gmem((size_t) (num_bins + 1), sizeof(sf4));
	plot->ys[0] = (sf4 *) gmem((size_t) (num_bins + 1), sizeof(sf4));

	temp = (sf8) sess->params->autocorr_start_ms * ((sf8) TIME_STAMPS_PER_SECOND / 1000.0);
	min_int = (ui4) floor(temp);
	temp = (sf8) sess->params->autocorr_end_ms * ((sf8) TIME_STAMPS_PER_SECOND / 1000.0);
	max_int = (ui4) ceil(temp);

	probe = cell->probe;
	clust = cell->clust;

	bin_width = (sf4) (max_int - min_int) / (sf4) num_bins;
	for (i = 0; i < n_recs; ++i) {
		type = recs[i].type;
		if (is_spike(*sess, type)) {
			if (recs[i].probe != probe || recs[i].clust != clust)
				continue;
			last_time = recs[i].time_stamp;
			for (j = i; j < n_recs; ++j) {
				intvl = recs[j].time_stamp - last_time;
				if (intvl >= min_int)
					break;
			}
			for (; j < n_recs; ++j) {
				intvl = recs[j].time_stamp - last_time;
				if (intvl > max_int)
					break;
				type = recs[i].type;
				if (is_spike(*sess, type)) {
					if (recs[j].probe != probe || recs[j].clust != clust)
						continue;
					bin = (si4) ((sf4) intvl / bin_width);
					plot->ys[0][bin] += 1.0;
				}
			}
		}
	}

	i = 0;
	if ((sf4) min_int < bin_width)
		i = 1;
	plot->y_min[0] = plot->y_max[0] = plot->ys[0][i];
	for (; i < num_bins; ++i) {
		plot->xs[0][i] = (sf4) i * bin_width / ((sf8) TIME_STAMPS_PER_SECOND / 1000.0); /* convert to milliseconds */
		if (plot->ys[0][i] < plot->y_min[0])
			plot->y_min[0] = plot->ys[0][i];
		else if (plot->ys[0][i] > plot->y_max[0])
			plot->y_max[0] = plot->ys[0][i];
	}
	if (plot->y_min[0] != 0.0) {
		plot->y_min[0] -= 0.05 * (plot->y_max - plot->y_min);
		if (plot->y_min[0] < 0.0)
			plot->y_min[0] = 0.0;
	}
	if ((sf4) min_int < bin_width)
		if (plot->ys[0][0] > plot->y_max[0])
			plot->ys[0][0] = plot->y_max[0];
	plot->x_min[0] = plot->xs[0][0] = sess->params->autocorr_start_ms;
	plot->x_max[0] = sess->params->autocorr_end_ms;

	oom = 1.0;
	while (plot->y_max[0] >= 100.0) {
		plot->y_max[0] /= 10.0;
		oom *= 10.0;
	}
	if (oom > 1.0) {
		plot->y_min[0] /= oom;
		for (i = 0; i < num_bins; ++i)
			plot->ys[0][i] /= oom;
	}

	if (oom == 1.0)
		plot->y_units = "Counts";
	else if (oom == 10.0)
		plot->y_units = "Counts (tens)";
	else if (oom == 100.0)
		plot->y_units = "Counts (hundreds)";
	else if (oom == 1000.0)
		plot->y_units = "Counts (thousands)";
	else if (oom == 10000.0)
		plot->y_units = "Counts (ten thousands)";
	else if (oom == 100000.0)
		plot->y_units = "Counts (hundred thousands)";
	else if (oom == 1000000.0)
		plot->y_units = "Counts (millions)";

	plot->x_units = "Milliseconds";
	strcpy(plot->name, cell->name_str);
	plot->type = "Autocorrelation";
	plot->num_points[0] = num_bins;
	plot->draw_bars[0] = TRUE;
	

	return;
}

void	cross_correlation_plot(plot, cell1, cell2, sess)
PLOT	*plot;
CELL	*cell1, *cell2;
SESSION	*sess;
{
	static si1		string[256];
	si4		i, j, num_bins, bin, n_recs, probe, clust1, clust2, type;
	ui4		max_int, min_int, intvl, last_time;
	sf4		bin_width, oom;	
	sf8		temp;
	RECORD		*recs;
	extern void	*gmem();

	num_bins = sess->params->interspike_int_bins;

	recs = sess->recs;
	n_recs = sess->num_recs;	

	plot->xs[0] = (sf4 *) gmem((size_t) (num_bins + 1), sizeof(sf4));
	plot->ys[0] = (sf4 *) gmem((size_t) (num_bins + 1), sizeof(sf4));

	temp = (sf8) sess->params->autocorr_start_ms * ((sf8) TIME_STAMPS_PER_SECOND / 1000.0);
	min_int = (ui4) floor(temp);
	temp = (sf8) sess->params->autocorr_end_ms * ((sf8) TIME_STAMPS_PER_SECOND / 1000.0);
	max_int = (ui4) ceil(temp);

	probe = cell1->probe;
	clust1 = cell1->clust;
	clust2 = cell2->clust;

	bin_width = (sf4) (max_int - min_int) / (sf4) num_bins;
	for (i = 0; i < n_recs; ++i) {
		type = recs[i].type;
		if (is_spike(*sess, type)) {
			if (recs[i].probe != probe || recs[i].clust != clust1)
				continue;
			last_time = recs[i].time_stamp;
			for (j = i; j < n_recs; ++j) {
				intvl = recs[j].time_stamp - last_time;
				if (intvl >= min_int)
					break;
			}
			for (; j < n_recs; ++j) {
				intvl = recs[j].time_stamp - last_time;
				if (intvl > max_int)
					break;
				type = recs[i].type;
				if (is_spike(*sess, type)) {
					if (recs[j].probe != probe || recs[j].clust != clust2)
						continue;
					bin = (si4) ((sf4) intvl / bin_width);
					plot->ys[0][bin] += 1.0;
				}
			}
		}
	}

	i = 0;
	if ((sf4) min_int < bin_width)
		i = 1;
	plot->y_min[0] = plot->y_max[0] = plot->ys[0][i];
	for (; i < num_bins; ++i) {
		plot->xs[0][i] = (sf4) i * bin_width / ((sf8) TIME_STAMPS_PER_SECOND / 1000.0); /* convert to milliseconds */
		if (plot->ys[0][i] < plot->y_min[0])
			plot->y_min[0] = plot->ys[0][i];
		else if (plot->ys[0][i] > plot->y_max[0])
			plot->y_max[0] = plot->ys[0][i];
	}
	if (plot->y_min[0] != 0.0) {
		plot->y_min[0] -= 0.05 * (plot->y_max[0] - plot->y_min[0]);
		if (plot->y_min[0] < 0.0)
			plot->y_min[0] = 0.0;
	}
	if ((sf4) min_int < bin_width)
		if (plot->ys[0][0] > plot->y_max[0])
			plot->ys[0][0] = plot->y_max[0];
	plot->x_min[0] = plot->xs[0][0] = sess->params->autocorr_start_ms;
	plot->x_max[0] = sess->params->autocorr_end_ms;

	oom = 1.0;
	while (plot->y_max[0] >= 100.0) {
		plot->y_max[0] /= 10.0;
		oom *= 10.0;
	}
	if (oom > 1.0) {
		plot->y_min[0] /= oom;
		for (i = 0; i < num_bins; ++i)
			plot->ys[0][i] /= oom;
	}

	if (oom == 1.0)
		plot->y_units = "Counts";
	else if (oom == 10.0)
		plot->y_units = "Counts (tens)";
	else if (oom == 100.0)
		plot->y_units = "Counts (hundreds)";
	else if (oom == 1000.0)
		plot->y_units = "Counts (thousands)";
	else if (oom == 10000.0)
		plot->y_units = "Counts (ten thousands)";
	else if (oom == 100000.0)
		plot->y_units = "Counts (hundred thousands)";
	else if (oom == 1000000.0)
		plot->y_units = "Counts (millions)";

	plot->x_units = "Milliseconds";
	sprintf(string, "(%s X %s)", cell1->name_str, cell2->name_str);
	strcpy(plot->name, string);
	plot->type = "Cross Correlation";
	plot->num_points[0] = num_bins;
	plot->draw_bars[0] = TRUE;
	
	return;
}


void	mean_waveform_plot(plot, cell, sess, num_ad_vals, n_channels)
PLOT	*plot;
CELL	*cell;
SESSION	*sess;
si4	num_ad_vals, n_channels;
{
	si4		i, j, num_bins, bin, n_recs, probe, clust, type;
	ui4		max_int, min_int, intvl, last_time;
	sf4		bin_width, oom;	
	sf8		temp;
	RECORD		*recs;
	extern void	*gmem();

	recs = sess->recs;
	n_recs = sess->num_recs;	

	for (j = 0; j < MAX_POINT_SETS; ++j) {
		plot->draw_points[j] = FALSE;
		plot->draw_lines[j] = FALSE;
                plot->draw_bars[j] = FALSE;
                plot->draw_errs[j] = FALSE;
	}

	for (j = 0; j < n_channels; ++j) {
		plot->xs[j] = (sf4 *) gmem((size_t) num_ad_vals, sizeof(sf4));
		plot->ys[j] = (sf4 *) gmem((size_t) num_ad_vals, sizeof(sf4));

		if (sess->opts->error_bars_on_waveforms)
			plot->errs[j] = (sf4 *) gmem((size_t) num_ad_vals, sizeof(sf4));
		if(is_bpf(*sess)){
			plot->y_min[j] = ((sf4) BPF_MIN_AD / 1000.0);
			plot->y_max[j] = ((sf4) BPF_MAX_AD / 1000.0);
		}else{
			plot->y_min[j] = ((sf4) BW_MIN_AD / 2.0) / 1000.0;
			plot->y_max[j] = ((sf4) BW_MAX_AD + 100.0) / 1000.0;
		}
	}

	for (i = 0; i < num_ad_vals; ++i) {
		for (j = 0; j < n_channels; ++j) {
			plot->xs[j][i] = (sf4) (i + 1);
			plot->ys[j][i] = (sf4) cell->mean_waveform[j][i] / 1000.0;
			if (plot->ys[j][i] < plot->y_min[j])
				plot->ys[j][i] = plot->y_min[j];
		}
	}
	if (sess->opts->error_bars_on_waveforms) {
		for (i = 0; i < num_ad_vals; ++i) {
			for (j = 0; j < n_channels; ++j) {
				plot->errs[j][i] = (sf4) cell->mean_waveform_sds[j][i] / 1000.0;
				if ((plot->ys[j][i] - plot->errs[j][i]) < plot->y_min[j])
					plot->errs[j][i] = plot->ys[j][i] - plot->y_min[j];
				if ((plot->ys[j][i] + plot->errs[j][i]) > plot->y_max[j])
					plot->errs[j][i] = plot->y_max[j] - plot->ys[j][i];
			}
		}
	}

	for (j = 0; j < n_channels; ++j) {
		plot->x_min[j] = plot->xs[j][0];
		plot->x_max[j] = plot->xs[j][num_ad_vals - 1];
		plot->num_points[j] = num_ad_vals;

		plot->vals[j] = (sf4) cell->mean_waveform_cv[j];
		plot->vals[j+n_channels] = (sf4) cell->mean_waveform_width[j];
		plot->val_names[j] = "cv";
		plot->val_names[j+n_channels] = "width";

		plot->draw_lines[j] = TRUE;
		if (sess->opts->error_bars_on_waveforms)
			plot->draw_errs[j] = TRUE;
		else
			plot->draw_errs[j] = FALSE;
	}

	plot->x_units = "A/D Sample Number";
	plot->y_units = "A/D Sample Value (thousands)";
	strcpy(plot->name, cell->name_str);
	plot->type = "Mean Waveform";

	// produce a V=0 line in the waveform plots

	plot->xs[n_channels] = (sf4 *) gmem((size_t) num_ad_vals, sizeof(sf4));
	plot->ys[n_channels] = (sf4 *) gmem((size_t) num_ad_vals, sizeof(sf4));
	for (i = 0; i < num_ad_vals; ++i){
		plot->xs[n_channels][i] = (sf4) (i + 1);
		plot->ys[n_channels][i] = (sf4) 0.0;
	}
	plot->x_min[n_channels] = 1;
	plot->x_max[n_channels] = num_ad_vals;
	plot->y_min[n_channels] = -0.1;
	plot->y_max[n_channels] = 0.1;
	plot->num_points[n_channels] = num_ad_vals;
	plot->draw_lines[n_channels] = TRUE;
	
	return;
}

