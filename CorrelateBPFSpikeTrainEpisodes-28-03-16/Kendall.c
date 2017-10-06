// Given data arrays x[1..n] and y[1..n], this program returns Kendall's correlation tau,
// its number of standard deviations from zero as z, and its two-tailed probability level as prob.
// Small values of prob indicate a significant correlation (tau positive) or anticorrelation (tau
// negative).
//
// the necessary formulas are:
// tau = numerator / denominator
// 	numerator = (concordant_pairs - discordant_pairs)
// 	denominator = sqrt(concordant_pairs + discordant_pairs+ extra-y) * sqrt(concordant_pairs + discordant_pairs+ extra-x)
// 		denominator = sqrt(LeftNum) * sqrt(RightNum)
//	where concordant pairs are those differences (xi - xj) that have the same sign as the differences (yi - yj)
//	extra-y are those non-zero differences (yi - yj) where (xi - xj) = 0
//	extra-x are those non-zero differences (xi - xj) where (yi - yj) = 0
// variance(tau) = (4N + 10) / 9N(N - 1)
// z = tau / sqrt(variance)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "size_types.h" 
#include "sess_anal.h" 
// #include "bpf.h"

void KendallTauRaw(si4 x[], si4 y[], ui4 NumEvents, sf8 *tau, sf8 *z, sf8 *prob)
{
	sf8 erfcc(sf8 x);
	ui4 i, j;
	ui4 LeftNum = 0, RightNum = 0;
	si4 Numerator=0;
	si4 dxdy, dx, dy;
	sf8 Variance;

	for (i=0; i < NumEvents; i++){
		for (j=(i+1); j < NumEvents; j++) { 
			dx = x[i]- x[j];
			dy = y[i]- y[j];
			dxdy = dx*dy;

			if (dxdy) { // Neither array has a tie.
				LeftNum++;
				RightNum++;
				dxdy > 0.0 ? Numerator++ : Numerator--;
			} else { //One or both arrays have ties.
				if(dy) LeftNum++;	//an extra y event
				if(dx) RightNum++;	//an extra x event
			}
		}
	}

	*tau = (sf8) Numerator / (sqrt((sf8)LeftNum) * sqrt((sf8)RightNum));
	Variance = (sf8)(4.0 * NumEvents + 10.0) / (sf8)(9.0 * NumEvents * (NumEvents - 1.0));
	*z = (*tau) / sqrt(Variance);
	*prob = erfcc(fabs(*z)/1.4142136);
	return;
}

// Computes the pair-wise differences within a data set, upon which Kendall's tau is based
void	PairwiseDiffSigns(si4 *data, si1 *signs, ui4 *I, ui4 *J, ui8 NumEvents, ui8 NumPairs)
{
	ui8 	n, i, j;
	si4	dif;

	for (n=0; n < NumPairs; n++)
		signs[n] = 0;
	n = 0;
	for (i=0; i < NumEvents; i++){
		for (j=(i+1); j < NumEvents; j++, n++) { 
			if(dif = (data[i] - data[j])){		 // if dif is non-zero set the sign
				signs[n] = (dif > 0) ? 1 : -1; // Assumes diffs has been initialized to zero
			}
			I[n] = i;
			J[n] = j;
		}
	}
	return;
}

// Given arrays dx[1..n] and dy[1..n] which represent the signs (-1, 0, 1) of all pairs in the data sets x and y,
// this program returns Kendall's correlation tau. dx and dy are the result of calling PairwiseDiffSigns
// its number of standard deviations from zero as z, and its two-tailed probability level as prob.
// Small values of prob indicate a significant correlation (tau positive) or anticorrelation (tau
// negative).
//

void KendallTauPairs(si1 *dx, si1 *dy, si4 *A, si4 *B, ui1 *PixA, ui1 *PixB, ui4 *I, ui4 *J, ui8 NumEvents, ui8 NumPairs, sf8 *tau, sf8 *z, sf8 *prob, ui4 *NumSamples, si1 IgnoreZeros)
{
	sf8 erfcc(sf8 x);
	ui8 i, ii, jj, n;
	ui4 LeftNum = 0, RightNum = 0;
	si4 Numerator=0;
	si1 dxdy;
	sf8 Variance;

	n = 0;
	for (i=0; i < NumPairs; i++){
		ii = I[i];
		jj = J[i];
		if(!PixA[ii] || !PixA[jj] || !PixB[ii] || !PixB[jj]){ // ignore the pair of intervals unless the rat was in an active pixel of both cells
			continue;
		}
// printf("%d\t%d\t%d\t%d\n", PixA[ii], PixA[jj], PixB[ii], PixB[jj]);
		if(IgnoreZeros){
			if(!A[ii] && !B[ii])  	// if no spike counts in one bin
				continue;	// Ignore this pair of intervals
			if(!A[jj] && !B[jj]) 
				continue;
		}
		
		n++;

		dxdy = dx[i] * dy[i];

		if (dxdy) { // Neither array has a tie.
			LeftNum++;
			RightNum++;
			dxdy > 0.0 ? Numerator++ : Numerator--;
		} else { // One or both arrays have ties.
			if(dy[i])
				 LeftNum++;	//an extra y event
			else if(dx[i])
				 RightNum++;	//an extra x event
		}
	}

	*NumSamples = n;
	if(LeftNum && RightNum){
		*tau = (sf8) Numerator / (sqrt((sf8)LeftNum) * sqrt((sf8)RightNum));
		Variance = (sf8)(4.0 * NumEvents + 10.0) / (sf8)(9.0 * NumEvents * (NumEvents - 1.0));
		*z = (*tau) / sqrt(Variance);
		*prob = erfcc(fabs(*z)/1.4142136);
	}else{
		*tau = *z = 0.0;
		*prob = 1.0;
	}
	return;
}

sf8 erfcc(sf8 x)
// Returns the complementary error function erfc(x) with fractional error everywhere less than 1.2 x 10^-7.
{
	sf8	t,z,ans;
	z=fabs(x);
	t=1.0/(1.0+0.5*z);
	ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+ t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+ t*(-0.82215223+t*0.17087277)))))))));
	return x >= 0.0 ? ans : 2.0-ans;
}
