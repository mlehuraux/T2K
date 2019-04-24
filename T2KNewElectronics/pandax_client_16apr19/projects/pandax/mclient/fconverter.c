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
#include <fcntl.h>

#include "fdecoder.h"
#include "datum_decoder.h"
#include "platform_spec.h"
#include "frame.h"

typedef struct _Param {
	char inp_dir[120];
	char inp_file[120];
	FILE *fsrc;
	int  has_no_run;
	int  show_run;
	unsigned int vflag;
	int  sample_index_offset_zs;
} Param;

/*******************************************************************************
Global Variables
*******************************************************************************/
Param param;
Features fea;
DatumContext dc;
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
	printf("   -h                   : print this message help\n");
	printf("   -i <file>            : input file name\n");
	printf("   -zs_preamble <Value> : number of pre-samples below threshold in zero-suppressed mode\n");
	printf("   -v <level>           : verbose\n");
	printf("   -vflag <0xFlags>     : flags to determine printed items\n");
}

/*******************************************************************************
parse_cmd_args() to parse command line arguments
*******************************************************************************/
int parse_cmd_args(int argc, char **argv, Param* p)
{
	int i;
	int match=0;
	int err = 0;

	for (i = 1; i<argc; i++)
	{

		// Input file name
		if (strncmp(argv[i], "-i", 2) == 0)
		{
			match = 1;
			if ((i + 2) < argc)
			{
				i++;
				strcpy(&(p->inp_dir[0]), argv[i]);
				strcpy(&(p->inp_file[0]), argv[i+1]);
				printf("Input directory : %s", p->inp_dir);
				printf("\n");
				printf("Input file : %s", p->inp_file);
				printf("\n");
			}
			else
			{
				printf("mising argument after %s\n", argv[i]);
				return (-1);
			}
		}

/*
		// vflag
		else if (strncmp(argv[i], "-vflag", 6) == 0)
		{
			match = 1;
			if ((i + 1) < argc)
			{
				if (sscanf(argv[i + 1], "%i", &(p->vflag)) == 1)
				{
					i++;
				}
				else
				{
					printf("Warning: could not scan argument after option -vflag. Ignored\n");
				}
			}
			else
			{
				printf("Warning: missing argument after option -vflag. Ignored\n");
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
*/
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
	//sprintf(param.inp_file, "C:\\users\\calvet\\projects\\bin\\pandax\\data\\R2018_11_27-15_24_07-000.aqs"); //Change default file path
	param.sample_index_offset_zs = 4;
	param.vflag                  = 0;
	param.has_no_run             = 0;
	param.show_run               = 0;
	verbose                      = 0;

	// Parse command line arguments
	if (parse_cmd_args(argc, argv, &param) < 0)
	{
		return (-1);
	}

	// Open input file
	char filename[140];
	strcpy(filename, param.inp_dir);
	strcat(filename, param.inp_file);
	//printf("%s", filename);
	if (!(param.fsrc = fopen(filename, "rb")))
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

	// Create .txt file to write in
	// Find length of directory name
	int ldir;
	for (int i=1; i<sizeof(param.inp_dir); i++)
	{
		if ((param.inp_dir[i]=='/') && (param.inp_dir[i+1]==' '))
		{
			ldir=i+1;
			break;
		}
	}
	char output_dir[ldir];
	// write the reduced string
	strncpy(output_dir, param.inp_dir, ldir-1);
	output_dir[ldir-1]='\0';
	strcat(output_dir, "../txt/");
	printf("Output directory : %s", output_dir);
	printf("\n");

	// Find length of filename
	int l;
	for (int i=1; i<sizeof(param.inp_file); i++)
	{
		if ((param.inp_file[i]=='.') && (param.inp_file[i+1]=='a') && (param.inp_file[i+2]=='q') && (param.inp_file[i+3]=='s'))
		{
			l=i+1;
			break;
		}
	}
	char name[l];
	// write the reduced string
	strncpy(name, param.inp_file, l-1); // name without extension
	name[l-1]='\0';
	strcat(name, ".txt");
	printf("Output .txt file : %s", name);
	printf("\n");

	strcat(output_dir, name);
	FILE * output;
	output = fopen(output_dir, "w");

	// Scan the file
	done = 0;
	i    = 0;
	int current = 0;
	int prev = 0;
	while (!done)
	{
		// Read one short word
		if (fread(&datum, sizeof(unsigned short), 1, param.fsrc) != 1)
		{
			printf("\n");
			printf("*** End of file reached ***\n");
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
			Item_Print(stdout, &dc, param.vflag);
			current = dc.StartOfEventBeCount;
			if (current-prev != 0)
			{
				fprintf(output, "%i\t%i\t%i\t%i\t%i\t%i\n", dc.EventNumber, dc.CardIndex, dc.ChipIndex, dc.ChannelIndex, dc.TimeBinIndex, dc.AdcSample);
			}
			prev = dc.StartOfEventBeCount;
		}
	}

	fclose(output);

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
