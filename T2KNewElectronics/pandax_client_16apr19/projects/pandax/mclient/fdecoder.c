/*******************************************************************************
                         PandaX-III / T2K-II
                         ___________________

File:        fdecoder.c

Description: This program decodes the binary data files recorded by the TDCM.


Author:      D. Calvet,        denis.calvetATcea.fr


History:
April 2019    : created from mreader.c

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "fdecoder.h"
#include "datum_decoder.h"
#include "platform_spec.h"
#include "frame.h"

typedef struct _Param {
	char inp_file[120];
	FILE *fsrc;
	int  has_no_run;
	int  sample_index_offset_zs;
} Param;

/*******************************************************************************
Global Variables
*******************************************************************************/
Param param;
Features fea;
DatumContext dc;
PrintFilter pf;
int verbose;

/*******************************************************************************
Features_Clear
*******************************************************************************/
void Features_Clear(Features *f)
{
	int i, j;

	f->tot_file_rd = 0;
	sprintf(f->run_str, "");
}

/*******************************************************************************
help() to display usage
*******************************************************************************/
void help()
{
	printf("fdecoder <options>\n");
	printf("   -h                               : print this message help\n");
	printf("   -i <file>                        : input file name\n");
	printf("   -zs_preamble <Value>             : number of pre-samples below threshold in zero-suppressed mode\n");
	printf("   -v <level>                       : verbose\n");
	printf("   -flags <0xFlags>                 : flags to determine printed items\n");
	printf("   -condensed                       : show channel data in condensed format\n");
	printf("   -event <Max:Min or index>        : select event number or event range for printout\n");
	printf("   -card <Max:Min or index>         : select card or card range for printout\n");
	printf("   -chip <Max:Min or index>         : select chip or chip range for printout\n");
	printf("   -channel <Max:Min or index>      : select channel or channel range for printout\n");
	printf("   -sample_index <Max:Min or index> : select timebin or timebin range for printout\n");
	printf("   -sample_ampl <Max:Min>           : select sample amplitude range for printout\n");
	printf("   -show_all                        : show all content\n");
	printf("   -show_frame                      : show frame boundaries\n");
	printf("   -show_event                      : show event boundaries\n");
	printf("   -show_wave                       : show channel header and waveform samples\n");
}

/*******************************************************************************
parse_cmd_args() to parse command line arguments
*******************************************************************************/
int parse_cmd_args(int argc, char **argv, Param *p, PrintFilter *f)
{
	int i;
	int match;
	int err = 0;
	int param[4];

	for (i = 1; i<argc; i++)
	{
		match = 0;

		// Input file name
		if (strncmp(argv[i], "-i", 2) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				i++;
				strcpy(&(p->inp_file[0]), argv[i]);
			}
			else
			{
				printf("mising argument after %s\n", argv[i]);
				return (-1);
			}
		}

		// condensed
		else if (strncmp(argv[i], "-condensed", 10) == 0)
		{
			match = 1;
			f->isCondensedFormat = 1;
		}

		// show_frame
		else if (strncmp(argv[i], "-show_all", 9) == 0)
		{
			match = 1;
			f->flags |= 0xFFFFFFFF;
		}

		// show_frame
		else if (strncmp(argv[i], "-show_frame", 11) == 0)
		{
			match = 1;
			f->flags |= (IT_DATA_FRAME | IT_MONITORING_FRAME | IT_CONFIGURATION_FRAME | IT_END_OF_FRAME);
		}

		// show_event
		else if (strncmp(argv[i], "-show_event", 11) == 0)
		{
			match = 1;
			f->flags |= (IT_START_OF_EVENT | IT_END_OF_EVENT);
		}

		// show_wave
		else if (strncmp(argv[i], "-show_wave", 10) == 0)
		{
			match = 1;
			f->flags |= (IT_CHANNEL_HIT_HEADER | IT_TIME_BIN_INDEX | IT_ADC_SAMPLE);
		}

		// flags
		else if (strncmp(argv[i], "-flags", 6) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if (sscanf(argv[i + 1], "%i", &(f->flags)) == 1)
				{
					i++;
				}
				else
				{
					printf("Warning: could not scan argument after option -flags. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -flags. Ignored\n");
			}
		}

		// event
		else if (strncmp(argv[i], "-event", 6) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if ((sscanf(argv[i + 1], "%i:%i", &param[0], &param[1])) == 2)
				{
					i++;
					if (param[0] > param[1])
					{
						param[2] = param[0];
						param[0] = param[1];
						param[1] = param[2];
					}
					f->EventIndexMin = param[0];
					f->EventIndexMax = param[1];
					f->isAnyEvent    = 0;
				}
				else if ((sscanf(argv[i + 1], "%i", &param[0])) == 1)
				{
					i++;
					f->EventIndexMin = param[0];
					f->EventIndexMax = param[0];
					f->isAnyEvent    = 0;
				}
				else
				{
					printf("Warning: could not scan argument after option -card. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -card. Ignored\n");
			}
		}

		// card
		else if (strncmp(argv[i], "-card", 5) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if ((sscanf(argv[i + 1], "%i:%i", &param[0], &param[1])) == 2)
				{
					i++;
					if (param[0] > param[1])
					{
						param[2] = param[0];
						param[0] = param[1];
						param[1] = param[2];
					}
					f->CardIndexMin = param[0];
					f->CardIndexMax = param[1];
					f->isAnyCard    = 0;
				}
				else if ((sscanf(argv[i + 1], "%i", &param[0])) == 1)
				{
					i++;
					f->CardIndexMin = param[0];
					f->CardIndexMax = param[0];
					f->isAnyCard = 0;
				}
				else
				{
					printf("Warning: could not scan argument after option -card. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -card. Ignored\n");
			}
		}

		// chip
		else if (strncmp(argv[i], "-chip", 5) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if ((sscanf(argv[i + 1], "%i:%i", &param[0], &param[1])) == 2)
				{
					i++;
					if (param[0] > param[1])
					{
						param[2] = param[0];
						param[0] = param[1];
						param[1] = param[2];
					}
					f->ChipIndexMin = param[0];
					f->ChipIndexMax = param[1];
					f->isAnyChip = 0;
				}
				else if ((sscanf(argv[i + 1], "%i", &param[0])) == 1)
				{
					i++;
					f->ChipIndexMin = param[0];
					f->ChipIndexMax = param[0];
					f->isAnyChip = 0;
				}
				else
				{
					printf("Warning: could not scan argument after option -chip. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -chip. Ignored\n");
			}
		}

		// channel
		else if (strncmp(argv[i], "-channel", 8) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if ((sscanf(argv[i + 1], "%i:%i", &param[0], &param[1])) == 2)
				{
					i++;
					if (param[0] > param[1])
					{
						param[2] = param[0];
						param[0] = param[1];
						param[1] = param[2];
					}
					f->ChannelIndexMin = param[0];
					f->ChannelIndexMax = param[1];
					f->isAnyChannel = 0;
				}
				else if ((sscanf(argv[i + 1], "%i", &param[0])) == 1)
				{
					i++;
					f->ChannelIndexMin = param[0];
					f->ChannelIndexMax = param[0];
					f->isAnyChannel = 0;
				}
				else
				{
					printf("Warning: could not scan argument after option -channel. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -channel. Ignored\n");
			}
		}

		// sample_index
		else if (strncmp(argv[i], "-sample_index", 13) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if ((sscanf(argv[i + 1], "%i:%i", &param[0], &param[1])) == 2)
				{
					i++;
					if (param[0] > param[1])
					{
						param[2] = param[0];
						param[0] = param[1];
						param[1] = param[2];
					}
					f->AbsoluteSampleIndexMin = param[0];
					f->AbsoluteSampleIndexMax = param[1];
					f->isAnySampleIndex       = 0;
				}
				else if ((sscanf(argv[i + 1], "%i", &param[0])) == 1)
				{
					i++;
					f->AbsoluteSampleIndexMin = param[0];
					f->AbsoluteSampleIndexMax = param[0];
					f->isAnySampleIndex       = 0;
				}
				else
				{
					printf("Warning: could not scan argument after option -timebin. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -timebin. Ignored\n");
			}
		}

		// sample_ampl
		else if (strncmp(argv[i], "-sample_ampl", 12) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if ((sscanf(argv[i + 1], "%i:%i", &param[0], &param[1])) == 2)
				{
					i++;
					if (param[0] > param[1])
					{
						param[2] = param[0];
						param[0] = param[1];
						param[1] = param[2];
					}
					f->SampleAmplMin   = param[0];
					f->SampleAmplMax   = param[1];
					f->isAnySampleAmpl = 0;
				}
				else if ((sscanf(argv[i + 1], "%i", &param[0])) == 1)
				{
					i++;
					f->AbsoluteSampleIndexMin = param[0];
					f->AbsoluteSampleIndexMax = param[0];
					f->isAnySampleIndex       = 0;
				}
				else
				{
					printf("Warning: could not scan argument after option -timebin. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -timebin. Ignored\n");
			}
		}

		// zs_preamble
		else if (strncmp(argv[i], "-zs_preamble", 12) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if (sscanf(argv[i + 1], "%i", &(p->sample_index_offset_zs)) == 1)
				{
					i++;
				}
				else
				{
					printf("Warning: could not scan argument after option -zs_preamble. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -zs_preamble. Ignored\n");
			}

		}

		// verbose
		else if (strncmp(argv[i], "-v", 2) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if (sscanf(argv[i + 1], "%d", &verbose) == 1)
				{
					i++;
				}
				else
				{
					verbose = 1;
				}
			}
			else
			{
				verbose = 1;
			}

		}

		// help
		else if (strncmp(argv[i], "-h", 2) == 0)
		{
			match = 1;
			help();
			return (-1); // force an error to exit
		}

		// unmatched options
		if (match == 0)
		{
			printf("Warning: unsupported option %s\n", argv[i]);
		}
	}
	return (0);

}

/*******************************************************************************
Main
*******************************************************************************/
int main(int argc, char **argv)
{
	unsigned short datum;
	int i;
	int err;
	int done;
	
	// Default parameters
	sprintf(param.inp_file, "C:\\users\\calvet\\projects\\bin\\pandax\\data\\R2018_11_27-15_24_07-000.aqs");
	param.sample_index_offset_zs = 4;
	verbose                      = 0;
	Item_PrintFilter_Init(&pf);

	// Parse command line arguments
	if (parse_cmd_args(argc, argv, &param, &pf) < 0)
	{
		return (-1);
	}

	// Open input file
	if (!(param.fsrc = fopen(param.inp_file, "rb")))
	{
		printf("could not open file %s.\n", param.inp_file);
		return(-1);
	}

	// Initialize the data interpretation context
	DatumContext_Init(&dc, param.sample_index_offset_zs);

	if (verbose)
	{
		printf("Decoder    : Version %d.%d Compiled %s at %s\n", dc.DecoderMajorVersion, dc.DecoderMinorVersion, dc.DecoderCompilationDate, dc.DecoderCompilationTime);
		printf("Input file : %s\n", param.inp_file);
	}

	// Scan the file
	done = 0;
	i    = 0;
	while (!done)
	{
		// Read one short word
		if (fread(&datum, sizeof(unsigned short), 1, param.fsrc) != 1)
		{
			printf("\n"); 
			printf("*** End of file reached.\n");
			done = 1;
		}
		else
		{
			fea.tot_file_rd += sizeof(unsigned short);

			// Interpret datum
			if ((err = Datum_Decode(&dc, datum)) < 0)
			{
				printf("%d Datum_Decode: %s\n", err, &dc.ErrorString[0]);
				done = 1;
			}

			// Print item
			Item_Print(stdout, &dc, &pf);
		}
	}

	// Show final results
	printf("\n");
	printf("-------------------------------------------------------------\n");
	printf("Total read                     : %lli bytes (%.3f MB)\n", fea.tot_file_rd, (double)fea.tot_file_rd / (1024.0 * 1024.0));
	printf("Datum count                    : %i\n", dc.DatumCount);
	printf("Start Of Event (FE)            : %d\n", dc.StartOfEventFeCount);
	printf("End Of Event (FE)              : %d\n", dc.EndOfEventFeCount);
	printf("Start Of Event (BE)            : %d\n", dc.StartOfEventBeCount);
	printf("End Of Event (BE)              : %d\n", dc.EndOfEventBeCount);
	printf("Short Message                  : %d\n", dc.ShortMessageCount);
	printf("Long Message                   : %d\n", dc.LongMessageCount);
	printf("Start of Data Frame            : %d\n", dc.StartOfDataFrameCount);
	printf("Start of Configuration Frame   : %d\n", dc.StartOfConfigurationFrameCount);
	printf("Start of Monitoring Frame      : %d\n", dc.StartOfMonitoringFrameCount);
	printf("End of Frame                   : %d\n", dc.EndOfFrameCount);
	printf("Start Of Built Event           : %d\n", dc.StartOfBuiltEventCount);
	printf("End Of Built Event             : %d\n", dc.EndOfBuiltEventCount);
	printf("-------------------------------------------------------------\n");

	// Close file if it has been opened
	if (param.fsrc)
	{
		fclose(param.fsrc);
	}

	return(0);
}
