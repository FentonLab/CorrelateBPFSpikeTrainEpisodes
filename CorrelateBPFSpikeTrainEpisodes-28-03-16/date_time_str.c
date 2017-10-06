#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "size_types.h"


void	date_time_str(str)
si1	*str;
{
	time_t		clk;
	struct tm	*time_ptr;
	si1		*c1, *c2;

	
	(void) time(&clk);
	time_ptr = localtime(&clk);
	(void) strftime(str, 64, "%A %B %e, %Y - %l:%M:%S %p (%Z)", time_ptr);

	c1 = str;
	while (*c1++ != '-');
	if (*++c1 == ' ') {
		c2 = c1;
		do {
			*c2 = *(c2 + 1);
		} while (*c2++ != '\0');
	}

	while (*c1 != 'P' && *c1 != 'A' && *c1 != '\0')
		++c1;
	if (*c1 != '\0' && *(c1 + 1) == 'M') {
		*c1++ += 32;
		*c1 += 32;
	}
	
		
	return;
}
