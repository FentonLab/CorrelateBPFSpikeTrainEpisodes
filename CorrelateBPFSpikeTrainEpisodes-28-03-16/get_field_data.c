
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	get_field_data(sess)
SESSION	*sess;
{
	si4		i, j, n_units, n_fields;
	 void	one_cell_field_data(), out_of_field_data();
	CELL		*cp;
	extern void	*gmem();

	
	n_units = sess->num_cells + sess->num_probes;

	for (i = 0; i < n_units; ++i) {

		cp = sess->cells[i];
		n_fields = cp->num_fields + 1;
		cp->field_data = (FIELD_DATA **) gmem((size_t) n_fields, sizeof(FIELD_DATA *));
		for (j = 0; j < n_fields; ++j)
			cp->field_data[j] = (FIELD_DATA *) gmem((size_t) 1, sizeof(FIELD_DATA));
		cp->out_of_field_data = cp->field_data[cp->num_fields];

		one_cell_field_data(sess, i);
		out_of_field_data(sess, i);
	}
	for (i = 0; i < n_units; ++i) {

		cp = sess->secondary_cells[i];
		n_fields = cp->num_fields + 1;
		cp->field_data = (FIELD_DATA **) gmem((size_t) n_fields, sizeof(FIELD_DATA *));
		for (j = 0; j < n_fields; ++j)
			cp->field_data[j] = (FIELD_DATA *) gmem((size_t) 1, sizeof(FIELD_DATA));
		cp->out_of_field_data = cp->field_data[cp->num_fields];

		one_cell_field_data(sess, i);
		out_of_field_data(sess, i);
	}

	return;
}


 void	one_cell_field_data(sess, cell_num)
SESSION		*sess;
si4		cell_num;
{
	si4		i, j, k, l, f_idx, f_num, temp_spikes;
	si4		i_lim, j_lim, k_lim, l_lim, x_size, y_size;
	sf8		temp_time, temp_rate;
	sf4		**time; 
	ui2		**spikes;
	ui1		**f_map;
	FIELD_DATA	*fd;
	CELL		*cp;


	cp = sess->cells[cell_num];

	for (i = 0; i < cp->num_fields; ++i) {
		fd = cp->field_data[i];
		fd->number = i + 1;
		fd->center_rate = fd->total_time = 0.0;
		fd->total_spikes = fd->total_pixels = 0;
	}

	f_map = cp->field_map;
	time = sess->time->map;
	spikes = cp->spike_map;
	x_size = sess->params->x_size;
	y_size = sess->params->y_size;

	i_lim = y_size - 1;
	j_lim = x_size - 1;
	for (i = 1; i < i_lim; ++i) {
		for (j = 1; j < j_lim; ++j) {
			if (!f_map[i][j]) 
				continue;
			f_num = f_map[i][j];
			f_idx = f_num - 1;
			fd = cp->field_data[f_idx];
			k_lim = i + 2;
			l_lim = j + 2;
			temp_spikes = 0;
			temp_time = 0.0;
			for (k = i - 1; k < k_lim; ++k)
				for (l = j - 1; l < l_lim; ++l)
					if (f_map[k][l] == f_num) {
						temp_spikes += (si4) spikes[k][l];
						temp_time += (sf8) time[k][l];
					}
			temp_rate = (sf8) temp_spikes / (sf8) temp_time;
			if (temp_rate > fd->center_rate) {
				fd->center_rate = temp_rate;
				fd->center_x = j;
				fd->center_y = i;
			}
			++fd->total_pixels;
			fd->total_spikes += (si4) spikes[i][j];
			fd->total_time += (sf8) time[i][j];
		}
	}

	for (i = 0; i < cp->num_fields; ++i) {
		fd = cp->field_data[i];
		fd->grand_rate = (sf8) fd->total_spikes / fd->total_time;
	}
	cp = sess->secondary_cells[cell_num];

	for (i = 0; i < cp->num_fields; ++i) {
		fd = cp->field_data[i];
		fd->number = i + 1;
		fd->center_rate = fd->total_time = 0.0;
		fd->total_spikes = fd->total_pixels = 0;
	}

	f_map = cp->field_map;
	time = sess->secondary_time->map;
	spikes = cp->spike_map;
	x_size = sess->params->x_size;
	y_size = sess->params->y_size;

	i_lim = y_size - 1;
	j_lim = x_size - 1;
	for (i = 1; i < i_lim; ++i) {
		for (j = 1; j < j_lim; ++j) {
			if (!f_map[i][j]) 
				continue;
			f_num = f_map[i][j];
			f_idx = f_num - 1;
			fd = cp->field_data[f_idx];
			k_lim = i + 2;
			l_lim = j + 2;
			temp_spikes = 0;
			temp_time = 0.0;
			for (k = i - 1; k < k_lim; ++k)
				for (l = j - 1; l < l_lim; ++l)
					if (f_map[k][l] == f_num) {
						temp_spikes += (si4) spikes[k][l];
						temp_time += (sf8) time[k][l];
					}
			temp_rate = (sf8) temp_spikes / (sf8) temp_time;
			if (temp_rate > fd->center_rate) {
				fd->center_rate = temp_rate;
				fd->center_x = j;
				fd->center_y = i;
			}
			++fd->total_pixels;
			fd->total_spikes += (si4) spikes[i][j];
			fd->total_time += (sf8) time[i][j];
		}
	}

	for (i = 0; i < cp->num_fields; ++i) {
		fd = cp->field_data[i];
		fd->grand_rate = (sf8) fd->total_spikes / fd->total_time;
	}
	
	return;
}


 void	out_of_field_data(sess, cell_num)
SESSION		*sess;
si4		cell_num;
{
	si4		i, area;
	sf4		*time;
	ui2		*spikes;
	ui1		*f_map;
	CELL		*cp;
	FIELD_DATA	*ofd;

	cp = sess->cells[cell_num];
	f_map = cp->field_map[0];
	time = sess->time->map[0];
	spikes = cp->spike_map[0];
	ofd = cp->out_of_field_data;

	ofd->total_spikes = 0;
	ofd->total_time = 0.0;

	area = sess->params->area;

	for (i = 0; i < area; ++i) {
		if (f_map[i] || !time[i]) 
			continue;
		++ofd->total_pixels;
		ofd->total_spikes += (si4) spikes[i];
		ofd->total_time += (sf8) time[i];
	}
	cp = sess->secondary_cells[cell_num];
	f_map = cp->field_map[0];
	time = sess->secondary_time->map[0];
	spikes = cp->spike_map[0];
	ofd = cp->out_of_field_data;

	ofd->total_spikes = 0;
	ofd->total_time = 0.0;

	area = sess->params->area;

	for (i = 0; i < area; ++i) {
		if (f_map[i] || !time[i]) 
			continue;
		++ofd->total_pixels;
		ofd->total_spikes += (si4) spikes[i];
		ofd->total_time += (sf8) time[i];
	}

	if (!ofd->total_time)
		ofd->grand_rate = NO_RATE;
	else
		ofd->grand_rate = (sf8) ofd->total_spikes / ofd->total_time;

	return;
}
