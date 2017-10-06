
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "speed.h"


void	filter_recs_for_walking(sess)
SESSION	*sess;
{
	si4		i, j, n_recs;
	RECORD		*recs;
	ui1		min_walking_speed;

	n_recs = sess->num_recs;
	recs = sess->recs;
	min_walking_speed = sess->params->min_walking_speed;

	for (i = 0; i < n_recs; ++i)
		if (recs[i].running_speed < min_walking_speed)
			break;

	for (j = i + 1; j < n_recs; ++j)
		if (recs[j].running_speed >= min_walking_speed)
			break;

	while (j < n_recs) {
		recs[i++] = recs[j++];
		for (; j < n_recs; ++j)
			if (recs[j].running_speed >= min_walking_speed)
				break;
	}

	sess->num_recs = i;
	sess->recs = (RECORD *) realloc((void *) recs, (size_t) i * sizeof(RECORD));

	return;
}

