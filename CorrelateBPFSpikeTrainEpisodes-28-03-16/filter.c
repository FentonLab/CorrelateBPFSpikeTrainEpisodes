
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
// #include <values.h>
#include "size_types.h"
#include "filter.h"


void	filter(data, n_samps, samp_freq, filt_desc)
sf8		*data, samp_freq;
ui4		n_samps;
FILT_DESC	*filt_desc;
{
	register si4	i, j;
	register sf8	*b1, *b2, *bt1, *bt2, *d, val;
	si4		len, half_len, loops, leftovers;
	size_t		buf_bytes;
	sf8		*buf1, *buf2, *filt, *wind;
	 void	make_filter(), make_window(), filter_buffer();
	extern void	*gmem();

	/* useful constants */
	len = (si4) FILT_BUFFER_LEN;
	half_len = len >> 1;
	buf_bytes = (size_t) len * sizeof(sf8);

	/* temporary memory */
	buf1 = gmem((size_t) len, sizeof(sf8));
	buf2 = gmem((size_t) len, sizeof(sf8));
	filt = gmem((size_t) (half_len + 1), sizeof(sf8));
	wind = gmem((size_t) half_len, sizeof(sf8));
	
	/* obvious */
	make_filter(filt, samp_freq, half_len, filt_desc);
	make_window(wind, half_len);

	/* copy first sample si4o first half of first buffer */
	bt1 = buf1; val = *data;
	for (i = half_len; i--;)
		*bt1++ = val;

	/* copy half a buffer length of data si4o second half of first buffer */
	(void) memcpy((void *) (buf1 + half_len), (void *) data, (size_t) half_len * sizeof(sf8));
 	filter_buffer(buf1, filt, wind, len);

	/* overlap and add filtering of data */
	loops = (n_samps / half_len) - 1;
	b1 = buf1; b2 = buf2; d = data;
	for (i = loops; i--;) {
		(void) memcpy((void *) b2, (void *) d, buf_bytes);
 		filter_buffer(b2, filt, wind, len);
		bt1 = b1 + half_len; bt2 = b2;
		for (j = half_len; j--;)
			*d++ = *bt1++ + *bt2++;
		bt1 = b1; b1 = b2; b2 = bt1;
	}

	/* add first half of "leftovers" buffer and last half of last complete buffer */
	leftovers = n_samps - (d - data);
	(void) memcpy((void *) b2, (void *) d, (size_t) leftovers * sizeof(sf8));
	val = b2[leftovers - 1];
	bt2 = b2 + leftovers;
	for (i = len - leftovers; i--;)
		*bt2++ = val;
 	filter_buffer(b2, filt, wind, len);
	bt1 = b1 + half_len; bt2 = b2;
	for (i = half_len; i--;)
		*d++ = *bt1++ + *bt2++;
	bt1 = b1; b1 = b2; b2 = bt1;

	/* add last part of "leftovers" buffer */
	leftovers -= half_len;
	(void) memcpy((void *) b2, (void *) d, (size_t) leftovers * sizeof(sf8));
	val = b2[leftovers - 1];
	bt2 = b2 + leftovers;
	for (i = len - leftovers; i--;)
		*bt2++ = val;
 	filter_buffer(b2, filt, wind, len);
	bt1 = b1 + half_len; bt2 = b2;
	for (i = leftovers; i--;)
		*d++ = *bt1++ + *bt2++;
	
	/* clean up */	
	free((void *) buf1);
	free((void *) buf2);
	free((void *) filt);
	free((void *) wind);

	return;
}


 void	make_filter(filt, samp_freq, half_len, filt_desc)
sf8		*filt, samp_freq;
si4		half_len;
FILT_DESC	*filt_desc;
{
	si4	i, j, k, n;
	sf8	nyquist_freq, delta, target_freq, cos_sum;

	nyquist_freq = samp_freq / 2.0;
	delta = nyquist_freq / (sf8) half_len;
	
	for (i = 0; i <= half_len; ++i)
		filt[i] = 0.5;

	/* fill included-band regions of the filter with 1.0 */
	n = filt_desc->n_inc_bands;
	for (i = 0; i < n; ++i) {
		target_freq = filt_desc->inc_bands[i].lo_freq;
		j = (si4) ((target_freq / delta) + 0.5);
		if (j > half_len)
			j = half_len;
		target_freq = filt_desc->inc_bands[i].hi_freq;
		k = (si4) ((target_freq / delta) + 0.5);
		if (k > half_len)
			k = half_len;
		for (; j <= k; ++j)
			filt[j] = 1.0;
	}

	/* fill excluded-band regions of the filter with 0.0 */
	n = filt_desc->n_exc_bands;
	for (i = 0; i < n; ++i) {
		target_freq = filt_desc->exc_bands[i].lo_freq;
		j = (si4) ((target_freq / delta) + 0.5);
		if (j > half_len)
			j = half_len;
		target_freq = filt_desc->exc_bands[i].hi_freq;
		k = (si4) ((target_freq / delta) + 0.5);
		if (k > half_len)
			k = half_len;
		for (; j <= k; ++j)
			filt[j] = 0.0;
	}

	/* link included and excluded bands with cosine functions */		
	for (i = 0; i <= half_len; ++i) {
		if (filt[i] == 0.5) {
			for (j = i; i <= half_len; ++i)
				if (filt[i] != 0.5)
					break;
			cos_sum = 0.0;
			if (j == 0) {
				if (filt[i] == 1.0)
					cos_sum = M_PI;
			} else if (!filt[j - 1])
				cos_sum = M_PI;
			delta = M_PI / (sf8) (i - j + 1);
			for (; j < i; ++j) {
				cos_sum += delta;
				filt[j] = (cos(cos_sum) + 1.0) / 2.0;
			}
		}
	}
	
	/* arrange filter to match output of realft() */
	target_freq = filt[half_len];
	(void) memmove((void *) (filt + 2), (void *) (filt + 1), (size_t) (half_len - 2) * sizeof(sf8));
	filt[1] = target_freq;			
	
	return;
}


 void	make_window(wind, half_len)
sf8	*wind;
si4	half_len;
{
	si4	i;
	sf8	sum, delta, len;

	len = 2.0 * (sf8) half_len;
	sum = M_PI;
	delta = M_PI / (sf8) (half_len - 1);
	for (i = 0; i < half_len; ++i) {		/* for efficiency the window includes a scaling factor */
		wind[i] = (cos(sum) + 1.0) / len;	/* of (2/len) which would otherwise have to be applied */
		sum += delta;				/* to the data after the inverse fft */
	}						
	return;
}


 void 	filter_buffer(buf, filt, wind, len)
sf8	*buf, *filt, *wind;
si4	len;
{
	register si4	i;
	si4		half_len;
	register sf8	*t1, *t2;
	 void	realft();

	/* apply window */
	t1 = buf; t2 = wind;
	half_len = len >> 1;
	for (i = half_len; i--;)
		*t1++ *= *t2++;
	for (i = half_len; i--;)
		*t1++ *= *--t2;
	
	/* forward fft */
	realft(NRC_ARR(buf), len, FORWARD);

	/* apply filter */
	t1 = buf; t2 = filt;
	*t1++ *= *t2++; *t1++ *= *t2++;
	for (i = half_len; --i;) {
		*t1++ *= *t2;
		*t1++ *= *t2++;
	}

	/* inverse fft */
	realft(NRC_ARR(buf), len, INVERSE);

	return;
}


 void	realft(data, n, isign)	/* this routine is from "Numerical Recipes in C" (2nd edition) */
register sf8	*data;
si4		n, isign;
{
	register si4	i, i1, i2, i3, i4, np3; 
	register sf8	c1, c2, h1r, h1i, h2r, h2i; 
	register sf8	wr, wi, wpr, wpi, wtemp, theta;
	 void	four1(); 

	theta = M_PI / (sf8) (n >> 1);
	c1 = 0.5;
	
	if (isign == FORWARD) {
		c2 = -0.5; 
		four1(data, n >> 1, FORWARD);
	} else { 
		c2 = 0.5;
		theta = -theta;
	}
	wtemp = sin(0.5 * theta); 
	wpr = -2.0 * wtemp * wtemp; 
	wpi = sin(theta); 
	wr = 1.0 + wpr; 
	wi = wpi; 
	np3 = n + 3; 
	for (i = 2; i <= (n >> 2); i++) {
		i4 = 1 + (i3 = np3 - (i2 = 1 + (i1 = i + i - 1))); 
		h1r = c1 * (data[i1] + data[i3]);
		h1i = c1 * (data[i2] - data[i4]);
		h2r = -c2 * (data[i2] + data[i4]); 
		h2i = c2 * (data[i1] - data[i3]); 
		data[i1] = h1r + wr * h2r - wi * h2i;
		data[i2] = h1i + wr * h2i + wi * h2r;
		data[i3] = h1r - wr * h2r + wi * h2i;
		data[i4] = -h1i + wr * h2i + wi * h2r; 
		wr = (wtemp = wr) * wpr - wi * wpi + wr;
		wi = wi * wpr + wtemp * wpi + wi; 
	}
	if (isign == 1) {
		data[1] = (h1r = data[1]) + data[2];
		data[2] = h1r - data[2];
	} else {
		data[1] = c1 * ((h1r = data[1]) + data[2]); 
		data[2] = c1 * (h1r - data[2]);
		four1(data, n >> 1, INVERSE);
	}

	return;
}


 void	four1(data, nn, isign)	/* this routine is from "Numerical Recipes in C" (2nd edition) */
register sf8	*data;
si4		nn;
si4		isign; 
{ 
	register si4		i, j, n, mmax, m, istep; 
	register sf8		wtemp, wr, wpr, wpi, wi, theta;
	register sf8		tempr, tempi;

	n = nn << 1; 
	j = 1;
	for (i = 1; i < n; i += 2) {
		if (j > i) {
			tempr = data[j];
			data[j] = data [i];
			data [i] = tempr;
			tempi = data[j + 1];
			data[j + 1] = data [i + 1];
			data [i + 1] = tempi;
		}
		m = n >> 1; 
		while (m >= 2 && j > m) { 
			j -= m; 
			m >>= 1; 
		}
		j += m; 
	}

	mmax = 2; 
	while (n > mmax) {
		istep = mmax << 1; 
		theta = (sf8) isign * 2.0 * M_PI / (sf8) mmax;
		wtemp = sin(0.5 * theta); 
		wpr = -2.0 * wtemp * wtemp; 
		wpi = sin(theta); 
		wr = 1.0; 
		wi = 0.0; 
		for (m = 1; m < mmax; m += 2) {
			for (i = m; i <= n; i += istep) { 
				j = i + mmax;
				tempr = wr * data[j] - wi * data[j + 1];
				tempi = wr * data[j + 1] + wi * data[j]; 
				data[j] = data[i] - tempr; 
				data[j + 1] = data[i + 1] - tempi; 
				data[i] += tempr; 
				data[i + 1] += tempi; 
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi; 
		}
		mmax = istep; 
	}

	return;
}
