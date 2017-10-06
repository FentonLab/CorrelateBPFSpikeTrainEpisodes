
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	read_app_map(sess)
SESSION	*sess;
{
	si1		*app_map_dir, app_map_path[256];
	si4		i, area, dummy;
	FILE		*fp;
	ui1		*am;
	extern si1	*get_env_var();
	void	bad_app_file(), bad_app_file2();


	app_map_dir = get_env_var("APP_MAP_DIR");
	(void) sprintf(app_map_path, "%s/%s", app_map_dir, sess->params->read_app_map_file);

	fp = fopen(app_map_path, "r");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCan't open the file \"%s\". Terminating program.\n\n", 7, app_map_path);
		exit(1);
	}

	area = sess->params->area;
	am = sess->apparatus->map[0];
	for (i = 0; i < area; ++i) {
		if (fscanf(fp, "%d", &dummy) == EOF)
			bad_app_file(sess);
		if (dummy == 0 || dummy == 1)
			am[i] = (ui1) dummy;
		else
			bad_app_file2(sess);
	}
	if (fscanf(fp, "%d", &dummy) != EOF)
		bad_app_file(sess);

	(void) fclose(fp);
	

	return;
}
		

void	write_app_map(sess)
SESSION	*sess;
{
	si1		*app_map_dir, app_map_path[256];
	si4		i, j, x_size, y_size;
	FILE		*fp;
	ui1		**am;
	extern si1	*get_env_var();


	app_map_dir = get_env_var("APP_MAP_DIR");
	(void) sprintf(app_map_path, "%s/%s", app_map_dir, sess->params->write_app_map_file);

	fp = fopen(app_map_path, "w");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCan't open the file \"%s\". Terminating program.\n\n", 7, app_map_path);
		exit(1);
	}

	x_size = sess->params->x_size;
	y_size = sess->params->y_size;
	am = sess->apparatus->map;
	for (i = 0; i < y_size; ++i) {
		for (j = 0; j < x_size; ++j)
			(void) fprintf(fp, "%d ", am[i][j]);
		(void) fprintf(fp, "\n");
	}

	(void) fclose(fp);
	

	return;
}


void	bad_app_file(sess)
SESSION		*sess;
{
	(void) fprintf(stderr, "%c\n\tThe apparatus specified in the file \"%s\" is not of the correct dimensions for the current analysis. Terminating program.\n\n", 7, sess->params->read_app_map_file);
	exit(1);
}


void	bad_app_file2(sess)
SESSION		*sess;
{
	(void) fprintf(stderr, "%c\n\tThe apparatus specified in the file \"%s\" contains incorrect entries. Terminating program.\n\n", 7, sess->params->read_app_map_file);
	exit(1);
}

