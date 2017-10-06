
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"
#include "theta.h"
#include "filter.h"


void	calculate_theta(sess)
SESSION	*sess;
{
	si4		i;
	ui4		temp_wind;
	sf8		*sf8_ptr;
	si2		*si2_ptr;
	EEG_DATA	eeg_data;
	FILT_DESC	filt_desc;
	void	set_filt_desc();
	extern void	get_eeg(), filter(), get_phase(), fill_theta_fields();
	extern void	median_smooth(), calc_corrs(), *gmem();
	extern void	set_state(), find_crits(), norm_filt();
	

	get_eeg(sess, &eeg_data);

	eeg_data.filt_samps = (sf8 *) gmem((size_t) eeg_data.n_samps, sizeof(sf8));
	eeg_data.norm_filt = (sf8 *) gmem((size_t) eeg_data.n_samps, sizeof(sf8));
	eeg_data.phases = (si2 *) gmem((size_t) eeg_data.n_samps, sizeof(si2));
	eeg_data.corrs = (ui1 *) gmem((size_t) eeg_data.n_samps, sizeof(ui1));
	eeg_data.state = (ui1 *) gmem((size_t) eeg_data.n_samps, sizeof(ui1));

	sf8_ptr = eeg_data.filt_samps;
	si2_ptr = eeg_data.samps;
	for (i = eeg_data.n_samps; i--;)
		*sf8_ptr++ = (sf8) *si2_ptr++;

	eeg_data.window = (ui4) ((eeg_data.samp_freq * CORR_WIND_SECS) + 0.5);
	if (!(eeg_data.window % 2))
		++eeg_data.window;

	set_filt_desc(&filt_desc, eeg_data.samp_freq);

	filter(eeg_data.filt_samps, eeg_data.n_samps, eeg_data.samp_freq, &filt_desc);

	/* state array not needed until later, use it for temporary storage of critical points */
	find_crits(eeg_data.n_samps, eeg_data.filt_samps, eeg_data.state);

	(void) memcpy((void *) eeg_data.norm_filt, (void *) eeg_data.filt_samps, (size_t) eeg_data.n_samps * sizeof(sf8));
	norm_filt(eeg_data.n_samps, eeg_data.norm_filt, eeg_data.state);

	get_phase(eeg_data.norm_filt, eeg_data.n_samps, eeg_data.phases, eeg_data.state);

	calc_corrs(eeg_data.samps, eeg_data.norm_filt, eeg_data.corrs, eeg_data.n_samps, eeg_data.window);
	
	median_smooth(eeg_data.n_samps, eeg_data.window, eeg_data.corrs, 8); 

	set_state(sess, eeg_data.n_samps, eeg_data.window, eeg_data.corrs, eeg_data.state);
		
	temp_wind = (ui4) (((sf8) eeg_data.window / 4.0) + 0.5);
	median_smooth(eeg_data.n_samps, temp_wind, eeg_data.state, 0);
	
	fill_theta_fields(sess, &eeg_data);

	free((void *) eeg_data.filt_samps);
	free((void *) eeg_data.norm_filt);
	free((void *) eeg_data.phases);
	free((void *) eeg_data.corrs);
	free((void *) eeg_data.state);
	free((void *) filt_desc.inc_bands);
	free((void *) filt_desc.exc_bands);

	return;
}


void	set_filt_desc(filt_desc, samp_freq)
FILT_DESC	*filt_desc;
sf8		samp_freq;
{
	extern void	*gmem();

	filt_desc->n_inc_bands = 1;
	filt_desc->n_exc_bands = 2;
	filt_desc->inc_bands = gmem((size_t) filt_desc->n_inc_bands, sizeof(BAND));
	filt_desc->exc_bands = gmem((size_t) filt_desc->n_exc_bands, sizeof(BAND));
	filt_desc->exc_bands[0].lo_freq = 0.0;
	filt_desc->exc_bands[0].hi_freq = MIN_BOT_THETA_FILT;
	filt_desc->inc_bands[0].lo_freq = MIN_TOP_THETA_FILT;
	filt_desc->inc_bands[0].hi_freq = MAX_TOP_THETA_FILT;
	filt_desc->exc_bands[1].lo_freq = MAX_BOT_THETA_FILT;
	filt_desc->exc_bands[1].hi_freq = (samp_freq / 2.0) + 1.0;
	
	return;
}

