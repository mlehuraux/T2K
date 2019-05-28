#include "fdecoder.h"
#include "datum_decoder.h"
#include "platform_spec.h"
#include "frame.h"

// Marion's classes
#include "Mapping.h"
#include "DAQ.h"
#include "Pads.h"
#include "Pixel.h"
#include "T2KConstants.h"
#include "T2KMainFrame.h"

#include <fstream>
#include <string>
#include <iostream>

// ROOT
#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

using namespace std;

/*******************************************************************************
Global Variables
*******************************************************************************/
Param param;
Features fea;
DatumContext dc;
int verbose;
TH1D *hADCvsTIME[n::pads];
TH2D *pads = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
std::vector<int> eventPos;
int iEvent = 0;

/*******************************************************************************
Useful functions
*******************************************************************************/
int padNum(int i, int j){return(j*geom::nPadx+i);}
int iFrompad(int padnum){return(padnum%geom::nPadx);}
int jFrompad(int padnum){return(padnum/geom::nPadx);}

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

void Features_Clear(Features *f)
{
	f->tot_file_rd = 0;
	sprintf(f->run_str, "");
}

T2KMainFrame::T2KMainFrame(const TGWindow *p,UInt_t w,UInt_t h)
{
  // Create a main frame
  fMain = new TGMainFrame(p,w,h);

  fMain->Connect("CloseWindow()","T2KMainFrame",this,"CloseWindow()");

  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,800,600);
  fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX |

                  kLHintsExpandY, 10,10,10,1));
  // Create a horizontal frame widget with buttons
  hframe = new TGHorizontalFrame(fMain,200,40);
  prev = new TGTextButton(hframe,"&Prev");
  prev->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=1)");
  hframe->AddFrame(prev, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  next = new TGTextButton(hframe,"&Next");
  next->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=2)");
  hframe->AddFrame(next, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  exit = new TGTextButton(hframe,"&Exit");
  exit->Connect("Clicked()","T2KMainFrame",this,"CloseWindow()");
  hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,
                                            2,2,2,2));

  // Set a name to the main frame
  fMain->SetWindowName("T2K Display");

  // Map all subwindows of main frame
  fMain->MapSubwindows();

  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());

  // Map main frame
  fMain->MapWindow();
}

void T2KMainFrame::DrawNext(Int_t ev)
{
	// Load classes
	DAQ daq;
  daq.loadDAQ();
  cout << "... DAQ loaded successfully" << endl;
  Mapping T2K;
  T2K.loadMapping();
  cout << "...Mapping loaded succesfully." << endl;

  gStyle->SetTitleTextColor(50);
  gStyle->SetTitleFont(102);
  gStyle->SetTitleColor(60, "XY");
  gStyle->SetTitleFont(102, "XY");
  gStyle->SetTitleFontSize(0.07);
  gStyle->SetTextFont(82);
  gStyle->SetLabelFont(82,"XY");
  gStyle->SetLabelSize(0.04,"XY");

  Int_t MyPalette[20];
  const Int_t NRGBs = 5;
  const Int_t NCont = 20;
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  gStyle->SetNumberContours(NCont);
  Int_t FI = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  for (int i=0;i<20;i++)
  {
    MyPalette[i] = FI+i;
  }

	// Default
	param.sample_index_offset_zs = 4;
	param.vflag                  = 0;
	param.has_no_run             = 0;
	param.show_run               = 0;
	verbose                      = 0;

  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->cd();
  TString nevt = "Event: ";
  nevt += ev;

  // Fill in histo
	DatumContext_Init(&dc, param.sample_index_offset_zs);

	// decodeEvent
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
			if (dc.isItemComplete && dc.EventNumber==ev)
			{
					cout << dc.ChannelIndex << endl;
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
						//cout << a << "	" << b << endl;
						hADCvsTIME[k]->Fill(a,b);
					}
					else{done=true;}
			}
			if (dc.isItemComplete && dc.EventNumber>ev && dc.EventNumber < 100000)
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

  pads->SetMinimum(0);
  pads->SetMaximum(4096);
  //tree->Draw("PadMaxAmpl[][]:Alt$(PadMaxAmpl[][]):>>h","eventNumber==2","colz");
  pads->SetMinimum(-0.1);
  pads->GetXaxis()->SetTitle("Pads on X axis");
  pads->GetYaxis()->SetTitle("Pads on Y axis");
  pads->Draw("COLZ");

  fCanvas->SetTickx();
  fCanvas->SetTicky();
  fCanvas->SetRightMargin(0.12);
  fCanvas->Update();

  ev++;
}

T2KMainFrame::~T2KMainFrame()
{
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

void T2KMainFrame::CloseWindow()
{
  gApplication->Terminate(0);
}

void T2KMainFrame::HandleButton(Int_t id)
{
//  if (id == -1) {
//    TGButton *btn = (TGButton *) gTQSender;
//    id = btn->WidgetId();
//  }
  switch (id) {
  case 1:
    iEvent = iEvent-1;
    DrawNext(iEvent);
    break;
  case 2:
    iEvent = iEvent+1;
    DrawNext(iEvent);
    break;
  default:
    break;
  }
}
