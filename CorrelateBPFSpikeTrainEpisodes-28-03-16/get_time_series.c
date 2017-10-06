#include <stdio.h>
#include "track.h"

int get_time_series(file, track, event)
	char	*file;
	TRACK	**track;
	int	**event;
{
	FILE 	*fp;
	static 	TRACK 	*t;
	static int	*e;
	unsigned char x, y, s;
	unsigned int	i, j, n, spk_time;
	char	line[256], *get_line();
	long	data_offset;

	fp = fopen(file, "r");
	if(fp == NULL){
		printf("Can't open %s\n", file);
		exit(-1);
	}

	if(fscanf(fp,"%d\n", &n) != EOF){
		n--; // to account for the 1st line
		for(i = 0; i < n; i++){	
			get_line(fp, line);
		}
	}else{
		printf("file is corrupt\n");
		exit(-1);
	}

	data_offset = ftell(fp);
	n = 0;
	while(fread(&x,sizeof(unsigned char), 1, fp) != 0){
		fread(&y,sizeof(unsigned char), 1, fp);
		fread(&s,sizeof(unsigned char), 1, fp);
	//	printf("%d\t%d\t%d\t%d\n",n, x,y,s);

		for(i = 0; i < s; i++){
			fread(&spk_time,sizeof(unsigned int), 1, fp);
		}
		n++;	// count the number of data lines
	}
	t = (TRACK *)calloc(n, sizeof(TRACK));
	e = (int *)calloc(n, sizeof(int));
	if((t == NULL) || (e == NULL)){
		fprintf(stderr,"Can't allocate memory in get_time_series\n");
		exit(-1);
	}
	*track = t;
	*event = e;

	fseek(fp, data_offset, SEEK_SET);
	for(i=0; i < n; i++){
		fread(&(t[i].x),sizeof(unsigned char), 1, fp);
		fread(&(t[i].y),sizeof(unsigned char), 1, fp);
		fread(&(e[i]),sizeof(unsigned char), 1, fp);
	//	printf("%d\t%d\t%d\t%d\n",i, t[i].x,t[i].y,e[i]);

		for(j = 0; j < e[i]; j++){
			fread(&spk_time,sizeof(unsigned int), 1, fp);
		}
	}
	return (n);
}

int get_timed_time_series(file, track, event)
	char	*file;
	TRACK	**track;
	int	**event;
{
	FILE 	*fp;
	static 	TRACK 	*t;
	static int	*e;
	unsigned char x, y, s;
	unsigned int	i, j, n, time spk_time;
	char	line[256], *get_line();
	long	data_offset;

	fp = fopen(file, "r");
	if(fp == NULL){
		printf("Can't open %s\n", file);
		exit(-1);
	}

	if(fscanf(fp,"%d\n", &n) != EOF){
		n--; // to account for the 1st line
		for(i = 0; i < n; i++){	
			get_line(fp, line);
		}
	}else{
		printf("file is corrupt\n");
		exit(-1);
	}

	data_offset = ftell(fp);
	n = 0;
	while(fread(&time, sizeof(unsigned int), 1, fp) != 0){
		fread(&x,sizeof(unsigned char), 1, fp);
		fread(&y,sizeof(unsigned char), 1, fp);
		fread(&s,sizeof(unsigned char), 1, fp);
	//	printf("%d\t%d\t%d\t%d\t%d\n",n, t, x,y,s);

		for(i = 0; i < s; i++){
			fread(&spk_time,sizeof(unsigned int), 1, fp);
		}
		n++;	// count the number of data lines
	}
	t = (TRACK *)calloc(n, sizeof(TRACK));
	e = (int *)calloc(n, sizeof(int));
	if((t == NULL) || (e == NULL)){
		fprintf(stderr,"Can't allocate memory in get_time_series\n");
		exit(-1);
	}
	*track = t;
	*event = e;

	fseek(fp, data_offset, SEEK_SET);
	for(i=0; i < n; i++){
		fread(&(t[i].t),sizeof(unsigned int), 1, fp);
		fread(&(t[i].x),sizeof(unsigned char), 1, fp);
		fread(&(t[i].y),sizeof(unsigned char), 1, fp);
		fread(&(e[i]),sizeof(unsigned char), 1, fp);
	//	printf("%d\t%d\t%d\t%d\t%d\n",i, t[i].t,t[i].x,t[i].y,e[i]);

		for(j = 0; j < e[i]; j++){
			fread(&spk_time,sizeof(unsigned int), 1, fp);
		}
	}
	return (n);
}
