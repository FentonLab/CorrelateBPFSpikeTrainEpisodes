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

void	CorrelateSpikeTrains(SESSION *sess)
{
	FILE 	*ofp;
	RECORD	*recs;
	CELL	**cp, **cp2;
	// si1	*strcpy();
	si1	outfile[256], taudir[256], base_name[256], *c1, *c2;
	si1	**Diffs;
	ui4	*I, *J, NumSamples, area;
	si1	**ClustMap;
	ui1	**ActPix;
	si1	**FileName;
	ui4	**SpikeCounts, *ObsSpikes, obs1, obs2;
	ui4	n_positions, start_sec, stop_sec;
	sf4	*tm, *tm2;
	sf8	rate, rate1, rate2, *ExpSpikes, exp1, exp2, z1, z2;
	sf8	**MapCorrelation;
	sf8	**Map2Correlation;
	sf4	pos_samp_freq, KendallRateThreshold;
	sf4	*arr1, *arr2, *rates;
	ui8	NumPairs, MaxBins, AnalysisInterval = 0;
	ui4	BinSize, bin; 
	ui4	x, y, rx, ry, x1, y1, ax, ay, IntervalStart, IntervalStop;
	si1	type; 
	si4	n_recs, n_units, AA, BB;
	ui1	probe, clust, pb, cl, max_probe, max_clust;
	ui1	*probes, *clusts;
	si1	cell_num, cn;
	ui4	k, time_stamp, RecordingStartTime; 
	ui8	n, i, j; 
	sf8	tau, r, p, z, prob, distance, dx, dy, hypot();
	void	KendallTauPairs(), PairwiseDiffSigns();
	void	*gmem();

	ClustMap = sess->cell_map;
	n_recs = sess->num_recs;
	recs = sess->recs;
	cp = sess->cells;
	cp2 = sess->secondary_cells;
	tm = sess->time->map[0];
	tm2 = sess->secondary_time->map[0];
	n_units = sess->num_cells + sess->num_probes;
	RecordingStartTime = sess->recs[0].time_stamp;
	pos_samp_freq = sess->params->position_samp_freq;

	max_probe = sess->params->max_probe;
	max_clust = sess->params->max_clust;
	BinSize = sess->params->bin_size_ms;
	BinSize *= TIMESTAMPS_PER_MS;

	rate1 = rate2 = 0.0;

	AnalysisInterval = sess->params->analysis_interval_sec;
	AnalysisInterval *= TIMESTAMPS_PER_SEC;
	if(!AnalysisInterval){ // If this was not set then use the whole session
		// find the time of the last spike record with a good position sample
		for(i= n_recs;--i;){ 
			type = recs[i].type;
			if (is_spike(*sess, type)){ 
				if(sess->opts->bpf_room_file){
                        		x1 = recs[i].RoomX;
                    		 	y1 = recs[i].RoomY;
                		}else if(sess->opts->bpf_arena_file){
                        		x1 = recs[i].ArenaX;
                        		y1 = recs[i].ArenaY;
                		}else{
                        		x1 = recs[i].red_x;
                        		y1 = recs[i].red_y;
                		}
                		if(x1 && y1){
					AnalysisInterval = recs[i].time_stamp - RecordingStartTime;	 
					break;
				}
			}
		}
	}

	MaxBins = AnalysisInterval / BinSize;
	NumPairs = (MaxBins * (MaxBins - 1))/2;

	// Allocate memory 
	FileName = (si1 **) gmem((size_t) n_units, sizeof(si1 *));
	probes = (ui1 *) gmem((size_t) n_units, sizeof(ui1));
	clusts = (ui1 *) gmem((size_t) n_units, sizeof(ui1));
	MapCorrelation = (sf8 **) gmem((size_t)n_units, sizeof(sf8 *));
	Map2Correlation = (sf8 **) gmem((size_t)n_units, sizeof(sf8 *));
        SpikeCounts = (ui4 **) gmem((size_t)n_units, sizeof(ui4 *));
        Diffs = (si1 **) gmem((size_t)n_units, sizeof(si1 *));
        ActPix = (ui1 **) gmem((size_t)n_units, sizeof(ui1 *));
        ObsSpikes = (ui4 *) gmem((size_t)n_units, sizeof(ui4));
        ExpSpikes = (sf8 *) gmem((size_t)n_units, sizeof(sf8));

	for(i=0; i < n_units; i++){
		MapCorrelation[i] = (sf8 *) gmem((size_t)n_units,sizeof(sf8));
		Map2Correlation[i] = (sf8 *) gmem((size_t)n_units,sizeof(sf8));
		SpikeCounts[i] = (ui4 *) gmem((size_t)MaxBins, sizeof(ui4));
		Diffs[i] = (si1 *) gmem((size_t)NumPairs, sizeof(si1));
		ActPix[i] = (ui1 *) gmem((size_t)MaxBins, sizeof(ui1));
		FileName[i] = (si1 *) gmem((size_t)FILE_NAME_CHARS, sizeof(si1));
	}

	// Allocate memory for the Spike Count bin indices
	I = (ui4 *) gmem((size_t)NumPairs, sizeof(ui4));
	J = (ui4 *) gmem((size_t)NumPairs, sizeof(ui4));

	// verify max_probe and max_clust and determine the probe and clust of each unit.
	// max_probe = max_clust = 0;
	max_probe = sess->params->max_probe;
	max_clust = sess->params->max_clust;
	for(probe = 0; probe < MAX_PROBES; probe++){
		for(clust = 0; clust < MAX_CLUSTS; clust++){
			if(ClustMap[probe][clust] != NOT_CUT){
				cell_num = ClustMap[probe][clust];
				probes[cell_num] = probe;
				clusts[cell_num] = clust;
				if(probe > max_probe)
					max_probe = probe;
				if(clust> max_clust)
					max_clust = clust;
			}
		}
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
	if (sess->params->kendall_rate_threshold > 20.0)        // values > 20 are a percentage ofthe grand_rate
		fprintf(ofp,"\t%%RateThresholdPercentageOfOverallAvg.0 ( %0.2f )\n", sess->params->kendall_rate_threshold);
	else
		fprintf(ofp,"\t%%RateThresholdAP/sec.0 ( %0.2f )\n", sess->params->kendall_rate_threshold);
	fprintf(ofp,"\t%%BinSizeInMS.0 ( %d )\n", (BinSize / TIMESTAMPS_PER_MS));
	if(sess->opts->ts_file)
		fprintf(ofp,"\t%%RecordFormat.0 ( StartSec StopSec FileName[1] FileNum[1] FileName[2] FileNum[2] tau z prob NumSampsInCorr speed[pix/s] Obs[1] Exp[1] z[1] Avg[1] Obs[2] Exp[2] z[2] Avg[2] RateMapSimilarity)\n");
	else
		fprintf(ofp,"\t%%RecordFormat.0 ( StartSec StopSec Probe[1] Cluster[1] Probe[2] Cluster[2] tau z prob NumSampsInCorr speed[pix/s] Obs[1] Exp[1] z[1] Avg[1] Obs[2] Exp[2] z[2] Avg[2] RoomRateMapSimilarity ArenaRateMapSimilarity)\n");
	fprintf(ofp,"%%%%END_HEADER\n");
	fflush(ofp);

	// Build the Spike Count time series 
	IntervalStart = RecordingStartTime;
	IntervalStop = IntervalStart + AnalysisInterval;


	// figure out what position to use for an initial location
	distance = 0.0;
	n_positions = 0;
	i = 0;
	while(1){
		if(sess->opts->bpf_room_file){
			x1 = recs[i].RoomX;
			y1 = recs[i].RoomY;
		}else if(sess->opts->bpf_arena_file){
			x1 = recs[i].ArenaX;
			y1 = recs[i].ArenaY;
		}else{
			x1 = recs[i].red_x;
			y1 = recs[i].red_y;
		}
		if(x1 && y1)
			break;
		i++;
	}

	// Assign strings to FileNames
	n = n_units;
	for (i = 0; i < n_recs; ++i) {
		if(!n)
			break;
		type = recs[i].type;
                if (is_spike(*sess, type)){
			probe = recs[i].probe;
			clust = recs[i].clust;
			if(ClustMap[probe][clust] != NOT_CUT){ 
				cell_num = ClustMap[probe][clust];
				if(cell_num >= 0){
					if(*(FileName[cell_num]) == '\0'){
						strncpy(FileName[cell_num],recs[i].filename, FILE_NAME_CHARS);
						n--;
					}
				}
			}
		}
	}

        // calculate rate map correlations
	area = sess->params->area;
	arr1 = (sf4 *) gmem((size_t) area, sizeof(sf4));
	arr2 = (sf4 *) gmem((size_t) area, sizeof(sf4));

	for(cell_num = 0; cell_num < n_units; cell_num++){
		rates = cp[cell_num]->rate_map[0];
		for (i = n = 0; i < area; i++){
		        if (tm[i] > 0.0){
		                arr1[n++] = rates[i];
			}
		}

		for(cn = cell_num + 1; cn < n_units; cn++){
			rates = cp[cn]->rate_map[0];
			for (i = n = 0; i < area; ++i){
		        	if (tm[i] > 0.0){
		               		arr2[n++] = rates[i];
				}
			}

			correl(arr1, arr2, n, &r, &z, &p);
			MapCorrelation[cell_num][cn] = r; 
		}
	} 

	if((sess->opts->bpf_room_file) || (sess->opts->bpf_arena_file)){
		for(cell_num = 0; cell_num < n_units; cell_num++){
			rates = cp2[cell_num]->rate_map[0];
			for (i = n = 0; i < area; i++){
		        	if (tm2[i] > 0.0){
		                	arr1[n++] = rates[i];
				}
			}

			for(cn = cell_num + 1; cn < n_units; cn++){
				rates = cp2[cn]->rate_map[0];
				for (i = n = 0; i < area; ++i){
		        		if (tm2[i] > 0.0){
		               			arr2[n++] = rates[i];
					}
				}
				if(n){
					correl(arr1, arr2, n, &r, &z, &p);
					Map2Correlation[cell_num][cn] = r; 
				}else
					Map2Correlation[cell_num][cn] = -2.0; 
			}
		}
	}

	for (i = 0; i < n_recs; ++i) {
		type = recs[i].type;
		time_stamp = recs[i].time_stamp;
		if(time_stamp < IntervalStart)
			continue;
		bin = (time_stamp - IntervalStart) / BinSize;

		rx = recs[i].RoomX;
		ry = recs[i].RoomY;
		ax = recs[i].ArenaX;
		ay = recs[i].ArenaY;
		// figure out what position to use
		if(sess->opts->bpf_room_file){
			x = rx;
			y = ry;
		}else if(sess->opts->bpf_arena_file){
			x = ax;
			y = ay;
		}else{
			x = recs[i].red_x;
			y = recs[i].red_y;
		}
		if(!(x && y) && !(rx && ry) && !(ax && ay)) // only consider if position was sampled correctly
			continue;

		if(time_stamp >= IntervalStop){	// prepare to output stuff
			// calculate the sign of all pairwise differences for Kendall's tau
			for(cell_num = 0; cell_num < n_units; cell_num++){
				PairwiseDiffSigns(SpikeCounts[cell_num], Diffs[cell_num], I, J, MaxBins, NumPairs);
			}

			// calculate Kendall's tau for each pair of units
			start_sec = (IntervalStart - RecordingStartTime) / TIMESTAMPS_PER_SEC;
			stop_sec = (IntervalStop - RecordingStartTime) / TIMESTAMPS_PER_SEC;
			for(cell_num = 0; cell_num < n_units; cell_num++){
				for(cn = cell_num + 1; cn < n_units; cn++){
					KendallTauPairs(Diffs[cell_num], Diffs[cn], SpikeCounts[cell_num], SpikeCounts[cn], ActPix[cell_num], ActPix[cn], I, J, MaxBins, NumPairs, &tau, &z, &prob, &NumSamples, sess->opts->ignore_zeros);
					obs1 = ObsSpikes[cell_num];
					exp1 = ExpSpikes[cell_num];
					z1 = (exp1 > 0.0) ? (obs1 - exp1)/sqrt(exp1):-100.0;
					obs2 = ObsSpikes[cn];
					exp2 = ExpSpikes[cn];
					z2 = (exp2 > 0.0) ? (obs2 - exp2)/sqrt(exp2):-100.0;
					if(sess->opts->ts_file){
						fprintf(ofp,"%d\t%d\t%s\t%d\t%s\t%d\t%0.6lf\t%0.3lf\t%0.3lf\t%d\t%0.2lf\t%d\t%0.3lf\t%0.3lf\t%0.3lf\t%d\t%0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\n", 
								start_sec, stop_sec, FileName[cell_num], clusts[cell_num], FileName[cn], clusts[cn], tau, z, prob, NumSamples, (distance/(stop_sec - start_sec)), obs1, exp1, z1, cp[cell_num]->grand_rate, obs2, exp2, z2, cp[cn]->grand_rate, MapCorrelation[cell_num][cn]);
					}else{
						fprintf(ofp,"%d\t%d\t%d\t%d\t%d\t%d\t%0.6lf\t%0.3lf\t%0.3lf\t%d\t%0.2lf\t%d\t%0.3lf\t%0.3lf\t%0.3lf\t%d\t%0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\n", 
								start_sec, stop_sec, probes[cell_num], clusts[cell_num], probes[cn], clusts[cn], tau, z, prob, NumSamples, (distance/(stop_sec - start_sec)), obs1, exp1, z1, cp[cell_num]->grand_rate, obs2, exp2, z2, cp[cn]->grand_rate, MapCorrelation[cell_num][cn], Map2Correlation[cell_num][cn]);
					}
				}
			}

			IntervalStart = IntervalStop;
			IntervalStop = IntervalStart + AnalysisInterval;

			// Initialize the Spike Count and ActPix bins
			for(cell_num = 0; cell_num < n_units; cell_num++){
				for(j = 0; j < MaxBins; j++){
					SpikeCounts[cell_num][j] = 0;
					ActPix[cell_num][j] = 0;
				}
			}
			ObsSpikes[cell_num] = 0;
			ExpSpikes[cell_num] = 0.0;
			// reset distance
			distance = 0.0;
		} // if (time_stamp > IntervalStop)
		
                if (is_spike(*sess, type)){ // process the new spike/position record
			probe = recs[i].probe;
			clust = recs[i].clust;
			if(ClustMap[probe][clust] != NOT_CUT){ 
				cell_num = ClustMap[probe][clust];
				SpikeCounts[cell_num][bin]++;
				ObsSpikes[cell_num]++;
				
			}
		}else if(type == sess->PositionRec){ 
			for(cell_num = 0; cell_num < n_units; cell_num++){ // figure out if this is an active pixel for all cells
				// record if the current pixel is a place where the cell fired above the threshold for including the bin in the analysis
				rate = cp[cell_num]->rate_map[y][x];
				rate1 = cp[cell_num]->rate_map[ry][rx];
				rate2 = cp[cell_num]->rate_map[ay][ax];
				if (sess->params->kendall_rate_threshold > 20.0)	// values > 20 are a percentage ofthe grand_rate
					KendallRateThreshold =  (cp[cell_num]->grand_rate * sess->params->kendall_rate_threshold / 100.0);
				else 
					KendallRateThreshold = sess->params->kendall_rate_threshold;
				if((rate >= KendallRateThreshold) || (rate1 >= KendallRateThreshold) || (rate2 >= KendallRateThreshold)){
					ActPix[cell_num][bin] = 1;
				}

				// estimate the expected firing rate
				if(rate > 0.0)
					ExpSpikes[cell_num] += (rate / pos_samp_freq);
			}

			// measure distance moved
			if(!(n_positions % POSITION_SAMPLING_FOR_SPEED)){
				dy = ((double)y - (double)y1);
				dx = ((double)x - (double)x1);
				distance += hypot(dy, dx);
				x1 = x;
				y1 = y;
			}
			n_positions++;
		}
	} // loop for recs

	// Do not calculate the correlation unless data exists for a full analysis interval.

	fclose(ofp);
	return;
}
