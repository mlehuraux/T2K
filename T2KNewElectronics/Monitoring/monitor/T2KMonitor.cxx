#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <vector>

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


using namespace std;

Param param;
Features fea;
DatumContext dc;
int verbose;
TH1D *hADCvsTIME[n::pads];
TH2D *pads;// = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
std::vector<long int> eventPos;
int iEvent;
Pixel P;
TH2D *occupation;
TCanvas *stack;
int maxev;
int prevmaxev;

void scan()
{
	// Reset eventPos vector
	eventPos.clear();

	// Scan the file
	DatumContext_Init(&dc, param.sample_index_offset_zs);
	unsigned short datum;
	int err;
	bool done = true;
	int prevEvnum = -1;
	while (done)
	{
		// Read one short word
		if (fread(&datum, sizeof(unsigned short), 1, param.fsrc) != 1)
		{
			done = false;
		}
		else
		{
			fea.tot_file_rd += sizeof(unsigned short);
			// Interpret datum
			if ((err = Datum_Decode(&dc, datum)) < 0)
			{
				printf("%d Datum_Decode: %s\n", err, &dc.ErrorString[0]);
				done = true;
			}
			else
			{
				int evnum = (int) dc.EventNumber;
				if (dc.isItemComplete && evnum!=prevEvnum)
				{
					eventPos.push_back(fea.tot_file_rd);
					prevEvnum = evnum;
				}
			}
		}
	}
	cout << "Size of the vector ie. #events : " << eventPos.size() << endl;
}


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

/******************************************************************************
GUI framework
*******************************************************************************/
int main(int argc, char **argv)
{
	// Palette
	const Int_t NCont = 400;
  Int_t MyPalette[NCont];
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
	TRint *theApp = new TRint("App", 0, 0);
	// Popup the GUI...
	iEvent = 0;
	new T2KMainFrame(gClient->GetRoot(),800,800);
	theApp->Run();

	// Close file if it has been opened
	if (param.fsrc){fclose(param.fsrc);}
	//delete stack;
	//delete occupation;
	return(0);
}
