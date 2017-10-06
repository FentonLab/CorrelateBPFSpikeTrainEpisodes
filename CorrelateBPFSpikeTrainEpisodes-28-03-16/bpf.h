
#ifndef BPF_IN
#define BPF_IN

#define TIMESTAMPS_PER_SEC       			10000
#define TIMESTAMPS_PER_MS        			10
#define MAX_PROBES                                      64
#define MAX_CLUSTS                                      30
#define MAX_CLUSTS_PER_PROBE                            MAX_CLUSTS

// These are the Labels that are searched for in the Header file
#define HEADER_BEGIN_LABEL                              "%%BEGIN_HEADER"
#define HEADER_END_LABEL                                "%%END_HEADER"
#define SECTION_BEGIN_LABEL                             "%%BEGIN"
#define SECTION_END_LABEL                               "%%END"
#define COMMENT                                         "//"
#define TYPE_PREFIX                                     '.'

#define BPF_HEADER_TYPE                                 "BRAIN_POTENTIAL_FILE"

#define SECTION_GENERAL_INFORMATION                     "GENERAL_INFORMATION"
#define SECTION_DATABASE_INFORMATION                    "DATABASE_INFORMATION"
#define SECTION_SETUP_INFORMATION                       "SETUP_INFORMATION"
#define SECTION_RECORD_FORMAT_INFORMATION               "RECORD_FORMAT_INFORMATION"

#define MAX_NUMBER_BPF_FORMAT_PARAMETERS                100
#define MAX_BPF_RECORD_TYPES                            256

#define DATE_LABEL                                      "%Date."
#define TIME_LABEL                                      "%Time."
#define SAMPLE_FORMAT_LABEL                             "%Sample."
#define NUMBER_OF_SAMPLING_TYPES                        "%NumberOfSamplingTypes."
#define SPIKE_SAMPLING_FREQUENCY                        "%SpikeSamplingFrequency."
#define EEG_SAMPLING_FREQUENCY                          "%EegSamplingFrequency."
#define NUMBER_OF_CHANNEL_TYPES                         "%NumberOfChannelTypes."
#define NUMBER_OF_EEG_CHANNELS                          "%NumberOfEegChannels."
#define LIST_OF_EEG_CHANNELS                            "%ListOfEegChannels."
#define NUMBER_OF_SYNC_CHANNELS                         "%NumberOfSyncChannels."
#define LIST_OF_SYNC_CHANNELS                           "%ListOfSyncChannels."
#define NUMBER_OF_SINGLE_CHANNELS                       "%NumberOfSingleChannels."
#define LIST_OF_SINGLE_CHANNELS                         "%ListOfSingleChannels."
#define NUMBER_OF_STEREO_CHANNELS                       "%NumberOfStereoChannels."
#define LIST_OF_STEREO_CHANNELS                         "%ListOfStereoChannels."
#define NUMBER_OF_TETRODE_CHANNELS                      "%NumberOfTetrodeChannels."
#define LIST_OF_TETRODE_CHANNELS                        "%ListOfTetrodeChannels."
#define ROOM_POSITIONS			                "%RoomPositions."
#define ROOMX1						"RoomX1."
#define ROOMY1						"RoomY1."
#define ROOMZ1						"RoomZ1."
#define ROOMANGX					"RoomAngX."
#define ROOMANGY					"RoomAngY."
#define ROOMANG						"RoomAng."
#define ARENA_POSITIONS			                "%ArenaPositions."
#define ARENAX1						"ArenaX1."
#define ARENAY1						"ArenaY1."
#define ARENAZ1						"ArenaZ1."
#define ARENAANGX					"ArenaAngX."
#define ARENAANGY					"ArenaAngY."
#define ARENAANG					"ArenaAng."
#define LIST_OF_GAINS                                   "%ListOfGains."
#define LIST_OF_CALIBRATIONS                            "%ListOfCalibrations."
#define ACQUISITION_SYSTEM_LABEL                        "%AcquisitionSystem."
#define KEY_EVENTS                                      "%KeyEvents."
#define INPUT_EVENTS                                    "%InputEvents."
#define OUTPUT_EVENTS                                   "%OutputEvents."
#define VOLTAGE_STRING                                  "Voltage.2["
#define NSAMPLES_STRING                                 "]["

#define RECORD_ID_ENCODING                              1
#define ASCII_STARTING_MARK                             "("
#define ASCII_TERMINATING_MARK                          ")"
#define BINARY_STARTING_MARK                            "('"
#define BINARY_TERMINATING_MARK                         "')"

#define EEG_BPF_REC_TYPE                                'E'
#define SINGLE_BPF_REC_TYPE                             'U'
#define STEREO_BPF_REC_TYPE                             'S'
#define TETRODE_BPF_REC_TYPE                            'T'
#define SYNC_BPF_REC_TYPE                               'Y'
#define ROOM_POSITION_BPF_REC_TYPE                      'R'
#define ARENA_POSITION_BPF_REC_TYPE                     'A'
#define EVOKED_POTENTIAL_BPF_REC_TYPE                   'V'
#define KEY_EVENT_BPF_REC_TYPE                          'K'
#define INPUT_EVENT_BPF_REC_TYPE                        'I'
#define OUTPUT_EVENT_BPF_REC_TYPE                       'O'

#define EEG_BPF_REC_DATA_SIZE                                 2000
#define EEG_BPF_REC_ID_SIZE                                   5
#define EEG_BPF_REC_SIZE                                4005
#define SINGLE_BPF_REC_SIZE                             71
#define STEREO_BPF_REC_SIZE                             135
#define TETRODE_BPF_REC_SIZE                            519
#define SYNC_BPF_REC_SIZE                               2005
#define ROOM_POSITION_BPF_REC_SIZE                      9
#define ARENA_POSITION_BPF_REC_SIZE                     9
#define KEY_EVENT_BPF_REC_SIZE                          6

#define	BPF_SPIKE_OFFSET				7
// #define	BPF_SPIKE_SIZE					MAX_SPIKE_AD_VALS

#define EEG_ID                                          "%EegIdentifier."
#define EEG_RECORD_FORMAT                               "%EegRecordFormat."
#define SYNC_ID                                         "%SyncIdentifier."
#define SYNC_RECORD_FORMAT                              "%SyncRecordFormat."
#define SINGLE_ID                                       "%SingleIdentifier."
#define SINGLE_RECORD_FORMAT                            "%SingleRecordFormat."
#define STEREO_ID                                       "%StereoIdentifier."
#define STEREO_RECORD_FORMAT                            "%StereoRecordFormat."
#define TETRODE_ID                                      "%TetrodeIdentifier."
#define TETRODE_RECORD_FORMAT                           "%TetrodeRecordFormat."
#define ROOM_POSITION_ID                                "%RoomPositionIdentifier."
#define ROOM_POSITION_RECORD_FORMAT                     "%RoomPositionRecordFormat."
#define ARENA_POSITION_ID                               "%ArenaPositionIdentifier."
#define ARENA_POSITION_RECORD_FORMAT                    "%ArenaPositionRecordFormat."
#define KEY_EVENT_ID                                    "%KeyEventIdentifier."
#define KEY_EVENT_RECORD_FORMAT                         "%KeyEventRecordFormat."
#define INPUT_EVENT_ID                                  "%InputEventIdentifier."
#define INPUT_EVENT_RECORD_FORMAT                       "%InputEventRecordFormat."
#define OUTPUT_EVENT_ID                                 "%OutputEventIdentifier."
#define OUTPUT_EVENT_RECORD_FORMAT                      "%OutputEventRecordFormat."

#define	BPF_RECORD_TIME_STAMP_OFFSET			1
#define BPF_RECORD_PROBE_OFFSET				5
#define BPF_SPK_REC_CLUST_OFFSET			6
#define BPF_KEY_EVENT_REC_OFFSET                        5
#define BPF_INPUT_EVENT_REC_OFFSET                      5
#define BPF_OUTPUT_EVENT_REC_OFFSET                     5
#define BPF_SYNC_PROBE					31
#define	BPF_POS_REC_X_OFFSET       			5 
#define BPF_POS_REC_Y_OFFSET 				6 
#define BPF_POS_REC_Z_OFFSET				7
#define BPF_POS_REC_ANG_OFFSET				7

#define	BPF_MIN_AD					-32768
#define	BPF_MAX_AD					32768

#define	ROOM_FRAME					1
#define	ARENA_FRAME					2

#define	CENTER_X					127
#define CENTER_Y					127
#endif

