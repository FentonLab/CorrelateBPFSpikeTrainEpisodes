#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"sess_anal.h"
#include	"bpf.h"
#include	"size_types.h"
		
ui1	*ReadBPFHeader(SESSION *sess)
{
	ui1	VerifyRecordFormat(SESSION *, ui1 *, si1 *, si1 *, ui1);
	ui1	*FindHeaderStart(), *FindHeaderEnd(), *FindSection(), *FindSectionEnd();
	void	GetRecordTypesUsed(); 
	ui1	*dp, *BPFRecordTypes;
	ui1	*HeaderStart, *HeaderEnd, *SectionStart, *SectionEnd, *RecordStart;
	si1	*strstr();
	si1	KeyWordString[1024], Encoding[1024], **Parameters, *Strings;
	si4	Type, NumberOfParameters;
	si4	Day, Month, Year, Hours, Minutes;

	dp = sess->data;
	BPFRecordTypes = sess->BPFRecordTypes;

	// set default values


	// Look for header beginning
	HeaderStart = FindHeaderStart(dp);
	if(HeaderStart == NULL){
		fprintf(MyStdErr,"\nCan't find start of header (%s).\nWrong File Format.\n", HEADER_BEGIN_LABEL);
		exit(-1);
	}
	dp = (HeaderStart + strlen(HEADER_BEGIN_LABEL));
	// Check to see if it's the BPF header or not
	sscanf((si1 *)dp,"%s", KeyWordString);
	if(strstr(KeyWordString, BPF_HEADER_TYPE) == NULL){
		fprintf(MyStdErr,"This is not a BPF file. It is a %s file.\n", KeyWordString);
		exit(-1);
	}

	// Section: DATABASE_INFORMATION
	dp = HeaderStart;

	SectionStart = FindSection(dp, SECTION_DATABASE_INFORMATION);
	if(SectionStart == NULL){
		fprintf(MyStdErr, "\nCan't find Header section %s.\nWrong File Format.\n", SECTION_DATABASE_INFORMATION);
		exit(-1);
	}
	// Date
	if(!FindKeyWord(SectionStart, DATE_LABEL, KeyWordString, Encoding)){
		fprintf(MyStdErr,"\nCan't find keyWord (%s).\nWrong File Format.\n", DATE_LABEL);
		return 0;
	}
	Type = GetType(KeyWordString);
	switch(Type){
	case -1:
		fprintf(MyStdErr,"KeyWord.integer format not respected in: %s\nWrong File Format.\n", KeyWordString);
		return(0);

	case 0:	// ASCII encoded
		NumberOfParameters = GetASCIIEncoding(Encoding, &Parameters);
		if(!NumberOfParameters){
			fprintf(MyStdErr,"(%s).\nWrong File Format.\n", DATE_LABEL);
			return 0;
		}
		if(NumberOfParameters == 1){
			sscanf(Parameters[0],"%d/%d/%d", &Day, &Month, &Year);
		}else if(NumberOfParameters == 3){
			sscanf(Parameters[0],"%d/ %d/ %d", &Day, &Month, &Year);
		}else{
			fprintf(MyStdErr,"%s format must be dd/mm/yyyy\nWrong File Format.\n", DATE_LABEL);
			return(0);
		}
		(void) sprintf(sess->date_str, "(dd/mm/yyyy) %s", Parameters[0]);
		break;

	default:
		fprintf(MyStdErr,"%s must be ASCII encoded\nWrong File Format.\n", DATE_LABEL);
		return(0);
	}
	// Time
	
	dp = SectionStart;
	if(!FindKeyWord(dp, TIME_LABEL, KeyWordString, Encoding)){
		fprintf(MyStdErr,"\nCan't find keyWord (%s).\nWrong File Format.\n", TIME_LABEL);
		return 0;
	}
	Type = GetType(KeyWordString);
	switch(Type){
	case -1:
		fprintf(MyStdErr,"KeyWord.integer format not respected in: %s\nWrong File Format.\n", KeyWordString);
		return(0);

	case 0:	// ASCII encoded
		NumberOfParameters = GetASCIIEncoding(Encoding, &Parameters);
		if(!NumberOfParameters){
			fprintf(MyStdErr,"(%s).\nWrong File Format.\n", TIME_LABEL);
			return 0;
		}
		if(NumberOfParameters == 1){
			sscanf(Parameters[0],"%d:%d", &Hours, &Minutes);
		}else{
			fprintf(MyStdErr,"%s format must be hh:mm\nWrong File Format.\n", TIME_LABEL);
			return(0);
		}
		(void) sprintf(sess->time_str, "(hh:mm) %s", Parameters[0]);
		break;

	default:
		fprintf(MyStdErr,"%s must be ASCII encoded\nWrong File Format.\n", TIME_LABEL);
		return(0);
	}

	SectionEnd = FindSectionEnd(SectionStart);
	if(SectionEnd == NULL)
		fprintf(MyStdErr,"\nCan't find end of Header Section (%s).\nWrong File Format.\n", SECTION_DATABASE_INFORMATION);

	// Section: SETUP_INFORMATION
	dp = HeaderStart;
	SectionStart = FindSection(dp, SECTION_SETUP_INFORMATION);
	if(SectionStart == NULL){
		fprintf(MyStdErr, "\nCan't find Header section %s.\nWrong File Format.\n", SECTION_SETUP_INFORMATION);
		exit(-1);
	}

	dp = SectionStart;
	GetRecordTypesUsed(sess, dp);

	SectionEnd = FindSectionEnd(SectionStart);
	if(SectionEnd == NULL)
		fprintf(MyStdErr,"\nCan't find end of Header Section (%s).\nWrong File Format.\n", SECTION_SETUP_INFORMATION);

	// Section: RECORD_FORMAT

	// Set these as defaults. They are optional
	sess->OffsetRoomZ1 = sess->OffsetArenaZ1 = 0;
	sess->OffsetRoomAng = sess->OffsetArenaAng = 0;
	sess->OffsetRoomAngX = sess->OffsetArenaAngX = 0;
	sess->OffsetRoomAngY = sess->OffsetArenaAngY = 0;

	dp = HeaderStart;
	SectionStart = FindSection(dp, SECTION_RECORD_FORMAT_INFORMATION);
	if(SectionStart == NULL){ 
		fprintf(MyStdErr, "\nCan't find section %s. Wrong data format\n", SECTION_RECORD_FORMAT_INFORMATION);
		return (0);
	}

	// EEG RECORD ID
	if(sess->BPFRecordTypeNumbers[EEG_BPF_REC_TYPE]){
                sess->BPFRecordSizes[EEG_BPF_REC_TYPE] = sess->BPFRecordTypeNumbers[EEG_BPF_REC_TYPE] * EEG_BPF_REC_DATA_SIZE + EEG_BPF_REC_ID_SIZE;
		if(!VerifyRecordFormat(sess, SectionStart, EEG_ID, EEG_RECORD_FORMAT, EEG_BPF_REC_TYPE))
			exit(-1);
	}

	// SINGLE ELECTRODE RECORD ID
	if(sess->BPFRecordTypeNumbers[SINGLE_BPF_REC_TYPE]){
		if(!VerifyRecordFormat(sess, SectionStart, SINGLE_ID, SINGLE_RECORD_FORMAT, SINGLE_BPF_REC_TYPE))
			exit(-1);
	}


	// STEREOTRODE RECORD ID
	if(sess->BPFRecordTypeNumbers[STEREO_BPF_REC_TYPE]){
		if(!VerifyRecordFormat(sess, SectionStart, STEREO_ID, STEREO_RECORD_FORMAT, STEREO_BPF_REC_TYPE))
			exit(-1);
	}

	// TETROTRODE RECORD ID
	if(sess->BPFRecordTypeNumbers[TETRODE_BPF_REC_TYPE]){
		if(!VerifyRecordFormat(sess, SectionStart, TETRODE_ID, TETRODE_RECORD_FORMAT, TETRODE_BPF_REC_TYPE))
			exit(-1);
	}

	// SYNC RECORD ID
	if(sess->BPFRecordTypeNumbers[SYNC_BPF_REC_TYPE]){
		if(!VerifyRecordFormat(sess, SectionStart, SYNC_ID, SYNC_RECORD_FORMAT, SYNC_BPF_REC_TYPE))
			exit(-1);
	}

	// ROOM POSITION RECORD ID
	if(sess->BPFRecordTypeNumbers[ROOM_POSITION_BPF_REC_TYPE]){
		if(!VerifyRecordFormat(sess, SectionStart, ROOM_POSITION_ID, ROOM_POSITION_RECORD_FORMAT, ROOM_POSITION_BPF_REC_TYPE))
			exit(-1);

		// Determine what data are in the file
	        if(FindKeyWord(SectionStart, ROOMX1, KeyWordString, Encoding)){
			// assume thes are present
			sess->OffsetRoomX1 = BPF_POS_REC_X_OFFSET;
			sess->OffsetRoomY1 = BPF_POS_REC_Y_OFFSET;
			
			// look for optional Z and Angle data
			Strings = strstr(Encoding, ROOMZ1);
			if(Strings != NULL){
				sess->OffsetRoomZ1 = BPF_POS_REC_Z_OFFSET;
        	        	// fprintf(MyStdErr,"\n(%s).\n", ROOMZ1);

				// found RoomZ1 now look for RoomAng	
				Strings = strstr(Encoding, ROOMANG);
				if(Strings != NULL){
					sess->OffsetRoomAng = sess->OffsetRoomZ1 + 1;
        	        		// fprintf(MyStdErr,"\n(Z %s).\n", ROOMANG);
				
					// found RoomAng now look for RoomAngX	
					Strings = strstr(Encoding, ROOMANGX);
					if(Strings != NULL){
						sess->OffsetRoomAngX = sess->OffsetRoomAng + 2;
        	        			// fprintf(MyStdErr,"\n(Z %s).\n", ROOMANGX);
					}

					// found RoomAngX now look for RoomAngY	
					Strings = strstr(Encoding, ROOMANGY);
					if(Strings != NULL){
						sess->OffsetRoomAngY = sess->OffsetRoomAngX + 2;
        	        			// fprintf(MyStdErr,"\n(Z %s).\n", ROOMANGY);
					}
				}
			}else {
				// did not find RoomZ1 look for RoomAng	
				Strings = strstr(Encoding, ROOMANG);
				if(Strings != NULL){
					sess->OffsetRoomAng = BPF_POS_REC_ANG_OFFSET;
        	        		// fprintf(MyStdErr,"\n(NOZ %s).\n", Strings);
				}
			}
				
        	}
	}

	// ARENA POSITION RECORD ID
	if(sess->BPFRecordTypeNumbers[ARENA_POSITION_BPF_REC_TYPE]){
		if(!VerifyRecordFormat(sess, SectionStart, ARENA_POSITION_ID, ARENA_POSITION_RECORD_FORMAT, ARENA_POSITION_BPF_REC_TYPE))
			exit(-1);

		// Determine what data are in the file
	        if(FindKeyWord(SectionStart, ARENAX1, KeyWordString, Encoding)){
			// assume these are present
			sess->OffsetArenaX1 = BPF_POS_REC_X_OFFSET;
			sess->OffsetArenaY1 = BPF_POS_REC_Y_OFFSET;
			
			// look for optional Z and Angle data
			Strings = strstr(Encoding, ARENAZ1);
			if(Strings != NULL){
				sess->OffsetArenaZ1 = BPF_POS_REC_Z_OFFSET;
        	        	// fprintf(MyStdErr,"\n(%s).\n", ARENAZ1);

				// found ArenaZ1 now look for ArenaAng	
				Strings = strstr(Encoding, ARENAANG);
				if(Strings != NULL){
					sess->OffsetArenaAng = sess->OffsetArenaZ1 + 1;
        	        		// fprintf(MyStdErr,"\n(Z %s).\n", ARENAANG);
				
					// found ArenaAng now look for ArenaAngX	
					Strings = strstr(Encoding, ARENAANGX);
					if(Strings != NULL){
						sess->OffsetArenaAngX = sess->OffsetArenaAng + 2;
        	        			// fprintf(MyStdErr,"\n(Z %s).\n", ARENAANGX);
					}

					// found ArenaAngX now look for ArenaAngY	
					Strings = strstr(Encoding, ARENAANGY);
					if(Strings != NULL){
						sess->OffsetArenaAngY = sess->OffsetArenaAngX + 2;
        	        			// fprintf(MyStdErr,"\n(Z %s).\n", ARENAANGY);
					}
				}
			}else {
				// did not find ArenaZ1 look for ArenaAng	
				Strings = strstr(Encoding, ARENAANG);
				if(Strings != NULL){
					sess->OffsetArenaAng = BPF_POS_REC_ANG_OFFSET;
        	        		// fprintf(MyStdErr,"\n(NOZ %s).\n", Strings);
				}
			}
		}
	}

        // KEY EVENT RECORD ID
	if(sess->BPFRecordTypeNumbers[KEY_EVENT_BPF_REC_TYPE]){
		VerifyRecordFormat(sess, SectionStart, KEY_EVENT_ID, KEY_EVENT_RECORD_FORMAT, KEY_EVENT_BPF_REC_TYPE);
                // This record may not be defined
        }

        // INPUT EVENT RECORD ID
	// if(sess->BPFRecordTypeNumbers[INPUT_EVENT_BPF_REC_TYPE]){
        // This record may not be defined and is typically not counted so always verify
		VerifyRecordFormat(sess, SectionStart, INPUT_EVENT_ID, INPUT_EVENT_RECORD_FORMAT, INPUT_EVENT_BPF_REC_TYPE);
        // }

        // OUTPUT EVENT RECORD ID
	// if(sess->BPFRecordTypeNumbers[OUTPUT_EVENT_BPF_REC_TYPE]){
        // This record may not be defined and is typically not counted so always verify
		VerifyRecordFormat(sess, SectionStart, OUTPUT_EVENT_ID, OUTPUT_EVENT_RECORD_FORMAT, OUTPUT_EVENT_BPF_REC_TYPE);
        // }

	SectionEnd = FindSectionEnd(SectionStart);
	if(SectionEnd == NULL)
		fprintf(MyStdErr,"\nCan't find end of Header Section (%s).\nWrong File Format.\n", SECTION_RECORD_FORMAT_INFORMATION);


	HeaderEnd = FindHeaderEnd(SectionEnd);
	if(HeaderEnd == NULL){
		fprintf(MyStdErr, "\nCan't find end of header. Wrong data format\n");
		return (0);
	}
	sess->bpf_data = HeaderEnd;

	return(HeaderEnd);
}

ui1     *FindHeaderStart(ui1 *data)
{
        si1 String[1024], Line[1024], *HeaderStart;
	ui1 *dp;

	dp = data;
        while(sscanf((si1 *)dp, "%s", String) == 1){
                HeaderStart = (si1 *)strstr(String, HEADER_BEGIN_LABEL);
		if(HeaderStart != NULL){
                        return(dp + (HeaderStart - String));
                }       
        	sscanfLine(dp, Line);
                dp += strlen(Line);
        }       

        fprintf(MyStdErr,"Can't find BPF Header begin label: %s\n", HEADER_BEGIN_LABEL);
	return(NULL);
}       

ui1	*FindHeaderEnd(ui1 *data)
{
	si1 String[1024], Line[1024];
	ui1 *dp;
	
	dp = data;
	
	while(sscanfLine(dp, Line) != '\0'){
		dp = (ui1 *)strstr((si1 *)dp, Line);
		dp += strlen(Line);
		sscanf(Line, "%s", String);
		if(strstr(String, HEADER_END_LABEL) != NULL){
			return(dp);
		}
	}
	
	fprintf(MyStdErr,"Can't find BPF Header end label: %s\n", HEADER_END_LABEL);
	return(NULL);
}

ui1	*FindSection(ui1 *dp, ui1 *Section)
{
	si1	Flag[1024];

	while (sscanf((si1 *)dp,"%s", Flag) != EOF){
		dp = (ui1 *)strstr((si1 *)dp, Flag);
		dp += strlen(Flag);
		if(Comment(dp, Flag)) continue;

		if(!strcmp(Flag, SECTION_BEGIN_LABEL)){
			sscanf((si1 *)dp,"%s", Flag);
			dp = (ui1 *)strstr((si1 *)dp, Flag);
			dp += strlen(Flag);
			if(!strcmp(Flag, (si1 *)Section)){
				return(dp);
			}
		}
	}
	return NULL;
}

si4	FindKeyWord(ui1 *dp, si1 *KeyWord, si1 *KeyWordString, si1 *Encoding)
{
	while (sscanf((si1 *)dp,"%s", KeyWordString) != EOF){
		dp = (ui1 *)strstr((si1 *)dp, KeyWordString);
		dp += strlen(KeyWordString);
		if(Comment(dp, KeyWordString)) continue;
		if(SectionEnd(KeyWordString)) break;

		if(strstr(KeyWordString, KeyWord) != NULL){
			(void)sscanfLine(dp, Encoding);
			return(1);
		}
	}
	return 0;
}

ui1	*FindSectionEnd(ui1 *dp)
{
	si1	Line[1024], String[1024];

	while (sscanfLine(dp, Line) != EOF){
		dp = (ui1 *)strstr((si1 *)dp, Line);
		dp+= strlen(Line);
		sscanf(Line,"%s",String);
		if(SectionEnd(String))
			return(dp);
	}
	return(NULL);
}

si4	Comment(ui1 *dp, si1 *String)
{
	si1	Line[1024];
	if(!strcmp(String, COMMENT)){
		sscanfLine((si1 *)dp, Line);
		dp += strlen(Line);
		return(1);
	}else{
		return(0);
	}
}

si4	SectionEnd(si1 *String)
{
	if(!strcmp(String, SECTION_END_LABEL))
		return(1);
	return(0);
}

si4	GetType(si1 *KeyWord)
{
	si1 *TypeString;
	si4	Type = -1;
	
	TypeString = (si1 *)strchr(KeyWord, TYPE_PREFIX);
	if(TypeString == NULL)
		return(-1);
	
	sscanf(TypeString+1,"%d", &Type);	// +1 to step past the '.'
	return(Type);
}

si4	GetBinaryEncoding(si1 *Encoding, si1 *P)
{
	si4 i, n; 
	static si1 String[1024], *TerminateString, *Line;

	//  The character pair (' must start the Binary Encoding
	if((Line = (si1 *)strstr(Encoding, BINARY_STARTING_MARK)) == NULL){
		fprintf(MyStdErr,"a (' must start binary encodings.\n");
		return (0);
	}
	Line+= strlen(BINARY_STARTING_MARK);;
	sscanf(Line,"%c",P);
	if(P == NULL){
		fprintf(MyStdErr,"Can't read Binary code.\n");
		return (0);
	}
	return(1);
}

si4	GetASCIIEncoding(si1 *Encoding, si1 ***P)
{
	si4 i, n; 
	static si1 String[1024], *TerminateString, *Line, **Parameters;

	
	Parameters = (si1 **)calloc(MAX_NUMBER_BPF_FORMAT_PARAMETERS, sizeof(si1 *));
	if(Parameters == NULL){
		fprintf(MyStdErr, "\nCan't allocate Parameters\n");
		return (0);
	}
	*P = Parameters;

	// a '(' must start the ASCII Encoding
	if((Line = (si1 *)strstr(Encoding, ASCII_STARTING_MARK )) == NULL){
		fprintf(MyStdErr,"a '(' must start ASCII encodings.\n");
		return (0);
	}
	Line+= strlen(ASCII_STARTING_MARK);;

	if(sscanf(Line, "%s", String) != 1){
		fprintf(MyStdErr,"No ASCII string to decode\n");
		return (0);
	}

	Parameters[0] = (si1 *)calloc(strlen(String + 1), sizeof(si1));
	strcpy(Parameters[0], String);

	if((TerminateString = (si1 *)strstr(Parameters[0], ASCII_TERMINATING_MARK )) != NULL){	// a ')' ends the encoding
		*TerminateString = '\0';
		return(1);
	}

	n = 1;

	Line += (strlen(String) + 1);
	while((*Line == ' ') || (*Line == '\t')){	// move to start of next string
		Line ++;
	}
	while(sscanf(Line, "%s", String) != EOF){
		if(!strcmp(String, ")"))
			return (n);
		Parameters[n] = (si1 *)calloc(strlen(String) + 1, sizeof(si1));
		strcpy(Parameters[n], String);
		if((TerminateString = (si1 *)strchr(Parameters[n], ')')) != NULL){ // a ')' indicates the end of the ASCII encoding
			*TerminateString = '\0' ;
			return(++n);
		}
		Line += (strlen(String) + 1);
		while((*Line == ' ') || (*Line == '\t')){	// move to start of next string
			Line ++;
		}
		n++;

		if(n == MAX_NUMBER_BPF_FORMAT_PARAMETERS){
			fprintf(MyStdErr,"Too many Parameters in Encoding of:\t");
			return(0);
		}
	}

	return (n);
}

ui1	VerifyRecordFormat(SESSION *sess, ui1 *SectionStart, si1 *id, si1 *RecordFormat, ui1 RecType){

	ui1	*dp, *RecordTypes;
	si1	KeyWordString[1024], Encoding[1024];
	ui4	*RecordSizes;
	si4	Type, GetNumberOfVoltageSamples(si1 *, ui1);
	
	// Get record type
	RecordTypes = sess->BPFRecordTypes;
        dp = SectionStart;
        if(!FindKeyWord(dp, id, KeyWordString, Encoding)){
		if(!strcmp(id, KEY_EVENT_ID) && !strcmp(id, INPUT_EVENT_ID) && !strcmp(id, OUTPUT_EVENT_ID))
                	fprintf(MyStdErr,"\nCan't find keyWord (%s).\nWrong File Format.\n", id);
                return 0;
        }
        Type = GetType(KeyWordString);   
        switch(Type){   
        case -1:        
                fprintf(MyStdErr,"KeyWord.integer format not respected in: %s\nWrong File Format.\n", KeyWordString);
                return 0;
                                
        case 0: // ASCII encoded
                fprintf(MyStdErr,"%s\nWrong File Format.\n", id);
                return 0;       

        case RECORD_ID_ENCODING: // 1 byte Binary encoded
                if(!GetBinaryEncoding(Encoding, (si1 *)(RecordTypes+RecType))){
                        fprintf(MyStdErr,"\nCan't get RECORD ID from header.\n");
                        return 0;
                }
                if(RecordTypes[RecType] != RecType){
                        fprintf(MyStdErr,"\nRecord type (%c) does not match line in  the header.\n", RecType);
                        return(0);
                }
		break;
        default:
                fprintf(MyStdErr,"Record id  must be Binary encoded\nWrong File Format.\n");
                return (0);
        }

	//Get record size
        dp = SectionStart;
        if(!FindKeyWord(dp, RecordFormat, KeyWordString, Encoding)){
                fprintf(MyStdErr,"\nCan't find keyWord (%s).\nWrong File Format.\n", RecordFormat);
                return 0;
        }
       sess->BPFRecordSizes[RecType] = (ui4)GetType(KeyWordString);
	// printf("DEBUG %c %d\n", RecType, sess->BPFRecordSizes[RecType]);

	// Get the number of voltage samples if there are voltage samples
	sess->BPFRecordVoltageSamples[RecType] = GetNumberOfVoltageSamples(Encoding, RecType);

	return(1);
}

void GetRecordTypesUsed(SESSION *sess, si1 *SectionStart){

	ui4	GetNumberOfChannels(SESSION *, ui1 *, si1*, ui1);
	si4	GetIfPositions(SESSION *, ui1 *, si1*, ui1);
	ui1	*dp;
	ui4	i, n;
	void	GetEEGChannelList();

	for(i = 0; i < MAX_BPF_RECORD_TYPES; i++)
		sess->BPFRecordTypeNumbers[i] = 0;

	/* assume these record types have been used although they may not have been */

	sess->BPFRecordTypeNumbers[KEY_EVENT_BPF_REC_TYPE] = 1;
	sess->BPFRecordTypeNumbers[INPUT_EVENT_BPF_REC_TYPE] = 1;
	sess->BPFRecordTypeNumbers[OUTPUT_EVENT_BPF_REC_TYPE] = 1;

	n = 0;
        dp = (ui1 *)SectionStart;
	n += GetNumberOfChannels(sess, dp, NUMBER_OF_EEG_CHANNELS, EEG_BPF_REC_TYPE);
	if(n){	// There are EEG channels
		GetEEGChannelList(sess, dp);
	}else
		sess->BPFEEGChannels = NULL;
	n += GetNumberOfChannels(sess, dp, NUMBER_OF_SINGLE_CHANNELS, SINGLE_BPF_REC_TYPE);
	n += GetNumberOfChannels(sess, dp, NUMBER_OF_STEREO_CHANNELS, STEREO_BPF_REC_TYPE);
	n += GetNumberOfChannels(sess, dp, NUMBER_OF_TETRODE_CHANNELS, TETRODE_BPF_REC_TYPE);
	if(!n){
                fprintf(MyStdErr,"\nNo indication in header that there are electrophysiological data.\nWrong File Format.\n");
		exit(-1);
	}

	if(GetNumberOfChannels(sess, dp, NUMBER_OF_SYNC_CHANNELS, SYNC_BPF_REC_TYPE) == 0)
		fprintf(MyStdErr,"\nNo sync channels. No positions possible.\n");
	GetIfPositions(sess, dp, ROOM_POSITIONS, ROOM_POSITION_BPF_REC_TYPE);
	GetIfPositions(sess, dp, ARENA_POSITIONS, ARENA_POSITION_BPF_REC_TYPE);
}

void	GetEEGChannelList(SESSION *sess, ui1 *SectionStart){
	ui1	*dp;
	si1	KeyWordString[1024], Encoding[1024], **Parameters;
	si4	NumberOfParameters, Type, i;

	dp = SectionStart;
        if(!FindKeyWord(dp, LIST_OF_EEG_CHANNELS, KeyWordString, Encoding)){
                fprintf(MyStdErr,"\nCan't find keyWord (%s).\n", LIST_OF_EEG_CHANNELS);
			exit(-1);
        }
        Type = GetType(KeyWordString);
        switch(Type){
        case -1:
                fprintf(MyStdErr,"KeyWord.integer format not respected in: %s\nWrong File Format.\n", KeyWordString);
                exit(-1); 

        case 0: // ASCII encoded
                NumberOfParameters = GetASCIIEncoding(Encoding, &Parameters);
		if(NumberOfParameters){
			sess->BPFEEGChannels = (ui4 *)calloc(NumberOfParameters, sizeof(ui4));
			for(i=0; i < NumberOfParameters; i++){
                		sscanf(Parameters[i],"%d", &(sess->BPFEEGChannels[i]));
			}
		}
                return;

        default:
                fprintf(MyStdErr,"%s must be ASCII encoded\nWrong File Format.\n", KeyWordString);
                exit(-1);
        }
}	

ui4	GetNumberOfChannels(SESSION *sess, ui1 *SectionStart, si1* KeyWord, ui1 RecType){

	ui1	*dp;
	si1	KeyWordString[1024], Encoding[1024], **Parameters;
	si4	Type, NumberOfParameters;

        dp = SectionStart;
        if(!FindKeyWord(dp, KeyWord, KeyWordString, Encoding)){
                fprintf(MyStdErr,"\nCan't find keyWord (%s).\n", KeyWord);
		return(0);
        }

        Type = GetType(KeyWordString);
        switch(Type){
        case -1:
                fprintf(MyStdErr,"KeyWord.integer format not respected in: %s\nWrong File Format.\n", KeyWordString);
                return(0);   

        case 0: // ASCII encoded
                NumberOfParameters = GetASCIIEncoding(Encoding, &Parameters);
                if(NumberOfParameters != 1){    
                        fprintf(MyStdErr,"(%s).\nWrong File Format.\n", KeyWord);
                        return 0;
                }
                sscanf(Parameters[0],"%d", &(sess->BPFRecordTypeNumbers[RecType]));
                return(1);

        default:
                fprintf(MyStdErr,"%s must be ASCII encoded\nWrong File Format.\n", KeyWord);
                return(0);
        }
}	
si4	GetIfPositions(SESSION *sess, ui1 *SectionStart, si1* KeyWord, ui1 RecType){

	ui1	*dp;
	si1	KeyWordString[1024], Encoding[1024], **Parameters;
	si4	Type, NumberOfParameters;

        dp = SectionStart;
        if(!FindKeyWord(dp, KeyWord, KeyWordString, Encoding)){
               //  fprintf(MyStdErr,"Can't find keyWord (%s). This is not critical.\n", KeyWord);
		sess->BPFRecordTypeNumbers[RecType] = 0;
               	return 0;
        }

        Type = GetType(KeyWordString);
        switch(Type){
        case -1:
                fprintf(MyStdErr,"KeyWord.integer format not respected in: %s\nWrong File Format.\n", KeyWordString);
                return(0);   

        case 0: // ASCII encoded
                NumberOfParameters = GetASCIIEncoding(Encoding, &Parameters);
                if(NumberOfParameters != 1){    
                        fprintf(MyStdErr,"(%s).\nWrong File Format.\n", KeyWord);
                        return 0;
                }
                sscanf(Parameters[0],"%d", &(sess->BPFRecordTypeNumbers[RecType]));
                return(1);

        default:
                fprintf(MyStdErr,"%s must be ASCII encoded\nWrong File Format.\n", KeyWord);
                return(0);
        }
}	

si4	GetNumberOfVoltageSamples(si1 *Encoding, ui1 RecType){
	si1 *VoltageString, *value;
	si4 NumSamples;

	switch (RecType){
		case TETRODE_BPF_REC_TYPE:
			VoltageString = strstr(Encoding,"Voltage.2[4][");
			if(VoltageString == NULL)
				return 0;
        		NumSamples = atoi(VoltageString+strlen("Voltage.2[4]["));
			return NumSamples;
		
		case STEREO_BPF_REC_TYPE:
			VoltageString = strstr(Encoding,"Voltage.2[2][");
			if(VoltageString == NULL)
				return 0;
        		NumSamples = atoi(VoltageString+strlen("Voltage.2[2]["));
			return NumSamples;

		case SINGLE_BPF_REC_TYPE:
			VoltageString = strstr(Encoding,"Voltage.2[1][");
			if(VoltageString == NULL)
				return 0;
        		NumSamples = atoi(VoltageString+strlen("Voltage.2[1]["));
			return NumSamples;

		case EEG_BPF_REC_TYPE:
			VoltageString = strstr(Encoding,"Voltage.2[");
			if(VoltageString == NULL)
				return 0;
			
			value = strrchr(VoltageString,'[');
			if(value == NULL)
				return 0;

			value+=strlen("[");
        		NumSamples = atoi(value);
			return NumSamples;

		case SYNC_BPF_REC_TYPE:
			VoltageString = strstr(Encoding,"Voltage.2[");
			if(VoltageString == NULL)
				return 0;
			NumSamples = atoi(VoltageString+strlen("Voltage.2["));
			return NumSamples;

		case ROOM_POSITION_BPF_REC_TYPE:
		case ARENA_POSITION_BPF_REC_TYPE:
		default:
			return 0;
	}
}
