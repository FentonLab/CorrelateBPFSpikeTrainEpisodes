
#include <stdlib.h> 
#include <stdio.h> 
#include "size_types.h"
#include "sess_anal.h"


void	get_app_map(sess)
SESSION	*sess;
{
	si4		i, j, stack_sz, *i_stack, *j_stack, size;
	si4		top, bottom, left, right, x_size, y_size, area;
	sf4		**tm, **tm2;
	ui1		**am, **am2;
	APPARATUS	*app, *app2;
	extern void	*gmem(), fill_app_holes(), read_app_map(), write_app_map();

        x_size = sess->params->x_size;
        y_size = sess->params->y_size;
        area = sess->params->area;

	tm = sess->time->map;
	tm2 = sess->secondary_time->map;
	app = sess->apparatus;
	app2 = sess->secondary_apparatus;
	am = app->map;
	am2 = app2->map;
	size = 0;

	if (sess->opts->read_app_map) {
		read_app_map(sess);
	}else{

		for (i = 0; i < y_size; ++i) {
			for (j = 0; j < x_size; ++j) {
				if(tm[i][j]){
					am[i][j] = TRUE;
					++size;
				}else{
					am[i][j] = FALSE;
				}
				if(tm2[i][j]){
					am2[i][j] = TRUE;
					++size;
				}else{
					am2[i][j] = FALSE;
				}
			}
		}
	}

        for (i = 0; i < y_size; i++)
                for (j = 0; j < x_size; j++)
                        if (am[i][j])
                                goto found_top;

found_top:
        ;

        app->top = i;

        for (i = y_size - 1; i >= 0; i--)
                for (j = 0; j < x_size; j++)
                        if (am[i][j])
                                goto found_bot;

found_bot:
        ;

        app->bottom = i;

        for (j = 0; j < x_size; j++)
                for (i = 0; i < y_size; i++)
                        if (am[i][j])
                                goto found_lft;

found_lft:
        ;

        app->left = j;

        for (j = x_size - 1; j >= 0; j--)
                for (i = 0; i < y_size; i++)
                        if (am[i][j])
                                goto found_rgt;

found_rgt:
        ;

        app->right = j;
	app->total_pixels = size;
	app->center_x = (app->right + app->left) / 2;
	app->center_y = (app->bottom + app->top) / 2;

        for (i = 0; i < y_size; i++)
                for (j = 0; j < x_size; j++)
                        if (am2[i][j])
                                goto found_top2;

found_top2:
        ;

        app2->top = i;

        for (i = y_size - 1; i >= 0; i--)
                for (j = 0; j < x_size; j++)
                        if (am2[i][j])
                                goto found_bot2;

found_bot2:
        ;

        app2->bottom = i;

        for (j = 0; j < x_size; j++)
                for (i = 0; i < y_size; i++)
                        if (am2[i][j])
                                goto found_lft2;

found_lft2:
        ;

        app2->left = j;

        for (j = x_size - 1; j >= 0; j--)
                for (i = 0; i < y_size; i++)
                        if (am2[i][j])
                                goto found_rgt2;

found_rgt2:
        ;

        app2->right = j;
	app2->total_pixels = size;
	app2->center_x = (app2->right + app2->left) / 2;

	if (sess->opts->write_app_map)
		write_app_map(sess);

	return;
}
