/*******************************************************************************

                           _____________________

 File:        mreader.c

 Description: This application is used to analyze the data files recorded by
 the Feminos readout system. It provides a number of options to decode and
 print the content of such binary files.


 Author:      D. Calvet,        calvet@hep.saclay.cea.fr
              

 History:
  August 2012    : created
 
  September 2014: added various performance histograms and statistics

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "mreader.h"
#include "platform_spec.h"
#include "histo_int.h"
#include "frame.h"
#include "frame_scan.h"
#include "accu_array.h"

typedef struct _Param {
	char inp_file[120];
	FILE *fsrc;
	int  has_no_run;
	int  show_run;
	unsigned int vflag;
} Param;
Param param;

Features fea;

int verbose;

/*******************************************************************************
 Features_Clear
*******************************************************************************/
void Features_Clear(Features *f)
{
	int i, j;
	
	f->tot_file_rd     = 0;
	f->tot_fr_cnt      = 0;
	f->tot_sobe        = 0;
	f->tot_sobe_size   = 0;
	f->tot_eobe        = 0;
	f->tot_fem_soe     = 0;
	f->tot_fem_eoe     = 0;
	sprintf(f->run_str, "");
	HistoInt_Init(&(f->h_event_size), 0, 1023, 1, &(f->h_event_size_bins[0]));
	f->sum_built_event_size = 0;
	HistoInt_Init(&(f->h_glo_asic_ch_hit), 0, 79, 1, &(f->h_glo_asic_ch_hit_bins[0]));
	HistoInt_Init(&(f->h_glo_asic_ch_ne), 0, 79, 1, &(f->h_glo_asic_ch_ne_bins[0]));
	HistoInt_Init(&(f->h_highest_ch_hit), 0, 79, 1, &(f->h_highest_ch_hit_bins[0]));

	for (i = 0; i<MAX_NB_OF_FEMINOS; i++)
	{
		for (j = 0; j<ASIC_PER_FEMINOS; j++)
		{
			HistoInt_Init(&(f->h_asic_ch_hit[i][j]), 0, 79, 1, &(f->h_asic_ch_hit_bins[i][j][0]));
		}
	}

	AccuArray_Init(&f->a_ch_hit_per_asic, (MAX_NB_OF_FEMINOS*ASIC_PER_FEMINOS), &f->ch_hit_per_asic[0]);

	HistoInt_Init(&(f->h_inter_event_time), 0, 1023, 1, &(f->h_inter_event_time_bins[0]));

	HistoInt_Init(&(f->h_cha_data_size), 0, 2079, 16, &(f->h_cha_data_size_bins[0]));

	AccuArray_Init(&f->a_first_time_bin_othr, 512, &f->first_time_bin_othr[0]);

	for (i = 0; i<MAX_EVENT_SIZE; i++)
	{
		f->cur_fr[i] = 0x00;
	}
	f->cur_event_size = 0;
	f->cur_highest_ch_hit_count = 0;
	f->cur_ts_h = 0;
	f->cur_ts_m = 0;
	f->cur_ts_l = 0;
	f->cur_fem = 0;
	f->cur_asic = 0;
	f->cur_cha = 0;
	f->is_eb_active = -1;
	f->got_first_fragment = 0;
	f->cur_cha_data_size = -1;

	// Clear the number of non-empty channel for each ASIC
	for (i = 0; i<MAX_NB_OF_FEMINOS; i++)
	{
		for (j = 0; j<ASIC_PER_FEMINOS; j++)
		{
			f->cur_ne_ch[i][j] = 0;
		}
	}
}

/*******************************************************************************
 help() to display usage
*******************************************************************************/
void help()
{
	printf("midreader <options>\n");
	printf("   -h                : print this message help\n");
	printf("   -i <file>         : input file name\n");
	printf("   -show_run         : display run information string\n");
	printf("   -has_no_run       : process file that does not have run information string\n");
	printf("   -show_eb          : display event builder framing\n");
	printf("   -vflag <0xFlags>  : specify verbose flags for frame printout\n");
	printf("   -show_fullframe   : print fully decoded frames\n");
	printf("   -show_framesize   : print frame size\n");
	printf("   -show_hitchannel  : print hit channel\n");
	printf("   -show_hitchacnt   : print hit channel count\n");
	printf("   -show_data        : print event data\n");
	printf("   -show_hbins       : print histogram bins\n");
	printf("   -show_ascii       : print ASCII content\n");
	printf("   -show_framebound  : print frame boundaries\n");
	printf("   -show_eventbound  : print event boundaries\n");
	printf("   -show_nullwords   : print null words\n");
	printf("   -show_histostat   : print histogram statistics\n");
	printf("   -show_lists       : print lists (pedestal and thresholds)\n");
	printf("   -v <level>        : verbose\n");
}

/*******************************************************************************
 parse_cmd_args() to parse command line arguments
*******************************************************************************/
int parse_cmd_args(int argc, char **argv, Param* p)
{
	int i;
	int match;
	int err = 0;

	for (i=1;i<argc; i++)
	{
		match = 0;
		
		// Input file name
		if (strncmp(argv[i], "-i", 2) == 0)
		{
			match = 1;
			if ((i+1) < argc)
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

		// set option for files that do not have the run string information
		else if (strncmp(argv[i], "-has_no_run", 11) == 0)
		{
			p->has_no_run = 1;
			match = 1;
		}

		// show run information string
		else if (strncmp(argv[i], "-show_run", 9) == 0)
		{
			p->show_run = 1;
			match = 1;
		}
		
		// show event builder delimiters
		else if (strncmp(argv[i], "-show_eb", 8) == 0)
		{
			p->vflag |= FRAME_PRINT_EBBND;
			match = 1;
		}

		// vflag
		else if (strncmp(argv[i], "-vflag", 6) == 0)
		{
			match = 1;
			if ((i+1) < argc)
			{
				if (sscanf(argv[i+1],"%i", &(p->vflag)) == 1)
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

		// show complete decoded frame
		else if (strncmp(argv[i], "-show_fullframe", 15) == 0)
		{
			p->vflag |= FRAME_PRINT_ALL;
			match = 1;
		}

		// show frame size
		else if (strncmp(argv[i], "-show_framesize", 15) == 0)
		{
			p->vflag |= FRAME_PRINT_SIZE;
			match = 1;
		}
		
		// show hit channel
		else if (strncmp(argv[i], "-show_hitchannel", 16) == 0)
		{
			p->vflag |= FRAME_PRINT_HIT_CH;
			match = 1;
		}
		
		// show hit channel count
		else if (strncmp(argv[i], "-show_hitchacnt", 15) == 0)
		{
			p->vflag |= FRAME_PRINT_HIT_CNT;
			match = 1;
		}
		
		// show event data
		else if (strncmp(argv[i], "-show_data", 10) == 0)
		{
			p->vflag |= FRAME_PRINT_CHAN_DATA;
			match = 1;
		}
		
		// show histograms bins
		else if (strncmp(argv[i], "-show_hbins", 11) == 0)
		{
			p->vflag |= FRAME_PRINT_HISTO_BINS;
			match = 1;
		}
		
		// show ASCII messsages
		else if (strncmp(argv[i], "-show_ascii", 11) == 0)
		{
			p->vflag |= FRAME_PRINT_ASCII;
			match = 1;
		}
		
		// show frame boundaries
		else if (strncmp(argv[i], "-show_framebound", 16) == 0)
		{
			p->vflag |= FRAME_PRINT_FRBND;
			match = 1;
		}
		
		// show event boundaries
		else if (strncmp(argv[i], "-show_eventbound", 16) == 0)
		{
			p->vflag |= FRAME_PRINT_EVBND;
			match = 1;
		}
		
		// show event boundaries
		else if (strncmp(argv[i], "-show_nullwords", 15) == 0)
		{
			p->vflag |= FRAME_PRINT_NULLW;
			match = 1;
		}
		
		// show histogram statistics
		else if (strncmp(argv[i], "-show_histostat", 15) == 0)
		{
			p->vflag |= FRAME_PRINT_HISTO_STAT;
			match = 1;
		}
		
		// show lists
		else if (strncmp(argv[i], "-show_lists", 15) == 0)
		{
			p->vflag |= FRAME_PRINT_LISTS;
			match = 1;
		}
		
		// verbose
		else if (strncmp(argv[i], "-v", 2) == 0)
		{
			match = 1;
			if ((i+1) < argc)
			{
				if (sscanf(argv[i+1],"%d", &verbose) == 1)
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
 MReader_GetRunInfo
*******************************************************************************/
int MReader_GetRunInfo(Param *p, Features *f)
{
	unsigned short sh;
	unsigned short al;

	// Read prefix
	if (fread(&sh, sizeof(unsigned short), 1, p->fsrc) != 1)
	{
		printf("Error: could not read first prefix.\n");
		return(-1);
	}
	f->tot_file_rd+= sizeof(unsigned short);

	// This must be the prefix for an ASCII string
	if ((sh & PFX_8_BIT_CONTENT_MASK) != PFX_ASCII_MSG_LEN)
	{
		printf("Error: missing string prefix in 0x%x\n", sh);
		return(-1);
	}
	al = GET_ASCII_LEN(sh);

	// Read Run information string
	if (fread(&(f->run_str[0]), sizeof(char), al, p->fsrc) != al)
	{
		printf("Error: could not read %d characters.\n", al);
		return(-1);
	}
	f->tot_file_rd+= sizeof(char) * al;

	// Show run string information if desired
	if (p->show_run)
	{
		printf("Run string: %s\n", &(f->run_str[0]));
	}

	return(0);
}

/*******************************************************************************
 MReader_GetFrame
*******************************************************************************/
int MReader_GetFrame(Param *p, Features *f)
{
	unsigned short *sh;
	unsigned int nb_sh;
	int fr_sz;
	int fr_offset;

	sh = (unsigned short *) &(f->cur_fr[2]);
	
	// Read one short word
	if (fread(sh, sizeof(unsigned short), 1, p->fsrc) != 1)
	{
		printf("End of file reached.\n");
		return(1);
	}
	f->tot_file_rd+= sizeof(unsigned short);

	// Start of built event is just a delimiter coded in a 2 byte word not counted as a frame
	if ((*sh & PFX_0_BIT_CONTENT_MASK) == PFX_START_OF_BUILT_EVENT)
	{
		nb_sh = 0;
		fr_sz = 2;
		fr_offset = 2; // value not used in this case 
	}
	// End of built event is just a delimiter coded in a 2 byte word not counted as a frame
	else if ((*sh & PFX_0_BIT_CONTENT_MASK) == PFX_END_OF_BUILT_EVENT)
	{
		nb_sh = 0;
		fr_sz = 2;
		fr_offset = 2; // value not used in this case 
	}
	// Start of built event with size contains a complete event in one "frame"
	else if ((*sh & PFX_0_BIT_CONTENT_MASK) == PFX_SOBE_SIZE)
	{
		// Read two short words to get the size of the event
		if (fread((sh+1), sizeof(unsigned short), 2, p->fsrc) != 2)
		{
			printf("Error: could not read two short words.\n");
			return(-1);
		}
		f->tot_file_rd+= sizeof(unsigned short) * 2;
		f->tot_fr_cnt++;

		// Get the size of the event in bytes
		fr_sz = (int) (((*(sh+2)) << 16) | (*(sh+1)));

		// Compute the number of short words to read for the complete event
		nb_sh = fr_sz / 2; // number of short words is half the event size in bytes
		nb_sh-=3; // we have already read three short words from this event
		fr_offset = 8;
	}
	// Other types of frame delimiters
	else if (
		((*sh & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_DFRAME) ||
		((*sh & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_CFRAME) ||
		((*sh & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_MFRAME)
	)
	{
		// Read one short word
		if (fread((sh+1), sizeof(unsigned short), 1, p->fsrc) != 1)
		{
			printf("Error: could not read short word.\n");
			return(-1);
		}
		f->tot_file_rd+= sizeof(unsigned short);
		f->tot_fr_cnt++;
			
		// Get the size of the frame in bytes
		fr_sz = (int) *(sh+1);

		// Compute the number of short word to read for this frame
		nb_sh = fr_sz / 2; // number of short words is half the frame size in bytes
		nb_sh-=2; // we have already read two short words from this frame 
		fr_offset = 6;
	}
	// No other delimiters is allowed at this level
	else
	{
		printf("Error: cannot interpret short word 0x%x\n", *sh);
		return(-1);
	}

	// Read binary frame
	if (fread(&(f->cur_fr[fr_offset]), sizeof(unsigned short), nb_sh, p->fsrc) != nb_sh)
	{
		printf("Error: could not read %d bytes.\n", (nb_sh*2));
		return(-1);
	}
	f->tot_file_rd+= sizeof(unsigned short) * nb_sh;

	// Zero the first two bytes because these are no longer used to specify the size of the frame
	f->cur_fr[0] = 0x00;
	f->cur_fr[1] = 0x00;

	// Analyze frame content
	Frame_Scan(f, stdout, (void*)&(f->cur_fr[2]), fr_sz, p->vflag);
	
	//Print frame content
	//Frame_Print(stdout, (void*) &(f->cur_fr[2]), fr_sz, p->vflag);

	return(0);
}

/*******************************************************************************
 Main
*******************************************************************************/
int main(int argc, char **argv)
{
	int i;
	int err;
	int done;
	int tot_built_events;
	int mean_event_size_b;
	float mean_event_size_kb;

	// Default parameters
	sprintf(param.inp_file, "C:\\users\\calvet\\projects\\bin\\pandax\\data\\R2018_11_27-15_24_07-000.aqs");

	param.vflag       = 0;
	param.has_no_run  = 0;
	param.show_run    = 0;
	verbose           = 0;

	// Parse command line arguments
	if (parse_cmd_args(argc, argv, &param) < 0)
	{
		return (-1);	
	}

	if (verbose)
	{
		printf("Input file : %s\n", param.inp_file);
	}

	// Open input file
	if (!(param.fsrc = fopen(param.inp_file, "rb")))
	{
		printf("could not open file %s.\n", param.inp_file);
		return(-1);
	}

	// Various init
	Features_Clear(&fea);

	if (param.has_no_run == 0)
	{
		// Get run string information
		if ((err = MReader_GetRunInfo(&param, &fea)) < 0)
		{
			goto end;
		}
	}

	// Read all the frames contained in this file
	done = 0;
	while (!done)
	{
		err = MReader_GetFrame(&param, &fea);
		
		if (err == 1)
		{
			done = 1;
		}
		else if (err < 0)
		{
			done = 1;
		}
	}

	// Show statistics if desired
	
	// Show final results
	printf("\n");
	printf("---------------------------------------------------------\n");
	printf("Total read                     : %lli bytes (%.3f MB)\n", fea.tot_file_rd, (double) fea.tot_file_rd / (1024.0 * 1024.0));
	printf("Number of frames               : %d frames\n", fea.tot_fr_cnt);
	printf("Start of Event                 : %d\n", fea.tot_fem_soe);
	printf("End of Event                   : %d\n", fea.tot_fem_eoe);
	printf("Start Of Built Event           : %d\n", fea.tot_sobe);
	printf("Start Of Built Event With Size : %d\n", fea.tot_sobe_size);
	printf("End Of Built Event             : %d\n", fea.tot_eobe);
	printf("Average Size of (Built) Event  : ");
	if (fea.tot_sobe_size != 0)
	{
		tot_built_events = fea.tot_sobe_size;
	}
	else if (fea.tot_eobe != 0)
	{
		tot_built_events = fea.tot_eobe; 
	}
	else if (fea.tot_fem_eoe != 0)
	{
		tot_built_events = fea.tot_fem_eoe;
	}
	else
	{
		tot_built_events = 0;
		printf("N/A\n");
	}
	if (tot_built_events)
	{
		mean_event_size_b = (int) (fea.sum_built_event_size / tot_built_events);
		mean_event_size_kb = (float) mean_event_size_b / 1024.0; 
		printf("%d bytes (%.2f KBytes)\n", mean_event_size_b, mean_event_size_kb);
	}
	printf("\n");

	printf("Histogram of Size of (Built) Event (1 bin = 1 KB)\n");
	HistoInt_ComputeStatistics(&(fea.h_event_size));
	HistoInt_PrintStat(&(fea.h_event_size));
	HistoInt_Print(&(fea.h_event_size), 0);
	printf("\n");

	printf("Histogram of ASIC Channel Hit Count\n");
	HistoInt_ComputeStatistics(&(fea.h_glo_asic_ch_hit));
	HistoInt_PrintStat(&(fea.h_glo_asic_ch_hit));
	HistoInt_Print(&(fea.h_glo_asic_ch_hit), 0);
	printf("\n");

	/*
	printf("Histogram of Non-Empty Channel Count per ASIC\n");
	HistoInt_ComputeStatistics(&(fea.h_glo_asic_ch_ne));
	HistoInt_PrintStat(&(fea.h_glo_asic_ch_ne));
	HistoInt_Print(&(fea.h_glo_asic_ch_ne), 0);
	printf("\n");
	*/

	/*
	printf("Histogram of Highest ASIC Channel Hit Count per event\n");
	HistoInt_ComputeStatistics(&(fea.h_highest_ch_hit));
	HistoInt_PrintStat(&(fea.h_highest_ch_hit));
	HistoInt_Print(&(fea.h_highest_ch_hit), 0);
	printf("\n");
	*/

	/*
	if (tot_built_events)
	{
		printf("Average Channel Hit Count per ASIC per built event\n");
	}
	else
	{
		printf("Accumulated Channel Hit Count per ASIC\n");
	}
	for (i = 0; i < (MAX_NB_OF_FEMINOS * ASIC_PER_FEMINOS); i++)
	{
		if (fea.ch_hit_per_asic[i])
		{
				printf("FEC[%02d][%01d] or ASIC[%02d] : ", i / 4, i % 4, i);
				if (tot_built_events)
				{
					printf("%.2f\n", (float)(fea.ch_hit_per_asic[i]) / (float)tot_built_events);
				}
				else
				{
					printf("%d\n", fea.ch_hit_per_asic[i]);
				}

		}
	}
	printf("\n");
	*/

	printf("Histogram of Channel Data Size (bytes)\n");
	HistoInt_ComputeStatistics(&(fea.h_cha_data_size));
	HistoInt_PrintStat(&(fea.h_cha_data_size));
	HistoInt_Print(&(fea.h_cha_data_size), 0);
	printf("\n");

	printf("Histogram of Inter Event Time (1 bin = 100 us)\n");
	HistoInt_ComputeStatistics(&(fea.h_inter_event_time));
	HistoInt_PrintStat(&(fea.h_inter_event_time));
	HistoInt_Print(&(fea.h_inter_event_time), 0);
	printf("\n");

	printf("Index of First Time Bin Over Threshold\n");
	AccuArray_Print(&(fea.a_first_time_bin_othr), 1);
	printf("\n");

	printf("---------------------------------------------------------\n");
	printf("\n");

end:

	if (param.fsrc)
	{
		fclose (param.fsrc);
	}

	return(0);
}
