
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"


void	collapse_pixels(sess)	
SESSION	*sess;
{
	si4	i, n_recs, bits;
	ui4	int_pcf;
	sf8	pcf;
	RECORD	*rec;

	rec = sess->recs;
	n_recs = sess->num_recs;

	pcf = sess->params->pixel_collapse_factor;
	if (pcf < 1.0) {
		(void) fprintf(stderr, "%c\n\tBad pixel collapse factor. Using default %0.2f.\n\n", 7, PIXEL_COLLAPSE_FACTOR_DFLT);
		pcf = sess->params->pixel_collapse_factor = PIXEL_COLLAPSE_FACTOR_DFLT;
	}
	int_pcf = 1;
	
	for (bits = 0; (sf8) int_pcf < pcf; ++bits)
		int_pcf <<= 1;
	if ((sf8) int_pcf == pcf)
		goto bitwise_collapse;

	for (i = 0; i < n_recs; ++i) {
		rec->red_x = (ui1) (((sf8) rec->red_x / pcf) + 0.5);
		rec->red_y = (ui1) (((sf8) rec->red_y / pcf) + 0.5);
		rec->green_x = (ui1) (((sf8) rec->green_x / pcf) + 0.5);
		rec->green_y = (ui1) (((sf8) rec->green_y / pcf) + 0.5);
		rec->RoomX = (ui1) (((sf8) rec->RoomX / pcf) + 0.5);
		rec->RoomY = (ui1) (((sf8) rec->RoomY / pcf) + 0.5);
		rec->ArenaX = (ui1) (((sf8) rec->ArenaX / pcf) + 0.5);
		rec->ArenaY = (ui1) (((sf8) rec->ArenaY / pcf) + 0.5);
		++rec;
	}

	return;	

bitwise_collapse:

	for (i = 0; i < n_recs; ++i) {
		rec->red_x >>= bits;
		rec->red_y >>= bits;
		rec->green_x >>= bits;
		rec->green_y >>= bits;
		rec->RoomX >>= bits;
		rec->RoomY >>= bits;
		rec->ArenaX >>= bits;
		rec->ArenaY >>= bits;
		++rec;
	}

	return;	
}
