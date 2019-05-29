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
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"


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

// Histograms for monitoring
TH1D *hADCvsTIME[n::pads];
TH2D *pads = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);

/*******************************************************************************
Features_Clear
*******************************************************************************/
void Features_Clear(Features *f)
{
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

void histoEventInit()
{
	gStyle->SetOptStat(0);
	gStyle->SetPalette(kBird);
	for(int k=0;k<n::pads;k++)
	{
		char histName[40];
		sprintf(histName,"hADCvsTIME %d",k);
		hADCvsTIME[k] = new TH1D( histName, histName, n::samples,0,n::samples);
		hADCvsTIME[k]->SetMinimum(-1);
		hADCvsTIME[k]->Reset();
	}
	pads->Reset();
}

int padNum(int i, int j){return(j*geom::nPadx+i);}
int iFrompad(int padnum){return(padnum%geom::nPadx);}
int jFrompad(int padnum){return(padnum/geom::nPadx);}

void decodeEvent(DAQ& daq, Mapping& T2K, int evtnum)
{
	unsigned short datum;
	int err;

	// Initialize histos
	histoEventInit();

	// Scan the file
	bool done = true;
	int current = 0;
	while (done)
	{
		// Read one short word
		if (fread(&datum, sizeof(unsigned short), 1, param.fsrc) != 1)
		{
			printf("\n");
			printf("*** End of file reached ***\n");
			done = false;
		}
		else
		{
			fea.tot_file_rd += sizeof(unsigned short);
			cout << dc.EventNumber << endl;
			// Interpret datum
			if ((err = Datum_Decode(&dc, datum)) < 0)
			{
				printf("%d Datum_Decode: %s\n", err, &dc.ErrorString[0]);
				done = true;
			}
			else{ done=true;}
			if (dc.isItemComplete && dc.EventNumber==evtnum)
			{
					//cout << dc.ChannelIndex << endl;
					if (dc.ChannelIndex!=15&&dc.ChannelIndex!=28&&dc.ChannelIndex!=53&&dc.ChannelIndex!=66&&dc.ChannelIndex>2&&dc.ChannelIndex<79)
					{
						// histo and display
						int x = T2K.i(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex));
						int y = T2K.j(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex));
						int k = padNum(x, y);
						cout << x << "	" << y << "		" << k << endl;
						//cout << int(dc.AbsoluteSampleIndex) << "	" << int(dc.AdcSample) << endl;
						int a = (int)dc.AbsoluteSampleIndex;
						double b = (double)dc.AdcSample;
						cout << a << "	" << b << endl;
						hADCvsTIME[k]->Fill(a,b);
					}
					else{done=true;}
			}
			if (dc.isItemComplete && dc.EventNumber>evtnum && dc.EventNumber < 100000)
			{
				cout << "Done" << endl;
				done = false;
			}
		}
	}


	// Extract max from signals for display
	for (int q=0; q<n::pads; q++)
	{
		pads->Fill(iFrompad(q), jFrompad(q), hADCvsTIME[q]->GetMaximum());
	}
	TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10,geom::wx,geom::wy);
	pads->Draw("COLZ");
	canvas->SaveAs((loc::outputs + "monitoring/" + to_string(evtnum) + ".gif").c_str());
	delete canvas;
}

/*******************************************************************************
Main
*******************************************************************************/
int main(int argc, char **argv)
{
	DAQ daq;
  daq.loadDAQ();
  cout << "... DAQ loaded successfully" << endl;

  Mapping T2K;
  T2K.loadMapping();
  cout << "...Mapping loaded succesfully." << endl;

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

	// Open input 	TH1D *hADCvsTIME[n::pads];
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

	// Initialize the data interpretation context
	DatumContext_Init(&dc, param.sample_index_offset_zs);
	for (int p = 3; p < 25; p++)
	{
		decodeEvent(daq, T2K, p);
	}
	// Close file if it has been opened
	if (param.fsrc)
	{
		fclose(param.fsrc);
	}
	return(0);
}
