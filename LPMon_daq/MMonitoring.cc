//--------------------------------------------------------
//                        MMonitoring.cc
// This program could be used for online monitoring of
// LPTPC MICROMEGAS modules during tests phase.
//
// Author : D. Attié
// Date : 30.06.2010 
//
// Modified from J. Beucher
// Date : 20.08.2008
//--------------------------------------------------------

//Standard librairies
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

//Librairies for reading acquisition files
#include <arpa/inet.h>
#include "mygblink.h"

//ROOT librairies
#include <TROOT.h>
#include <TRint.h>
#include <TApplication.h>
#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRootCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
#include <TProfile.h>
#include <TGWindow.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGFileDialog.h>
#include <TSystem.h>
#include <TGMsgBox.h>
#include <TBrowser.h>
#include "TMath.h"
#include "TPolyLine.h"
#include "RQ_OBJECT.h"
#include "TRootCanvas.h"

//Other
#include "MMonMainFrame.h"
#include "MMonSelectCanvasFrame.h"
#include "Globals.h"
#include "KeyStonePad.h"

using namespace std;

const int kPhysChannelMax  = 1728;
const int kASIC            = 24;

const int Ndet=24;				// number of detectors
const int Nasic=2;				// number of ASIC per detector
const Int_t Nstrip=72;				// number of strips per detector
const Int_t Ntime=530;				// number of time samples

///--------------------------------------------------------
/// Structure acquisition data
///--------------------------------------------------------
struct Header
{
  int eventSize;
  int eventNumb;
};

struct EventHeader 
{
  int eventSize;
  int eventNumb;
  //int eventTime;  
};

struct DataPacketHeader 
{
  unsigned short size;
  unsigned short dcc_hdr;			// To change if the data after May 2010
  unsigned short hdr;
  unsigned short args;
  unsigned short ts_h;
  unsigned short ts_l;
  unsigned short ecnt;
  unsigned short scnt;
};

struct DataPacketData
{
  unsigned short data[Ntime];
};

struct NewDataHeader
{
  unsigned short args;
  unsigned short wordcount;
};

struct DataPacketEnd 
{
  unsigned short crc1;
  unsigned short crc2;
};

///--------------------------------------------------------
/// Declaration zone
///--------------------------------------------------------

Int_t COMPRESS=1; 				// 0: compress false; 1: compress true
Int_t evNumber; 				// current event number 
double Time_inloop;				// time in the current loop time
int Nloop; 					// number of loops
double Time;					// time of current event
Int_t compteur;
int sampleCountMax;
Bool_t event_flag=kFALSE;
Bool_t select_flag=kFALSE;

int buffer;
int NbEntries;

bool fStop;
bool fEvenTargetReach;
bool fAffichageMonitoring;
bool fDefaultFile;
bool fFinAnalyse;
bool fRecovering;
bool fWriteBackup;

long int curPos=0;

FILE* _fData;
FILE* _fLayout;
char str[80];

Int_t dataADC[Ntime];

int asic_fec, row, column, pad;
int iROW[kPhysChannelMax];
int iCOLUMN[kPhysChannelMax];
int iPAD[kPhysChannelMax];
int iCHAN[kPhysChannelMax];			// Map for pad ID ( col+row*nCOL ) to chan ID

int MaxCharge[kPhysChannelMax];
int padtotcharge[kPhysChannelMax];
int nHit[kPhysChannelMax];
int timeHit[kPhysChannelMax];

int dataContent[kPhysChannelMax][511];
float evtpadcharge[kPhysChannelMax][100000]; 

/// Define a hit list:  pad ch nslice time max 
int nhit; 
int hitInfo[5][1000];
int _hitTimeMult=0;
int _hitMaximum=-10;
int _hitBinMaximum=0;
int _hitSum=0;
int _nhitPerChan=0;
vector<int> HitMaxCharge, HitBinMaxCharge, HitTimeMultiplicity, HitSum, nHitPerChannel, HitChannelId;

int board = 0;
int channel = 0;
int page = 0;

int Version; 				// version of the daq
int nEvent = 1000000;
int Threshold = 40;			// 40
int RefreshEvent = 100;

int HitsOnPads = 0;			// 5

/// Define data structures 
// EventHeader      head;
// DataPacketHeader pHeader;
// DataPacketData   pData;
// DataPacketEnd    pEnd;



Header head;
DataPacketHeader pHeader;  
DataPacketData pData;
DataPacketEnd pEnd; 

/// Define canvases
vector<TCanvas*> cPermanent;

TCanvas *cADC_Raw;
TCanvas *cADCvsTime_monitoring;
TCanvas *cHitTimeMult_ChargeMax;
TCanvas *cPattern;
TCanvas *cHitChargeMax;
TCanvas *cHitSum;
TCanvas *cHitMaxVSPad;
TCanvas *cHitTimeMult;
TCanvas *cHitBinMax;
TCanvas *cHitNumberPerEvent;
TCanvas *cHitNumberPerPad;
TCanvas *cHitASIC;
TCanvas *cHitFEC;
TCanvas *cChargeASIC;
TCanvas *cChargeFEC;
TCanvas *c3DPanel;
TCanvas *cMaxHitTimeBin;
TCanvas *cADCaccum;			// YHS
TCanvas *cPadPanel;			// YHS

TCanvas *cBeamVSx;	//wwx
TCanvas *cBeamVSt;	//wwx
TCanvas *cBeam;		//wwx
TCanvas *cBeam2;	//wwx

/// Define histograms
vector<TH1*> hPermanent;

TH1F* hChargeHIST[kASIC];
TH1F* hHitHIST[kASIC];
TH1F* hADCvsTIME[kPhysChannelMax];
TH1F* hADC[kPhysChannelMax];		// Copy to keep for display
TH1F* hADC_ped[kPhysChannelMax];	// Copy to keep for display

TH1F* hHitASIC;
TH1F* hHitFEC;
TH1F* hChargeASIC;
TH1F* hChargeFEC;

TH1F *hChargeTot;
TH1F *hHitTime;
TH1F *hHitMax;
TProfile *hTimeChargeH;

TH1F *hRawCharge;
TH1F *hADCvsTime_monitoring;
TH2D *hPattern;  
TH2F *hDebug; 

TH1F *hHitChargeMax;
TH1F *hHitBinMax;
TH1F *hHitMaxVSPad;
TH1F *hHitSum;
TH1F *hHitTimeMult;
TH1F *hHitNumberPerEvent;
TH1F *hHitNumberPerPad;
TH2F *hHitTimeMultVSChargeMax;

TH1F *hADCall;
TH3F *h3D;
TH1F *hMaxHitTimeBin;

TH1F *hx144;  //wwx
TH1F *ht144;  //wwx
TH2F *hxt144; //wwx
TH2F *hyt144; //wwx

void Init();
void DoMonitoring();
void DrawXvsTime();  //wwx

Int_t CanvasID;

//-----------------------------------------------------------------------------
// Some other globals : YHS
//
bool fNextEvent           = false; // if set, display next event
bool fSkipToNextThreshold = false; // if set, skip until next evt over thrshld

Bool_t compressMod_flag= (bool) COMPRESS;

const int kPhysChanPerAsic = 72;
const int kMaxCols         = 72;

class KeyStonePad;
const int kNPoints = 4;
vector<KeyStonePad*> tpcPad(kPhysChannelMax);

double tpcPanelWidth  = 0.0;
double tpcPanelHeight = 0.0;

double x[kPhysChannelMax][kNPoints];
double y[kPhysChannelMax][kNPoints];

int      ReadGeomMap();
TCanvas* CreatePadPanel();
void     FillPadPanelWithColor();

UInt_t cPadPanelWidth  = 0;
UInt_t cPadPanelHeight = 0;
UInt_t xPadding = 0;  // decoration for left and right side of the window
UInt_t yPadding = 0;  // decoration for top and bottom of the window (menu,..)


// Handle maximize/resize event :
//
// NOTE: Both of them have the SAME event type: kConfigureNotify
//
void HandleResizeEvent(Event_t *event)
{
  if ( event->fType == kConfigureNotify && !event->fSendEvent ) {

    UInt_t oWidth  = cPadPanelWidth;
    UInt_t oHeight = cPadPanelHeight;
    
    // Get changed width and height of the main frame
    UInt_t nWidth  = event->fWidth  - xPadding;
    UInt_t nHeight = event->fHeight - yPadding;

    // To prevent infinite loop we have to have a way to stop calling
    if ( (oWidth == nWidth) && (oHeight == nHeight) )  return;

    cout << "Resizing PadPanel ...: " << oWidth << "," << oHeight << endl;

    double aspectHW = oHeight / oWidth;
    double aspectWH = oWidth  / oHeight;
    
    // Determine new dimension
    if ( oHeight != nHeight ) {
      nWidth  = (UInt_t) (nHeight * aspectWH );
    } else {
      nHeight = (UInt_t) (nWidth * aspectHW);
    }

    cout << "0" << nWidth << "," << nHeight <<endl;

    // Check display size
    // UInt_t dispW = gClient->GetDisplayWidth();
    UInt_t dispH = gClient->GetDisplayHeight();

    cout << "1" << nWidth << "," << nHeight <<endl;

    if ( nHeight > dispH ) {   // height is larger than display size
      nHeight = dispH;
      nWidth  = (UInt_t) (nHeight * aspectWH );
    }

    //cout << "Leaving ResizeMainFrame()" << endl;
    
    cPadPanelWidth  = nWidth;
    cPadPanelHeight = nHeight;

    cout << "2" << nWidth << "," << nHeight <<endl;

    // resize the main frame
    TRootCanvas * rc = (TRootCanvas*)(cPadPanel->GetCanvasImp());
    rc->SetCanvasSize( nWidth, nHeight );
    
  }
}

//
//-----------------------------------------------------------------------------
//
//________________________________________________________
/// GUI MainFrame class
//________________________________________________________
MMonMainFrame::MMonMainFrame(const TGWindow *p, UInt_t w, UInt_t h) {
  fMain = new TGMainFrame(p,w,h);

  fMain->Connect("CloseWindow()","MMonMainFrame",this,"CloseWindow()");

  fStop = kFALSE;
  fEvenTargetReach = kFALSE;
  fAffichageMonitoring = kTRUE;
  fFinAnalyse = kFALSE;
  fRecovering = kFALSE;
  fWriteBackup = kFALSE;

  TGLayoutHints *fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
  TGLayoutHints *fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,0,4,0,0);

  TGPopupMenu *fMenuFileOpen = new TGPopupMenu(gClient->GetRoot());
  fMenuFileOpen->AddEntry("&newTBrowser...", M_OPEN_BROWSER);
  fMenuFileOpen->Connect("Activated(Int_t)","MMonMainFrame",this,"HandleMenu(Int_t)");

  TGPopupMenu *fMenuCanvasSelect = new TGPopupMenu(gClient->GetRoot());
  fMenuCanvasSelect->AddEntry("&Select...", M_CANVAS_SELECT);
  fMenuCanvasSelect->Connect("Activated(Int_t)","MMonMainFrame",this,"HandleMenu(Int_t)");

  fMenuBar = new TGMenuBar(fMain,1,1,kHorizontalFrame);
  fMenuBar->AddPopup("&Open", fMenuFileOpen, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Canvas",fMenuCanvasSelect,fMenuBarItemLayout);

  fHFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX,2,2,2,2);
  fHFrame = new TGVerticalFrame(fMain,400,40);

  fFillButton = new TGTextButton(fHFrame,"&Start Monitoring",B_START_ANALYSE);
  fFillButton->Connect("Clicked()","MMonMainFrame",this,"HandleButton(Int_t)");
  fFillButtonLayout = new TGLayoutHints(kLHintsRight,5,5,3,4);

  fHFrame->AddFrame(fFillButton,fFillButtonLayout);

  //// Next Button
  //fNextButton = new TGTextButton(fHFrame,"&Next Event",B_NEXT_EVENT);
  //fNextButton->Connect("Clicked()","MMonMainFrame",this,"HandleButton(Int_t)");
  //
  //fHFrame->AddFrame(fNextButton,fFillButtonLayout);

  // Skip Button
  fSkipButton = new TGTextButton(fHFrame,"&Next   ",
                                 B_SKIP_TO_NEXT_THRESHOLD);
  fSkipButton->Connect("Clicked()","MMonMainFrame",this,"HandleButton(Int_t)");

  fHFrame->AddFrame(fSkipButton,fFillButtonLayout);

  fStopButton = new TGTextButton(fHFrame,"&Stop Monitoring",B_STOP_ANALYSE);
  fStopButton->Connect("Clicked()","MMonMainFrame",this,"HandleButton(Int_t)");
  fStopButtonLayout = new TGLayoutHints(kLHintsRight,5,5,3,4);

  fHFrame->AddFrame(fStopButton,fStopButtonLayout);

  fHProg1 = new TGHProgressBar(fHFrame,TGProgressBar::kStandard,100);
  fHProg1->ShowPosition();
  fHProg1->SetBarColor("yellow");
  fHProg1->SetPosition(0);
  fHFrame->AddFrame(fHProg1,new TGLayoutHints(kLHintsTop | kLHintsLeft |kLHintsExpandX,5,5,5,10));

  fMain->AddFrame(fMenuBar,fMenuBarLayout);
  fMain->AddFrame(fHFrame,fHFrameLayout);

  TTimer *timer = new TTimer();
  timer->Connect("Timeout()","MMonMainFrame",this,"TimerDone()");
  timer->Start(60000, kFALSE);
  
  fMain->SetWindowName("LP-TPC MicroMegas module monitoring");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->MapWindow();
}

//_________________________________________________
MMonMainFrame::~MMonMainFrame() {
//   delete fMain;
//   delete fMenuBar;
//   delete fMenuFile;
//   delete fMenuBarLayout;
//   delete fMenuBarItemLayout;
}

//________________________________________________________
void MMonMainFrame::HandleButton(Int_t id) 
{
  static bool isInitDone = false;

  if (id == -1) {
    TGButton *btn = (TGButton *) gTQSender;
    id = btn->WidgetId();
  }

  switch (id) {

  case B_START_ANALYSE:                                    // continuos
    if ( ! isInitDone ) {
      cout << "Biginning initialization ..." << endl;
      Init();
      cout << endl;
      cout << "--------------------------------" << endl;
      isInitDone = true;
    }
    cout << "Monitoring is under progress ..." << endl;

    fStop                = kFALSE;
    fEvenTargetReach     = kFALSE;  // reset it to false to resume monitoring
    fSkipToNextThreshold = kFALSE;
    //fNextEvent           = kTRUE;
    //nEvent = 1;
    DoMonitoring();
    break;

  // case B_NEXT_EVENT:
  //  fEvenTargetReach     = kFALSE;  // reset it to false to resume monitoring
  //  fSkipToNextThreshold = kFALSE;
  //  //fNextEvent           = kTRUE;
  //  nEvent++;
  //  DoMonitoring();
  //  break;

  case B_SKIP_TO_NEXT_THRESHOLD:  // Skip to next event of maxadc > threshold
    if ( ! isInitDone ) {
      cout << "Biginning initialization ..." << endl;
      Init();
      cout << endl;
      cout << "--------------------------------" << endl;
      isInitDone = true;
    }
    fStop                = kFALSE;
    fEvenTargetReach     = kFALSE;  // reset it to false to resume monitoring
    //fNextEvent         = kFALSE;
    fSkipToNextThreshold = kTRUE;
    DoMonitoring();
    break;

  case B_STOP_ANALYSE:
    cout << "Monitoring was stopping ..." << endl;
    curPos = 0;
    fStop = kTRUE;
    fRecovering = kFALSE;
    //fNextEvent           = kFALSE;
    fSkipToNextThreshold = kFALSE;
    break;

  default:
    break;
  }
}

//________________________________________________________
void MMonMainFrame::HandleMenu(Int_t id) {
  switch (id) {
  case M_OPEN_BROWSER:
    cout << "openning new TBrowser" << endl;
    new TBrowser();
    break;
  case M_CANVAS_SELECT:
    cout << "Beginning initialization ..." << endl;
    Init();
    new MMonSelectCanvasFrame(gClient->GetRoot(),fMain,400,200);
    break;

  default:
    break;
  }
}

//________________________________________________________
void MMonMainFrame::CloseWindow() {
  gApplication->Terminate();
}

//________________________________________________________
///Histo manager
//________________________________________________________
void CreateHisto() {
  //Histogramme for Rawdata
  // now book global hist
  for(int k=0;k<kPhysChannelMax;k++){
    char histNamef[40];
    sprintf(histNamef,"hADCvsTIME %d",k);
    hADCvsTIME[k] = new TH1F( histNamef , histNamef,512,-0.5,511.5);
    hADCvsTIME[k]->SetMinimum(-20.);
    hADCvsTIME[k]->Reset();

    sprintf(histNamef,"hADC %d",k);
    hADC[k] = new TH1F( histNamef , histNamef,512,-0.5,511.5);
    hADC[k]->SetMinimum(-20.);

    sprintf(histNamef,"hADC_ped %d",k);
    hADC_ped[k] = new TH1F( histNamef , histNamef,512,-0.5,511.5);
    hADC_ped[k]->SetMinimum(-20.);
  }

  for(int k=0;k<kASIC;k++){
    char histNamef[40];
    sprintf(histNamef,"hChargeHIST %d",k);
    hChargeHIST[k] = new TH1F( histNamef , histNamef,24,-0.5,23.5);
  }

  for(int k=0;k<kASIC;k++){
    char histNamef[40];
    sprintf(histNamef,"hHitHIST %d",k);
    hHitHIST[k] = new TH1F( histNamef , histNamef,24,-0.5,23.5);
  }

  hRawCharge = new TH1F("hRawCharge","Charge Max", 1000 , -0.5, 4999.5);
  hRawCharge->SetXTitle("ADC");

  hADCvsTime_monitoring = new TH1F("hADCvsTime_monitoring","ADC VS Time", 512 , -0.5, 511.5);
  hADCvsTime_monitoring->SetXTitle("Time Slice");

  hPattern = new TH2D("hPattern","ADC for each pad", 72,-0.5,71.5,24, -0.5,23.5);
  //	hPattern = new TH2F("hPattern","ADC for each pad", 72,0.5,72.5,24, 0.5,24.5);
  hPattern->SetXTitle("column");
  hPattern->SetYTitle("row");

  hHitTimeMultVSChargeMax = new TH2F("hHitTimeMultVSChargeMax","Hit Time Multiplicity VS Charge Max", 2000,-0.5,3999.5,2000, -0.5,1999.5);
  hHitTimeMultVSChargeMax->SetXTitle("Charge Max ADC");
  hHitTimeMultVSChargeMax->SetYTitle("Time multiplicity");

  hDebug = new TH2F("hDebug","Debug", 72,-0.5,71.5,24, -0.5,23.5);
  hDebug->SetXTitle("column");
  hDebug->SetYTitle("row");

  hHitChargeMax = new TH1F("hHitChargeMax","Charge Max", 5000 , -0.5, 4999.5);
  hHitChargeMax->SetXTitle("Charge in ADC");

  hHitBinMax = new TH1F("hHitBinMax","Bin Max", 512 , -0.5, 511.5);
  hHitBinMax->SetXTitle("Time bin of Max");

  hHitMaxVSPad = new TH1F("hHitMaxVSPad","Charge Max VS Pad", kPhysChannelMax, -0.5, kPhysChannelMax-0.5);
  hHitMaxVSPad->SetXTitle("# pad");
  hHitMaxVSPad->SetYTitle("Charge in ADC");

  hHitSum = new TH1F("hHitSum","Integral of hit charge",1000 ,-0.5, 49999.5);
  hHitSum->SetXTitle("Integral charge");

  hHitTimeMult = new TH1F("hHitTimeMult","width of hit", 2000 , -0.5, 1999.5);
  hHitTimeMult->SetXTitle("# Time bin");

  hHitNumberPerEvent = new TH1F("hHitNumberVSEvent","Number of hit per event", 2000 , -0.5, 1999.5);
  hHitNumberPerEvent->SetXTitle("# Hit per event");

  hHitNumberPerPad = new TH1F("hHitNumberVSPad","Number of hit per pad", 50 , -0.5, 49.5);
  hHitNumberPerPad->SetXTitle("# Hit per pad");

  hHitASIC = new TH1F("hHitASIC", "HitASIC" ,24,-0.5,23.5);
  hHitASIC->SetXTitle("ASIC #");

  hHitFEC = new TH1F("hHitFEC", "HitFEC" ,6,-0.5,5.5);
  hHitFEC->SetXTitle("FEC #");

  hChargeASIC = new TH1F("hChargeASIC", "ChargeASIC" ,24,-0.5,23.5);
  hChargeASIC->SetXTitle("ASIC #");

  hChargeFEC = new TH1F("hChargeFEC", "ChargeFEC" ,6,-0.5,5.5);
  hChargeFEC->SetXTitle("FEC #");

  hChargeTot = new TH1F("hChargeTot", "Total charge in event" ,100,0.,1000000.);

  hHitTime = new TH1F("hHitTime", "HitTime" ,110,0.,550.);
  hHitMax = new TH1F("hHitMax", "Max Charge in Pad time series" ,100,0.,4000.);
  hTimeChargeH = new TProfile("hTimeChargeH"," Total Charge in Row versus Time", 20, -0.5,510.5, 0., 20000.);

  //hADCall = new TH1F("hADCall", "Accumulated ADC dist from all pads", 
  //                   4096, -0.5, 4095.5);
  h3D = new TH3F("h3D", "3D", 
                 511, -0.5, 510.5, 72, -0.5, 71.5,  24, -0.5, 23.5 );

  hx144 = new TH1F("hx144", "X beam", 74, -120., 120.);					//wwx
  hx144->SetXTitle("X [mm]");
  hx144->SetYTitle("Counts");
  ht144 = new TH1F("ht144", "Time beam", 512, -0.5, 511.5);				//wwx
  ht144->SetXTitle("Time [Bucket]");
  ht144->SetYTitle("Counts");
  hxt144 = new TH2F("hxt144", "Beam position", 512, -0.5, 511.5, 74, -120., 120.);	//wwx
  hxt144->SetXTitle("Time [Bucket]");
  hxt144->SetYTitle("X [mm]");
  hxt144->SetZTitle("Counts");
  hyt144 = new TH2F("hyt144", "Beam position", 512, -0.5, 511.5, 512, -256., 256.);	//wwx
  hyt144->SetXTitle("Time [Bucket]");
  hyt144->SetYTitle("Y [mm]");
  hyt144->SetZTitle("Counts");

  
  hMaxHitTimeBin = new TH1F("hMaxHitTimeBin", "hMaxHitTimeBin", 
                            512, -0.5, 511.5);

  //Filing up Histogramme vector
  //TH2F first
  hPermanent.push_back(hPattern);			//0
  hPermanent.push_back(hHitTimeMultVSChargeMax);	//1

  //TH1F
  hPermanent.push_back(hRawCharge);			//2
  hPermanent.push_back(hADCvsTime_monitoring);		//3
  hPermanent.push_back(hHitSum);			//4
  hPermanent.push_back(hHitMaxVSPad);			//5
  hPermanent.push_back(hHitTimeMult);			//6
  hPermanent.push_back(hHitBinMax);			//7
  hPermanent.push_back(hHitNumberPerEvent);		//8    
  hPermanent.push_back(hHitChargeMax);			//4
  hPermanent.push_back(hHitNumberPerPad);		//10      
  hPermanent.push_back(hHitASIC);			//11
  hPermanent.push_back(hHitFEC);			//12
  hPermanent.push_back(hChargeASIC);			//13
  hPermanent.push_back(hChargeFEC);			//14

  hPermanent.push_back(hChargeTot);			//15

  hPermanent.push_back(hHitTime);			//16
  hPermanent.push_back(hHitMax);			//17
  hPermanent.push_back(hTimeChargeH);			//18

  hPermanent.push_back(hDebug); 			//19

  hPermanent.push_back(hADCall);			//20
  hPermanent.push_back(h3D);				//21
  hPermanent.push_back(hx144);  //wwx
  hPermanent.push_back(ht144);  //wwx 
  hPermanent.push_back(hxt144);  //wwx
  hPermanent.push_back(hyt144);  //wwx
  
  hPermanent.push_back(hMaxHitTimeBin);			//22
}

//________________________________________________________
void ResetHisto() {
  for (uint i=0;i<hPermanent.size();i++) {
    hPermanent[i]->Reset();
  }
}

//________________________________________________________
void RemoveHisto() {
  for (uint i=0;i<hPermanent.size();i++) {
    hPermanent[i]->Delete();
  }
  hPermanent.clear();
  for(int k=0;k<kPhysChannelMax;k++){
    hADCvsTIME[k]->Delete();
  }
  cout << "Removing Histos" << endl;
}

//------------------------------------------------------------
// Add histograms to corresponding pads
//
void DrawOnCanvases() 
{
  for (uint i=0;i<9;i++) {
    //for (uint i=0;i<cPermanent.size();i++) {
    cPermanent[i]->cd();
    //For TH2F
    if (i<2) {
      hPermanent[i]->Draw("colz");
    }
    else {
      hPermanent[i]->Draw();
    }
  }

  TVirtualPad *gPadOld = gPad;
  //cADCaccum->cd();
  //hADCall->Draw();
  c3DPanel->cd();
  h3D->Draw("box");
  cMaxHitTimeBin->cd();
  hMaxHitTimeBin->Draw();
  
  cBeamVSx -> cd();	//wwx
  hx144 -> Draw();	//wwx
  
  cBeamVSt -> cd();	//wwx
  ht144 -> Draw();	//wwx 
  
  cBeam -> cd();	//wwx
  hxt144 -> Draw();	//wwx 

  cBeam2 -> cd();	//wwx
  hyt144 -> Draw();	//wwx 
  
  
  gPad = gPadOld;
  gPad->cd();

}

//________________________________________________________
///Canvases Manager
//________________________________________________________
void CreateCanvases() 
{
  //Canvases definition
  cPattern = new TCanvas("cPattern","ADC for each pad");
  cHitTimeMult_ChargeMax = new TCanvas("cHitTimeMult_ChargeMax","Hit duration VS Charge Max");
  cADC_Raw = new TCanvas("cADC_Raw","Raw charge ADC for full detector");
  cADCvsTime_monitoring = new TCanvas("cADCvsTime_monitoring","ADC vs Time for pad contents hit with maximum charge");
  cHitSum = new TCanvas("cHitSum","Hit inegrated charge distribution");
  cHitMaxVSPad = new TCanvas("cHitMaxVSPad","Hit MAX VS Pad");
  cHitTimeMult = new TCanvas("cHitTimeMult","Hit timeslice multiplicity");
  cHitBinMax = new TCanvas("cHitBinMax","Hit time binMax"); 
  cHitNumberPerEvent = new TCanvas("cHitNumberPerEvent","Hit number per event"); 
  //cADCaccum = new TCanvas("cADCaccum", "Accumulated ADC distribution");
  c3DPanel = new TCanvas("c3DPanel", "3D Display");

  cBeamVSx = new TCanvas("cBeamVSx","Beam vs x");	//wwx
  cBeamVSt = new TCanvas("cBeamVSt","Beam vs time");	//wwx
  cBeam = new TCanvas("cBeam","Beam");			//wwx
  cBeam2 = new TCanvas("cBeam2","Beam");		//wwx
  
  cMaxHitTimeBin = new TCanvas("cMaxHitTimeBin", "Timebin for MaxHit in event");

  cPadPanel = CreatePadPanel();  // YHS
  for (int ipad=0; ipad<kPhysChannelMax; ipad++) {
    cPadPanel->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
                       "KeyStonePad", tpcPad[ipad], 
                       "ExecuteEvent(Int_t,Int_t,Int_t,TObject*)");
    //tpcPad[ipad]->Connect("KeyStonePadSelected(Int_t)", 0,
    //                      0, "PrintSelectedPadInfo(Int_t)");
  }

  // To handle maximize button
  TRootCanvas * rc = (TRootCanvas*)(cPadPanel->GetCanvasImp());
  rc->Connect("ProcessedEvent(Event_t *)", 0, 0, 
              "HandleResizeEvent(Event_t *)");
  // Save old value
  cPadPanelWidth  = rc->GetCwidth();
  cPadPanelHeight = rc->GetCheight();
  xPadding = rc->GetWidth()  - rc->GetCwidth();
  yPadding = rc->GetHeight() - rc->GetCheight();
  
  /*	cHitChargeMax = new TCanvas("cHitChargeMax","Hit charge maximum distribution"); 
	cHitNumberPerPad = new TCanvas("cHitNumberPerPad","Hit number per pad");
	cHitASIC = new TCanvas("cHitASIC","Hit sorted by ASIC");
	cHitFEC = new TCanvas("cHitFEC","Hit sorted by FEC");
	cChargeASIC = new TCanvas("cChargeASIC","Charge sorted by ASIC");
	cChargeFEC = new TCanvas("cChargeFEC","Charge sorted by FEC");
  */	
  //Filling up canvases vector
	
  cPermanent.push_back(cPattern);			//0
  cPermanent.push_back(cHitTimeMult_ChargeMax); 	//1
  cPermanent.push_back(cADC_Raw);			//2
  cPermanent.push_back(cADCvsTime_monitoring);	        //3
  cPermanent.push_back(cHitSum);			//4
  cPermanent.push_back(cHitMaxVSPad);	                //5
  cPermanent.push_back(cHitTimeMult);	                //6
  cPermanent.push_back(cHitBinMax);	                //7
  cPermanent.push_back(cHitNumberPerEvent);             //8
  /*cPermanent.push_back(cHitChargeMax);		//9
    cPermanent.push_back(cHitNumberPerPad);		//10
    cPermanent.push_back(cHitASIC);			//11
    cPermanent.push_back(cHitFEC);			//12
    cPermanent.push_back(cChargeASIC);			//13
    cPermanent.push_back(cChargeFEC);			//14
  */
  //cPermanent.push_back(cADCaccum);			//15
  //cPermanent.push_back(c3DPanel);			//16
  
  cPermanent.push_back(cBeamVSx);   //wwx
  cPermanent.push_back(cBeamVSt);   //wwx
  cPermanent.push_back(cBeam);   //wwx
  cPermanent.push_back(cBeam2);   //wwx
  
  cPermanent.push_back(cMaxHitTimeBin);			//17
  cPermanent.push_back(cPadPanel);			//18


//   for (int i=0;i<gROOT->GetListOfCanvases()->GetEntries();i++){
//     cout<<"GetlistCanvases into CreateCanvases method"<<endl;
//     ((TRootCanvas*)((TCanvas*)gROOT->GetListOfCanvases()->At(i))->GetCanvasImp());//->DontCallClose();
//   }
}

//________________________________________________________
void UpdateCanvases(bool upd) 
{
  if (upd) {
    for (uint i=1;i<cPermanent.size();i++) {      //wwx  ? why not from i=0
      //		for (uint i=0;i<cPermanent.size();i++) {
      //		for (uint i=1;i<9;i++) {
      cPermanent[i]->Modified();
      cPermanent[i]->Update();
    }

    cMaxHitTimeBin->Modified();
    cMaxHitTimeBin->Update();
  }
  else {
    gPad->GetCanvas()->Modified();
    gPad->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
}

//________________________________________________________
void RemoveCanvases() {
  for (uint i=0;i<cPermanent.size();i++) {
    cPermanent[i]->Close();
  }
  cPermanent.clear();
  cout << "Removing canvases" << endl;
}

//________________________________________________________
int NumberOfCanvases() 
{
  return (cPermanent.size());
}

//--------------------------------------------------------
//
TCanvas* CreatePadPanel()
{
  //
  // Create canvas
  //
  double aspectRatio = tpcPanelHeight / tpcPanelWidth; // to get H from W
  int canvasWidth    = 1024;
  int canvasHeight   = static_cast<int>(aspectRatio * canvasWidth);

  cout << tpcPanelHeight << ", " << tpcPanelWidth <<  ", " << aspectRatio
       << ", " << canvasWidth << ", " << canvasHeight << endl;

  cPadPanelWidth  = canvasWidth;
  cPadPanelHeight = canvasHeight;

  TCanvas *c = new TCanvas("cPadPanel", "LP-TPC Display", 
                           canvasWidth, canvasHeight);

  //
  // Create Key Stone Pad objects
  //
  //vector<KeyStone*> tpcPad(kNumPads);

  //
  // Assign its id to corresponding histogram ID
  //
  // Algo : (row,col) --> keyStone ID = col + row*NCOLS
  //                  ==> chanID[keyID] = Map(row,col) 
  //
  const double scale  = 0.95;          // scale down further for padding 
  const double xscale = scale * 1./tpcPanelWidth;
  const double yscale = scale * 1./tpcPanelHeight;

  const double yoffset = 0.0;     // for KeyStone
  //const double yoffset = 0.02;   // for Rectangle

  for (int ipad=0; ipad<kPhysChannelMax; ipad++) {

    // Transform pad geom to (0,0) ~ (1,1)
    double xCorner[kNPoints];
    double yCorner[kNPoints];
    
    for (int ic=0; ic<kNPoints; ic++) {
      xCorner[ic] = x[ipad][ic] * xscale + 0.5;
      yCorner[ic] = y[ipad][ic] * yscale + 0.5 + yoffset;
    }
    
    //
    // left and right pads' hid
    //
    int lpadHid = -1;
    int rpadHid = -1;

    int col = iCOLUMN[ipad];
    if ( col > 0 )
      lpadHid = iCHAN[ipad-1];  // ipad is consecutive numbers
    if ( col < kMaxCols )
      rpadHid = iCHAN[ipad+1];

    char pName[20];
    sprintf( pName, "p%4d", ipad);
    tpcPad[ipad] = new KeyStonePad(ipad, xCorner, yCorner);
    tpcPad[ipad]->SetHistID( iCHAN[ipad] );
    tpcPad[ipad]->SetHistIDLeft(  lpadHid );
    tpcPad[ipad]->SetHistIDRight( rpadHid );
    tpcPad[ipad]->Draw();
    tpcPad[ipad]->SetCanvas( c );
  }
  
  c->SetFixedAspectRatio(true);
  c->SetEditable(false);

  return c;
}


//--------------------------------------------------------
const int nColors = 10;

//int    colorCode[] = {  1,  3,  5,  7,  9,  11, 14, 16, 20, 22,
//                       25, 27, 29, 33, 37,  40, 43, 45, 47, 49 };

int     colorCode[] = { 55, 60, 65, 70, 75,  80, 85, 90, 95, 98, 100 };

//double  adcBound[]  = { 200, 300, 400, 500, 750, 1000, 1250, 1500, 1750, 2000 };
//double  adcBound[]  = { 300, 350, 400, 550, 750, 1000, 1250, 1500, 1750, 2000 };
//double  adcBound[]  = { 0, 10, 25, 50, 75, 100, 250, 500, 1000, 2000 };
//double  adcBound[]  = { 0, 5, 10, 20, 30, 40, 50, 100, 250, 1000 };
//double  adcBound[]  = { 100, 200, 300, 350, 400, 500, 1000, 1500, 1750, 2000 };
// double  adcBound[]  = { 350, 450, 500, 600, 700, 800, 900, 1000, 2000, 3000 };
// double  adcBound[]  = { 0, 100, 200, 300, 400, 500, 750, 1000, 1500, 2000 };
 double  adcBound[]  = { -100, 0, 20, 40, 60, 80, 100, 200, 300, 500 };
//  double  adcBound[]  = { 350, 400, 450, 500, 550, 600, 650, 1000, 1500, 2000 };

void FillPadPanelWithColor()
{
  // Calculate color index
/*  double MAXADC = -1000.;
  int    maxHitHist = -1;
  for (int hid=0; hid<kPhysChannelMax; hid++) {
    if ( hADCvsTIME[hid]->GetMaximum() > MAXADC ) {
      MAXADC = hADCvsTIME[hid]->GetMaximum();
      maxHitHist = hid;
    }
  }

  ////MAXADC -= Threshold;                        //<---- THIS 090519
  //cout << "MAXADC = " << MAXADC << endl;

  // Skip if max adc of all the hist is below threshold
  //if ( MAXADC <= 0 )  // 20090130
  //  return;
*/
  // 3D 
  h3D->Reset();


  for (int hid=0; hid<kPhysChannelMax; hid++) {
    hADC_ped[hid]->Reset();
  }

  bool PED = 1;

  for (int ipad=0; ipad<kPhysChannelMax; ipad++) {  // layout index
    // Get max adc of the pads
    int    hId    = iCHAN[ ipad ];
    // double adcMax = hADCvsTIME[hId]->GetMaximum() - Threshold;
    double adcMax = 0;

    if (!PED) adcMax = hADCvsTIME[hId]->GetMaximum();

    TArrayF* Ta = (TArrayF*) hADCvsTIME[hId];
    Float_t* a = hADCvsTIME[hId]->GetArray();
    Float_t* b = hADC[hId]->GetArray();

    memcpy(b, a, Ta->GetSize()*sizeof(Float_t));

    if (PED) {
    // Copy histogram

    float firstN=0., lastN=0.;
    float data = 0., datanew=0.;
    float firstSum = 0.;
    float lastSum = 0.;
    float slope = 0.;

    for (int bin=1; bin<10; bin++)  {
      float firstn;
      firstn = hADC[hId] -> GetBinContent(bin);
      firstSum = firstSum + firstn;
     }
    firstN = firstSum / 9;

    for (int bin=501; bin<510; bin++)  {
      float lastn;
      lastn = hADC[hId] -> GetBinContent(bin);
      lastSum = lastSum + lastn;
    }
    lastN = lastSum / 9;

    slope = (lastN - firstN) / 500;

    float absdatanew = 0.;
    for (int bin=1; bin<510; bin++)  {
      data = hADC[hId] -> GetBinContent(bin);
      datanew = data - (slope * (bin - 5) + firstN);
      absdatanew = fabs(datanew); 

      if (datanew > Threshold) hADC_ped[hId]->SetBinContent(bin, datanew);

    }

    adcMax = hADC_ped[hId]->GetMaximum();

   }

    int color = 19;  // background 

    if ( adcMax > adcBound[nColors-1] ) 
      color = colorCode[nColors];
    else {
      for (int i=0; i<nColors; i++) {
        if (adcMax<=adcBound[i]) {
          color = colorCode[i];
          break;
        }
      }
    }

    // 3D panel
    if ( adcMax > 0 ) {
      int row  = ipad / 72;
      int col  = ipad % 72;
      int tmax = 0;
      if (!PED) tmax = hADCvsTIME[hId]->GetMaximumBin();
      if (PED)  tmax = hADC_ped[hId]->GetMaximumBin();
      //h3D->SetBinContent(row,col,tmax,adcMax);
      //h3D->SetBinContent(col,tmax,row,adcMax);
      h3D->Fill(tmax,col,row,adcMax);
    }

    tpcPad[ipad]->SetFillColor( color );
    //tpcPad[ipad]->SetFillColor( color );
    tpcPad[ipad]->Modified();  // Tell Canvas that I've some changed attributes
  }

  DrawXvsTime(); //wwx
  c3DPanel->Modified();
  c3DPanel->Update();
  //c3DPanel->Draw();
}


//________________________________________________________
///Init histo and canvas
//________________________________________________________
void Init() 
{
  static bool ffirst = kTRUE;

  if ( ! ffirst ) {
    fStop  = kFALSE;
    curPos = 0;
    cout << "Resuming monitoring..." << endl;
    cout << "--------------------------------" << endl;
    return;  // SKIP creating canvases when resuming
  }

  if (!ffirst) {
    RemoveCanvases();
    RemoveHisto();
  }

  CreateHisto();
  cout << "CreateHistogrammes" << endl;
  CreateCanvases();
  cout << "CreateCanvases" << endl;
  gStyle->SetPalette(1);
  DrawOnCanvases();
  cout << "DrawOnCanvases" << endl;

  //Filename = info.fFilename;
  fStop = kFALSE;
  curPos = 0;
  ffirst = kFALSE;
  cout << "End of Init" << endl;
  cout << "--------------------------------" << endl;
}

//________________________________________________________
///Mapping
//________________________________________________________
int OpenLayoutFile(char * layoutfilename) 
{
  char * c_tmp;
  
  // Now opening layout file
  _fLayout = fopen(layoutfilename, "r");

  if( ! _fLayout ) {
    cout<<" Can't open file : MapLayoutFile.txt" << endl;
    return(-1);
  }
   
  // Read layout headerint;
  //skip first lignes
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);

  return 1;
}

//________________________________________________________
int CloseLayoutFile()
{
  fclose(_fLayout);
  return 1; 
}

//________________________________________________________
void ReadMapping_T2K() 
{
  //now get mapping
  int iASIC=0;
  int iFEC=0;
  int pad=0;
  
  for (int i=0 ; i<kPhysChannelMax; i++){
    buffer=fscanf(_fLayout, "%d %d %d %d", &asic_fec, &row, &column, &pad);
    if(column%6==0 && column!=0 && row==0) {
      iFEC++;
    }
    iASIC = asic_fec+4*iFEC;
    iPAD[i] = pad+72*iASIC;
    iROW[iPAD[i]] = row;
    iCOLUMN[iPAD[i]] = column;

    cout << "PhysChan: " << i << 
      "\tFEC: " << iFEC << 
      "\tASIC: " << iASIC << 
      "\tPad: " << iPAD[i] <<
      "\trow,col: " << iROW[iPAD[i]]<< "," << iCOLUMN[iPAD[i]] << endl;

  }
}

//--------------------------------------------------------------------
// 
void ReadMapping() 
{
  // now get mapping
  int iASIC=0;
  int iFEC=0;
  int pad=0;

  vector<string> missingChannelInfo;

  for (int i=0 ; i<kPhysChannelMax; i++) 
  {
    buffer=fscanf(_fLayout, "%d %d %d %d", &asic_fec, &row, &column, &pad);

    if (column>=60)              iFEC=0;
    if (column>=48 && column<60) iFEC=1;
    if (column>=36 && column<48) iFEC=2;
    if (column>=24 && column<36) iFEC=3;
    if (column>=12 && column<24) iFEC=4;
    if (column<12)               iFEC=5;

    // Comment by YHS
    //
    // The Map : (Row, Col) --> PhysChan
    //
    // NOTE: PhysChan is the most important one,
    //       which is the one that we can get from the ACQ data file, and
    //       which should be used to find (row, col) of the pad from it !
    //
    //
    //
    // NOTE : 2 pads denoted by -1 are being used for HV.
    //        This results in two missing channel numbers : 72 and 73 !!!
    //
    iASIC = asic_fec + 4*iFEC;
      
    if  ( pad != -1 ) {
      iPAD[i] = pad + kPhysChanPerAsic*iASIC;
    }
    else {
      stringstream iss;
      iss << "  Pad at row,col: " << setw(2) << row << "," << setw(2) << column
          << " is for HV :  ";

      // NOTE: Modify this when mapping is being changed !
      //
      if ( column == 65 ) iPAD[i] = 72;
      if ( column == 64 ) iPAD[i] = 73;

      iss << "Channel ID is assinged manually to " << iPAD[i];
      missingChannelInfo.push_back( iss.str() );
    }

    iROW[iPAD[i]]    = row;
    iCOLUMN[iPAD[i]] = column;

    // Copy of iPAD with different name
    int padid = column + row * kMaxCols;
    iCHAN[padid] = iPAD[i];  // this is the map that we need 

    //cout << "Line: "    << setw(4) << i       << "  "
    //     << "FEC: "     << setw(2) << iFEC    << "  "
    //     << "ASIC: "    << setw(2) << iASIC   << "  " 
    //     << "Chan: "    << setw(4) << iPAD[i] << "  "
    //     << "row,col: " << setw(2) << iROW[iPAD[i]] 
    //     << ","         << setw(2) << iCOLUMN[iPAD[i]] 
    //     << "     "     <<  asic_fec << "," << row << "," << column 
    //     << endl;
  }

  cout << endl;
  cout << "NOTE: " << endl;
  for ( string::size_type i=0; i<missingChannelInfo.size(); i++) {
    cout << missingChannelInfo[i] << endl;
  }
  cout << "  --> Modify this when mapping is changed <---\n" << endl;

}

//--------------------------------------------------------
//
int ReadGeomMap()
{
  char filename[] = "keystonePadGeom.txt";
  //char filename[] = "rectCorners.txt";

  ifstream padGeomFile( filename );

  
  if ( !padGeomFile ) {
    cout << "Can't open file : " << filename << endl;
    return -1;
  }

  char   varName[20];
  double width;
  double ymin, ymax;
  
  padGeomFile >> varName >> width;  // half width of the pad panel
  padGeomFile >> varName >> ymin;
  padGeomFile >> varName >> ymax;

  tpcPanelWidth  = 2. * width;         // total width  of the pad array
  tpcPanelHeight = ymax - ymin;        // total height of the pad array

  cout << "Pad Panel: w,ymin,ymax = " 
       << width << ", " << ymin << ", " << ymax << endl;

  for (int i=0; i<kPhysChannelMax; i++) {
    int id, row, col;
    double x0, y0, x1, y1, x2, y2, x3, y3;
    //padGeomFile >> id >> row >> col 
    padGeomFile >> row >> col 
                >> x0 >> y0 >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
    id = i;
    x[id][0] = x0;    y[id][0] = y0;
    x[id][1] = x1;    y[id][1] = y1;
    x[id][2] = x2;    y[id][2] = y2;
    x[id][3] = x3;    y[id][3] = y3;
  }

  padGeomFile.close();

  return 0;
}



//________________________________________________________
///Data acquisiton reader
//________________________________________________________
int OpenDataFile(char * fileName) 
{
  printf("OomonAcqReader OpenFile opening file %s \n",fileName); 
  _fData = fopen(fileName,"rb");
  // check if this input datafile exist
  if(_fData==NULL){
    cout<<"Can't open data file : " << fileName <<endl;
    return(-1);// End of program
  }
  
  // Read Run UID characters
  char runUid[21];
  buffer=fread(runUid, 1, 20, _fData);
  runUid[20] = '\0'; // Null terminated C string
  printf("File UID is %s \n",runUid); 
  return 1; 
}

//________________________________________________________
int CloseDataFile() {
  fclose(_fData);
  return 1; 
}


//________________________________________________________
int DecodeNextEvent(void) 
{

  bool DEBUG = false;
  
  int countPads =0;
  
  Time=0; Time_inloop=0; Nloop=0;

  if(fread(&head, sizeof(Header), 1, _fData) != 1) {fclose(_fData); return -1; /*return 0; End of program*/}
  
  head.eventSize = ntohl(head.eventSize); // size of current event
  head.eventNumb = ntohl(head.eventNumb); // current event number
//   int evN = head.eventNumb;
  if (DEBUG) cout <<  "Event number: " << head.eventNumb << endl;
  int rawDataSize = head.eventSize - sizeof(head) - sizeof(pEnd);
  int nByteTot = rawDataSize ; 
  int nByteRead=0;
  sampleCountMax=0;

  while (nByteRead < nByteTot ) { // nByteTot represents the size of the full event, not only the current packet
    // now read data packet header 
    buffer = fread(&pHeader, sizeof(DataPacketHeader), 1, _fData);
    nByteRead+=sizeof(DataPacketHeader); 
    int  nlinespacket = (ntohs(pHeader.size)-2-12-2-4)/sizeof(short); // number of lines to be read in the current packet
//     cout << "Payload size (-20/2): " << nlinespacket << endl;
    // check sync fail
    if (GET_LOS_FLAG((ntohs(pHeader.hdr) ))) cout<<"LOS FLAG SET"<< endl;
    if (GET_SYNCH_FAIL((ntohs(pHeader.hdr) ))) cout<<"SYNC FAIL FLAG SET"<< endl;
    int tempAsic1 = GET_RB_ARG1(ntohs(pHeader.args)); 
    int tempAsic2 = GET_RB_ARG2(ntohs(pHeader.args)); 
    Version = GET_VERSION(ntohs(pHeader.dcc_hdr)); // old is 0, new is 1
    int Flag = GET_FLAG(ntohs(pHeader.dcc_hdr)); 
//     int RT = GET_RT(ntohs(pHeader.dcc_hdr)); 
//     int FrameType = GET_FRAME_TY_V2(ntohs(pHeader.dcc_hdr));
//     cout << "FrameType: " <<  FrameType << endl;
//     cout << "RT: "   <<  RT << endl;
    if (DEBUG) cout << "Version: " << Version << endl;
    if (DEBUG) cout << "Flag: "    << Flag    << endl;
    // compute asic# 
    int channelN = tempAsic1/6; 
    int asicN = 10*(tempAsic1%6)/2 + tempAsic2;
    // cout << "asicN, channelN: " << asicN << ", " << channelN << endl;
    //  get sample count
    int sampleCount = ntohs(pHeader.scnt);
    if (DEBUG) cout << "Word count: " << sampleCount << endl;
    // now read the data (should use the sample count !!!!
    // take into account padding short int to get even total
    
    int physChannel=-10; 

    int nlinesread=0;
    while(nlinesread<nlinespacket) {
      if(nlinesread!=0 && Flag!=2) { // some data have already been read in the packet
//      if(nlinesread!=0 && Flag!=2 && RT==0) { // some data have already been read in the packet
//      if((FrameType & 3)==0) { // some data have already been read in the packet
//      if(nlinesread!=0 && (FrameType & 3)==0) { // some data have already been read in the packet
          NewDataHeader newData;
          buffer = fread(&newData, sizeof(unsigned short int), 2, _fData); 
    	  nlinesread+=2;
	  nByteRead+=2*sizeof(unsigned short int); 
	  if(GET_TYPE1(ntohs(newData.args))==0) {cout << "something wrong happened... exiting" << endl; exit(0);}
	  tempAsic1 = GET_RB_ARG1(ntohs(newData.args)); 
	  tempAsic2 = GET_RB_ARG2(ntohs(newData.args)); 
	  // compute asic# 
	  channelN = tempAsic1/6;
	  asicN = 10*(tempAsic1%6)/2 + tempAsic2;
	  sampleCount = GET_NEWWORDCOUNT(ntohs(newData.wordcount));
	  // cout << "new word count: " << sampleCount << endl;
      }
      
      bool chstatus = false;
     
      // compute physical channel (0-71)
      if (channelN> 2 && channelN < 15 ) {
        physChannel= channelN -3; chstatus = true;
      } else if (channelN> 15 && channelN < 28 ) {
        physChannel= channelN -4; chstatus = true;
      } else if (channelN> 28 && channelN < 53 ) {
        physChannel= channelN -5; chstatus = true;
      } else if (channelN> 53 && channelN < 66 ) {
        physChannel= channelN -6; chstatus = true;
      } else if (channelN> 66  ) {
        physChannel= channelN -7; chstatus = true;
      }
      
      int maxcharge = -50;
      int tempCharge = 0;
     
      int thisPhysChannel = asicN*72+physChannel;
 
      int sampleCountRead = sampleCount;
      if (sampleCount%2==1) sampleCountRead++;
 
      if(Flag!=2 && sampleCount!=0) {
        buffer = fread(&pData, sizeof(unsigned short int), sampleCountRead, _fData);
        nByteRead+=sampleCountRead*sizeof(unsigned short int);
        nlinesread+=sampleCountRead;
      } else {
        buffer = fread(&pData, sizeof(unsigned short int), nlinespacket, _fData);
        nByteRead+=nlinespacket*sizeof(unsigned short int);
        nlinesread+=nlinespacket;
      }
      
      if (physChannel>=0 && physChannel<72 && chstatus) {

        if(Flag!=2) {
	  int thisCell = -999;
	  timeHit[channelN] = 0;
	  
	  for (Int_t itime=0; itime<Ntime; itime++) {dataADC[itime]=0;}
	  
	  if(compressMod_flag) {
	    if (sampleCount<=Ntime) {
	      for (Int_t itime=0; itime<sampleCount-2; itime++) {
		unsigned short int datum = ntohs(pData.data[itime]);
		if (datum & 0x1000) {//4096
		  thisCell = (datum & 0xFFF);
		  if (thisCell>0 && thisCell<511) {
		    timeHit[thisPhysChannel]= (datum&0xFFF);
		    dataContent[thisPhysChannel][itime] = timeHit[thisPhysChannel];	// 12 LS
		    if (datum > 0) {
		      tempCharge+=datum;
		    }
		    if (datum > maxcharge) {
		      maxcharge = datum;
		    }
		  }
	        } else {
		  if (datum != 0) {  // Dans le cas du mode COMPRESS
//>>>		    dataADC[itime] = datum - 250;
//>>>		    int datumC = datum - 250; 
		    dataADC[itime] = datum;
		    int datumC = datum; 
		    if (datumC > 0) {
		      tempCharge+=datumC;
		    }
		    if (datumC > maxcharge) {
		      maxcharge = datumC;
		    }
		    dataContent[thisPhysChannel][itime] = dataADC[itime];
//  		      if (itime != 11) hADCvsTIME[thisPhysChannel]->Fill(itime, dataADC[itime]);
 		    hADCvsTIME[thisPhysChannel]->Fill(itime, dataADC[itime]);
		  }
	        }
	      } //end of boucle over data
	    }
	  } else {
	    for (Int_t itime=0; itime<sampleCount; itime++) {
	      dataADC[itime]= ntohs(pData.data[itime]);
	      hADCvsTIME[thisPhysChannel]->Fill(itime, dataADC[itime]);
	      if (dataADC[itime] > 0) {
	        tempCharge+=dataADC[itime];
	      }
	      if (dataADC[itime] > maxcharge) {
	        maxcharge = dataADC[itime];
	      }
	    }
	  }
        }  //end of boucle over data	
   
        _nhitPerChan=0;
        _hitTimeMult=0;

        countPads++;

	
        for (int ip=0; ip<sampleCount; ip++) {
	  if (dataContent[thisPhysChannel][ip] > Threshold) {
            //start oh Hit
            _hitTimeMult++;
            //find maximum
            if (dataContent[thisPhysChannel][ip]>_hitMaximum) {
              _hitMaximum = dataContent[thisPhysChannel][ip];
              _hitBinMaximum = ip;
            }
            //compute sum of charge for all sample count for each channel
            _hitSum+= dataContent[thisPhysChannel][ip];
            //end of Hit
            if (dataContent[thisPhysChannel][ip+1] <= Threshold) {
              //record hit info
              HitTimeMultiplicity.push_back(_hitTimeMult);
              HitMaxCharge.push_back(_hitMaximum);
              HitBinMaxCharge.push_back(_hitBinMaximum);
              HitSum.push_back(_hitSum);
              HitChannelId.push_back(thisPhysChannel);
              //reset for new hit
              _hitTimeMult=0;
              _hitMaximum=-10;
              _hitSum=0;
              _hitBinMaximum=0;
              _nhitPerChan++;
// 	      cout << "_nhitPerChan: " << _nhitPerChan << endl;
// 	      cout << "thisPhysChannel: " << thisPhysChannel << endl;
	    }
	  }
        }
      
//       cout << "tempCharge, maxcharge, MaxCharge[thisPhysChannel]: " << tempCharge << ", " << maxcharge << ", " << MaxCharge[thisPhysChannel] << endl;
      
        if (thisPhysChannel != -10 && maxcharge>Threshold) {
	  if (tempCharge > MaxCharge[thisPhysChannel]) {
 	    MaxCharge[thisPhysChannel]= maxcharge;
	  }
	  //compute sum of ADC charge per event for each channel above threshold set at 100 ADC unit
//  	    padtotcharge[thisPhysChannel]+= tempCharge;
// 	    evtpadcharge[thisPhysChannel][evN] += tempCharge;
        }
      }
    }

    buffer = fread(&pEnd,sizeof(DataPacketEnd), 1, _fData);
    nByteRead+=sizeof(DataPacketEnd);
  
  }//end main channel


  if(Time_inloop<=1e-8*((ntohs(pHeader.ts_h))*pow(2,16)+ ntohs(pHeader.ts_l))) Time_inloop= 1e-8*((ntohs(pHeader.ts_h))*pow(2,16)+ ntohs(pHeader.ts_l));
  else {
    Nloop++;
    Time_inloop= 1e-8*((ntohs(pHeader.ts_h))*pow(2,16)+ ntohs(pHeader.ts_l));
  }
  Time = 1e-8*pow(2,32)*Nloop+Time_inloop;
  evNumber++; 
  select_flag=kFALSE;
  event_flag=kFALSE;
  sampleCountMax=Ntime;
  
  float maxbin = 0; 
  for ( int hid=0; hid<kPhysChannelMax; hid++ ) {
    if (hADCvsTIME[hid]->GetMaximumBin() >  maxbin) maxbin = hADCvsTIME[hid]->GetMaximumBin();
  }
  if ( 1 < maxbin) hMaxHitTimeBin->Fill(maxbin);

//   cout << "maxbin: " << maxbin << endl;
//   cout << "countPads: " << countPads << endl;

  return countPads;
}



//________________________________________________________
int DecodeNextEvent_old(void) 
{
  // Read next header or quit of end of file
  if(fread(&head, sizeof(EventHeader), 1, _fData) != 1) {
    fclose(_fData);
    return -1; // End of program
  }

  head.eventSize = ntohl(head.eventSize);
  head.eventNumb = ntohl(head.eventNumb);
  //head.eventTime = ntohl(head.eventTime);

  int rawDataSize = head.eventSize - sizeof(head);
  // cout << "Event number from header --> " << head.eventNumb << endl;
  // cout << " total rawdata size " << head.eventSize << endl ;
  // cout << "Rawdata size " << rawDataSize << endl ;
  // iFirstPacket=0;

  int nByteTot = rawDataSize ;
  int nByteRead=0;

  int countPads =0;

  for (int i=0; i<kPhysChannelMax; i++)
    timeHit[i] = 0;

  // Data reading
  while (nByteRead < nByteTot ) {

    // now read data packet header
    buffer=fread(&pHeader, sizeof(DataPacketHeader), 1, _fData);
    nByteRead+=sizeof(DataPacketHeader);

    // check sync fail
    if (GET_LOS_FLAG((ntohs(pHeader.hdr))))   cout << " LOS FLAG SET "       << endl;
    if (GET_SYNCH_FAIL((ntohs(pHeader.hdr)))) cout << " SYNC FAIL FLAG SET " << endl;

    // Check compress mode
    int compressed = GET_RB_COMPRESS(ntohs(pHeader.args));

    // compute asic#
    int tempAsic1 = GET_RB_ARG1(ntohs(pHeader.args));
    int tempAsic2 = GET_RB_ARG2(ntohs(pHeader.args));
    int channel   = tempAsic1/6;
    int asicN     = 10*(tempAsic1%6)/2 + tempAsic2;
    //cout << " channel " << channel << " asic " << asicN << endl;

    // get sample count
    int sampleCount = ntohs(pHeader.scnt);

    // take into account padding short int to get even total
    int sampleCountRead = sampleCount;
    if (sampleCount%2==1)  sampleCountRead++;

    // now read the data 
    buffer=fread(&pData, sizeof(unsigned short int), sampleCountRead, _fData);
    nByteRead+=sampleCountRead*sizeof(unsigned short int);
   
    // read end (crc stuff)
    buffer=fread(&pEnd,sizeof(DataPacketEnd), 1, _fData);
    nByteRead+=sizeof(DataPacketEnd);

    // compute physical channel (0-71)
    int physChannel=-10; 
    if (channel> 2 && channel < 15 ) {
      physChannel= channel -3; 
    } else if (channel> 15 && channel < 28 ) {
      physChannel= channel -4; 
    } else if (channel> 28 && channel < 53 ) {
      physChannel= channel -5; 
    } else if (channel> 53 && channel < 66 ) {
      physChannel= channel -6; 
    } else if (channel> 66  ) {
      physChannel= channel -7; 
    }

    int thisPhysChannel = asicN*72+physChannel;
    int tempCharge = 0; 
    // int tempNHit = 0;
    int thisCell = -999;
    int maxcharge = -50;
    // int maxTime = 510; 
 
    int timeBin = 0;

    //Generate Hit info
    _nhitPerChan   = 0;
    _hitTimeMult   = 0;

    bool hasOverThreshold = false;
    // bool wasOverThreshold = false;

    // Only physical channels
    if (physChannel>=0 && physChannel<72 && sampleCount>8) { 	// sampleCount>8

      // -------------------
      //  Uncompressed data
      // -------------------
      if (compressed == 0) {
        for (int ip=0; ip<sampleCount; ip++) {
          unsigned short int datum = ntohs(pData.data[ip]);
          dataContent[thisPhysChannel][ip] = ( datum & 0xFFF );	// 12 LSB 

          if (ip < (sampleCount-2) )
          {
              hADCvsTIME[thisPhysChannel]->Fill(ip, datum);
              countPads++;
          }

          // compute sum of charge for all sample count for each channel
          if (datum > 0) {
            tempCharge+=datum;
          }
          if (datum > maxcharge) {
            maxcharge = datum;
          }
        }
      }

      // -------------------
      //   Compressed data
      // -------------------
      if (compressed == 1) {
        for (int ip=0; ip<sampleCount; ip++) {			// ip<sampleCount-2

          unsigned short int datum = ntohs(pData.data[ip]);

          if (datum & CELL_INDEX_FLAG) {			// 0x1000
            //cout << "new packet found cell " << (datum&0xFFF) << endl;//4095
            thisCell = datum & 0x1FF;     			// or GET_CELL_INDEX(datum);
            if ( thisCell < (sampleCount-1) ) {
              timeHit[thisPhysChannel]= thisCell;
              nHit[thisPhysChannel]++; 

             //timeBin = TMath::Max(thisCell-10, 0);
              timeBin = thisCell - 10;   			// there are pads even for bin 0 !
            }
            else 
              thisCell = -999;
          } 							// end of timebin cell
          else if ( thisCell != -999 ) {
            if ( timeBin < 0 ) {
              timeBin++;
              continue;
            }

            datum &= 0xFFF;					// 12 LSB are for sample data

            //hADCall->Fill(datum);				// YHS: temp

//>>>            int datumC = datum - 250; 
            int datumC = datum - 250; 

            dataContent[thisPhysChannel][ip]=datumC;
            // hADCvsTIME[thisPhysChannel]->Fill(ip,dataContent[thisPhysChannel][ip]);
            // compute sum of charge for all sample count for each channel
            if (datumC > 0 ) {
              tempCharge+=datumC;
            }
            if (datumC > maxcharge) {
              maxcharge = datumC; 
	    }
            if (timeBin != (sampleCount-1))			// by chance it could be the 511
              {
                hADCvsTIME[thisPhysChannel]->Fill(timeBin++, datumC); 
                countPads++;
              }
            else
              break;						// no more meaningful adc
//              continue;
          }
        }
      }
    
      if ( hasOverThreshold )
        countPads++;

      //
      // Process hits
      //
      
      //Generate Hit info
      _nhitPerChan=0;

      for (int ip=0; ip<sampleCount; ip++) {
         if (dataContent[thisPhysChannel][ip] > Threshold) {
           //countPads++;
           //start oh Hit
           _hitTimeMult++;
           //find maximum
           if (dataContent[thisPhysChannel][ip]>_hitMaximum) {
              _hitMaximum = dataContent[thisPhysChannel][ip];
              _hitBinMaximum = ip;
            }
	
            //compute sum of charge for all sample count for each channel
            _hitSum+= dataContent[thisPhysChannel][ip];
            //end of Hit
            if (dataContent[thisPhysChannel][ip+1] <= Threshold) {
              //record hit info
              HitTimeMultiplicity.push_back(_hitTimeMult);
              HitMaxCharge.push_back(_hitMaximum);
              HitBinMaxCharge.push_back(_hitBinMaximum);
              HitSum.push_back(_hitSum);
              HitChannelId.push_back(thisPhysChannel);
              //reset for new hit
              _hitTimeMult=0;
              _hitMaximum=-10;
              _hitSum=0;
              _hitBinMaximum=0;
              _nhitPerChan++;
            }
          }
       }
 
       if(_nhitPerChan!=0) {
         // cout << "Event \t" << head.eventNumb << "\t # Hit per chan = " << _nhitPerChan << "\t for channel\t" << thisPhysChannel << endl;
       }

       if (maxcharge>Threshold) {
         if (tempCharge > MaxCharge[thisPhysChannel]) {
           MaxCharge[thisPhysChannel]= maxcharge;
         }
         //compute sum of ADC charge per event for each channel above threshold set at 100 ADC unit
         padtotcharge[thisPhysChannel]+= tempCharge; 
         evtpadcharge[thisPhysChannel][head.eventNumb] += tempCharge;
       }

     }//end of if physchanel

   }//end of while nByteRead
  
  /*
  float time0 = 1024.;
  //int   maxHid = -1;
  for ( int hid=0; hid<kPhysChannelMax; hid++ ) {
    if (  hADCvsTIME[hid]->GetMaximum() > Threshold ) {
      float maxbin = hADCvsTIME[hid]->GetMaximumBin();
      if ( 1 < maxbin  && maxbin < time0 ) {
        time0 = maxbin;
        //maxHid = hid;
      }
    }
  }
  //hMaxHitTimeBin->Fill( hADCvsTIME[maxHid]->GetMaximumBin() );
  hMaxHitTimeBin->Fill( time0 );
  */

  float maxbin; 
  for ( int hid=0; hid<kPhysChannelMax; hid++ ) {
    maxbin = hADCvsTIME[hid]->GetMaximumBin();
  }
//    cout << "maxbin: " << maxbin << endl;
  if ( 1 < maxbin) {
    hMaxHitTimeBin->Fill(maxbin);
  }

/*
  //
  // Find time0 among the top- and bottom-rows
  // 
  // 1. Check if there is hits on both rows
  // 2. If so compare timebin for max hit on the row
  //
  bool isHitOnTop = false;
  bool isHitOnBot = false;
  float time0 = 2048.;

  for (int ipad=0; ipad<72; ipad++) {
    int hid = iCHAN[ipad];
    if ( hADCvsTIME[hid]->GetMaximum() > Threshold ) {
      isHitOnBot = true;
      if ( hADCvsTIME[hid]->GetMaximumBin() < time0 ) {
        time0 = hADCvsTIME[hid]->GetMaximumBin();
     }
    }
  }
  for (int ipad=1656; ipad<kPhysChannelMax; ipad++) {
    int hid = iCHAN[ipad];
    if ( hADCvsTIME[hid]->GetMaximum() > Threshold ) {
      isHitOnTop = true;
      if ( hADCvsTIME[hid]->GetMaximumBin() < time0 ) {
        time0 = hADCvsTIME[hid]->GetMaximumBin();
      }
    }
  }
  if ( isHitOnTop && isHitOnBot ) {
//    hMaxHitTimeBin->Fill( time0 );
  }

  //hMaxHitTimeBin->Fill( hADCvsTIME[maxHid]->GetMaximumBin() );
 

  //	if(countPads>HitsOnPads) cout << "countPads :" << countPads << endl;;
  //if(countPads>15) cout << "countPads :" << countPads << endl;;
*/
  return countPads;
}


//________________________________________________________
///Fill histo
//________________________________________________________
void FillRoHisto() {
	
  //hPattern->Reset();		
  for (int iPhysChan=0; iPhysChan < kPhysChannelMax; iPhysChan++) {
		
		
    if (MaxCharge[iPhysChan] > Threshold) {
      hRawCharge->Fill(MaxCharge[iPhysChan]);
      //		cout << "iPhysChan: " << iPhysChan << "\tcol,row: " << iCOLUMN[iPhysChan] << ","
      //		<< iROW[iPhysChan]<< "\tChargeMax:" << MaxCharge[iPhysChan] << endl;
    }
    hPattern->Fill(iCOLUMN[iPhysChan],iROW[iPhysChan],MaxCharge[iPhysChan]);
  }

}

//________________________________________________________
void FillHitHisto() 
{
  int _hitmax = -10;
  int _hitmaxpadId = 0;
	
  for (uint iHit=0; iHit<HitTimeMultiplicity.size(); iHit++) 
  {
    hHitChargeMax->Fill(HitMaxCharge[iHit]);
    hHitBinMax->Fill(HitBinMaxCharge[iHit]);
    hHitMaxVSPad->Fill(HitChannelId[iHit],HitMaxCharge[iHit]);
    hHitSum->Fill(HitSum[iHit]);
    hHitTimeMult->Fill(HitTimeMultiplicity[iHit]);
    hHitTimeMultVSChargeMax->Fill(HitMaxCharge[iHit],HitTimeMultiplicity[iHit]);
    //Search which pad contents max hit
    if (HitMaxCharge[iHit]>_hitmax) {
      _hitmax = HitMaxCharge[iHit];
      _hitmaxpadId = HitChannelId[iHit];
      cADCvsTime_monitoring->cd();
      hADCvsTIME[_hitmaxpadId]->Draw();
    }
    if (HitMaxCharge[iHit]>0) {
      int jASIC = HitChannelId[iHit]/72;
      int jchRed = HitChannelId[iHit]%72;
      int jFEC = jASIC/4;
      hHitHIST[jASIC]->Fill(float(jchRed));
      hChargeHIST[jASIC]->Fill(float(jchRed),float(HitMaxCharge[iHit]));
      hChargeASIC->Fill(float(jASIC),float(HitMaxCharge[iHit]));
      hChargeFEC->Fill(float(jFEC),float(HitMaxCharge[iHit]));
      hHitASIC->Fill(float(jASIC));
      hHitFEC->Fill(float(jFEC));
    }
  }

  hHitNumberPerEvent->Fill(HitChannelId.size());
  hHitNumberPerPad->Fill(_nhitPerChan);

  //cADCaccum->cd();
  //hADCall->Draw();
}

///Event loop (DoMonitoring function)
//________________________________________________________
void DoMonitoring() 

{
  int iEvent = 0;
  static int iEventLoop = 0;
  // int ThlEvents=5;

  //Loop to check monitoring stop decided by user
  // while (!fStop && !fEvenTargetReach) {
  // gSystem->ProcessEvents();

  //Main event loop
  while (!fStop && !fEvenTargetReach) {
    //reset event
    for (int ich=0; ich<kPhysChannelMax; ich++) {
      timeHit[ich]=0;
      padtotcharge[ich]=0;
      MaxCharge[ich]=0;
      nHit[ich]=0;
      hADCvsTIME[ich]->Reset();
    }
    nhit = 0;
    //reset hit vector
    HitTimeMultiplicity.clear();
    HitMaxCharge.clear();
    HitBinMaxCharge.clear();
    HitSum.clear();
    HitChannelId.clear();
    //decode next event
    //	//get event time
    //	int eventTimeH = ntohs(pHeader.ts_h);
    //	int eventTimeL = ntohs(pHeader.ts_l);
    //	cout << "eventTimeH =\t" << eventTimeH << "\t eventTimeL=\t" << eventTimeL << endl;

    HitsOnPads = DecodeNextEvent();
    if ( HitsOnPads == -1 ) {
      cout << "EOF reached" << endl;
      break;
    }

    FillHitHisto();

    //get event number
    iEvent = head.eventNumb;
    if (iEvent!=iEventLoop) {
      //cout << "Event maybe lost, check this !" << endl;
      //cout << " iEvent, iEventLoop = " << iEvent << ", " << iEventLoop << endl;
      //break;
    }
    //Update histo
    if (iEvent%RefreshEvent==0 && iEvent!=0){
      cout << "Number of analyzed events so far " << iEvent << endl;
      //cout << "Number of analyzed events so far " << iEventLoop << endl;
      UpdateCanvases(kTRUE);
      gSystem->ProcessEvents();
    }
    if (iEvent==nEvent) {
      cout << "Number of event target to be monitored is reached" << endl;
      fEvenTargetReach = kTRUE;
    }

//    if (HitsOnPads>ThlEvents) {
      
      FillRoHisto();
      cPermanent[0]->Modified();
      cPermanent[0]->Update();

      // PadPanel
//       FillPadPanelWithColor();
//       cPadPanel->Modified();
//       cPadPanel->Update();
      if (iEvent==nEvent && nEvent!=1000000) {
        FillPadPanelWithColor();
        cPadPanel->Modified();
        cPadPanel->Update();
        
	char gifName[20];
        sprintf(gifName , "./GIF/%4.5d.gif", head.eventNumb);
        cPadPanel->Print(gifName);
      } else {
        FillPadPanelWithColor();
        cPadPanel->Modified();
        cPadPanel->Update();

	char gifName[20];
        sprintf(gifName , "./GIF/%4.5d.gif", head.eventNumb);
        cPadPanel->Print(gifName);
      }


      gSystem->ProcessEvents();

      if ( fSkipToNextThreshold )
        fEvenTargetReach = kTRUE;  // make it to stop
//    }

    iEventLoop ++;

  }//End of event loop

  // 	iLoop++;
  // 	}//End of while (fStop)

  UpdateCanvases(kTRUE);
  if ( fStop )
    cout << "Monitoring is finished" << endl;
}

//________________________________________________________
///Main
//________________________________________________________
int main(int argc, char **argv) {

  int loop;

  char Filename_acq[255];
  strcpy(Filename_acq,"./RUN.acq");
  char FilenameLayout_txt[255];
  strcpy(FilenameLayout_txt,"./MapLayoutFile.txt");

  for (loop = 1; loop < argc; loop++) {
    if (argv[loop][0] == '-') {
      switch (argv[loop][1]) {
      case 'n':
        nEvent = strtol(argv[loop+1], NULL, 0);
        loop++;
        break;
      case 'r':
        RefreshEvent = strtol(argv[loop+1], NULL, 0);
        loop++;
        break;
      case 't':
        Threshold = strtol(argv[loop+1], NULL, 0);
        loop++;
        break;
      case 'f':
        strcpy(Filename_acq, argv[loop+1]);
        loop++;
        break;
      case 'h':
        printf("Usage: %s [OPTIONS]\n\n", argv[0]);
        printf("Options:\n");
        printf("\n-n [number of events] (default = %d)", nEvent);
        printf("\n-r [number of events before histogram updating] (default = %d)", RefreshEvent);
        printf("\n-t [threshold in ADC] (default = %d)", Threshold);
        printf("\n-f <input file without .acq extension>         (default = %s)", Filename_acq);
        printf("\n");
        return 0;
        break;
      }
    }
  }
  
  TRint *theApp = new TRint("App", 0, 0);

  if (gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }

  //Launch GUI
  cout << "Running GUI environment for MM monitoring ..." << endl;
  MMonMainFrame(0,400,200);
  cout << "--------------------------------" << endl;

  //Prepare mapping
  cout << "Opening layout file ..." << endl;
  OpenLayoutFile(FilenameLayout_txt);
  if ( ! _fLayout ) 
    return 1;

  cout << "Prepare mapping ..." << endl;
  ReadMapping();
  cout << "--------------------------------" << endl;

  cout << "Reading geomtry info ..." << endl;
  int status = ReadGeomMap();
  if ( status != 0 ) 
    return 1;
  cout << "--------------------------------" << endl;
  
  //Open data file
  OpenDataFile(Filename_acq);
  if ( ! _fData )
    return 1;
  cout << "--------------------------------" << endl;

  theApp->Run();

  //Close files
  CloseLayoutFile();
  CloseDataFile();

  return (0);
}


//wwx--------------------------------------------------------------------------------------
void DrawXvsTime()
{

  float x0=0., x11=0., x12=0., q11=0., q12=0.;
  int t11=0., t12=0., t0=0.;
 
  for (int iPad=0; iPad < kPhysChannelMax; iPad++)  {
    int Row = iPad / 72;

    if (Row == 11 || Row == 12)  {
      char hname[20];
      float firstN=0., lastN=0.;
      float data = 0., datanew=0., x01=0., x02=0.;
      float firstSum = 0.;
      float lastSum = 0.;
      float slope = 0.;
      int maxBin = -1;

      int hiPad = iCHAN[ iPad ];	
	sprintf(hname, "hADC %d", hiPad);

      TH1F *hxtime = dynamic_cast<TH1F*>( gROOT->FindObject( hname ) );
      if ( hxtime ) {

        for (int bin=1; bin<10; bin++)  {
          float firstn;
          firstn = hxtime -> GetBinContent(bin);
          firstSum = firstSum + firstn;
        }
        firstN = firstSum / 9;

        for (int bin=501; bin<510; bin++)  {
	    float lastn;
          lastn = hxtime -> GetBinContent(bin);
          lastSum = lastSum + lastn;
        }
        lastN = lastSum / 9;

        slope = (lastN - firstN) / 500;
        
        float maxA = 0.;
        float absdatanew = 0.;
        float newdata=0.; 
        for (int bin=1; bin<510; bin++)  {
          data = hxtime -> GetBinContent(bin);
          datanew = data - (slope * (bin - 5) + firstN);
	  absdatanew = fabs(datanew); 
	    //if ( absdatanew < 30 ) { datanew = 0.0 ;}  //why can't fill 0.0?
          if (absdatanew > maxA)  {
            maxA = absdatanew;
            maxBin = bin;
            newdata = datanew;

          }
        }

        if (Row == 11 && maxA > 40)  {
          x01 = (x[iPad][0] + x[iPad][1]) / 2;
          x11 = x11 + newdata*x01; 
          t11 = t11 + newdata*maxBin;
          q11 = q11 + newdata;
        }


        if (Row == 12 && maxA > 40)  {
          x02 = (x[iPad][0] + x[iPad][1]) / 2;

          x12 = x12 + newdata*x02; 
          t12 = t12 + newdata*maxBin;
          q12 = q12 + newdata;
        }
      }
    }
  }

  x0 = (x11/q11 + x12/q12) / 2; 
  t0 = (t11/q11 + t12/q12) / 2;

  hx144 -> Fill(x0);
  ht144 -> Fill(t0);
  hxt144 -> Fill(t0, x0);
         
  cBeamVSx -> Modified(); 
  cBeamVSx -> Update();
  cBeamVSt -> Modified(); 
  cBeamVSt -> Update();
  cBeam -> Modified(); 
  cBeam -> Update();  
  cBeam2 -> Modified(); 
  cBeam2 -> Update();  
} 


 
