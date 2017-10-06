
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sess_anal.h"
#include "uff.h"
#include "theta.h"


#ifdef BIG_ENDIAN_MACHINE

void		get_eeg(sess, eeg_data)
SESSION		*sess;
EEG_DATA	*eeg_data;
{
	RECORD		*recs;
	si4		i, n_recs;
	ui4		*curr_time, temp_ui4;
	si2		temp_si2;
	ui1		*new_high, *new_low, *old_high, *old_low;
	sf8		d_time, time_inc;
	si2		reverse_si2();
	ui4		reverse_ui4();
	si2		valid_samps;
	ui1		*curr_samp;
	extern void	*gmem();

	n_recs = sess->num_recs;
	recs = sess->recs;

	for (i = 0; i < n_recs; ++i) {
		if (recs[i].type == CONTINUOUS_DATA_REC) {
			eeg_data->probe = recs[i].probe;	
			eeg_data->samp_freq = (sf8) reverse_ui4(recs[i].data_offset + SU_CR_SAMP_FREQ_OFFSET);
			eeg_data->n_samps = 0;
			break;
		}
	}
	if (i == n_recs) {
		(void) fprintf(stderr, "%c\n\tNo eeg probes in file \"%s\". Terminating program.\n\n", 7, sess->name_str);
		exit(1);
	}

	for (i = 0; i < n_recs; ++i) {
		if (recs[i].type == CONTINUOUS_DATA_REC) {
			if (eeg_data->probe != recs[i].probe) {
				(void) fprintf(stderr, "%c\n\tMore than one eeg probe in file \"%s\". Terminating program.\n\n", 7, sess->name_str);
				exit(1);
			}
			eeg_data->n_samps += reverse_si2(recs[i].data_offset + SU_CR_VALID_SAMPS_OFFSET);
			
		}
	}

	eeg_data->samps = (si2 *) gmem((size_t) eeg_data->n_samps, sizeof(si2));	
	eeg_data->times = (ui4 *) gmem((size_t) eeg_data->n_samps, sizeof(ui4));
	curr_samp = (ui1 *) eeg_data->samps - 2;
	curr_time = (ui4 *) eeg_data->times;
	time_inc = 10000.0 / (sf8) eeg_data->samp_freq;	
	
	for (i = 0; i < n_recs; ++i) {
		if (recs[i].type == CONTINUOUS_DATA_REC) {
			d_time = (sf8) recs[i].time_stamp;
			valid_samps = reverse_si2(recs[i].data_offset + SU_CR_VALID_SAMPS_OFFSET);
			new_low = (new_high = curr_samp) + 1;
			old_high = (old_low = recs[i].data_offset + SU_CR_SAMPS_OFFSET - 2) + 1;
			while (valid_samps--) {
				*(new_low += 2) = *(old_low += 2);
				*(new_high += 2) = *(old_high += 2);
				*curr_time++ = (ui4) (d_time + 0.5);
				d_time += time_inc;
			}
			curr_samp = new_high;
		}
	}

	return;
}


#else

void		get_eeg(sess, eeg_data)
SESSION		*sess;
EEG_DATA	*eeg_data;
{
	RECORD		*recs;
	si4		i, n_recs;
	ui4		*curr_time, temp_ui4;
	si2		temp_si2;
	sf8		d_time, time_inc;
	si2		valid_samps;
	si2		*curr_samp;
	extern void	*gmem();

	n_recs = sess->num_recs;
	recs = sess->recs;

	for (i = 0; i < n_recs; ++i) {
		if (recs[i].type == CONTINUOUS_DATA_REC) {
			eeg_data->probe = recs[i].probe;	
			(void) memcpy((void *) &temp_ui4, (void *) (recs[i].data_offset + SU_CR_SAMP_FREQ_OFFSET), sizeof(ui4));
			eeg_data->samp_freq = (sf8) temp_ui4;
			eeg_data->n_samps = 0;
			break;
		}
	}
	if (i == n_recs) {
		(void) fprintf(stderr, "%c\n\tNo eeg probes in file \"%s\". Terminating program.\n\n", 7, sess->name_str);
		exit(1);
	}

	for (i = 0; i < n_recs; ++i) {
		if (recs[i].type == CONTINUOUS_DATA_REC) {
			if (eeg_data->probe != recs[i].probe) {
				(void) fprintf(stderr, "%c\n\tMore than one eeg probe in file \"%s\". Terminating program.\n\n", 7, sess->name_str);
				exit(1);
			}
			(void) memcpy((void *) &temp_si2, (void *) (recs[i].data_offset + SU_CR_VALID_SAMPS_OFFSET), sizeof(si2));
			eeg_data->n_samps += temp_si2;
		}
	}

	eeg_data->samps = (si2 *) gmem((size_t) eeg_data->n_samps, sizeof(si2));	
	eeg_data->times = (ui4 *) gmem((size_t) eeg_data->n_samps, sizeof(ui4));
	curr_samp = eeg_data->samps;
	curr_time = (ui4 *) eeg_data->times;
	time_inc = 10000.0 / (sf8) eeg_data->samp_freq;	
	
	for (i = 0; i < n_recs; ++i) {
		if (recs[i].type == CONTINUOUS_DATA_REC) {
			d_time = (sf8) recs[i].time_stamp;
			(void) memcpy((void *) &valid_samps, (void *) (recs[i].data_offset + SU_CR_VALID_SAMPS_OFFSET), sizeof(si2));
			(void) memcpy((void *) curr_samp, (void *) (recs[i].data_offset + SU_CR_SAMPS_OFFSET), sizeof(si2) * (size_t) valid_samps);
			curr_samp += valid_samps;
			while (valid_samps--) {
				*curr_time++ = (ui4) (d_time + 0.5);
				d_time += time_inc;
			}
		}
	}

	return;
}

#endif

/* defined in reverse.c

ui4	reverse_ui4(b)
ui1		*b;
{
	ui1	*f;
	ui4	u;

	b += 3;
	f = (ui1 *) &u;

	*f++ = *b--;
	*f++ = *b--;
	*f++ = *b--;
	*f = *b;

	return(u);
}


si2	reverse_si2(b)
ui1		*b;
{
	ui1	*f;
	si2	u;

	++b;
	f = (ui1 *) &u;

	*f++ = *b--;
	*f = *b;

	return(u);
}

*/
