
#ifndef SPEED_IN
#define SPEED_IN


#define SPEED_TIME_INTERVAL	0.25		/* (floating point) sampling interval used to judge speed (in seconds, 0.25 recommended) */
#define REASONABLE_SPEED	65		/* (integer) maximum believable speed (roughly in cm/sec) */
#define MAXIMUM_SPEED		255		/* (integer) largest possible speed (should be greater than REASONABLE_SPEED) */

#define MAX_STILL_SPEED_DFLT		3	/* (integer) maximum speed to assume animal is not moving (in cm/sec) */
#define MIN_WALKING_SPEED_DFLT		9	/* (integer) minimum speed to assume animal is walking (in cm/sec) */
#define GLOBAL_SPEED_THRESH_DFLT	0	/* (integer) minimum speed to be used in making speed map (in cm/sec) */
#define NO_SPEED			-1.0	/* flag in speed map */


#endif
