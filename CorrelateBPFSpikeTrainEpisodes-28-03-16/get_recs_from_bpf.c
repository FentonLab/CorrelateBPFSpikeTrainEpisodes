
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "bpf.h"
#include "sess_anal.h"


void	get_recs_from_bpf(sess)
SESSION	*sess;
{
	ui4		i, total_recs, first_eeg_channel;
	si4		rec_time_sort();
	ui4		current_byte, file_len;
	ui4		*rec_sizes;
	ui4		reverse_ui4();
	ui4		drdb_size;
	ui1		str[256], type, *dp, *first_rec, *temp_ui1;
	si2		temp_si2;
	extern void	get_date_time(), *gmem();
	extern si1	*get_env_var();
	RECORD		*rp;

	/* read header -> get record sizes */
	
	ReadBPFHeader(sess);

	/* count records */
	dp = sess->bpf_data;
	rec_sizes = sess->BPFRecordSizes;

	first_rec = dp; 
	current_byte = sess->bpf_data - sess->data;
	total_recs = 0; 
	file_len = sess->file_len;
	do {
		type = *dp;
		if (rec_sizes[type] == 0) {
			if(current_byte < file_len -1){	// there is a new line at the end of the bpf file
				(void) fprintf(stderr, "%cRecord number %d. Unknown record type '%c' in file %s -> file may be damaged\n", 7, total_recs, type, sess->name_str); 
				exit(1); 
			}
			break;
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
	strcpy(sess->desc_str, "not available");
	first_eeg_channel = sess->BPFEEGChannels[0]; // BPF has a list of eeg channels in the header but not in the EEG records


	/* allocate RECORDS array */

	sess->recs = (RECORD *) gmem((size_t) total_recs, sizeof(RECORD)); 


	/* fill in recs */

	dp = first_rec; 
	rp = sess->recs;
	for (i = 0; i < total_recs; ++i) {
		type = *dp;
		rp->type = type;
		rp->time_stamp = reverse_ui4(dp + BPF_RECORD_TIME_STAMP_OFFSET);
		rp->data_offset = dp;
		switch (type) {
			case SINGLE_BPF_REC_TYPE:
			case STEREO_BPF_REC_TYPE:
			case TETRODE_BPF_REC_TYPE:
				rp->probe = *(dp + BPF_RECORD_PROBE_OFFSET);
				rp->clust = *(dp + BPF_SPK_REC_CLUST_OFFSET);
				break;
			case EEG_BPF_REC_TYPE:
				rp->probe = first_eeg_channel;
				break;
			case SYNC_BPF_REC_TYPE:
				rp->probe = BPF_SYNC_PROBE; // this is fixed 
				break;
			case ROOM_POSITION_BPF_REC_TYPE:
				rp->RoomX = rp->red_x = *(dp + BPF_POS_REC_X_OFFSET);	// set red_x/y since functions like calculate_speed use red_ to position calculations
				rp->RoomY = rp->red_y = *(dp + BPF_POS_REC_Y_OFFSET);
				rp->RoomAng = (ui2)*(dp + BPF_POS_REC_ANG_OFFSET);
				break;
			case ARENA_POSITION_BPF_REC_TYPE:
				rp->ArenaX = rp->green_x = *(dp + BPF_POS_REC_X_OFFSET);
				rp->ArenaY = rp->green_y = *(dp + BPF_POS_REC_Y_OFFSET);
				rp->ArenaAng = (ui2)*(dp + BPF_POS_REC_ANG_OFFSET);
				break;
			default:
				break;
		}							
		dp += rec_sizes[type];				
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


si4     rec_time_sort(a, b)
RECORD	*a, *b;
{
	if (a->time_stamp > b->time_stamp)
		return(1);
	if (a->time_stamp < b->time_stamp)
		return(-1);
	return(0);
}
*/

