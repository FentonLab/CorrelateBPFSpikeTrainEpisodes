
#ifndef FILTER_IN
#define FILTER_IN


#include "size_types.h"


typedef struct {                                /* space between included and excluded bands */
        sf8	lo_freq, hi_freq;               /* will be connected with a cosine function */
} BAND;
typedef struct {                                /* n.b. If there is no band including the zero */
        si4     n_inc_bands, n_exc_bands;       /* frequency, the lowest frequency included/excluded  */
        BAND    *inc_bands, *exc_bands;         /* band will be connected to zero with a cosine */
} FILT_DESC;                                    /* function. Likewise, unless the highest frequency */
                                                /* in the bands exceeds the nyquist frequency, it will */
                                                /* be connected to that frequency by a cosine function */
#define FILT_BUFFER_LEN         4096
#define FORWARD                 1
#define INVERSE                 -1
#define NRC_ARR(x)              (x - 1)         /* "Numerical Recipes in C" functions like arrays to */
                                                /* be indexed from 1 instead of 0, the standard in C */

#endif
