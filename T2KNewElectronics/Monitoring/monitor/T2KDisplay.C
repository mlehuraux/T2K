// example.C

#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

int iEvent = 0;

class T2KMainFrame {

  RQ_OBJECT("T2KMainFrame")

private:
  TGMainFrame          *fMain;
  TRootEmbeddedCanvas  *fEcanvas;
  TGHorizontalFrame    *hframe;
  TGTextButton         *next;
  TGTextButton         *prev;
  TGTextButton         *exit;

public:
  T2KMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~T2KMainFrame();
  void DrawNext(Int_t);
  void CloseWindow();
  void HandleButton(Int_t);
  void  SetValue(Int_t); // *SIGNAL*
};

T2KMainFrame::T2KMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {
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

void T2KMainFrame::DrawNext(Int_t ev) {

  static const int nPadx = 36;
  static const int nPady = 32;
  static const int samples = 511;

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

  gStyle->SetOptStat(0);
  TFile * f1 = TFile::Open("R2019_05_24-08_53_45-000_processed.root", "UPDATE");
  TTree * tree = (TTree*)f1->Get("tree");
  //cout << "File open" << endl;

  Int_t eventNumber;
  Int_t PadAmpl[nPadx][nPady][samples];
  Int_t PadMaxAmpl[nPadx][nPady];
  Int_t PadMaxAmplTimeSample[nPadx][nPady];
  Int_t ntot = tree->GetEntries();

  tree->SetBranchAddress("eventNumber", &eventNumber);
  tree->SetBranchAddress("PadAmpl", &PadAmpl);
  tree->SetBranchAddress("PadMaxAmpl", &PadMaxAmpl);
  tree->SetBranchAddress("PadMaxAmplTimeSample", &PadMaxAmplTimeSample);

  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->cd();
  tree->GetEntry(ev);
  TString nevt = "Event: "; 
  nevt += ev;
  TH2I *pads = new TH2I("pads",nevt.Data(), nPadx, 0, nPadx, nPady, 0, nPady);
  for (int i=0; i < nPadx; i++)
  {
    for (int j=0; j<nPady; j++)
    {
      pads->Fill(i, j, PadMaxAmpl[i][j]);
    }
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
  f1->Close();
}

T2KMainFrame::~T2KMainFrame() {
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

void T2KMainFrame::CloseWindow() {
  gApplication->Terminate(0);
}

void T2KMainFrame::HandleButton(Int_t id) {
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

void T2KDisplay() {
  // Popup the GUI...
  new T2KMainFrame(gClient->GetRoot(),800,800);
}
