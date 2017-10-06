
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "theta.h"


void	filter_recs_for_non_theta(sess)
SESSION	*sess;
{
	si4		i, j, n_recs;
	RECORD		*recs;


	n_recs = sess->num_recs;
	recs = sess->recs;

	for (i = 0; i < n_recs; ++i)
		if (recs[i].theta_state != NON_THETA)
			break;

	for (j = i + 1; j < n_recs; ++j)
		if (recs[j].theta_state == NON_THETA)
			break;

	while (j < n_recs) {
		recs[i++] = recs[j++];
		for (; j < n_recs; ++j)
			if (recs[j].theta_state == NON_THETA)
				break;
	}

	sess->num_recs = i;
	sess->recs = (RECORD *) realloc((void *) recs, (size_t) i * sizeof(RECORD));

	return;
}

