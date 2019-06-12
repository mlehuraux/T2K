#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <iostream>

#include "../src/fdecoder.h"
#include "../src/datum_decoder.h"
#include "../src/platform_spec.h"
#include "../src/frame.h"

// Marion's classes
#include "../src/Mapping.h"
#include "../src/DAQ.h"
#include "../src/Pads.h"
#include "../src/Pixel.h"
#include "../src/T2KConstants.h"
#include "../src/T2KMainFrame.h"
#include "Globals.h"

// ROOT
#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>
#include <TRint.h>
#include <TApplication.h>
#include "TH3.h"

using namespace std;

Param param;
Features fea;
DatumContext dc;
int verbose;
TH1D *hADCvsTIME[n::pads];
TH1I *timeWindow;
TH1I *transversepads;
TH2D *pads;// = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
TH3D *tracks;// = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
std::vector<long int> eventPos;
int iEvent;
Pixel P;
TH2D *occupation;
TCanvas *stack, *test;
int maxev;
int prevmaxev;
const Int_t NCont = 400;
Int_t MyPalette[NCont];
bool autoMon, endMon;
int mode;
double threshold; // 0 if wozs, around 250 if wzs

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

void paramInit()
{
	mode = 0; // cosmic
	autoMon=false; // mode auto
	endMon=false;
	threshold = 0; // amplitude threshold 0 if zero suppression
}

/******************************************************************************
GUI framework
*******************************************************************************/
int main(int argc, char **argv)
{
	// Palette
  const Int_t NRGBs = 5;
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  gStyle->SetNumberContours(NCont);
  Int_t FI = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  for (int i=0;i<NCont;i++)
  {
    MyPalette[i] = FI+int((double(NCont-1)/double(sqrt(NCont-1))))*int(sqrt(i));
  }
	//gStyle->SetPalette(kBird);
	gStyle->SetPalette(NCont, MyPalette);

	// Parse command line arguments
	if (parse_cmd_args(argc, argv, &param) < 0){return (-1);}

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
	else{cout << "File " << filename << " open" << endl;}

	scan();
	cout << "Scan of the file completed..." << endl;

	maxev = -1;
	prevmaxev = -1;
	paramInit();
	TRint *theApp = new TRint("App", 0, 0);
	// Popup the GUI...
	new T2KMainFrame(gClient->GetRoot(),800,800);
	theApp->Run();

	// Close file if it has been opened
	if (param.fsrc){fclose(param.fsrc);}
	return(0);
}
