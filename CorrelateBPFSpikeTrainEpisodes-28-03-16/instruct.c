
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "size_types.h"
#include "sess_anal.h"
#include "theta.h"
#include "speed.h"
#include "postscript.h"


void    instruct(prog_name)
si1	*prog_name;
{
	si4		len, i;
	si1		*b2a();
	FILE		*fp;

	fp = fopen("instruct_temp", "w");

	(void) system("clear");
	len = strlen(prog_name);
	for (i = 0; i < len; ++i)
		if (islower(prog_name[i]))
			prog_name[i] -= 32;
	(void) fprintf(fp, "%c\n%s\n", 7, prog_name);
	for (i = 0; i < len; ++i)
		(void) fprintf(fp, "=");
        (void) fprintf(fp, "\n\nThis program analyzes unit data. \n");
        (void) fprintf(fp, "\nIt calculates the Kendall correlation between all pairs of units. \n");

	(void) fprintf(fp, "Call it with any desired options, followed by the name of file to be analyzed.\n");
	(void) fprintf(fp, "The directory in which the (.bpf) data file is located should be defined in the shell's\n");
	(void) fprintf(fp, "environment as \"BPF_DIR\". The following options are available:\n\n\n");
	(void) fprintf(fp, "OPTION                  PURPOSE\n\n");

	(void) fprintf(fp, "-AnIn (int)             Set the Analysis Interval during which to calculate a correlation. The value of these episodes is in seconds.\n");
	(void) fprintf(fp, "                        Choose 0 to analyse the whole file as one episode. By default the value is %d seconds.\n\n", ANALYSIS_INTERVAL_SEC_DFLT);

	(void) fprintf(fp, "-AnZCl                  Consider zero clusters as cells. These are generally the uncut\n");
	(void) fprintf(fp, "                        spikes in a file. This is %sdone by default.\n\n", b2a(ANAL_ZERO_CLUSTS_DFLT));

	(void) fprintf(fp, "-BnSz (int)             Set the duration of a bin (time step) for calculating spike counts. The value of these bins is in ms.\n");
	(void) fprintf(fp, "                        By default the value is %d ms.\n\n", BIN_SIZE_MS_DFLT);

	(void) fprintf(fp, "-BPFroom                Specify that the input file is a \"Brain Potential File (.bpf)\". If possible analyse room frame firing.\n");
	(void) fprintf(fp, "                        By default the input file is assumed %sto be in \"Brain Potential File (.bpf)\" format in the BPF_DIR directory.\n\n", b2a(BPF_ROOM_FILE_DFLT));

	(void) fprintf(fp, "-BPFarena               Specify that the input file is a \"Brain Potential File (.bpf)\". If possible analyse arena frame firing.\n");
	(void) fprintf(fp, "                        By default the input file is assumed %sto be in \"Brain Potential File (.bpf)\" format in the BPF_DIR directory.\n\n", b2a(BPF_ARENA_FILE_DFLT));

	(void) fprintf(fp, "-BPFacute               Specify that the input file is a \"Brain Potential File (.bpf)\". Do not analyse position samples.\n");
	(void) fprintf(fp, "                        By default the input file is assumed %sto be in \"Brain Potential File (.bpf)\" format in the BPF_DIR directory.\n\n", b2a(BPF_ACUTE_FILE_DFLT));

        (void) fprintf(fp, "-ChSmRtThr              Change the smoothed rate thresholds of specific cells. All\n");
        (void) fprintf(fp, "                        pixels in the smoothed rate maps lower than this threshold\n");
        (void) fprintf(fp, "                        will be set to zero. This option brings up a dialog allowing\n");
        (void) fprintf(fp, "                        changes to the smoothed rate threshold for specific cells and\n");
        (void) fprintf(fp, "                        probes. Those thresholds not changed are assigned the global\n");
        (void) fprintf(fp, "                        smoothed rate threshold. This can be useful for removing the\n");
        (void) fprintf(fp, "                        smearing of isolated pixels in the final smoothed map output.\n");
        (void) fprintf(fp, "                        Note that any operations that depend upon the values in the\n");
        (void) fprintf(fp, "                        smoothed rate map may be affected by the threshold change. This\n");
        (void) fprintf(fp, "                        dialog is %sbrought up by default\n\n", b2a(CHANGE_SMOOTHED_RATE_THRESH_DFLT));

	(void) fprintf(fp, "-ChRtThr                Change the rate thresholds of specific cells. All pixels with\n");
	(void) fprintf(fp, "                        a rate lower than this threshold will have their rates set to\n");
	(void) fprintf(fp, "                        zero. A dialog is brought up allowing you to set specific rate\n");
	(void) fprintf(fp, "                        thresholds for each cell and probe to be analyzed. Unchanged\n");
	(void) fprintf(fp, "                        cells and probes are assigned the global rate threshold. This\n");
	(void) fprintf(fp, "                        can be useful for removing low rate pixels from the final map\n");
	(void) fprintf(fp, "                        output. Note that any operations that depend upon the values\n");
	(void) fprintf(fp, "                        in the rate map may be affected by the threshold change. This\n");
	(void) fprintf(fp, "                        dialog is %sbrought up by default.\n\n", b2a(CHANGE_RATE_THRESH_DFLT));

	(void) fprintf(fp, "-ElCl                   Suppress analysis of specific cells. This option brings up a\n");
	(void) fprintf(fp, "                        dialog which allows elimination of specific clusters from analysis.\n");
	(void) fprintf(fp, "                        This dialog is %sbrought up by default.\n\n", b2a(ELIM_CLUSTS_DFLT));

        (void) fprintf(fp, "-GlSmRtThr (float)      Set the global smoothed rate threshold. This works just as described for\n");
        (void) fprintf(fp, "                        the global rate threshold option mentioned above, except on the pixels in\n");
        (void) fprintf(fp, "                        the the smoothed rate maps. The default value of this parameter is %0.3f.\n\n", GLOBAL_SMOOTHED_RATE_THRESH_DFLT);      
  
	(void) fprintf(fp, "-GlRtThr (float)        Set the rate thresholds of all cells and probes at once. This option can\n");
	(void) fprintf(fp, "                        be overridden by the dialog invoked with the \"ChRtThr\" option, but this\n");
	(void) fprintf(fp, "                        value sets the defaults thresholds for that dialog, so these two options\n");
	(void) fprintf(fp, "                        can be effectively used in conjunction. Some reasons for doing this are\n");
	(void) fprintf(fp, "                        suggested in the description of the \"ChRtThr\" option. The default value\n");
	(void) fprintf(fp, "                        of the global rate threshold is %0.3f.\n\n", GLOBAL_RATE_THRESH_DFLT);

	(void) fprintf(fp, "-GlTmThr (float)        Set the global time threshold. This, like the global speed threshold, is a\n");
	(void) fprintf(fp, "                        misnomer, as there is only one time threshold, since there is only one time\n");
	(void) fprintf(fp, "                        map. Any pixels sampled for less time (in seconds) than the threshold will\n");
	(void) fprintf(fp, "                        be set to zero in the time map. This has direct consequences on all other\n");
	(void) fprintf(fp, "                        maps, since pixel in the time map with a value of zero are considered\n");
	(void) fprintf(fp, "                        unvisited, and thus no other values can be defined for them. A reason to\n");
	(void) fprintf(fp, "                        raise this limit could be to prevent undersampled pixels from entering the\n");
	(void) fprintf(fp, "                        other analyses. The defaults value of this threshold is %0.3f.\n\n", GLOBAL_TIME_THRESH_DFLT);

	(void) fprintf(fp, "-IgZer                  Ignore time steps when both cells have zero activity when calculating\n");
	(void) fprintf(fp, "                        Kendall's correlation. This is %s done by default.\n\n", b2a(IGNORE_ZEROS_KENDALL_TAU_DFLT));

	(void) fprintf(fp, "-KRtThr (float)         Only use time steps when the rat was in a pixel in which on average the cell fired above a threshold rate.\n");
	(void) fprintf(fp, "                        Specify the threshold rate. If all time steps should be analysed then set this option to 0.0\n");
	(void) fprintf(fp, "                        The rate can be set to a multiple of the overall average rate for each cell.\n");
	(void) fprintf(fp, "                        In this case enter the factor as a percentage greater than 10 (e.g.  200 means twice the overall average rate.\n");
	(void) fprintf(fp, "                        The default is %0.2f.\n\n", KENDALL_RATE_THRESH_DFLT);

	(void) fprintf(fp, "-KpCl                   Invoke a dialog to specify analysis of only specific cells and probes in a\n");
	(void) fprintf(fp, "                        file. This dialog is %sbrought up by default.\n\n", b2a(KEEP_CLUSTS_DFLT));

	(void) fprintf(fp, "-MxCl                   Specify the number of clusters to calculate Kendall's correlation for on each probe.\nNote that this has only been implemented only with the -BPFacute option.\n");
	(void) fprintf(fp, "                        Use this option to make sure that the correlation matrix is the same across recordings,");
	(void) fprintf(fp, "			    regardless of the same cells fired in all recordings.\n");
	(void) fprintf(fp, "			    Without this option only the active cells will have an entry in the correlation table.\n");

	(void) fprintf(fp, "-MxPr                   Specify the number of clusters to calculate Kendall's correlation for on each probe.\nNote that this has only been implemented only with the -BPFacute option\n");
	(void) fprintf(fp, "                        Use this option to make sure that the correlation matrix is the same across recordings,");
	(void) fprintf(fp, "			    regardless of the same cells fired in all recordings.\n");
	(void) fprintf(fp, "			    Without this option only the active cells will have an entry in the correlation table.\n");

	(void) fprintf(fp, "-PxClF (float)          Set the factor by which position data is reduced. Typically this number is an\n");
	(void) fprintf(fp, "                        exact power of two, but this is not a requirement. The argument must be a\n");
	(void) fprintf(fp, "                        positive floating point value greater than or equal to 1.0. The default\n"); 
	(void) fprintf(fp, "                        value is %0.3lf.\n\n", PIXEL_COLLAPSE_FACTOR_DFLT);

	(void) fprintf(fp, "-RdApMp (string)        Read the apparatus map from a file. The file must be an ascii file of the same\n");
	(void) fprintf(fp, "                        format created by the option \"WtApMp\" which is the complement of this option.\n");
	(void) fprintf(fp, "                        Furthermore, the apparatus map must be located in the environmentally defined\n");
	(void) fprintf(fp, "                        directory \"APP_MAP_DIR\", under the name provided as the argument to this option.\n");
	(void) fprintf(fp, "                        The utility of reading an apparatus map, rather than generating one from the time\n");
	(void) fprintf(fp, "                        map is that certain regions of the apparatus can be intentionally excluded from\n");
	(void) fprintf(fp, "                        analysis by editing the apparatus map file. The format of this file is self-evident\n");
	(void) fprintf(fp, "                        when viewed in any text editor. This is the only occasion on which an apparatus map\n");
	(void) fprintf(fp, "                        does more than specify where the map outlines should fall because the apparatus map\n");
	(void) fprintf(fp, "                        is used to \"clean\" the time map once it is created. If, as is usually the case,\n");
	(void) fprintf(fp, "                        the apparatus map is generated from the time map, this process is self-referential\n");
	(void) fprintf(fp, "                        and accomplishes only the removal of mis- and non-detects, hence the \"cleaning\"\n");
	(void) fprintf(fp, "                        terminology. If the apparatus map is significantly different from the time map\n");
	(void) fprintf(fp, "                        the \"cleaning\" will significantly alter the time map, and thus the output of\n");
	(void) fprintf(fp, "                        the program. The apparatus map %s is %sread by default.\n\n", READ_APP_MAP_FILE_DFLT, b2a(READ_APP_MAP_DFLT));

	(void) fprintf(fp, "-RBin                   If the TS files were created on a BIG_ENDIAN machine, this option must be set to reverse the binary reading order.\n\n");

        (void) fprintf(fp, "-SmF                    Set the smoothing factor for smoothed rate maps. This varies between 0.0 and\n");
        (void) fprintf(fp, "                        1.0. A smoothing factor of zero is equivalent to no smoothing, and this will\n");
        (void) fprintf(fp, "                        produce smoothed rate maps that are identical to the plain rate maps. A value\n");
        (void) fprintf(fp, "                        of 1.0 produces maximal smoothing in the context of the smoothing algorithm.\n");
        (void) fprintf(fp, "                        The algorithm calcluates the combined rate of the pixels at a distance of one\n");
        (void) fprintf(fp, "                        pixel from the center pixel (thus corner pixels count less than those joined\n");
        (void) fprintf(fp, "                        to the center by a side). It then weights this combined rate against the rate\n");
        (void) fprintf(fp, "                        of the center pixel. At maximal smoothing (sf = 1.0) the center pixel weighs as\n");
        (void) fprintf(fp, "                        much as one of the pixels joined by a side. At minimal smoothing (sf = 0.0) the\n");
        (void) fprintf(fp, "                        center pixel counts for all of the weight. The default value for the smoothing\n");
        (void) fprintf(fp, "                        factor is %0.3lf.\n\n", SMOOTHING_FACTOR_DFLT);

        (void) fprintf(fp, "-SmRtMp                 Use smoothed rate maps to estimate firing in an episode. This is %sdone by default.\n\n", b2a(SMOOTHED_RATE_MAPS_DFLT));

        (void) fprintf(fp, "-TSF (filename)         The input data format is a timeseries format (TS) file in TS_DIR.\n");
        (void) fprintf(fp, "                        Since a TS file has data from only one spike at least two files are needed for correlation analysis.\n");
        (void) fprintf(fp, "                        The file name that sess_anal is called with is the 'key' TS file. It will provide ONLY the position data\n");
        (void) fprintf(fp, "                        This means the spike data in the 'key' TS file will be ignored.\n");
        (void) fprintf(fp, "                        The spike data from several TS files will be analysed. These files must be listed in a text file.\n");
        (void) fprintf(fp, "                        The name of the file containing the TS files to be correlated is the argument of this option.\n");
        (void) fprintf(fp, "                        It is expected in the pwd.\n\n");

        (void) fprintf(fp, "-TTSF (filename)        The input data format is a timed timeseries format (TTS) file in TTS_DIR.\n");
        (void) fprintf(fp, "                        Since a TTS file has data from only one spike at least two files are needed for correlation analysis.\n");
        (void) fprintf(fp, "                        The file name that sess_anal is called with is the 'key' TTS file. It will provide ONLY the position data\n");
        (void) fprintf(fp, "                        This means the spike data in the 'key' TTS file will be ignored.\n");
        (void) fprintf(fp, "                        The spike data from several TTS files will be analysed. These files must be listed in a text file.\n");
        (void) fprintf(fp, "                        The name of the file containing the TTS files to be correlated is the argument of this option.\n");
        (void) fprintf(fp, "                        It is expected in the pwd.\n\n");

	(void) fprintf(fp, "\nOption Notes:  1) Options with parenthesized types after their names require an argument\n");
	(void) fprintf(fp, "                  of that type.\n");
	(void) fprintf(fp, "               2) Prefixing any of the Boolean options with \"No\" negates their function.\n");
	(void) fprintf(fp, "               3) These options cannot be strung together with a single hyphen (\"-\") as\n");
	(void) fprintf(fp, "                  in other Unix programs. Each option requires its own hyphen.\n");
	(void) fprintf(fp, "               4) Due to the large number of options available, it is probably useful to\n");
	(void) fprintf(fp, "                  create a set of shell scripts including the options necessary for the\n");
	(void) fprintf(fp, "                  more routine uses of this program.\n\n");

	(void) fflush(fp);
	(void) fclose(fp);
	(void) system("less instruct_temp");
	(void) system("rm -f instruct_temp");

	exit(1);
}


si1	*b2a(bool)
ui1		bool;
{
	if (bool)
		return("");
	return("not ");
}
