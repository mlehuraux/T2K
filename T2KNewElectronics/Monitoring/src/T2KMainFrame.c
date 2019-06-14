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
#include "PadSignal.h"

#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
//#include <boost/thread/thread.hpp>

// ROOT
#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH2I.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TPolyLine.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TPaletteAxis.h"
#include "TH3.h"
#include "TThread.h"

using namespace std;

extern Param param;
extern Features fea;
extern DatumContext dc;
extern int verbose;
extern TH1D *hADCvsTIME[n::pads];
extern TH1I *timeWindow;
extern TH1I *transversepads;
extern TH2D *pads;// = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
extern std::vector<long int> eventPos;
extern int iEvent;
extern 	Pixel P;
extern PadSignal *Pad;
extern TH2D *occupation;
extern TH3D *tracks;
extern TCanvas *stack, *test;
extern int maxev;
extern int prevmaxev;
extern const Int_t NCont=400;
extern Int_t MyPalette[NCont];
extern bool autoMon,endMon;
extern int mode;
extern int firstEv;
extern double threshold; // 0 if wozs, around 250 if wzs



/*******************************************************************************
Useful functions
*******************************************************************************/
int padNum(int i, int j){return(j*geom::nPadx+i);}
int iFrompad(int padnum){return(padnum%geom::nPadx);}
int jFrompad(int padnum){return(padnum/geom::nPadx);}

void histoEventInit()
{
	gStyle->SetOptStat(0);
	pads = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
	tracks = new TH3D("tracks", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady, n::samples, 0, n::samples);

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
	firstEv = -1;
	int evnum;
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
				if(dc.isItemComplete && dc.ItemType==IT_START_OF_EVENT)
				{
					//printf("Type : 0x%x \n", dc.ItemType);
					evnum = (int) dc.EventNumber;
					if (firstEv < 0)
					{
						//printf("Is first event type : 0x%x  and number %u \n", dc.ItemType, dc.EventNumber);
						cout << dc.EventNumber << endl;
						firstEv=evnum;
					}
				}
				else if (dc.isItemComplete && evnum!=prevEvnum)
				{
					eventPos.push_back(fea.tot_file_rd);
					prevEvnum = evnum;
				}
				else if (dc.isItemComplete && dc.ItemType==IT_ADC_SAMPLE){}
				else if (dc.isItemComplete && dc.ItemType==IT_TIME_BIN_INDEX){}
				else if (dc.isItemComplete && dc.ItemType==IT_CHANNEL_HIT_HEADER){}
				else if (dc.isItemComplete && dc.ItemType==IT_DATA_FRAME){}
				else if (dc.isItemComplete && dc.ItemType==IT_CHAN_PED_CORRECTION){}
				else if (dc.isItemComplete && dc.ItemType==IT_CHAN_ZERO_SUPPRESS_THRESHOLD){}
				else if (dc.isItemComplete && dc.ItemType==IT_NULL_DATUM){}
				else if (dc.isItemComplete && dc.ItemType==IT_CHANNEL_HIT_COUNT){}
				else if (dc.isItemComplete && dc.ItemType==IT_LAST_CELL_READ){}
				else if (dc.isItemComplete && dc.ItemType==IT_END_OF_EVENT){}
				else if (dc.isItemComplete){}//printf("Type : 0x%x \n", dc.ItemType);}
			}
		}
	}
	cout << "Events in the file : " << eventPos.size() << endl;
}
/********************************************************************************/

void Features_Clear(Features *f)
{
	f->tot_file_rd = 0;
	sprintf(f->run_str, "");
}

T2KMainFrame::~T2KMainFrame()
{
	//monitoring->Kill();
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

void T2KMainFrame::CloseWindow()
{
  gApplication->Terminate(0);
}

void T2KMainFrame::ChangeStartLabel()
{
  fStart->SetState(kButtonDown);
  if (!autoMon) {
     fStart->SetText("&Stop Monitoring");
     autoMon= kTRUE;
  } else {
     fStart->SetText("&Start Monitoring");
     autoMon = kFALSE;
  }
  fStart->SetState(kButtonUp);
}

void T2KMainFrame::HandleButton(Int_t id)
{
  switch (id) {
  case 0: // BeamMode
    mode = 1;
		break;
  case 4: // cosmic mode
    mode = 0;
		break;
  case 1: // prev
    autoMon=false;
    iEvent = iEvent-1;
    DrawNext(iEvent, mode);
    break;
  case 2: // next
    autoMon=false;
    iEvent = iEvent+1;
    DrawNext(iEvent, mode);
    break;
  case 5: // thresplus
  	threshold+= 10;
		cout << "\n" << "Amplitude threshold set to " << threshold << endl;
  	break;
  case 6: // thresplusplus
    threshold=260;
		cout << "\n" << "Amplitude threshold set to " << threshold << endl;
    break;
  case 7: // thresminus
		threshold-= 10;
		cout << "\n" << "Amplitude threshold set to " << threshold << endl;
		break;
  default:
    break;
  }
}

void *T2KMainFrame::loop(void* ptr)
{
	T2KMainFrame * p=(T2KMainFrame *)ptr;
	while(true)
	{
		sleep(0.5);
		if (autoMon==true)
		{
			iEvent = iEvent+1;
		    p->DrawNext(iEvent, mode);
		}
	}
}

T2KMainFrame::T2KMainFrame(const TGWindow *p,UInt_t w,UInt_t h)
{
  // Create a main frame
  fMain = new TGMainFrame(p,w,h);

  fMain->Connect("CloseWindow()","T2KMainFrame",this,"CloseWindow()");

  // Create canvas widget	theApp->Run();
	// Stack
	occupation = new TH2D("occupation", "Occupation", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
	timeWindow = new TH1I("timeWindow", "Time Window", n::samples, 0, n::samples);
	transversepads = new TH1I("transversepads", "#pads transverse", geom::nPadx, 0, geom::nPadx);

	stack = new TCanvas("stack", "Monitoring Information", 1100, 1100);
	//test = new TCanvas("test", "test", 600, 600);
	stack->Divide(2,2);
	stack->Draw();

  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain, 4*geom::times*geom::wx, 2*geom::times*geom::wy);
  fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX |
									kLHintsExpandY, 10,10,10,1));
  // Create a horizontal frame widget with buttons
  hframe = new TGHorizontalFrame(fMain,200,40);

  beam = new TGTextButton(hframe,"&Beam Mode");
  beam->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=0)");
  hframe->AddFrame(beam, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));

  cosmic = new TGTextButton(hframe,"&Cosmic Mode");
  cosmic->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=4)");
  hframe->AddFrame(cosmic, new TGLayoutHints(kLHintsCenterX,
										   5,5,3,4));

  prev = new TGTextButton(hframe,"&Prev");
  prev->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=1)");
  hframe->AddFrame(prev, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));

  next = new TGTextButton(hframe,"&Next");
  next->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=2)");
  hframe->AddFrame(next, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));

  fStart = new TGTextButton(hframe,"&Start Monitoring");
  fStart->Connect("Clicked()","T2KMainFrame",this,"ChangeStartLabel()");
  hframe->AddFrame(fStart, new TGLayoutHints(kLHintsCenterX,
									   	   5,5,3,4));

  thresplus = new TGTextButton(hframe,"&Threshold+");
  thresplus->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=5)");
  hframe->AddFrame(thresplus, new TGLayoutHints(kLHintsCenterX,
	                                       5,5,3,4));

  thresminus = new TGTextButton(hframe,"&Threshold-");
  thresminus->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=7)");
  hframe->AddFrame(thresminus, new TGLayoutHints(kLHintsCenterX,
									       5,5,3,4));

  thresplusplus = new TGTextButton(hframe,"&Threshold++");
  thresplusplus->Connect("Clicked()","T2KMainFrame",this,"HandleButton(=6)");
  hframe->AddFrame(thresplusplus, new TGLayoutHints(kLHintsCenterX,
										    5,5,3,4));

  exit = new TGTextButton(hframe,"&Exit");
  exit->Connect("Clicked()","T2KMainFrame",this,"CloseWindow()");
  hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,
                                            2,2,2,2));

  // Set a name to the main frame
  fMain->SetWindowName("T2K Monitoring");

  // Map all subwindows of main frame
  fMain->MapSubwindows();

  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());

  // Map main frame
  fMain->MapWindow();

	TThread *monitoring = new TThread("monitoring", (void(*)(void *))&loop, (void*)this);
	monitoring->Run();

}

void T2KMainFrame::Monitor(int mode)
{
	while(autoMon)
	{
		iEvent = iEvent+1;
	  DrawNext(iEvent, mode);
		sleep(1);
	}
}

void T2KMainFrame::DrawNext(Int_t ev0, int mode)
{
	// mode = 0 : cosmic; mode = 1 : beam
	int ev = ev0  + firstEv; // to handle files with subscript =! 0

  prevmaxev = maxev;
  if (maxev < ev){maxev=ev;}
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

  //gStyle->SetPalette(kBird);
  //gStyle->SetPalette(NCont, MyPalette);

  // Stack window for monitoring
  occupation->SetMinimum(-0.1);
  occupation->GetXaxis()->SetTitle("Pads on X axis");
  occupation->GetYaxis()->SetTitle("Pads on Y axis");

	// Default
	param.sample_index_offset_zs = 4;
	param.vflag                  = 0;
	param.has_no_run             = 0;
	param.show_run               = 0;
	verbose                      = 0;

  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->cd();
  fCanvas->Clear();
  TPad *p1 = new TPad("p1", "p1", 0.01, 0.01, 0.49, 0.99);
  p1->Range(-0.5*geom::nPadx*geom::dx, -0.5*geom::nPady*geom::dy, 0.5*geom::nPadx*geom::dx, 0.5*geom::nPady*geom::dy);
  p1->Draw();
  TPad *p2 = new TPad("p2", "p2", 0.51, 0.01, 0.99, 0.99);
  //p2->Range(-0.5*geom::nPadx*geom::dx, -0.5*geom::nPady*geom::dy, 0.5*geom::nPadx*geom::dx, 0.5*geom::nPady*geom::dy);
  p2->Draw();

  TString nevt = "Event ";
  nevt += ev;

  // Go back to the beginning of the file
  fseek(param.fsrc, 0, SEEK_SET);

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
	fseek(param.fsrc, eventPos[ev-1], SEEK_SET);
	while (done)
	{
		// Read one short word
		//if (fread(&datum, sizeof(unsigned short), 1, param.fsrc) != 1)
		if (fread(&datum, sizeof(unsigned short), 1, param.fsrc) != 1)
		{
			printf("\n");
			printf("*** End of file reached ***\n");
			scan();
			//done = false;
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
			// Decode
			if (dc.isItemComplete && dc.ItemType!=IT_START_OF_EVENT && dc.ItemType!=IT_ADC_SAMPLE){done=true;}
			else
			{
				int evnum;
				if (dc.isItemComplete && dc.ItemType==IT_START_OF_EVENT)
				{
					evnum = (int) dc.EventNumber;
				}
				if (dc.isItemComplete && evnum==ev && dc.ItemType==IT_ADC_SAMPLE)//dc.AbsoluteSampleIndex!=timesampleprev)
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
							if (b>0)
							{
								printf("Type : 0x%x \t", dc.ItemType);
								cout << "Event " << dc.EventNumber << " Card " << dc.CardIndex << " Chip " << dc.ChipIndex << " Channel " << dc.ChannelIndex << " Time " << dc.AbsoluteSampleIndex << " Ampl " << dc.AdcSample << endl;
							}
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
	}

	//To compute transverse pads
	int transverse[geom::nPadx]={0};
	// Extract max from signals for display
	for (int q=0; q<n::pads; q++)
	{
		double amp = hADCvsTIME[q]->GetMaximum();
		double time = double(hADCvsTIME[q]->GetMaximumBin());
		pads->Fill(iFrompad(q), jFrompad(q), amp);
		tracks->Fill(iFrompad(q), jFrompad(q), time, time);
		if (mode==0 && amp>threshold){transverse[jFrompad(q)]+=1;} // cosmic mode
		if (mode==1&& amp>threshold){transverse[iFrompad(q)]+=1;} // beam mode

		if (ev == maxev && prevmaxev!=maxev) // stacking condition not to double count if prev
		{
			occupation->Fill(iFrompad(q), jFrompad(q), amp);
			if (amp>(260+threshold)*1.5)
			//if (amp>threshold)
			{
				timeWindow->Fill(int(time));
			}
		}
		p1->cd();
		// TPolyLine Style for click and show signal
		P = padPlane.pad(iFrompad(q),jFrompad(q));
		if (P.channel()!=15&&P.channel()!=28&&P.channel()!=53&&P.channel()!=66&&P.channel()<79&&P.channel()>2)
		{
			P.setAmp(int(amp));
		}
		int color = (float(P.ampl())/4096*NCont);
		/*********************************
		Pad = new PadSignal(P);
		if (P.ampl()>threshold)
		{
			Pad->polyline()->SetFillColor(MyPalette[color]);
			Pad->polyline()->Draw("f");
		}
		else{Pad->polyline()->Draw("f");}
		Pad->polyline()->Draw();

		*********************************/


		if (P.ampl() > threshold )//&& time < 250)
		{
			// Test
			/*
			if (amp > 400)
			{
				test->Clear();
				test->cd();
				hADCvsTIME[q]->Draw("hist");
				test->SaveAs((loc::outputs + "signals/test_" + to_string((int) dc.EventNumber) + "_" + to_string(P.card()) + "_" + to_string(P.chip()) + "_" + to_string(P.channel())+ ".gif" ).c_str());
			}
			*/
			//end test
			//p1->cd();

			// Commented out to try PadSignal
			//if (color == 399){cout << "Amplitude in last bin : " << P.ampl()<< endl;}
			//else if (color == 398){cout << "Amplitude in prev last bin : " << P.ampl()<< endl;}
			//else if (color == 397){cout << "Amplitude in prev prev last bin : " << P.ampl()<< endl;}

			P.line()->SetFillColor(MyPalette[color]);
			P.line()->Draw("f");
		}
		else
		{
			P.line()->Draw("f");
		}
		P.line()->Draw();

	}

	p1->Modified();

	// Fill transverse size
	if (ev == maxev && prevmaxev!=maxev) // stacking condition not to double count if prev
	{
		for (int i=0; i<geom::nPadx; i++)
		{
			if (transverse[i]>0){transversepads->Fill(transverse[i]);}
		}
	}
	fCanvas->cd(1);
	pads->SetNameTitle("pads", nevt);
	pads->SetMaximum(4096);
  	pads->SetMinimum(-0.1);
  	pads->GetXaxis()->SetTitle("Pads on X axis");
  	pads->GetYaxis()->SetTitle("Pads on Y axis");
  	//pads->Draw("COLZ");

  	//fCanvas->SetTickx();
  	//fCanvas->SetTicky();
  	//fCanvas->SetRightMargin(0.15);
	p2->cd();
	tracks->SetMinimum(100);
	gStyle->SetPalette(kBird);
	tracks->Draw("LEGO2");
	p2->Modified();
  	fCanvas->Update();

	stack->cd(1);
	gStyle->SetPalette(NCont, MyPalette);
	occupation->Draw("COLZ");
	stack->cd(2);
	timeWindow->GetXaxis()->SetTitle("Time samples");
	timeWindow->GetYaxis()->SetTitle("#pads hit");
	timeWindow->Draw("hist");
	stack->cd(3);
	transversepads->GetXaxis()->SetTitle("#pads in the transverse direction");
	transversepads->GetYaxis()->SetTitle("#pads hit");
	transversepads->Draw("hist");
	stack->Update();
	cout << "\r" << "\t" << nevt << flush;

	// Delete histos
	delete pads;
	delete tracks;
	for (int q=0; q<n::pads; q++)
	{
		delete hADCvsTIME[q];
	}

	//ev++;
}
