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
#include <fstream>

#include "fdecoder.h"
#include "datum_decoder.h"
#include "platform_spec.h"
#include "frame.h"

// Marion's classes
#include "../src/Mapping.h"
#include "../src/DAQ.h"
#include "../src/Pads.h"
#include "../src/Pixel.h"
#include "../src/T2KConstants.h"

// ROOT
#include "TH1I.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TCanvas.h"


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

	DAQ daq;
        daq.loadDAQ();
        cout << "... DAQ loaded successfully" << endl;

        Mapping T2K;
        T2K.loadMapping();
        cout << "...Mapping loaded succesfully." << endl;

	TH2F *pedmean = new TH2F("pedmean", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
	TH2F *pedrms = new TH2F("pedrms", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);

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

	string inputdir(argv[2]);
	string inputfile(argv[3]);

	string input_file_name = inputfile.substr(0, inputfile.size()-4);
	string outputdir = (inputdir + "../txt/").c_str();
	string outputfile1 = outputdir + input_file_name + "_ped_geom.txt";
	string outputfile2 = outputdir + input_file_name + "_ped_elec.txt";

	FILE * output1;
	FILE * output2;
	output1 = fopen(outputfile1.c_str(), "w");
	output2 = fopen(outputfile2.c_str(), "w");

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
			//Item_Print(stdout, &dc, param.vflag);
			if (dc.isItemComplete)
			{
				printf("Decoder    : Version %d.%d Compiled %s at %s\n", dc.DecoderMajorVersion, dc.DecoderMinorVersion, dc.DecoderCompilationDate, dc.DecoderCompilationTime);
				cout << "Card Index : " << dc.CardIndex << " Chip	" << dc.ChipIndex << " DAQ " <<  dc.ChannelIndex << "	Connector" << daq.connector(dc.ChannelIndex)<< endl;
				fprintf(output2, "%hi\t%hi\t%hi\t%f\t%f\n", dc.CardIndex, dc.ChipIndex, dc.ChannelIndex, 0.01*dc.PedestalMean, 0.01*dc.PedestalDev);

				//cout << '\r' << "Event number : " << dc.EventNumber << flush;
				// 0.1 because 10 evts in pedestal run and stored as int so x10
				if (dc.ChannelIndex!=15&&dc.ChannelIndex!=28&&dc.ChannelIndex!=53&&dc.ChannelIndex!=66&&dc.ChannelIndex>2)
				{
					fprintf(output1, "%i\t%i\t%f\t%f\n", T2K.i(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex)), T2K.j(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex)), 0.01*dc.PedestalMean, 0.01*dc.PedestalDev);
					pedmean->Fill(T2K.i(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex)), T2K.j(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex)), 0.01*dc.PedestalMean);
					pedrms->Fill(T2K.i(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex)), T2K.j(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex)), 0.01*dc.PedestalDev);
				}
			}
		}
	}

	gStyle->SetOptStat(0);
	gStyle->SetPalette(kBird);
	TCanvas *canvas = new TCanvas("canvas", "canvas", 1000, 500);
	canvas->Divide(2,1);
	canvas->cd(1);
	pedmean->Draw("COLZ");
	canvas->cd(2);
	pedrms->Draw("COLZ");
	canvas->SaveAs((loc::outputs + "pedestals.C").c_str());

	delete canvas;
	delete pedmean;
	delete pedrms;

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

	fclose(output1);
	fclose(output2);
	return(0);
}
