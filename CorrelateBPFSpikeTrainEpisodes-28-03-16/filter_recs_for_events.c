
#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void	filter_recs_for_events(sess)
SESSION	*sess;
{
	si4		i, j, n_recs;
	ui1		bf, ef;
	RECORD		*recs;


	n_recs = sess->num_recs;
	recs = sess->recs;
	bf = sess->params->event_begin_flag;
	ef = sess->params->event_end_flag;

	for (i = 0; i < n_recs; ++i) {
		if (recs[i].type == EVENT_FLAG_REC) {
			if (recs[i].probe == bf) {
				++i;
				break;
			}
		}
	}

	for (j = 0; i < n_recs; ++i) {
		if (recs[i].type == EVENT_FLAG_REC) {
			if (recs[i].probe == ef) {
				for (++i; i < n_recs; ++i) {
					if (recs[i].type == EVENT_FLAG_REC) {
						if (recs[i].probe == bf) {
							break;
						}
					}
				}
			}
		}
		else
			recs[j++] = recs[i];
	}

	if (!j) {
		(void) fprintf(stderr, "%c\n\tNo records occurred between event flags in this session. Terminating Program.\n\n", 7);
		exit(1);
	}

	sess->num_recs = j;
	sess->recs = (RECORD *) realloc((void *) recs, (size_t) j * sizeof(RECORD));

	return;
}

