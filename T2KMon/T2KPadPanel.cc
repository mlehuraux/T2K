
#include <iostream>

using namespace std;

#include "TH1F.h"
#include "TROOT.h"

#include "TCanvas.h"
#include "TPolyLine.h"
#include "TMath.h"
#include "RQ_OBJECT.h"

#include "T2KPadPanel.h"

//-----------------------------------------------------------------------------
// Primitive Class for KeyStone Pad :
//-----------------------------------
TCanvas *T2KPadPanel::fPad;

T2KPadPanel::T2KPadPanel(int id, double *x, double *y)
  : fId(id), fHid(-1), fHidLeft(-1), fHidRight(-1)
{
  const int kNPoints = 4;
  double xCorner[kNPoints+1];  // +1 to close
  double yCorner[kNPoints+1];
  if ( x!=0 && y!=0 ) {
    for (int i=0; i<kNPoints; i++) {
      xCorner[i] = x[i];
      yCorner[i] = y[i];
    }
    xCorner[kNPoints] = x[0];
    yCorner[kNPoints] = y[0];
    
    fPolyLine = new TPolyLine(kNPoints+1, xCorner, yCorner);
    fPolyLine->SetLineColor(1);
    fPolyLine->SetLineWidth(2);
    fPolyLine->Draw("f");
  }
  else {
    fPolyLine = 0;
  }
}

// Mouse button event handler
void T2KPadPanel::ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel)
{
  //const int width  = 400;
  //const int height = 300;
  const int width  = 900;
  const int height = 400;
  static bool first = true;

  static TCanvas *cADC;

  if ( event == kButton1Down ) {

  
    //double *x = fPolyLine->GetX();
    //double *y = fPolyLine->GetY();
    //
    //cout << "padId = " << fId << " is inside ? " 
    //     << TMath::IsInside(fPad->AbsPixeltoX(px), fPad->AbsPixeltoY(py), 
    //                        5, x, y) << " : " 
    //     << "x[0]==x[4] ? " << (x[0]==x[4]) << ", "
    //     << "y[0]==y[4] ? " << (y[0]==y[4]) << ", "
    //     << "px=" << px << ", " << "py=" << py << ", " 
    //     << "gPad->AbsPixeltoX(px)=" << fPad->AbsPixeltoX(px) << ", "
    //     << "gPad->AbsPixeltoY(py)=" << fPad->AbsPixeltoY(py) << ", "
    //     << "gPad->GetName()="       << fPad->GetName() << endl;

    // Is the mouse inside ?
    if ( TMath::IsInside(fPad->AbsPixeltoX(px), fPad->AbsPixeltoY(py), 
                         5, fPolyLine->GetX(), fPolyLine->GetY())) {

      //if ( first ) {
      //  cADC = new TCanvas("cADC", "ADC", width, height);
      //  first = false;
      //}

      //cout << "We're in a pad : " << fId << " (hid=" << fHid << ")" << endl;

      // Get layout info
      int row = fId / 72;
      int col = fId % 72;

      // Left   -\  need map !  <== (r,w) -> pad id -> chan id,  OR
      // Right  -/  be able to get pointer of other obj by using index
      
      TVirtualPad* gPadOld = gPad;

      // Just in case check if the canvas is destroyed
      cADC = dynamic_cast<TCanvas*>( gROOT->FindObject("cADC") );
      if (!cADC) {
        cout << "Creating a window for ADC waveform" << endl;
        cADC = new TCanvas("cADC", "ADC", width, height);

        cADC->Divide(3,1);
      }

      cADC->cd(0); // NEED to change gPad to point this to make Draw() work cor.
      
      char hname[20];
      sprintf(hname, "hADC %d", fHid);
      TH1F *h = dynamic_cast<TH1F*>( gROOT->FindObject( hname ) );
      if ( h ) {
        //cout << "Found hist object : " << h->GetName() << endl;
        cADC->cd(2);  // centre
        h->SetStats( kFALSE );
        h->Draw();
        //cADC->Update();
      }
      else {
        cout << "Can't find hist object for hid = " << fHid << endl;
      }

      // Left Pad
      sprintf(hname, "hADC %d", fHidLeft);
      TH1F *hL = dynamic_cast<TH1F*>( gROOT->FindObject( hname ) );
      if (hL) {
        cADC->cd(1);
        hL->SetStats( kFALSE );
        hL->Draw();
      }

      // Right Pad
      sprintf(hname, "hADC %d", fHidRight);
      TH1F *hR = dynamic_cast<TH1F*>( gROOT->FindObject( hname ) );
      if (hR) {
        cADC->cd(3);
        hR->SetStats( kFALSE );
        hR->Draw();
      }

      cADC->Update();
      cADC->RaiseWindow();

      gPad = gPadOld;

      T2KPadPanelSelected( fId );
    }
  }

  // If so, draw histogram in a new window
  
}

void T2KPadPanel::T2KPadPanelSelected(Int_t) 
{
   // Emit T2KPadPanelSelected() signal.

   Emit("T2KPadPanelSelected(Int_t)", fId);
}


/*
void PrintSelectedPadInfo( Int_t padId )
{
  cout << "SelectedPadInfo: " ;

  int row = padId / kMaxCols;
  int col = padId % kMaxCols;
  int hId = iCHAN[padId];

  //static TCanvas *cADC = new TCanvas("cADC","ADC", 200, 150);
  //cADC->cd();
      
  //cout << "SelectedPadInfo: " 
  cout << "SelectedPadInfo: " 
       << "row=" << row << ", "
       << "col=" << col << ", "
       << "histId=" << hId << endl;
}
*/
