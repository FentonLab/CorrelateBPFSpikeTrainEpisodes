
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "uff.h"
#include "sess_anal.h"


void	get_records(sess)
SESSION	*sess;
{
	ui4		i, total_recs;
	si4		rec_time_sort();
	ui4		current_byte, file_len;
	ui4		rec_sizes[256];
	ui4		reverse_ui4();
	ui4		drdb_size;
	ui1		type, *dp, *first_rec, *temp_ui1;
	si2		temp_si2;
	extern void	get_date_time(), *gmem();
	extern si1	*get_env_var();
	RECORD		*rp;


	/* read header -> get DRDB length */

	dp = sess->data;
	if (dp[0] != HEADER_BYTE_0 || dp[1] != HEADER_BYTE_1) {
		(void) fprintf(stderr, "%cerror reading header of %s\n", 7, sess->name_str); 
		exit(1); 
	}
	
	get_date_time(sess, (si1 *) dp);

	temp_ui1 = (ui1 *) &temp_si2;
#ifdef BIG_ENDIAN_MACHINE
	temp_ui1[0] = dp[HEADER_DRDB_SIZE_OFFSET + 1]; temp_ui1[1] = dp[HEADER_DRDB_SIZE_OFFSET];
#else
	temp_ui1[0] = dp[HEADER_DRDB_SIZE_OFFSET]; temp_ui1[1] = dp[HEADER_DRDB_SIZE_OFFSET + 1];
#endif
	drdb_size = (ui4) temp_si2 + PAD_BYTES;
	dp += HEADER_SIZE + PAD_BYTES;


	/* read drdbs -> get record types and sizes */

	(void) memset((void *) rec_sizes, 0, (size_t) 256 * sizeof(ui4));
	while (TRUE) {
		if (dp[0] != DRDB_BYTE_0 || dp[1] != DRDB_BYTE_1)
			break; 
		rec_sizes[dp[DRDB_REC_TYPE_OFFSET]] = reverse_ui4(dp + DRDB_REC_SIZE_OFFSET) + PAD_BYTES;
		dp += drdb_size;
	}


	/* count records */

	first_rec = dp; 
	current_byte = dp - sess->data;
	total_recs = 0; 
	file_len = sess->file_len;
	do {
		type = *dp;
		if (rec_sizes[type] == 0) {
			(void) fprintf(stderr, "%cunknown record type in file %s -> file may be damaged\n", 7, sess->name_str); 
			exit(1); 
		}
		++total_recs; 
		dp += rec_sizes[type]; 
		current_byte += rec_sizes[type];
	} while (current_byte < file_len); 

	if (current_byte != file_len) {
		(void) fprintf(stderr, "%cfile %s appears truncated, ignoring last record\n", 7, sess->name_str);
		--total_recs;
	}
	sess->num_recs = total_recs;


	/* allocate RECORDS array */

	sess->recs = (RECORD *) gmem((size_t) total_recs, sizeof(RECORD)); 


	/* fill in recs */

	dp = first_rec; 
	rp = sess->recs;
	for (i = 0; i < total_recs; ++i) {
		type = *dp;
		rp->type = type;
		rp->time_stamp = reverse_ui4(dp + RECORD_TIME_STAMP_OFFSET);
		rp->data_offset = dp;
		switch (type) {
			case SINGLE_ELECTRODE_SPIKE_REC:
			case STEREOTRODE_SPIKE_REC:
			case TETRODE_SPIKE_REC:
				rp->probe = *(dp + RECORD_PROBE_OFFSET);
				rp->clust = *(dp + SPK_REC_CLUST_OFFSET);
				break;
			case CONTINUOUS_DATA_REC:
				rp->probe = *(dp + RECORD_PROBE_OFFSET);
				break;
			case POSITION_REC:
				rp->red_x = *(dp + POS_REC_RED_X_OFFSET);
				rp->red_y = *(dp + POS_REC_RED_Y_OFFSET);
				rp->green_x = *(dp + POS_REC_GREEN_X_OFFSET);
				rp->green_y = *(dp + POS_REC_GREEN_Y_OFFSET);
				break;
			case EVENT_FLAG_REC:
				rp->probe = *(dp + EV_FLAG_REC_TYPE_OFFSET);	/* this is put in the probe field since they */
				break;						/* are rarely used, and when they are, they occur */
		}								/* rarely, and thus are not worth their own field */
		dp += rec_sizes[type];						/* in a record structure */
		++rp;
	} 

	(void) qsort((void *) sess->recs, (size_t) total_recs, sizeof(RECORD), rec_time_sort); 


	return;
} 

/* defined in reverse.c

ui4	reverse_ui4(b)
ui1		*b;
{
	ui1	*f;
	ui4	u;

	f = (ui1 *) &u;

#ifdef BIG_ENDIAN_MACHINE
	b += 3;

	*f++ = *b--;
	*f++ = *b--;
	*f++ = *b--;
	*f = *b;
#else
	*f++ = *b++;
	*f++ = *b++;
	*f++ = *b++;
	*f = *b;
#endif

	return(u);
}

si2     reverse_si2(b)
ui1             *b;
{
        ui1     *f;
        si2     u;

        ++b;
        f = (ui1 *) &u;

        *f++ = *b--;
        *f = *b;

        return(u);
}
*/

si4     rec_time_sort(a, b)
RECORD	*a, *b;
{
	if (a->time_stamp > b->time_stamp)
		return(1);
	if (a->time_stamp < b->time_stamp)
		return(-1);
	return(0);
}
