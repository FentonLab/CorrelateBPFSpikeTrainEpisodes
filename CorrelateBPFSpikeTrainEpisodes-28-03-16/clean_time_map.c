
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"


void	clean_time_map(sess)
SESSION	*sess;
{
	si4	i, area;
	TIME	*tp;
	sf4	*tm, thresh;
	ui1	*am;

        area = sess->params->area;

	tp = sess->time;
	tm = tp->map[0];
	am = sess->apparatus->map[0];
	thresh = (sf4) sess->time->threshold;

	for (i = 0; i < area; ++i) {
		if (am[i]) {
			tp->total_in_apparatus_secs += (sf8) tm[i];
			if (tm[i] < thresh)
				tm[i] = 0.0;
		} else
			tm[i] = 0.0;
	}

	tp = sess->secondary_time;
	tm = tp->map[0];
	am = sess->secondary_apparatus->map[0];
	thresh = (sf4) sess->time->threshold;

	for (i = 0; i < area; ++i) {
		if (am[i]) {
			tp->total_in_apparatus_secs += (sf8) tm[i];
			if (tm[i] < thresh)
				tm[i] = 0.0;
		} else
			tm[i] = 0.0;
	}

	return;
}
