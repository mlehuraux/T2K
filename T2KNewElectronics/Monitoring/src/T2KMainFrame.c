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
#include "Globals.h"

#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>

// ROOT
#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TPolyLine.h"
using namespace std;

extern Param param;
extern Features fea;
extern DatumContext dc;
extern int verbose;
extern TH1D *hADCvsTIME[n::pads];
extern TH2D *pads;// = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
extern std::vector<int> eventPos;
extern int iEvent;
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
	pads = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);

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

TPolyLine *padline(Pixel& P, int color=602)
{
    Float_t x[4] = {geom::convx*(P.coordx()-0.5*geom::dx), geom::convx*(P.coordx()-0.5*geom::dx), geom::convx*(P.coordx()+0.5*geom::dx), geom::convx*(P.coordx()+0.5*geom::dx)};
    Float_t y[4] = {geom::convy*(P.coordy()-0.5*geom::dy), geom::convy*(P.coordy()+0.5*geom::dy), geom::convy*(P.coordy()+0.5*geom::dy), geom::convy*(P.coordy()-0.5*geom::dy)};
    TPolyLine *pline = new TPolyLine(4,x,y);
    pline->SetFillColor(color);
    pline->SetLineColor(kGray);
    pline->SetLineWidth(1);
    return(pline);
}

/********************************************************************************/

void Features_Clear(Features *f)
{
	f->tot_file_rd = 0;
	sprintf(f->run_str, "");
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

T2KMainFrame::T2KMainFrame(const TGWindow *p,UInt_t w,UInt_t h)
{
  // Create a main frame
  fMain = new TGMainFrame(p,w,h);

  fMain->Connect("CloseWindow()","T2KMainFrame",this,"CloseWindow()");

  // Create canvas widget	theApp->Run();

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
	cout << iEvent << "	" << ev << endl;
	// Load classes
	DAQ daq;
  daq.loadDAQ();
  //cout << "... DAQ loaded successfully" << endl;
  Mapping T2K;
  T2K.loadMapping();
  //cout << "...Mapping loaded succesfully." << endl;

	Pads padPlane;
	padPlane.loadPadPlane(daq, T2K);
	//cout << "...Pad plane loaded succesfully." << endl;

  gStyle->SetTitleTextColor(602);
  gStyle->SetTitleFont(102);
  gStyle->SetTitleColor(60, "XY");
  gStyle->SetTitleFont(102, "XY");
  gStyle->SetTitleFontSize(0.05);
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
  TString nevt = "Event ";
  nevt += ev;

  // Fill in histo
	DatumContext_Init(&dc, param.sample_index_offset_zs);
	// decodeEvent
	unsigned short datum;
	int err;

	// Initialize histos
	histoEventInit();
	//TPad *p1 = new TPad("p1", "p1", 0.01, 0.01, 0.99, 0.99);
	//p1->Range(-0.5*geom::nPadx*geom::dx, -0.5*geom::nPady*geom::dy, 0.5*geom::nPadx*geom::dx, 0.5*geom::nPady*geom::dy);
	//p1->Draw();
	//p1->cd();

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
			// Interpret datum
			if ((err = Datum_Decode(&dc, datum)) < 0)
			{
				printf("%d Datum_Decode: %s\n", err, &dc.ErrorString[0]);
				done = true;
			}
			else{ done=true;}
			int evnum = (int) dc.EventNumber;
			if (dc.isItemComplete && evnum==ev)
			{
					if (dc.ChannelIndex!=15&&dc.ChannelIndex!=28&&dc.ChannelIndex!=53&&dc.ChannelIndex!=66&&dc.ChannelIndex>2&&dc.ChannelIndex<79)
					{
						// histo and display
						int x = T2K.i(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex));
						int y = T2K.j(dc.CardIndex, dc.ChipIndex, daq.connector(dc.ChannelIndex));
						int k = padNum(x, y);
						int a = (int)dc.AbsoluteSampleIndex;
						double b = (double)dc.AdcSample;
						hADCvsTIME[k]->Fill(a,b);

/*
						// TPolyLine Style for click and show signal
						Pixel P;
						P = padPlane.pad(x,y);
						if (P.channel()!=15&&P.channel()!=28&&P.channel()!=53&&P.channel()!=66&&P.channel()<79&&P.channel()>2) // error somewhere in DAQ
						{
								P.setAmp(int(b));
						}
						int color = 2*floor(float(P.ampl())/2000*NCont);
						if (P.ampl() > 0){padline(P,MyPalette[color])->Draw("f");}
						else{padline(P)->Draw("f");}
						padline(P)->Draw();
*/
					}
					else{done=true;}
			}
			if (dc.isItemComplete && evnum>ev && evnum < 1000000)
			{
				done = false;
			}
		}
	}

	// Extract max from signals for display
	for (int q=0; q<n::pads; q++)
	{
		pads->Fill(iFrompad(q), jFrompad(q), hADCvsTIME[q]->GetMaximum());
	}

	pads->SetNameTitle("pads", nevt);
  pads->SetMinimum(0);
	//pads->SetMaximum(4096);
  pads->SetMaximum(1000);
  pads->SetMinimum(-0.1);
  pads->GetXaxis()->SetTitle("Pads on X axis");
  pads->GetYaxis()->SetTitle("Pads on Y axis");
  pads->Draw("COLZ");

	//p1->Modified();
  fCanvas->SetTickx();
  fCanvas->SetTicky();
  fCanvas->SetRightMargin(0.12);
  fCanvas->Update();

	// Delete histos
	delete pads;
	for (int q=0; q<n::pads; q++)
	{
		delete hADCvsTIME[q];
	}

	ev++;
}
