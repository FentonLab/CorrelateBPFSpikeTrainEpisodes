#include	<stdio.h>
#include	<stdlib.h>
#include	"size_types.h"
#include	"uff.h"
#include	"bpf.h"

#define         SHOW_DIRS   printf("\n\nHERE IS YOUR 'DIR' ENVIRONMENT\n\n"); system("env | grep DIR\n");

#define	POSITION_SAMPS_PER_SEC	50
#define	PRAHA_SAMPS_PER_SEC	50
#define	SUNY_SAMPS_PER_SEC	60
#define	NTSC_SAMPS_PER_SEC	60
#define	PAL_SAMPS_PER_SEC	50
#define MAX_NUMBER_OF_UNITS (MAX_PROBES*MAX_CLUSTS_PER_PROBE)
#define MAX_SPIKES_PER_TIMESTEP 40


typedef struct {
	ui1	rx;
	ui1	ry;
	ui1	bx;
	ui1	by;
	ui1	n_spikes[MAX_NUMBER_OF_UNITS];
	int	time[MAX_NUMBER_OF_UNITS][MAX_SPIKES_PER_TIMESTEP];
	} TS_RECORD;
