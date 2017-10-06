
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "size_types.h"
#include "sess_anal.h"


void	get_data(sess)
SESSION	*sess;
{
	struct stat	stat_struct;
	ui4		n_read;
	si1		sess_path[256], *data_dir;
	extern si1	*get_env_var();
	FILE		*fp;
	void		*gmem();


	/* read entire file into memory */

	if (sess->opts->km_file){
		data_dir = get_env_var("KM_DIR");
	}else if (sess->opts->ts_file){
		data_dir = get_env_var("TS_DIR");
	}else if (sess->opts->tts_file){
		data_dir = get_env_var("TTS_DIR");
	}else if (sess->opts->bpf_room_file || sess->opts->bpf_arena_file || sess->opts->bpf_acute_file){
		data_dir = get_env_var("BPF_DIR");
	}else{
		data_dir = get_env_var("UFF_DIR");
	}

	(void) sprintf(sess_path, "%s/%s", data_dir, sess->name_str); 
	if ((fp = fopen(sess_path, "r")) == NULL) {
		(void) fprintf(stderr, "%c\n\tCould not open file \"%s\". Terminating program.\n\n", 7, sess_path); 
		exit(1); 
	}
	(void) fstat(fileno(fp), &stat_struct);

	sess->file_len = (ui4) stat_struct.st_size;
	sess->data = (ui1 *) gmem((size_t) sess->file_len, sizeof(ui1));
	n_read = (ui4) fread((void *) sess->data, sizeof(ui1), (size_t) sess->file_len, fp);
	if (n_read != sess->file_len) {
		(void) fprintf(stderr, "%c\n\tError reading file \"%s\". read %d of %d bytes. Terminating program.\n\n", 7, sess_path, n_read, sess->file_len); 
		exit(1); 
	}
	(void) fclose(fp);

	return;
}
