
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void	 calculate_mean_waveforms(sess, offset, num_ad_vals)
SESSION	*sess;
si4	offset, num_ad_vals;
{
	si4		i, j, k, ch, t, n_recs, n_cells, cell_num, n_channels, first_zero, second_zero;
	si4		sign, peak, max_v, max_ch, max_t;	
	si4		tmp[TET_CHANNELS], ch_peak[TET_CHANNELS];
	si4		cmpr(), ch1, ch2;
	sf4		a[64], b[64];
	si2		temp_si2, **wave;
	RECORD		*recs;
	ui1		*lbt, *hbt, *lbd, *hbd, type;
	si1		**cm;
	sf8		***sum, ***sum_sq, *n_waves, temp_sf8, *tot_amp, *tot_sd, **width;
	sf8		r, z, p;
	CELL		*cp;
	extern void	*gmem();
	sf8		get_waveform_width();

	n_recs = sess->num_recs;
	recs = sess->recs;
	cm = sess->cell_map;
	n_cells = sess->num_cells;

	sum = (sf8 ***) gmem((size_t) n_cells, sizeof(sf8 **));
	sum_sq = (sf8 ***) gmem((size_t) n_cells, sizeof(sf8 **));
	width = (sf8 **) gmem((size_t) n_cells, sizeof(sf8 *));
	for (i = 0; i < n_cells; ++i) {
		sum[i] = (sf8 **) gmem((size_t) MAX_MULTITRODE_CHANNELS, sizeof(sf8 *));
		sum_sq[i] = (sf8 **) gmem((size_t) MAX_MULTITRODE_CHANNELS, sizeof(sf8 *));
		width[i] = (sf8 *) gmem((size_t) MAX_MULTITRODE_CHANNELS, sizeof(sf8));

		for (j = 0; j < MAX_MULTITRODE_CHANNELS; ++j) {
			sum[i][j] = (sf8 *) gmem((size_t) num_ad_vals, sizeof(sf8));
			sum_sq[i][j] = (sf8 *) gmem((size_t) num_ad_vals, sizeof(sf8));
		}
	}
	wave = (si2 **) gmem((size_t) MAX_MULTITRODE_CHANNELS, sizeof(si2 *));
	for (j = 0; j < MAX_MULTITRODE_CHANNELS; ++j)
		wave[j] = (si2 *) gmem((size_t) num_ad_vals, sizeof(si2));

	n_waves = (sf8 *) gmem((size_t) n_cells, sizeof(sf8));
	tot_amp = (sf8 *) gmem((size_t) MAX_MULTITRODE_CHANNELS, sizeof(sf8));
	tot_sd = (sf8 *) gmem((size_t) MAX_MULTITRODE_CHANNELS, sizeof(sf8));

#ifdef BIG_ENDIAN_MACHINE
	hbt = (ui1 *) &temp_si2;
	lbt = hbt + 1;
#else
	lbt = (ui1 *) &temp_si2;
	hbt = lbt + 1;
#endif

	for (i = 0; i < n_recs; ++i) {
		type = recs[i].type;
		if (!is_spike(*sess, type))
			continue;
		cell_num = cm[recs[i].probe][recs[i].clust];
		if (cell_num == NOT_CUT)
			continue;
		lbd = recs[i].data_offset + offset - 2;
		hbd = lbd + 1;

		if((type == SINGLE_BPF_REC_TYPE) || (type == SINGLE_ELECTRODE_SPIKE_REC))
			n_channels = 1;
		else if((type == STEREO_BPF_REC_TYPE) || (type == STEREOTRODE_SPIKE_REC))
			n_channels = 2;
		else if((type == TETRODE_BPF_REC_TYPE) || (type == TETRODE_SPIKE_REC))
			n_channels = 4;
 
		for (j = 0; j < n_channels; ++j) {
			for (k = 0; k < num_ad_vals; ++k) {
				*lbt = *(lbd += 2);
				*hbt = *(hbd += 2);

				sum[cell_num][j][k] += (temp_sf8 = (sf8) temp_si2);
				sum_sq[cell_num][j][k] += temp_sf8 * temp_sf8;
				wave[j][k] = temp_si2;
			}
			width[cell_num][j] += get_waveform_width(wave[j], num_ad_vals);
		}
		n_waves[cell_num] += 1.0;

	}

	for (j = 0; j < n_channels; ++j)
		tot_amp[j] = tot_sd[j] = 0.0;
	
	for (i = 0; i < n_cells; ++i) {
		cp = sess->cells[i];	
		for (j = 0; j < n_channels; ++j) {
			width[i][j] /= n_waves[i];
			for (k = 0; k < num_ad_vals; ++k) {
				cp->mean_waveform[j][k] = sum[i][j][k] / n_waves[i];
				cp->mean_waveform_sds[j][k] = (sum_sq[i][j][k] - ((sum[i][j][k] * sum[i][j][k]) / n_waves[i])) / n_waves[i];
				cp->mean_waveform_sds[j][k] = sqrt(cp->mean_waveform_sds[j][k]);
				if ((temp_sf8 = cp->mean_waveform[j][k]) < 0.0)
					temp_sf8 = -temp_sf8;
				tot_amp[j] += temp_sf8;
				tot_sd[j] +=  cp->mean_waveform_sds[j][k];
			}
		}
		for (j = 0; j < n_channels; ++j) {
			cp->mean_waveform_width[j] = width[i][j];
			if (tot_amp[j])
				cp->mean_waveform_cv[j] = tot_sd[j] / tot_amp[j];
			else
				cp->mean_waveform_cv[j] = -1.0;
		}
	}

	for (i = 0; i < n_cells; ++i) {
		for (j = 0; j < n_channels; ++j) {
			free((void *) sum[i][j]);
			free((void *) sum_sq[i][j]);
		}
		free((void *) sum[i]);
		free((void *) sum_sq[i]);
	}
	free((void *) sum); 
	free((void *) sum_sq);

	// select waveform width from the largest waveform
	for (i = 0; i < n_cells; ++i) {
		cp = sess->cells[i];	
		max_ch = max_t = max_v = 0;		// first find the peak across all channels of a probe
		for (ch = 0; ch < n_channels; ++ch) {
			for (t = 0; t < num_ad_vals; ++t) {
				if(max_v < abs(cp->mean_waveform[ch][t])){
					max_v = abs(cp->mean_waveform[ch][t]);
					peak = cp->mean_waveform[ch][t];
					max_t = t;
					max_ch = ch;
				}
			}
		}
		cp->spike_width_in_samps = width[i][max_ch];
	}

	// calculate correlation between two largest waveforms
	for (i = 0; i < n_cells; ++i) {
		cp = sess->cells[i];	

		if(n_channels == 1){
			cp->waveform_correlation = 1.0;
			continue;
		}
		for (ch = 0; ch < n_channels; ++ch) 
			ch_peak[ch] = 0;		// first find the peak of each channel of a probe
		for (ch = 0; ch < n_channels; ++ch) {
			if(ch_peak[ch] < abs(cp->mean_waveform[ch][t]))
				ch_peak[ch] = abs(cp->mean_waveform[ch][t]);
			tmp[ch] = ch_peak[ch];
		}
		(void) qsort((void *) tmp, (size_t) n_channels, sizeof(si4), cmpr);
		for (ch = 0; ch < n_channels; ++ch){
			if(ch_peak[ch] == tmp[0])
				ch1 = ch;
			if(ch_peak[ch] == tmp[1])
				ch2 = ch;
		}
		for (t = 0; t < num_ad_vals; ++t) {
			a[t] = (sf4)cp->mean_waveform[ch1][t];
			b[t] = (sf4)cp->mean_waveform[ch2][t];
		}
		correl(a, b, num_ad_vals, &(cp->waveform_correlation), &z, &p);
	}

	return;
}
si4      cmpr(a, b)
si4             *a, *b;
{
        return(*a < *b);
}

sf8 get_waveform_width( wave, num_ad_vals)
si2 *wave;
si4 num_ad_vals;
{
	si4 t, max_peak_t, peak_t;
	si4	peak;
	sf8	pre_zero_t, post_zero_t;
	
	if (num_ad_vals > 32)
		max_peak_t = 32;

	peak = peak_t = 0;
	for(t = 0; t < max_peak_t; t++){
		if(abs((si4)wave[t]) > peak){
			peak = abs((si4)wave[t]);
			peak_t = t;
		}
	}

	// find the zero crossing before the peak
	pre_zero_t = 0.0;
	for(t=peak_t; t > 0; t--){
		if(abs((si4)(wave[t] - wave[t-1])) > abs((si4)wave[t])){	// hit a zero crossing
			pre_zero_t = (sf8)t - (sf8)abs((si4)wave[t])/(sf8)abs((si4)(wave[t] - wave[t-1]));
			break;
		}
	}
	// find the zero crossing after the peak
	post_zero_t = num_ad_vals;
	for(t=peak_t; t < num_ad_vals; t++){
		if(abs((si4)(wave[t-1] - wave[t])) > abs((si4)wave[t])){	// hit a zero crossing
			post_zero_t = (sf8)t + (sf8)abs((si4)wave[t])/(sf8)abs((si4)(wave[t-1] - wave[t]));
			break;
		}
	}

	return(post_zero_t - pre_zero_t);
}
