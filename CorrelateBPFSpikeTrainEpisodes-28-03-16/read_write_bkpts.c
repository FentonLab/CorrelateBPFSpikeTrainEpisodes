
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"
#include "sess_anal.h"


void	read_bkpts(sess)
SESSION	*sess;
{
	si1		*bkpt_dir, bkpt_path[256];
	si4		i, j, n_bkpts, probe, clust;
	FILE		*fp;
	CELL		*cp;
	extern si1	*get_env_var();
	void	bad_bkpts_file();


	bkpt_dir = get_env_var("BKPT_DIR");
	(void) sprintf(bkpt_path, "%s/%s", bkpt_dir, sess->params->read_bkpts_file);

	fp = fopen(bkpt_path, "r");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCan't open the file \"%s\". Terminating program.\n\n", 7, bkpt_path);
		exit(1);
	}

	for (i = 0; i < sess->num_cells; ++i) {
		cp = sess->cells[i];

		if (fscanf(fp, "%d%d\n", &probe, &clust) != 2)
			bad_bkpts_file(sess);
		if (cp->probe != (ui1) probe || cp->clust != (ui1) clust)
			bad_bkpts_file(sess);

		if (fscanf(fp, "%d", &n_bkpts) != 1)
			bad_bkpts_file(sess);
		if (n_bkpts < 0 || n_bkpts > N_BKPTS)
			bad_bkpts_file(sess);
		cp->num_rate_bkpts = n_bkpts;

		for (j = 0; j < n_bkpts; ++j)
			if (fscanf(fp, "%f", cp->rate_bkpts + j) != 1)
				bad_bkpts_file(sess);
	}

	for (i = 0; i < sess->num_probes; ++i) {
		cp = sess->probes[i];

		if (fscanf(fp, "%d", &probe) != 1)
			bad_bkpts_file(sess);
		if (cp->probe != (ui1) probe)
			bad_bkpts_file(sess);

		if (fscanf(fp, "%d", &n_bkpts) != 1)
			bad_bkpts_file(sess);
		if (n_bkpts < 0 || n_bkpts > N_BKPTS)
			bad_bkpts_file(sess);
		cp->num_rate_bkpts = n_bkpts;

		for (j = 0; j < n_bkpts; ++j)
			if (fscanf(fp, "%f", cp->rate_bkpts + j) != 1)
				bad_bkpts_file(sess);
	}


	(void) fclose(fp);
	

	return;
}
		

void	write_bkpts(sess)
SESSION	*sess;
{
	si4		i, j;
	si1		*bkpt_dir, bkpt_path[256];
	FILE		*fp;
	CELL		*cp;
	extern si1	*get_env_var();


	bkpt_dir = get_env_var("BKPT_DIR");
	(void) sprintf(bkpt_path, "%s/%s", bkpt_dir, sess->params->write_bkpts_file);

	fp = fopen(bkpt_path, "w");
	if (fp == NULL) {
		(void) fprintf(stderr, "%c\n\tCan't open the file \"%s\". Terminating program.\n\n", 7, bkpt_path);
		exit(1);
	}

	for (i = 0; i < sess->num_cells; ++i) {
		cp = sess->cells[i];
		(void) fprintf(fp, "%d\t%d\n", cp->probe, cp->clust);
		(void) fprintf(fp, "%d\n", cp->num_rate_bkpts);
		for (j = 0; j < cp->num_rate_bkpts; ++j)
			(void) fprintf(fp, "%f\n", cp->rate_bkpts[j]);
		(void) fprintf(fp, "\n");
	}

	for (i = 0; i < sess->num_probes; ++i) {
		cp = sess->probes[i];
		(void) fprintf(fp, "%d\n", cp->probe);
		(void) fprintf(fp, "%d\n", cp->num_rate_bkpts);
		for (j = 0; j < cp->num_rate_bkpts; ++j)
			(void) fprintf(fp, "%f\n", cp->rate_bkpts[j]);
		(void) fprintf(fp, "\n");
	}

	(void) fclose(fp);
	

	return;
}
		

void	bad_bkpts_file(sess)
SESSION		*sess;
{
	(void) fprintf(stderr, "%c\n\tThe breakpoints listed in the file \"%s\" do not match those necessary for the current data file (\"%s\"). Terminating program.\n\n", 7, sess->params->read_bkpts_file, sess->name_str);
	exit(1);
}
