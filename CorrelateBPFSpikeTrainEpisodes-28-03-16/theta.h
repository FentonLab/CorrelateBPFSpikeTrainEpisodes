
#ifndef THETA_IN
#define THETA_IN


#include "size_types.h"


#define MIN_THETA_SCORE_DFLT		40		/* integer between 0 and 100 */
#define MAX_NON_THETA_SCORE_DFLT	25		/* integer between -100 and 100 */

#define MIN_BOT_THETA_FILT		5.0		/* float, in Hertz */
#define MIN_TOP_THETA_FILT		6.0		/* float, in Hertz */
#define MAX_TOP_THETA_FILT		11.0		/* float, in Hertz */
#define MAX_BOT_THETA_FILT		13.0		/* float, in Hertz */

#define CORR_WIND_SECS		0.50		/* float, in seconds */

#define	THETA		100		/* for threshold */
#define MAYBE_THETA	50		/* for threshold */
#define	NON_THETA	0		/* for threshold */

#define MN	1	/* used in find_crits(), get_phase(), and norm_filt() */
#define MD	2	/* used in find_crits(), get_phase(), and norm_filt() */
#define MX	3	/* used in find_crits(), get_phase(), and norm_filt() */


typedef struct {
		ui4	n_samps, *times, window;
		si2	*samps, *phases;
		sf8	*norm_filt, *filt_samps, samp_freq;
		ui1	probe, *state, *corrs;
} EEG_DATA;


#endif
