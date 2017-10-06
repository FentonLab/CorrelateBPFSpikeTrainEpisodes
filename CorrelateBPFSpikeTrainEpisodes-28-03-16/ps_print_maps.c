
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"
#include "postscript.h"


void	ps_print_maps(sess)
SESSION	*sess;
{
	si4		i, j, n_maps, *n_medians;
	si4		n_pages, page, probe_num, cell_num, x_sz, y_sz;
	sf4		map_x, map_y, field_medns[N_BKPTS], **medians, map_pix_sz;
	si1		command[256], out_file[256], name_str[256], **map_types;
	si1		*ps_dir, **map_names, **key_labels, *printer, *page_label;
	extern si1	*get_env_var();
	ui1		***maps;
	FILE		*fp;
	extern void	*gmem();
	RGBCOL		*colors;
	APPARATUS	*app;
	void	make_map(), make_key(), outline_map(), new_map_page();

	static RGBCOL	color_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						1.00, 1.00, 0.40,	/* YELLOW */
						1.00, 0.80, 0.15,	/* ORANGE */
						0.95, 0.25, 0.25,	/* RED */
						0.40, 1.00, 0.40,	/* GREEN */
						0.00, 0.65, 1.00,	/* BLUE */
						0.40, 0.00, 1.00,	/* PURPLE */
						0.80, 0.80, 0.80,	/* GRAY */
						0.00, 0.00, 0.00};	/* BLACK */

	static RGBCOL	gray_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						0.97, 0.97, 0.97,	/* GRAY 1 */
						0.89, 0.89, 0.89,	/* GRAY 2 */
						0.79, 0.79, 0.79,	/* GRAY 3 */
						0.55, 0.55, 0.55,	/* GRAY 4 */
						0.35, 0.35, 0.35,	/* GRAY 5 */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00};	/* BLACK */

	if (sess->opts->color_maps)
		colors = color_pix;
	else
		colors = gray_pix;

	n_maps = 0;
	if (sess->opts->time_map)
		++n_maps;
	if (sess->opts->speed_map)
		++n_maps;
	if (sess->opts->rate_maps)
		n_maps += sess->num_cells + sess->num_probes;
	if (sess->opts->smoothed_rate_maps)
		n_maps += sess->num_cells + sess->num_probes;
	if (sess->opts->field_maps)
		n_maps += sess->num_cells;
	if (sess->opts->rot_dir_div_rate_maps)
		n_maps += 2 * sess->num_cells;

	maps = (ui1 ***) gmem((size_t) n_maps, sizeof(ui1 **));
	map_types = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));
	map_names = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));
	medians = (sf4 **) gmem((size_t) n_maps, sizeof(sf4 *));
	n_medians = (si4 *) gmem((size_t) n_maps, sizeof(si4));
	key_labels = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));

	/* make array of maps to be printed */

	n_maps = 0;
	if (sess->opts->time_map) {
		maps[n_maps] = sess->time->show_map;
		map_types[n_maps] = "Time Map";
		map_names[n_maps] = "";
		medians[n_maps] = sess->time->medns;
		n_medians[n_maps] = sess->time->num_bkpts;
		key_labels[n_maps] = "Seconds";
		++n_maps;
	}
	if (sess->opts->speed_map) {
		maps[n_maps] = sess->speed->show_map;
		map_types[n_maps] = "Speed Map";
		map_names[n_maps] = "";
		medians[n_maps] = sess->speed->medns;
		n_medians[n_maps] = sess->speed->num_bkpts;
		key_labels[n_maps] = "Cm / Second";
		++n_maps;
	}
	for (i = 0; i < MAX_PROBES; ++i) {
		probe_num = sess->probe_map[i];
		if (probe_num != NOT_CUT) {
			if (sess->opts->rate_maps) {
				maps[n_maps] = sess->probes[probe_num]->show_rate_map;
				map_types[n_maps] = "Rate Map";
				map_names[n_maps] = sess->probes[probe_num]->name_str;
				medians[n_maps] = sess->probes[probe_num]->rate_medns;
				n_medians[n_maps] = sess->probes[probe_num]->num_rate_bkpts;
				key_labels[n_maps] = "Spikes / Second";
				++n_maps;
			}
			if (sess->opts->smoothed_rate_maps) {
				maps[n_maps] = sess->probes[probe_num]->show_smoothed_rate_map;
				map_types[n_maps] = "Smoothed Rate Map";
				map_names[n_maps] = sess->probes[probe_num]->name_str;
				medians[n_maps] = sess->probes[probe_num]->smoothed_rate_medns;
				n_medians[n_maps] = sess->probes[probe_num]->num_smoothed_rate_bkpts;
				key_labels[n_maps] = "Spikes / Second";
				++n_maps;
			}
		}
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
			cell_num = sess->cell_map[i][j];
			if (cell_num != NOT_CUT) {
				if (sess->opts->rate_maps) {
					maps[n_maps] = sess->cells[cell_num]->show_rate_map;
					map_types[n_maps] = "Rate Map";
					map_names[n_maps] = sess->cells[cell_num]->name_str;
					medians[n_maps] = sess->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
				if (sess->opts->smoothed_rate_maps) {
					maps[n_maps] = sess->cells[cell_num]->show_smoothed_rate_map;
					map_types[n_maps] = "Smoothed Rate Map";
					map_names[n_maps] = sess->cells[cell_num]->name_str;
					medians[n_maps] = sess->cells[cell_num]->smoothed_rate_medns;
					n_medians[n_maps] = sess->cells[cell_num]->num_smoothed_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
				if (sess->opts->field_maps) {
					maps[n_maps] = sess->cells[cell_num]->show_field_map;
					map_types[n_maps] = "Field Map";
					map_names[n_maps] = sess->cells[cell_num]->name_str;
					medians[n_maps] = field_medns;
					n_medians[n_maps] = N_BKPTS;
					key_labels[n_maps] = "Field Number";
					++n_maps;
				}
				if (sess->opts->rot_dir_div_rate_maps) {
					maps[n_maps] = sess->rotational_dir_subdivisions[CLOCKWISE]->cells[cell_num]->show_rate_map;
					map_types[n_maps] = "Clockwise Rate Map";
					map_names[n_maps] = sess->rotational_dir_subdivisions[CLOCKWISE]->cells[cell_num]->name_str;
					medians[n_maps] = sess->rotational_dir_subdivisions[CLOCKWISE]->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->rotational_dir_subdivisions[CLOCKWISE]->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
					maps[n_maps] = sess->rotational_dir_subdivisions[COUNTERCLOCKWISE]->cells[cell_num]->show_rate_map;
					map_types[n_maps] = "Counterclockwise Rate Map";
					map_names[n_maps] = sess->rotational_dir_subdivisions[COUNTERCLOCKWISE]->cells[cell_num]->name_str;
					medians[n_maps] = sess->rotational_dir_subdivisions[COUNTERCLOCKWISE]->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->rotational_dir_subdivisions[COUNTERCLOCKWISE]->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
			}
		}
	}	

	/* jerryrig some "medians" for the fields */

	if (sess->opts->field_maps) {
		field_medns[0] = 0.0;
		for (i = 1; i < N_BKPTS; ++i)
			field_medns[i] = (sf4) (N_BKPTS - i);
	}

	/* open postcript file */

	if (sess->opts->output_to_ps_files) {
		ps_dir = get_env_var("PS_MAP_DIR");
		if(sess->opts->bpf_room_file)
                        (void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
                else if (sess->opts->bpf_arena_file)
                        (void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
		else
			(void) sprintf(out_file, "%s/%s%s.ps", ps_dir, sess->FrameStr, sess->name_str);
	} else
		(void) strcpy(out_file, "/usr/tmp/junk1");
	fp = fopen(out_file, "w");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCould not open the file \"%s\". Terminating program.\n\n", 7, out_file);
		exit(1);
	}

	map_pix_sz = sess->params->map_pixel_size;
	page_label = "Maps for ";

	/* start postscript file with this stuff */
	(void) fprintf(fp, "%%!PS\n");
	(void) fprintf(fp, "/center {dup stringwidth pop 2 div neg 0 rmoveto} bind def\n");
	(void) fprintf(fp, "/mp {%f %f rectfill} bind def\n", map_pix_sz, map_pix_sz);
	(void) fprintf(fp, "/lm {moveto 2 pop lineto} bind def\n");
	(void) fprintf(fp, "/kp {setrgbcolor %f %f rectfill} bind def\n", KEY_PIXEL_SIZE_DFLT, KEY_PIXEL_SIZE_DFLT);
	
	n_pages = n_maps / MAPS_PER_PAGE;
	if (n_maps % MAPS_PER_PAGE)
		++n_pages;

	app = sess->apparatus;
	
	/* loop on maps */

	sprintf(name_str,"%s%s", sess->FrameStr, sess->name_str);

	x_sz = sess->params->x_size;
	y_sz = sess->params->y_size;
	page = 1;
	for (i = 0; i < n_maps; ++i) {
		if (!(i % MAPS_PER_PAGE)) {
			map_y = Y_START + Y_DECREMENT; /* Y_INCREMENT WILL BE ADDED BELOW */
			new_map_page(name_str, sess->anal_time, page, n_pages, page_label, fp);
			++page;
		}
		if (!(i % MAPS_PER_ROW)) {
			map_x = X_START;
			map_y -= Y_DECREMENT;
		}
		make_map(maps[i], map_x, map_y, map_types[i], map_names[i], app, colors, map_pix_sz, fp);
		outline_map(map_x, map_y, app, x_sz, y_sz, map_pix_sz, fp);
		make_key(medians[i], n_medians[i], map_x, map_y, key_labels[i], app, colors, map_pix_sz, fp);
		map_x += X_INCREMENT;
	}
		
	/* end file with showpage and EOF character */

	(void) fprintf(fp, "showpage\n%c", 4);
	(void) fflush(fp);
	(void) fclose(fp);

	/* send file to printer */

	if (sess->opts->output_to_printer) {
		if (sess->opts->color_maps)
			printer = get_env_var("COLOR_PRINTER");
		else
			printer = get_env_var("GRAY_PRINTER");
		(void) sprintf(command, "lpr -P %s %s", printer, out_file);
		(void) system(command);
	}

	free((void *) maps);
	free((void *) map_types);
	free((void *) map_names);
	free((void *) medians);
	free((void *) n_medians);
	free((void *) key_labels);


	return;
}


void	new_map_page(sess_name, anal_time, page, n_pages, page_label, fp)
si1		*sess_name, *anal_time, *page_label;
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
		(void) fprintf(fp, "(%sSession %s)", page_label, sess_name);
	else
		(void) fprintf(fp, "(%sSession %s   \\(page %d of %d\\))", page_label, sess_name, page, n_pages); 
	(void) fprintf(fp, " center show\n");

	return;
}


 void	make_map(map, map_x, map_y, map_type, map_name, app, colors, map_pix_sz, fp)
ui1		**map;
sf4		map_x, map_y, map_pix_sz;
si1		*map_type, *map_name;
APPARATUS	*app;
RGBCOL		*colors;
FILE		*fp;
{
	si4		i, j, k, top, bot, lft, rgt;
	sf4		x, y;
	RGBCOL		col;
	

        top = app->top;
        bot = app->bottom;
        lft = app->left;
        rgt = app->right;

	for (i = YELLOW; i <= PURPLE; ++i) {
		y = map_y;
		col = colors[i];
		(void) fprintf(fp, "%f %f %f setrgbcolor\n", col.red, col.grn, col.blu);
		for (j = top; j <= bot; ++j) {
			x = map_x;
			for (k = lft; k <= rgt; ++k) {
				if (map[j][k] == i)
					(void) fprintf(fp, "%0.2f %0.2f mp\n", x, y);
				x += map_pix_sz;
			}
			y -= map_pix_sz;
		}
	}

	col = colors[BLACK];
	(void) fprintf(fp, "%f %f %f setrgbcolor\n", col.red, col.grn, col.blu);

	(void) fprintf(fp, "/Times-Roman 10.0 selectfont\n");

	x = map_x + ((app->center_x - lft) * map_pix_sz);
	y = map_y - (((bot - top) * map_pix_sz) + 2.5 * KEY_PIXEL_SIZE_DFLT);
	(void) fprintf(fp, "%f %f moveto\n(%s) center show\n", x, y, map_type);
	y -= 1.5 * KEY_PIXEL_SIZE_DFLT;
	(void) fprintf(fp, "%f %f moveto\n(%s) center show\n", x, y, map_name);

	return;
}


 void	make_key(medians, n_medians, map_x, map_y, label, app, colors, map_pix_sz, fp)
sf4		*medians, map_x, map_y, map_pix_sz;
si4		n_medians;
si1		*label;
APPARATUS	*app;
RGBCOL		*colors;
FILE		*fp;
{
	si4		i;
	sf4		x, y;
	RGBCOL		col;

	x = map_x + ((app->right - app->left) * map_pix_sz) + KEY_X_OFFSET;
	y = map_y - ((app->center_y - app->top) * map_pix_sz) + (n_medians * KEY_PIXEL_SIZE_DFLT);
	
	col = colors[BLACK];
	(void) fprintf(fp, "%f %f %f setrgbcolor\n", col.red, col.grn, col.blu);
	(void) fprintf(fp, "%f %f moveto\n", x + 2.4 * KEY_PIXEL_SIZE_DFLT, y);
	(void) fprintf(fp, "/Times-Roman 8.0 selectfont\n");
	(void) fprintf(fp, "(%s) center show\n", label);

	for (i = 0; i < n_medians; ++i) {
		y -= 2.0 * KEY_PIXEL_SIZE_DFLT;
		col = colors[i + 1];
		(void) fprintf(fp, "%f %f %f %f %f kp\n", x, y, col.red, col.grn, col.blu);
	}

	y = map_y - ((app->center_y - app->top) * map_pix_sz) + (n_medians * KEY_PIXEL_SIZE_DFLT);
	col = colors[BLACK];
	(void) fprintf(fp, "%f %f %f setrgbcolor\n", col.red, col.grn, col.blu);
	(void) fprintf(fp, "/Times-Roman 9.0 selectfont\n");
	x += 2.0 * KEY_PIXEL_SIZE_DFLT;
	for (i = 0; i < n_medians; ++i) {
		y -= 2.0 * KEY_PIXEL_SIZE_DFLT;
		(void) fprintf(fp, "%f %f moveto\n", x, y);
		(void) fprintf(fp, "(%0.3f) show\n", medians[i]);
	}

	return;
}


 void	outline_map(map_x, map_y, app, x_sz, y_sz, map_pix_sz, fp)
sf4		map_x, map_y, map_pix_sz;
APPARATUS	*app;
FILE		*fp;
si4		x_sz, y_sz;
{
	si4	i, j, top, bot, rgt, lft;
	sf4	x, y;
	ui1	**map;

	map = app->map;
        top = app->top;
        bot = app->bottom;
        lft = app->left;
        rgt = app->right;
	--x_sz;
	--y_sz;

	(void) fprintf(fp, "0.0 0.0 0.0 setrgbcolor\n0.25 setlinewidth\n");

	y = map_y;
	for (i = top; i <= bot; ++i) {
		x = map_x;
		for (j = lft; j <= rgt; ++j, x += map_pix_sz) {
			if (!map[i][j]){
				continue;
			}
			if (i == y_sz)
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x, y, x + map_pix_sz, y); 
			else if (!map[i + 1][j])
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x, y, x + map_pix_sz, y); 
			if (!i)
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x, y + map_pix_sz, x + map_pix_sz, y + map_pix_sz); 
			else if (!map[i - 1][j])
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x, y + map_pix_sz, x + map_pix_sz, y + map_pix_sz); 
			if (j == x_sz)
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x + map_pix_sz, y + map_pix_sz, x + map_pix_sz, y); 
			else if (!map[i][j + 1])
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x + map_pix_sz, y + map_pix_sz, x + map_pix_sz, y); 
			if (!j)
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x, y + map_pix_sz, x, y); 
			else if (!map[i][j - 1])
				(void) fprintf(fp, "%0.2f %0.2f %0.2f %0.2f lm\n", x, y + map_pix_sz, x, y); 
		}
		y -= map_pix_sz;
	}
	(void) fprintf(fp, "stroke\n");

	return;
}


void	ps_print_dir_div_maps(sess)
SESSION	*sess;
{
	si4		i, j, k, n_maps, *n_medians;
	si4		n_pages, page, cell_num, dir_bins, x_sz, y_sz;
	sf4		map_x, map_y, **medians, map_pix_sz;
	si1		command[256], out_file[256], *page_label;
	si1		*ps_dir, **map_types, **key_labels, *printer, **angles;
	extern si1	*get_env_var();
	ui1		***maps, *mapmap;
	FILE		*fp;
	extern void	*gmem();
	RGBCOL		*colors;
	APPARATUS	*app;
	void	make_map(), make_key(), outline_map(), new_map_page();

	static RGBCOL	color_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						1.00, 1.00, 0.40,	/* YELLOW */
						1.00, 0.80, 0.15,	/* ORANGE */
						0.95, 0.25, 0.25,	/* RED */
						0.40, 1.00, 0.40,	/* GREEN */
						0.00, 0.65, 1.00,	/* BLUE */
						0.40, 0.00, 1.00,	/* PURPLE */
						0.80, 0.80, 0.80,	/* GRAY */
						0.00, 0.00, 0.00};	/* BLACK */

	static RGBCOL	gray_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						0.97, 0.97, 0.97,	/* GRAY 1 */
						0.89, 0.89, 0.89,	/* GRAY 2 */
						0.79, 0.79, 0.79,	/* GRAY 3 */
						0.55, 0.55, 0.55,	/* GRAY 4 */
						0.35, 0.35, 0.35,	/* GRAY 5 */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00};	/* BLACK */

	if (sess->opts->color_maps)
		colors = color_pix;
	else
		colors = gray_pix;

	dir_bins = sess->params->map_dir_bins;
	n_maps = sess->num_cells * (dir_bins + 1);

	maps = (ui1 ***) gmem((size_t) n_maps, sizeof(ui1 **));
	mapmap = (ui1 *) gmem((size_t) dir_bins, sizeof(ui1));
	angles = (si1 **) gmem((size_t) dir_bins, sizeof(si1 *));
	for (i = 0; i < dir_bins; ++i)
		angles[i] = (si1 *) gmem((size_t) 16, sizeof(si1));
	map_types = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));
	medians = (sf4 **) gmem((size_t) n_maps, sizeof(sf4 *));
	n_medians = (si4 *) gmem((size_t) n_maps, sizeof(si4));
	key_labels = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));

	for (i = 0; i < dir_bins; ++i)
		(void) sprintf(angles[i], "%d degrees", (si4) ((((sf8) i / (sf8) dir_bins) * 360.0) + 0.5));

	mapmap[0] = 3; /* Clockwise arrangement of maps. */
	mapmap[1] = 2; /* This needs some serious reworking */
	mapmap[2] = 1; /* if the user changes the number of */
	mapmap[3] = 4; /* directional bins from 8. This was */
	mapmap[4] = 0; /* lazy of me - sorry. */
	mapmap[5] = 5;
	mapmap[6] = 6;
	mapmap[7] = 7;

	/* make array of maps to be printed */

	n_maps = 0;
	for (i = 0; i < MAX_PROBES; ++i) {
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
			cell_num = sess->cell_map[i][j];
			if (cell_num != NOT_CUT) {
				for (k = 0; k < dir_bins / 2; ++k) {
					maps[n_maps] = sess->directional_subdivisions[mapmap[k]]->cells[cell_num]->show_rate_map;
					map_types[n_maps] = angles[mapmap[k]];
					medians[n_maps] = sess->directional_subdivisions[mapmap[k]]->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->directional_subdivisions[mapmap[k]]->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
				maps[n_maps] = sess->cells[cell_num]->show_rate_map;
				map_types[n_maps] = sess->cells[cell_num]->name_str; /* not actually a type for this one */
				medians[n_maps] = sess->cells[cell_num]->rate_medns;
				n_medians[n_maps] = sess->cells[cell_num]->num_rate_bkpts;
				key_labels[n_maps] = "Spikes / Second";
				++n_maps;
				for (; k < dir_bins; ++k) {
					maps[n_maps] = sess->directional_subdivisions[mapmap[k]]->cells[cell_num]->show_rate_map;
					map_types[n_maps] = angles[mapmap[k]];
					medians[n_maps] = sess->directional_subdivisions[mapmap[k]]->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->directional_subdivisions[mapmap[k]]->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
			}
		}
	}	

	map_pix_sz = sess->params->map_pixel_size;
	page_label = "Direction Divided Rate Maps for ";

	/* open postcript file */

	if (sess->opts->output_to_ps_files) {
		ps_dir = get_env_var("PS_MAP_DIR");
		if(sess->opts->bpf_room_file)
                        (void) sprintf(out_file, "%s/%s%s.direc.ps", ps_dir, sess->FrameStr, sess->name_str);
                else if (sess->opts->bpf_arena_file)
                        (void) sprintf(out_file, "%s/%s%s.direc.ps", ps_dir, sess->FrameStr, sess->name_str);
                else
			(void) sprintf(out_file, "%s/%s%s.direc.ps", ps_dir, sess->FrameStr, sess->name_str);
	} else
		(void) strcpy(out_file, "/usr/tmp/junk3");
	fp = fopen(out_file, "w");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCould not open the file \"%s\". Terminating program.\n\n", 7, out_file);
		exit(1);
	}

	/* start postscript file with this stuff */
	(void) fprintf(fp, "%%!PS\n");
	(void) fprintf(fp, "/center {dup stringwidth pop 2 div neg 0 rmoveto} bind def\n");
	(void) fprintf(fp, "/mp {%f %f rectfill} bind def\n", map_pix_sz, map_pix_sz);
	(void) fprintf(fp, "/lm {moveto 2 pop lineto} bind def\n");
	(void) fprintf(fp, "/kp {setrgbcolor %f %f rectfill} bind def\n", KEY_PIXEL_SIZE_DFLT, KEY_PIXEL_SIZE_DFLT);
	
	n_pages = n_maps / DIR_MAPS_PER_PAGE;
	if (n_maps % DIR_MAPS_PER_PAGE)
		++n_pages;

	app = sess->apparatus;
	
	/* loop on maps */

	x_sz = sess->params->x_size;
	y_sz = sess->params->y_size;
	page = 1;
	for (i = 0; i < n_maps; ++i) {
		if (!(i % DIR_MAPS_PER_PAGE)) {
			map_y = DIR_Y_START + DIR_Y_DECREMENT; /* Y_INCREMENT WILL BE ADDED BELOW */
			new_map_page(sess->name_str, sess->anal_time, page, n_pages, page_label, fp);
			++page;
		}
		if (!(i % DIR_MAPS_PER_ROW)) {
			map_x = DIR_X_START;
			map_y -= DIR_Y_DECREMENT;
		}
		make_map(maps[i], map_x, map_y, map_types[i], "", app, colors, map_pix_sz, fp);
		outline_map(map_x, map_y, app, x_sz, y_sz, map_pix_sz, fp);
		make_key(medians[i], n_medians[i], map_x, map_y, key_labels[i], app, colors, map_pix_sz, fp);
		map_x += DIR_X_INCREMENT;
	}
		
	/* end file with showpage and EOF character */

	(void) fprintf(fp, "showpage\n%c", 4);
	(void) fflush(fp);
	(void) fclose(fp);

	/* send file to printer */

	if (sess->opts->output_to_printer) {
		if (sess->opts->color_maps)
			printer = get_env_var("COLOR_PRINTER");
		else
			printer = get_env_var("GRAY_PRINTER");
		(void) sprintf(command, "lpr -P %s %s", printer, out_file);
		(void) system(command);
	}

	free((void *) maps);
	free((void *) mapmap);
	for (i = 0; i < dir_bins; ++i)
		free((void *) angles[i]);
	free((void *) angles);
	free((void *) map_types);
	free((void *) medians);
	free((void *) n_medians);
	free((void *) key_labels);


	return;
}


void	ps_print_phase_div_maps(sess)
SESSION	*sess;
{
	si4		i, j, k, n_maps, *n_medians;
	si4		n_pages, page, cell_num, ph_bins, x_sz, y_sz;
	sf4		map_x, map_y, **medians, map_pix_sz;
	si1		command[256], out_file[256], *page_label;
	si1		*ps_dir, **map_types, **key_labels, *printer, **angles;
	extern si1	*get_env_var();
	ui1		***maps, *mapmap;
	FILE		*fp;
	extern void	*gmem();
	RGBCOL		*colors;
	APPARATUS	*app;
	void	make_map(), make_key(), outline_map(), new_map_page();

	static RGBCOL	color_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						1.00, 1.00, 0.40,	/* YELLOW */
						1.00, 0.80, 0.15,	/* ORANGE */
						0.95, 0.25, 0.25,	/* RED */
						0.40, 1.00, 0.40,	/* GREEN */
						0.00, 0.65, 1.00,	/* BLUE */
						0.40, 0.00, 1.00,	/* PURPLE */
						0.80, 0.80, 0.80,	/* GRAY */
						0.00, 0.00, 0.00};	/* BLACK */

	static RGBCOL	gray_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						0.97, 0.97, 0.97,	/* GRAY 1 */
						0.89, 0.89, 0.89,	/* GRAY 2 */
						0.79, 0.79, 0.79,	/* GRAY 3 */
						0.55, 0.55, 0.55,	/* GRAY 4 */
						0.35, 0.35, 0.35,	/* GRAY 5 */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00};	/* BLACK */

	if (sess->opts->color_maps)
		colors = color_pix;
	else
		colors = gray_pix;

	ph_bins = sess->params->map_phase_bins;
	n_maps = sess->num_cells * (ph_bins + 1);

	maps = (ui1 ***) gmem((size_t) n_maps, sizeof(ui1 **));
	mapmap = (ui1 *) gmem((size_t) ph_bins, sizeof(ui1));
	angles = (si1 **) gmem((size_t) ph_bins, sizeof(si1 *));
	for (i = 0; i < ph_bins; ++i)
		angles[i] = (si1 *) gmem((size_t) 16, sizeof(si1));
	map_types = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));
	medians = (sf4 **) gmem((size_t) n_maps, sizeof(sf4 *));
	n_medians = (si4 *) gmem((size_t) n_maps, sizeof(si4));
	key_labels = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));

	for (i = 0; i < ph_bins; ++i)
		(void) sprintf(angles[i], "%d degrees", (si4) ((((sf8) i / (sf8) ph_bins) * 360.0) + 0.5));

	mapmap[0] = 3; /* Clockwise arrangement of maps. */
	mapmap[1] = 2; /* This needs some serious reworking */
	mapmap[2] = 1; /* if the user changes the number of */
	mapmap[3] = 4; /* phase bins from 8. This was */
	mapmap[4] = 0; /* lazy of me - sorry. */
	mapmap[5] = 5;
	mapmap[6] = 6;
	mapmap[7] = 7;

	/* make array of maps to be printed */

	n_maps = 0;
	for (i = 0; i < MAX_PROBES; ++i) {
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
			cell_num = sess->cell_map[i][j];
			if (cell_num != NOT_CUT) {
				for (k = 0; k < ph_bins / 2; ++k) {
					maps[n_maps] = sess->phase_subdivisions[mapmap[k]]->cells[cell_num]->show_rate_map;
					map_types[n_maps] = angles[mapmap[k]];
					medians[n_maps] = sess->phase_subdivisions[mapmap[k]]->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->phase_subdivisions[mapmap[k]]->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
				maps[n_maps] = sess->cells[cell_num]->show_rate_map;
				map_types[n_maps] = sess->cells[cell_num]->name_str; /* not actually a type for this one */
				medians[n_maps] = sess->cells[cell_num]->rate_medns;
				n_medians[n_maps] = sess->cells[cell_num]->num_rate_bkpts;
				key_labels[n_maps] = "Spikes / Second";
				++n_maps;
				for (; k < ph_bins; ++k) {
					maps[n_maps] = sess->phase_subdivisions[mapmap[k]]->cells[cell_num]->show_rate_map;
					map_types[n_maps] = angles[mapmap[k]];
					medians[n_maps] = sess->phase_subdivisions[mapmap[k]]->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->phase_subdivisions[mapmap[k]]->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
			}
		}
	}	

	map_pix_sz = sess->params->map_pixel_size;
	page_label = "Theta Phase Divided Rate Maps for ";

	/* open postcript file */

	if (sess->opts->output_to_ps_files) {
		ps_dir = get_env_var("PS_MAP_DIR");
		if(sess->opts->bpf_room_file)
                        (void) sprintf(out_file, "%s/%s%s.phase.ps", ps_dir, sess->FrameStr, sess->name_str);
                else if (sess->opts->bpf_arena_file)
                        (void) sprintf(out_file, "%s/%s%s.phase.ps", ps_dir, sess->FrameStr, sess->name_str);
                else
			(void) sprintf(out_file, "%s/%s%s.phase.phase.ps", ps_dir, sess->FrameStr, sess->name_str);
	} else
		(void) strcpy(out_file, "/usr/tmp/junk3");
	fp = fopen(out_file, "w");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCould not open the file \"%s\". Terminating program.\n\n", 7, out_file);
		exit(1);
	}

	/* start postscript file with this stuff */
	(void) fprintf(fp, "%%!PS\n");
	(void) fprintf(fp, "/center {dup stringwidth pop 2 div neg 0 rmoveto} bind def\n");
	(void) fprintf(fp, "/mp {%f %f rectfill} bind def\n", map_pix_sz, map_pix_sz);
	(void) fprintf(fp, "/lm {moveto 2 pop lineto} bind def\n");
	(void) fprintf(fp, "/kp {setrgbcolor %f %f rectfill} bind def\n", KEY_PIXEL_SIZE_DFLT, KEY_PIXEL_SIZE_DFLT);
	
	n_pages = n_maps / DIR_MAPS_PER_PAGE;
	if (n_maps % DIR_MAPS_PER_PAGE)
		++n_pages;

	app = sess->apparatus;
	
	/* loop on maps */

	x_sz = sess->params->x_size;
	y_sz = sess->params->y_size;
	page = 1;
	for (i = 0; i < n_maps; ++i) {
		if (!(i % DIR_MAPS_PER_PAGE)) {
			map_y = DIR_Y_START + DIR_Y_DECREMENT; /* Y_INCREMENT WILL BE ADDED BELOW */
			new_map_page(sess->name_str, sess->anal_time, page, n_pages, page_label, fp);
			++page;
		}
		if (!(i % DIR_MAPS_PER_ROW)) {
			map_x = DIR_X_START;
			map_y -= DIR_Y_DECREMENT;
		}
		make_map(maps[i], map_x, map_y, map_types[i], "", app, colors, map_pix_sz, fp);
		outline_map(map_x, map_y, app, x_sz, y_sz, map_pix_sz, fp);
		make_key(medians[i], n_medians[i], map_x, map_y, key_labels[i], app, colors, map_pix_sz, fp);
		map_x += DIR_X_INCREMENT;
	}
		
	/* end file with showpage and EOF character */

	(void) fprintf(fp, "showpage\n%c", 4);
	(void) fflush(fp);
	(void) fclose(fp);

	/* send file to printer */

	if (sess->opts->output_to_printer) {
		if (sess->opts->color_maps)
			printer = get_env_var("COLOR_PRINTER");
		else
			printer = get_env_var("GRAY_PRINTER");
		(void) sprintf(command, "lpr -P %s %s", printer, out_file);
		(void) system(command);
	}

	free((void *) maps);
	free((void *) mapmap);
	for (i = 0; i < ph_bins; ++i)
		free((void *) angles[i]);
	free((void *) angles);
	free((void *) map_types);
	free((void *) medians);
	free((void *) n_medians);
	free((void *) key_labels);


	return;
}


void	ps_print_time_div_maps(sess)
SESSION	*sess;
{
	si4		i, j, k, n_maps, *n_medians;
	si4		n_pages, page, cell_num, time_bins, x_sz, y_sz;
	sf4		map_x, map_y, **medians, map_pix_sz;
	sf4		start_min, end_min, total_mins;
	si1		command[256], out_file[256], *page_label;
	si1		*ps_dir, **map_types, **map_names, **key_labels, *printer, **times;
	extern si1	*get_env_var();
	ui1		***maps;
	FILE		*fp;
	extern void	*gmem();
	RGBCOL		*colors;
	APPARATUS	*app;
	 void	make_map(), make_key(), outline_map(), new_map_page();

	static RGBCOL	color_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						1.00, 1.00, 0.40,	/* YELLOW */
						1.00, 0.80, 0.15,	/* ORANGE */
						0.95, 0.25, 0.25,	/* RED */
						0.40, 1.00, 0.40,	/* GREEN */
						0.00, 0.65, 1.00,	/* BLUE */
						0.40, 0.00, 1.00,	/* PURPLE */
						0.80, 0.80, 0.80,	/* GRAY */
						0.00, 0.00, 0.00};	/* BLACK */

	static RGBCOL	gray_pix[N_COLORS] = {	1.00, 1.00, 1.00, 	/* WHITE */
						0.97, 0.97, 0.97,	/* GRAY 1 */
						0.89, 0.89, 0.89,	/* GRAY 2 */
						0.79, 0.79, 0.79,	/* GRAY 3 */
						0.55, 0.55, 0.55,	/* GRAY 4 */
						0.35, 0.35, 0.35,	/* GRAY 5 */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00,	/* BLACK */
						0.00, 0.00, 0.00};	/* BLACK */

	if (sess->opts->color_maps)
		colors = color_pix;
	else
		colors = gray_pix;

	time_bins = sess->params->time_bins;
	n_maps = sess->num_cells * (time_bins + 1);

	maps = (ui1 ***) gmem((size_t) n_maps, sizeof(ui1 **));
	times = (si1 **) gmem((size_t) time_bins, sizeof(si1 *));
	for (i = 0; i < time_bins; ++i)
		times[i] = (si1 *) gmem((size_t) 32, sizeof(si1));
	map_types = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));
	map_names = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));
	medians = (sf4 **) gmem((size_t) n_maps, sizeof(sf4 *));
	n_medians = (si4 *) gmem((size_t) n_maps, sizeof(si4));
	key_labels = (si1 **) gmem((size_t) n_maps, sizeof(si1 *));

	total_mins = (sf4) (sess->time->total_secs / 60.0);
	for (i = 0; i < time_bins; ++i) {
		start_min = i * sess->params->time_division_duration;
		end_min = (i + 1) * sess->params->time_division_duration;
		if (end_min > total_mins)
			end_min = total_mins;
		(void) sprintf(times[i], "Minutes %0.1f to %0.1f", start_min, end_min);
	}

	/* make array of maps to be printed */

	n_maps = 0;
	for (i = 0; i < MAX_PROBES; ++i) {
		for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
			cell_num = sess->cell_map[i][j];
			if (cell_num != NOT_CUT) {
				for (k = 0; k < time_bins; ++k) {
					maps[n_maps] = sess->temporal_subdivisions[k]->cells[cell_num]->show_rate_map;
					map_types[n_maps] = times[k];
					map_names[n_maps] = sess->cells[cell_num]->name_str;
					medians[n_maps] = sess->temporal_subdivisions[k]->cells[cell_num]->rate_medns;
					n_medians[n_maps] = sess->temporal_subdivisions[k]->cells[cell_num]->num_rate_bkpts;
					key_labels[n_maps] = "Spikes / Second";
					++n_maps;
				}
				maps[n_maps] = sess->cells[cell_num]->show_rate_map;
				map_types[n_maps] = "Entire Session";
				map_names[n_maps] = sess->cells[cell_num]->name_str;
				medians[n_maps] = sess->cells[cell_num]->rate_medns;
				n_medians[n_maps] = sess->cells[cell_num]->num_rate_bkpts;
				key_labels[n_maps] = "Spikes / Second";
				++n_maps;
			}
		}
	}	

	map_pix_sz = sess->params->map_pixel_size;
	page_label = "Time Divided Rate Maps for ";

	/* open postcript file */

	if (sess->opts->output_to_ps_files) {
		ps_dir = get_env_var("PS_MAP_DIR");
		if(sess->opts->bpf_room_file)
                        (void) sprintf(out_file, "%s/%s%s.time_div.ps", ps_dir, sess->FrameStr, sess->name_str);
                else if (sess->opts->bpf_arena_file)
                        (void) sprintf(out_file, "%s/%s%s.time_div.ps", ps_dir, sess->FrameStr, sess->name_str);
                else
			(void) sprintf(out_file, "%s/%s%s.time_div.ps", ps_dir, sess->FrameStr, sess->name_str);
	} else
		(void) strcpy(out_file, "/usr/tmp/junk5");
	fp = fopen(out_file, "w");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCould not open the file \"%s\". Terminating program.\n\n", 7, out_file);
		exit(1);
	}

	/* start postscript file with this stuff */
	(void) fprintf(fp, "%%!PS\n");
	(void) fprintf(fp, "/center {dup stringwidth pop 2 div neg 0 rmoveto} bind def\n");
	(void) fprintf(fp, "/mp {%f %f rectfill} bind def\n", map_pix_sz, map_pix_sz);
	(void) fprintf(fp, "/lm {moveto 2 pop lineto} bind def\n");
	(void) fprintf(fp, "/kp {setrgbcolor %f %f rectfill} bind def\n", KEY_PIXEL_SIZE_DFLT, KEY_PIXEL_SIZE_DFLT);
	
	n_pages = n_maps / MAPS_PER_PAGE;
	if (n_maps % MAPS_PER_PAGE)
		++n_pages;

	app = sess->apparatus;
	
	/* loop on maps */

	x_sz = sess->params->x_size;
	y_sz = sess->params->y_size;
	page = 1;
	for (i = 0; i < n_maps; ++i) {
		if (!(i % MAPS_PER_PAGE)) {
			map_y = Y_START + Y_DECREMENT; /* Y_INCREMENT WILL BE ADDED BELOW */
			new_map_page(sess->name_str, sess->anal_time, page, n_pages, page_label, fp);
			++page;
		}
		if (!(i % MAPS_PER_ROW)) {
			map_x = X_START;
			map_y -= Y_DECREMENT;
		}
		make_map(maps[i], map_x, map_y, map_names[i], map_types[i], app, colors, map_pix_sz, fp);
		outline_map(map_x, map_y, app, x_sz, y_sz, map_pix_sz, fp);
		make_key(medians[i], n_medians[i], map_x, map_y, key_labels[i], app, colors, map_pix_sz, fp);
		map_x += X_INCREMENT;
	}
		
	/* end file with showpage and EOF character */

	(void) fprintf(fp, "showpage\n%c", 4);
	(void) fflush(fp);
	(void) fclose(fp);

	/* send file to printer */

	if (sess->opts->output_to_printer) {
		if (sess->opts->color_maps)
			printer = get_env_var("COLOR_PRINTER");
		else
			printer = get_env_var("GRAY_PRINTER");
		(void) sprintf(command, "lpr -P %s %s", printer, out_file);
		(void) system(command);
	}

	free((void *) maps);
	for (i = 0; i < time_bins; ++i)
		free((void *) times[i]);
	free((void *) times);
	free((void *) map_types);
	free((void *) map_names);
	free((void *) medians);
	free((void *) n_medians);
	free((void *) key_labels);


	return;
}

