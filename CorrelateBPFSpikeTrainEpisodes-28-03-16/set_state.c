
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"
#include "theta.h"


void	set_state(sess, n_samps, wind, corrs, state)
SESSION	*sess;
ui4	n_samps, wind;
ui1	*corrs;
ui1	*state;
{
	ui4	i, quarter_wind;
	ui1	mode;
	ui1	theta_cutoff, maybe_cutoff;

	theta_cutoff = (ui1) sess->params->min_theta_score;
	maybe_cutoff = (ui1) sess->params->max_non_theta_score;;
	for (i = 0; i < n_samps; ++i) {
		if (corrs[i] >= theta_cutoff)
			state[i] = THETA;
		else if (corrs[i] >= maybe_cutoff)
			state[i] = MAYBE_THETA;
		else
			state[i] = NON_THETA;
	}	


	quarter_wind = wind >> 2;

	n_samps -= (quarter_wind + 1);
	mode = state[0];
	for (i = 0; i < n_samps; ++i) {
		if (state[i] == THETA && mode != THETA) {
			(void) memset((void *) (state + i - quarter_wind), THETA, (size_t) quarter_wind);
			mode = THETA;
		}
		if (state[i] != THETA && mode == THETA) {
			(void) memset((void *) (state + i), THETA, (size_t) quarter_wind);
			i += quarter_wind;
			mode = state[i];
		}
	}

	return;
}
