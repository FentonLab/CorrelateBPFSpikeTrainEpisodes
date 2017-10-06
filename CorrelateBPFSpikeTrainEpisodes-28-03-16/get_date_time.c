
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"
#include "uff.h"


void	get_date_time(sess, header)
SESSION	*sess;
si1	*header;
{
	si4	i, hour, day_of_month;
	si1	*dt_str, *meridian, *day_of_week;
	si1	*month, *minute, *year, c;

	for (i = UFF_CREATE_DATE_OFFSET; ; ++i) {
		c = header[i + 10];
		header[i + 10] = '\0';
		if (!strcmp(header + i, "Date/Time:")) {
			header[i + 10] = c;
			break;
		}
		header[i + 10] = c;
	}

	dt_str = header + i;

	dt_str += 11;
	dt_str[3] = '\0';
	if (!strcmp(dt_str, "Sun"))
		day_of_week = "Sunday";
	else if (!strcmp(dt_str, "Mon"))
		day_of_week = "Monday";
	else if (!strcmp(dt_str, "Tue"))
		day_of_week = "Tuesday";
	else if (!strcmp(dt_str, "Wed"))
		day_of_week = "Wednesday";
	else if (!strcmp(dt_str, "Thu"))
		day_of_week = "Thursday";
	else if (!strcmp(dt_str, "Fri"))
		day_of_week = "Friday";
	else if (!strcmp(dt_str, "Sat"))
		day_of_week = "Saturday";
	else {
		(void) fprintf(stderr, "%cBad weekday string in file %s.\n", 7, sess->name_str);
		return;
	}
		
	dt_str += 4;
	dt_str[3] = '\0';
	if (!strcmp(dt_str, "Jan"))
		month = "January";
	else if (!strcmp(dt_str, "Feb"))
		month = "February";
	else if (!strcmp(dt_str, "Mar"))
		month = "March";
	else if (!strcmp(dt_str, "Apr"))
		month = "April";
	else if (!strcmp(dt_str, "May"))
		month = "May";
	else if (!strcmp(dt_str, "Jun"))
		month = "June";
	else if (!strcmp(dt_str, "Jul"))
		month = "July";
	else if (!strcmp(dt_str, "Aug"))
		month = "August";
	else if (!strcmp(dt_str, "Sep"))
		month = "September";
	else if (!strcmp(dt_str, "Oct"))
		month = "October";
	else if (!strcmp(dt_str, "Nov"))
		month = "November";
	else if (!strcmp(dt_str, "Dec"))
		month = "December";
	else {
		(void) fprintf(stderr, "%cBad month string in file %s.\n", 7, sess->name_str);
		return;
	}

	dt_str += 4;
	dt_str[2] = '\0';
	day_of_month = atoi(dt_str);

	dt_str += 3;
	dt_str[2] = '\0';
	hour = atoi(dt_str);
	if (hour <= 12)
		meridian = "am";
	else {
		hour -= 12;
		meridian = "pm";
	}

	dt_str += 3;
	dt_str[2] = '\0';
	minute = dt_str;

	dt_str += 6;
	dt_str[4] = '\0';
	year = dt_str;

	(void) sprintf(sess->date_str, "%s, %s %d, %s", day_of_week, month, day_of_month, year);
	(void) sprintf(sess->time_str, "%d:%s %s", hour, minute, meridian);

	return;
}
