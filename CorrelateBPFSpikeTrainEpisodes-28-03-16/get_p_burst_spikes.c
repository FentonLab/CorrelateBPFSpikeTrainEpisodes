
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"

void    p_burst_spikes(sess)
SESSION *sess;
{
	ui4		BurstSpikes[MAX_PROBES][MAX_CLUSTS_PER_PROBE], TotalSpikes[MAX_PROBES][MAX_CLUSTS_PER_PROBE];
	ui4		LastSpikeBurst[MAX_PROBES][MAX_CLUSTS_PER_PROBE], LastTime[MAX_PROBES][MAX_CLUSTS_PER_PROBE];
        si4             i,j,  n_recs, probe, clust, type, cell_num;
        RECORD          *recs;  
	CELL **cp;

        recs = sess->recs;
        n_recs = sess->num_recs;
	cp = sess->cells;

        for (i = 0; i < MAX_PROBES; ++i) 
                for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) 
			BurstSpikes[i][j] = TotalSpikes[i][j] = LastSpikeBurst[i][j] = LastTime[i][j] = 0;

        for (i = 0; i < n_recs; ++i) {
                type = recs[i].type;
                if (is_spike(*sess, type)) {
                        probe = recs[i].probe;
			clust = recs[i].clust;

			TotalSpikes[probe][clust]++;
                        if((recs[i].time_stamp - LastTime[probe][clust]) <= MAX_BURST_ISI){	//if isi is short enough to be a burst
				BurstSpikes[probe][clust]++;		// count this spike as a burst
				if(!LastSpikeBurst[probe][clust])	// if this is first spike in a burst
					BurstSpikes[probe][clust]++;	// count previous spike as a burst too
				LastSpikeBurst[probe][clust] = 1;
			}else{
				LastSpikeBurst[probe][clust] = 0;
			}
                        LastTime[probe][clust] = recs[i].time_stamp;
                }
        }

        for (i = 0; i < MAX_PROBES; ++i) {
                for (j = 0; j < MAX_CLUSTS_PER_PROBE; ++j) {
                        cell_num = sess->cell_map[i][j];
                        if (cell_num != NOT_CUT) {
				cp[cell_num]->p_burst_spikes = (sf4)BurstSpikes[i][j] / (sf4)TotalSpikes[i][j];
			}
		}
	}
	return;
}
