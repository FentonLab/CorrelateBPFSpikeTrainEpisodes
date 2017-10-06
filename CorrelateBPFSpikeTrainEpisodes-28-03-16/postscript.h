
#ifndef POSTSCRIPT_IN
#define POSTSCRIPT_IN

//define A4_PAPER
#define LETTER_PAPER

#include "size_types.h"


typedef struct {
		sf4	red, grn, blu;
	} RGBCOL;


#define MAX_POINT_SETS	10
#define MAX_VALS	10

typedef struct {
		sf4	*xs[MAX_POINT_SETS], *ys[MAX_POINT_SETS], *errs[MAX_POINT_SETS];
		sf4	x_min[MAX_POINT_SETS], x_max[MAX_POINT_SETS], y_min[MAX_POINT_SETS], y_max[MAX_POINT_SETS], vals[MAX_VALS];
		si4	num_points[MAX_POINT_SETS];
		si1	name[64], *type, *x_units, *y_units;
		si1	*val_names[MAX_VALS], *point_set_labels[MAX_POINT_SETS];
		ui1	draw_errs[MAX_POINT_SETS], draw_lines[MAX_POINT_SETS];
		ui1	draw_points[MAX_POINT_SETS], draw_bars[MAX_POINT_SETS];
	} PLOT;


/* page size constants */
#ifdef A4_PAPER
	/* these were derived from A4 paper on an HP Laserjet 4 */
	#define PS_X_DIM                595.0   /* ~8.3 inches 72 dpi */
	#define PS_Y_DIM                842.0   /* ~11.7 inches at 72 dpi */
	#define PS_X_MIN                24.0    /* approximate printable area coordinate */
	#define PS_Y_MIN                55.0 /* 23.0     approximate printable area coordinate */
	#define PS_X_MAX		(PS_X_DIM - PS_X_MIN) /* approximate printable area coordinate */
	#define PS_Y_MAX		(PS_Y_DIM - PS_Y_MIN) /* approximate printable area coordinate */
	// #define PS_X_MAX                571.0   /* approximate printable area coordinate */
	// #define PS_Y_MAX                818.0   /* approximate printable area coordinate */
	#define PS_X_CENT               (PS_X_MIN + (PS_X_MAX - PS_X_MIN) / 2)
	#define PS_Y_CENT               (PS_Y_MIN + (PS_Y_MAX - PS_Y_MIN) / 2)
#endif

#ifdef LETTER_PAPER
	/* these were derived from letter (8.5 X 11 inches) paper on an HP Laserjet 4 */
	#define PS_X_DIM		595.0	/* ~8.3 inches 72 dpi */
	#define PS_Y_DIM		790 	/* ~11.0 inches at 72 dpi */
	#define PS_X_MIN		24.0	/* approximate printable area coordinate */
	#define PS_Y_MIN		55.0 /* 23.0	 approximate printable area coordinate */
	#define PS_X_MAX		(PS_X_DIM - PS_X_MIN) /* approximate printable area coordinate */
	#define PS_Y_MAX		(PS_Y_DIM - PS_Y_MIN) /* approximate printable area coordinate */
	#define	PS_X_CENT		(PS_X_MIN + (PS_X_MAX - PS_X_MIN) / 2)
	#define	PS_Y_CENT		(PS_Y_MIN + (PS_Y_MAX - PS_Y_MIN) / 2)
#endif

/* for regular map printouts */
#define MAPS_PER_ROW            4
#define MAPS_PER_COLUMN         3
#define MAPS_PER_PAGE		(MAPS_PER_ROW * MAPS_PER_COLUMN)
#define X_START                 20.0
#define Y_START			525.0
#define X_INCREMENT             195.0 // 210.0
#define Y_DECREMENT             185.0
#define KEY_X_OFFSET            15.0 // 20.0
#define N_COLORS                9
#define MAP_PIXEL_SIZE_DFLT	3.5
#define KEY_PIXEL_SIZE_DFLT	7.0

#define NO_POS_CELLS_PER_PAGE	6
#define CELLS_PER_PAGE		2
#define PAGE_Y_START		760.0
#define LINE_Y_DEC		11.0
#define LINE_SMALL_Y_DEC	8.0
#define COL_1			45.0
#define COL_2			195.0
#define COL_3			325.0
#define COL_4			455.0

/* for directional and phase map printouts */
#define DIR_MAPS_PER_ROW        3
#define DIR_MAPS_PER_COLUMN     3
#define DIR_MAPS_PER_PAGE	(DIR_MAPS_PER_ROW * DIR_MAPS_PER_COLUMN)
#define DIR_X_START             130.0
#define DIR_Y_START		525.0
#define DIR_X_INCREMENT         210.0
#define DIR_Y_DECREMENT         185.0

/* for plots */
#define PLOT_X_START		50.0
#define PLOT_Y_START		510.0
#define PLOT_X_INC		190.0 // 205.0
#define PLOT_Y_DEC		180.0
#define PLOT_X_SIZE		145.0
#define PLOT_Y_SIZE		105.0
#define PLOT_X_TICKS		8
#define PLOT_Y_TICKS		6
#define PLOT_TICK_SIZE		3.0
#define PLOT_ZERO_TICK_OFFSET	5.0
#define PLOT_ERR_TICK_SIZE	1.2
#define X_LABEL_Y_OFFSET	7.0
#define Y_LABEL_X_OFFSET	3.0
#define Y_LABEL_Y_OFFSET	2.0
#define Y_UNITS_Y_OFFSET	25.0
#define X_UNITS_Y_OFFSET	25.0
#define PLOT_TYPE_OFFSET	-20.0
#define PLOT_NAME_OFFSET	-10.0
#define PLOT_VAL_OFFSET		-3.0
#define PLOT_POINT_SIZE		2.0


#endif
