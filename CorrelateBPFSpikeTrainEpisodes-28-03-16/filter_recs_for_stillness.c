
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "speed.h"


void	filter_recs_for_stillness(sess)
SESSION	*sess;
{
	si4		i, j, n_recs;
	RECORD		*recs;
	ui1		max_still_speed;


	n_recs = sess->num_recs;
	recs = sess->recs;
	max_still_speed = sess->params->max_still_speed;

	for (i = 0; i < n_recs; ++i)
		if (recs[i].running_speed > max_still_speed)
			break;

	for (j = i + 1; j < n_recs; ++j)
		if (recs[j].running_speed <= max_still_speed)
			break;

	while (j < n_recs) {
		recs[i++] = recs[j++];
		for (; j < n_recs; ++j)
			if (recs[j].running_speed <= max_still_speed)
				break;
	}

	sess->num_recs = i;
	sess->recs = (RECORD *) realloc((void *) recs, (size_t) i * sizeof(RECORD));

	return;
}

