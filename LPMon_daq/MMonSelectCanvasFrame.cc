//---------------------------------------------------------------
//---------------------------------------------------------------
//           Class name : MMonSelectCanvasFrame      
//Class description : Allow to select canvas which should be displayed  
//Author : J. BEUCHER  
//Last update : 06/03/2008  
//
//---------------------------------------------------------------

// Include basic libraries
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <assert.h>
#include <vector>
#include <ctime>

// Include basic libraries for ROOT
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGButton.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TROOT.h"
#include "TSystem.h"

// Include MMon libraries
#include "MMonSelectCanvasFrame.h"

ClassImp(MMonSelectCanvasFrame);

using namespace std;

//_____________________________________________________________________________
MMonSelectCanvasFrame::MMonSelectCanvasFrame(const TGWindow *p, 
                                             const TGWindow *main, 
                                             UInt_t w, UInt_t h)
{
  TGTransientFrame *fTrMain  = new TGTransientFrame(p,main,w,h);

  TGVerticalFrame *fhFr1     = new TGVerticalFrame(fTrMain,400,40);	
  TGLayoutHints *fhFr1Layout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX,
                                                 2, 2, 2, 2);

  TGLayoutHints *fL4 = new TGLayoutHints(kLHintsTop | kLHintsLeft,0, 0, 5, 0);

	
  for (int i=0;i<gROOT->GetListOfCanvases()->GetEntries();i++) {
    //cout<<"GetlistCanvases into MMonSelectCanvases method"<<endl;
    fCheck[i] = new TGCheckButton(fhFr1,gROOT->GetListOfCanvases()->At(i)->GetName(),i);
    fCheck[i]->Connect("Clicked()","MMonSelectCanvasFrame",
                       this,"HandleButton(int)");
    if (((TRootCanvas*)((TCanvas*)gROOT->GetListOfCanvases()->At(i))->GetCanvasImp())->IsMapped())
      fCheck[i]->SetState(kButtonDown);
    else 
      fCheck[i]->SetState(kButtonUp);
    fhFr1->AddFrame(fCheck[i],fL4);
  }


  fTrMain->AddFrame(fhFr1,fhFr1Layout);
  fTrMain->CenterOnParent();
  fTrMain->SetWindowName("Canvas Selection");
  fTrMain->MapSubwindows();
  fTrMain->Resize();
  fTrMain->MapWindow();
}

//____________________________________________________________________________________________________
MMonSelectCanvasFrame::~MMonSelectCanvasFrame()
{
  delete [] fCheck;
}

//________________________________________________________________________________
void MMonSelectCanvasFrame::HandleButton(Int_t id)
{
  if (id == -1) {
    TGButton *btn = (TGButton *) gTQSender;
    id = btn->WidgetId();
  }
  if (!fCheck[id]->IsDown()) {
    cout << "!fCheck[id]->IsDown() " << id << endl;
    ((TRootCanvas*)((TCanvas*)gROOT->GetListOfCanvases()->At(id))->GetCanvasImp())->UnmapWindow();
  }
  else {
    cout << "fCheck[id]->IsDown() " << id << endl;
    ((TRootCanvas*)((TCanvas*)gROOT->GetListOfCanvases()->At(id))->GetCanvasImp())->MapWindow();
  }
}

