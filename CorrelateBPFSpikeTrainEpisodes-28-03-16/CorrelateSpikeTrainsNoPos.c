#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include <math.h>
#include "size_types.h"
#include "sess_anal.h"
#include "bpf.h"

#define	DEFAULT_BIN_SIZE	250
#define	DEFAULT_ANALYSIS_INTERVAL 0

void	CorrelateSpikeTrainsNoPos(SESSION *sess)
{
	FILE 	*ofp;
	RECORD	*recs;
	CELL	**cp;
	// si1	*strcpy();
	si1	outfile[256], taudir[256], base_name[256], *c1, *c2;
	si1	*Diffs[MAX_PROBES][MAX_CLUSTS];
	ui4	*I, *J, NumSamples;
	si1	**ClustMap;
	ui1	*ActPix[MAX_PROBES][MAX_CLUSTS];
	ui4	*SpikeCounts[MAX_PROBES][MAX_CLUSTS], ObsSpikes[MAX_PROBES][MAX_CLUSTS], obs1, obs2;
	ui4	start_sec, stop_sec;
	sf8	rate, room_rate, arena_rate, ExpSpikes[MAX_PROBES][MAX_CLUSTS], exp1, exp2, z1, z2;
	sf4	pos_samp_freq, KendallRateThreshold;
	ui8	NumPairs;
	ui4	BinSize, bin; 
	ui8	i, j, MaxBins, AnalysisInterval = 0;
	ui4	x, y, rx, ry, x1, y1, ax, ay, IntervalStart, IntervalStop;
	si1	type; 
	si4	n_recs, n_units;
	ui1	probe, clust, pb, cl, max_probe, max_clust;
	si1	cell_num, cn;
	ui4	k, time_stamp, RecordingStartTime; 
	sf8	tau, z, prob, distance, dx, dy, hypot();
	void	KendallTauPairs(), PairwiseDiffSigns();

	ClustMap = sess->cell_map;
	n_recs = sess->num_recs;
	recs = sess->recs;
	cp = sess->cells;
	pos_samp_freq = sess->params->position_samp_freq;

	max_probe = sess->params->max_probe;
	max_clust = sess->params->max_clust;
	BinSize = sess->params->bin_size_ms;
	BinSize *= TIMESTAMPS_PER_MS;

	// find the start of the Data recording (i.e first spike or EEG sample)
	for(i=0; i < n_recs; i++){
        	type = recs[i].type;
                if (is_spike(*sess, type))
                        break;
		if (type == EEG_BPF_REC_TYPE)
			break;
        }
	RecordingStartTime = recs[i].time_stamp;

	AnalysisInterval = sess->params->analysis_interval_sec;
	AnalysisInterval *= (1000 * TIMESTAMPS_PER_MS);
	if(!AnalysisInterval){ // If this was not set then use the whole session
		// find the time of the last spike record with a good position sample
		for(i= n_recs;--i;){ 
			type = recs[i].type;
			if (is_spike(*sess, type)){ 
				AnalysisInterval = recs[i].time_stamp - RecordingStartTime;	 
				break;
			}
		}
	}

	MaxBins = AnalysisInterval / BinSize;
	NumPairs = (MaxBins * (MaxBins - 1))/2;


	// make sure that max_probe and max_clust are large enough to include all the probes and clusters in the data set.
	for(probe = 0; probe < MAX_PROBES; probe++){
		for(clust = 0; clust < MAX_CLUSTS; clust++){
			if(ClustMap[probe][clust] != NOT_CUT){
				if(probe > max_probe)
					max_probe = probe;
				if(clust> max_clust)
					max_clust = clust;
			}
		}
	}
	// Allocate memory for the Spike Count bins
	for(probe = 0; probe < MAX_PROBES; probe++){
		for(clust = 0; clust < MAX_CLUSTS; clust++){
			if(ClustMap[probe][clust] != NOT_CUT){
				if(probe > max_probe)
					max_probe = probe;
				if(clust> max_clust)
					max_clust = clust;
				SpikeCounts[probe][clust] = (ui4 *)calloc(MaxBins, sizeof(ui4));
				if(SpikeCounts[probe][clust] == NULL){
					fprintf(MyStdErr, "Can't allocate %lld bytes for spike counts\n", MaxBins);
					exit(-1);
				}
				Diffs[probe][clust] = (si1 *)calloc(NumPairs, sizeof(si1));
				if(Diffs[probe][clust] == NULL){
					fprintf(MyStdErr, "Can't allocate %lld bytes for spike count differences\n", NumPairs);
					exit(-1);
				}
				ActPix[probe][clust] = (ui1 *)calloc(MaxBins, sizeof(ui1));
				if(SpikeCounts[probe][clust] == NULL){
					fprintf(MyStdErr, "Can't allocate %lld bytes for Active Pixels\n", MaxBins);
					exit(-1);
				}
			}
			ObsSpikes[probe][clust] = 0;
			ExpSpikes[probe][clust] = 0.0;
		}
	}
	// Allocate memory for the Spike Count bin indices
	I = (ui4 *)calloc(NumPairs, sizeof(ui4));
	J = (ui4 *)calloc(NumPairs, sizeof(ui4));
	if((I == NULL) || (J == NULL)){
		fprintf(MyStdErr, "Can't allocate %lld bytes for tracking indices\n", NumPairs);
		exit(-1);
	}


        if (getenv("TAU_DIR") == NULL) {
                fprintf(stderr,"%cTAU_DIR not defined\n",7);
                exit(1);
        }
	(void *)strcpy(taudir, (si1 *)getenv("TAU_DIR"));

	c1 = sess->name_str;
        c2 = base_name;
        while (*c1 != '.' && *c1 != '\0')
                *c2++ = *c1++;
        *c2 = '\0';


	sprintf(outfile,"%s/%s.tau", taudir, base_name);
	printf("working on file %s\n", outfile);
	fflush(stdout);

	ofp = fopen(outfile, "w");
	if(ofp == NULL){
		printf("Can't open %s\n", outfile);
		exit(-1);
	}	


	fprintf(ofp,"%%%%BEGIN_HEADER SpikeCrossCorrelationTau Version.1.0\n");
	fprintf(ofp,"\t%%BinSizeInMS.0 ( %d )\n", (BinSize / TIMESTAMPS_PER_MS));
	fprintf(ofp,"\t%%RecordFormat.0 ( StartSec StopSec Probe[1] Cluster[1] Probe[2] Cluster[2] tau z prob NumSamps Obs[1] Exp[1] z[1] Avg[1] Obs[2] Exp[2] z[2] Avg[2])\n");
	fprintf(ofp,"%%%%END_HEADER\n");
	fflush(ofp);


	n_units = sess->num_cells + sess->num_probes;


	// Build the Spike Count time series 
	IntervalStart = RecordingStartTime;
	IntervalStop = IntervalStart + AnalysisInterval;

	// Initialize the Spike Count and ActPix bins
	for(probe = 0; probe <= max_probe; probe++){
		for(clust = 0; clust <= max_clust; clust++){
			if(ClustMap[probe][clust] != NOT_CUT){
				for(i = 0; i < MaxBins; i++){
					SpikeCounts[probe][clust][i] = 0;
					ActPix[probe][clust][i] = 1; // Do this for compatibility with positional data
				}
			}
			ObsSpikes[probe][clust] = 0;
			ExpSpikes[probe][clust] = 0.0;
		}
	}

	// estimate the expected firing rate
	for(probe = 0; probe <= max_probe; probe++){
		for(clust = 0; clust <= max_clust; clust++){
			if(ClustMap[probe][clust] != NOT_CUT){ 
				cell_num = sess->cell_map[probe][clust];
				if(cell_num >= 0){
					ExpSpikes[probe][clust] = cp[cell_num]->grand_rate * AnalysisInterval / pos_samp_freq;
				}
			}
		}
	}

	// This is the main loop
	for (i=0; i < n_recs; ++i) {
		type = recs[i].type;
		time_stamp = recs[i].time_stamp;
		if(time_stamp < IntervalStart)
			continue;

		if(time_stamp >= IntervalStop){	// prepare to output stuff
			// calculate the sign of all pairwise differences for Kendall's tau
			for(probe = 0; probe <= max_probe; probe++){
				for(clust = 0; clust <= max_clust; clust++){
					if(ClustMap[probe][clust] != NOT_CUT){ 
						PairwiseDiffSigns(SpikeCounts[probe][clust], Diffs[probe][clust], I, J, MaxBins, NumPairs);
					}
				}
			}

			// calculate Kendall's tau for each pair of units
			start_sec = (IntervalStart - RecordingStartTime) / TIMESTAMPS_PER_SEC;
			stop_sec = (IntervalStop - RecordingStartTime) / TIMESTAMPS_PER_SEC;
	
			if(sess->opts->do_not_cut){	// process pairs even if they are not cut
				for(probe = 0; probe <= max_probe; probe++){
					for(clust = 0; clust <= max_clust; clust++){
						cell_num = sess->cell_map[probe][clust];
						// if(cell_num >= 0){
							for(pb = probe; pb <= max_probe; pb++){
								for(cl = 0; cl <= max_clust; cl++){
									cn = sess->cell_map[pb][cl];
									// if(cn <= cell_num) // so only one entry is made per cell pair
									//	continue;
									if((pb == probe) && (cl <= clust))
										continue;
									if((ClustMap[probe][clust] != NOT_CUT) && (ClustMap[pb][cl] != NOT_CUT)){
										KendallTauPairs(Diffs[probe][clust], Diffs[pb][cl], SpikeCounts[probe][clust], SpikeCounts[pb][cl], ActPix[probe][clust], ActPix[pb][cl], I, J, MaxBins, NumPairs, &tau, &z, &prob, &NumSamples, sess->opts->ignore_zeros);
										obs1 = ObsSpikes[probe][clust];
										exp1 = ExpSpikes[probe][clust];
										z1 = (exp1 > 0.0) ? (obs1 - exp1)/sqrt(exp1):-100.0;
										obs2 = ObsSpikes[pb][cl];
										exp2 = ExpSpikes[pb][cl];
										z2 = (exp2 > 0.0) ? (obs2 - exp2)/sqrt(exp2):-100.0;
										fprintf(ofp,"%d\t%d\t%d\t%d\t%d\t%d\t%0.6lf\t%0.3lf\t%0.3lf\t%d\t%d\t%0.3lf\t%0.3lf\t%0.3lf\t%d\t%0.3lf\t%0.3lf\t%0.3lf\n", 
											start_sec, stop_sec, probe, clust, pb, cl, tau, z, prob, NumSamples, obs1, exp1, z1, cp[cell_num]->grand_rate, obs2, exp2, z2, cp[cn]->grand_rate);
									}else{ // print to occupy the space so the data can be lined up with other sessions 
									 	fprintf(ofp,"%d\t%d\t%d\t%d\t%d\t%d\t    \t    \t    \t    \t    \t    \t    \t    \t    \t    \t    \t    \n", 
									 		start_sec, stop_sec, probe, clust, pb, cl);
									}
								}
							}
						// }
					}
				}
			}else{	//only process pairs of cut clusters
				for(probe = 0; probe <= max_probe; probe++){
					for(clust = 0; clust <= max_clust; clust++){
						if(ClustMap[probe][clust] != NOT_CUT){ 
							cell_num = sess->cell_map[probe][clust];
							if(cell_num >= 0){
								for(pb = probe; pb <= max_probe; pb++){
									for(cl = 0; cl <= max_clust; cl++){
										if((pb == probe) && (cl == clust))
											continue;
										if((ClustMap[probe][clust] != NOT_CUT) && (ClustMap[pb][cl] != NOT_CUT)){
											cn = sess->cell_map[pb][cl];
											if(cn > cell_num){ // so only one entry is made per cell pair
												KendallTauPairs(Diffs[probe][clust], Diffs[pb][cl], SpikeCounts[probe][clust], SpikeCounts[pb][cl], ActPix[probe][clust], ActPix[pb][cl], I, J, MaxBins, NumPairs, &tau, &z, &prob, &NumSamples, sess->opts->ignore_zeros);
												obs1 = ObsSpikes[probe][clust];
												exp1 = ExpSpikes[probe][clust];
												z1 = (exp1 > 0.0) ? (obs1 - exp1)/sqrt(exp1):-100.0;
												obs2 = ObsSpikes[pb][cl];
												exp2 = ExpSpikes[pb][cl];
												z2 = (exp2 > 0.0) ? (obs2 - exp2)/sqrt(exp2):-100.0;
												fprintf(ofp,"%d\t%d\t%d\t%d\t%d\t%d\t%0.6lf\t%0.3lf\t%0.3lf\t%d\t%d\t%0.3lf\t%0.3lf\t%0.3lf\t%d\t%0.3lf\t%0.3lf\t%0.3lf\n", 
													start_sec, stop_sec, probe, clust, pb, cl, tau, z, prob, NumSamples, obs1, exp1, z1, cp[cell_num]->grand_rate, obs2, exp2, z2, cp[cn]->grand_rate);
											}
										}
									}
								}
							}
						}
					}
				}
			}

			IntervalStart = IntervalStop;
			IntervalStop = IntervalStart + AnalysisInterval;

			// Initialize the Spike Count and ActPix bins
			for(probe = 0; probe <= max_probe; probe++){
				for(clust = 0; clust <= max_clust; clust++){
					if(ClustMap[probe][clust] != NOT_CUT){
						for(i = 0; i < MaxBins; i++){
							SpikeCounts[probe][clust][i] = 0;
							ActPix[probe][clust][i] = 1; // Do this for compatibility with positional data
						}
					}
					ObsSpikes[probe][clust] = 0;
				}
			}
		} // if (time_stamp > IntervalStop)

                if (is_spike(*sess, type)){ // process the new spike/position record
			probe = recs[i].probe;
			clust = recs[i].clust;
			bin = (time_stamp - IntervalStart) / BinSize;
			if(ClustMap[probe][clust] != NOT_CUT){ 
				SpikeCounts[probe][clust][bin]++;
				ObsSpikes[probe][clust]++;
				
				// record if the current pixel is a place where the cell fired above the threshold for including the bin in the analysis
				cell_num = sess->cell_map[probe][clust];
			}
		}
	} // loop for recs


	// Do not calculate the correlation unless data exists for a full analysis interval.

	fclose(ofp);
	return;
}
