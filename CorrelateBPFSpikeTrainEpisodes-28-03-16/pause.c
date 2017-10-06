
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "size_types.h"


void	pause(duration)
sf8	duration;
{
	struct timeval	the_time;
	void		*nothing;
	si4		start_secs, current_secs;
	sf8		current_time, target_time;

	(void) gettimeofday(&the_time, nothing);

	start_secs = the_time.tv_sec;
	target_time = ((sf8) the_time.tv_usec / 1000000.0) + duration;

	do {
		(void) gettimeofday(&the_time, nothing);
		current_secs = the_time.tv_sec - start_secs;
		current_time = (sf8) current_secs + ((sf8) the_time.tv_usec / 1000000.0);
	} while (current_time < target_time);

	return;
}

