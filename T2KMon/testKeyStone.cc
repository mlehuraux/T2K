
#include <iostream>

using namespace std;

#include "TCanvas.h"
#include "TPolyLine.h"
#include "TMath.h"
#include "RQ_OBJECT.h"


//-----------------------------------------------------------------------------
// Primitive Class for KeyStone Pad :
//-----------------------------------
class KeyStonePad
{
RQ_OBJECT("KeyStonePad")

public:
  KeyStonePad(int id=0, double *x=0, double *y=0);
  ~KeyStonePad() { delete fPolyLine; }
  
  int GetID() const { return fId; }
  
  void SetID(int id) { fId = id; }
  
  void SetFillColor(int colorId);
  
  void Modified() {} 
  void Draw();

  // Mouse button event handler
  void ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel);

  // SIGNALS
  void PadSelected(Int_t id); //*SIGNAL*
  
private:
  int fId;      // it's own id 
  TPolyLine *fPolyLine;

};


KeyStonePad::KeyStonePad(int id, double *x, double *y)
{
  fId = id;

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

inline void KeyStonePad::SetFillColor( int color )
{
  fPolyLine->SetFillColor( color );
}

inline void KeyStonePad::Draw()
{
  if ( fPolyLine != 0 ) {
    fPolyLine->Draw();
  }
}

// Mouse button event handler
void KeyStonePad::ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel)
{
  // Decide if the mouse is in this
  if ( event == kButton1Down ) {
    double *x = fPolyLine->GetX();
    double *y = fPolyLine->GetY();

    /*
    cout << "padId = " << fId << " is inside ? " 
         << TMath::IsInside(gPad->AbsPixeltoX(px), gPad->AbsPixeltoY(py), 
                            5, x, y) << " : " 
         << "x[0]==x[4] ? " << (x[0]==x[4]) << ", "
         << "y[0]==y[4] ? " << (y[0]==y[4]) << ", "
         << "px=" << px << ", " << "py=" << py << ", " 
         << "gPad->AbsPixeltoX(px)=" << gPad->AbsPixeltoX(px) << ", "
         << "gPad->AbsPixeltoY(py)=" << gPad->AbsPixeltoY(py) << endl;
    */

    if ( fPolyLine->DistancetoPrimitive(px,py) == 0 ) {
      //cout << "We're in a pad : " << fId << endl;
      PadSelected( fId );
    }
  }

  // If so, draw histogram in a new window

}

void KeyStonePad::PadSelected(Int_t id)
{
  Emit("PadSelected(Int_t)", fId);
}


void PrintSelectedPadInfo(Int_t id)
{
  cout << "We're in a pad : " << id << endl;
}


void testKeyStone()
{
  TCanvas *c = new TCanvas("c","c", 400, 200);

  double x0[] = { 0.2, 0.3, 0.4, 0.1 };
  double y0[] = { 0.2, 0.2, 0.8, 0.8 };
  double x1[] = { 0.7, 0.8, 0.9, 0.6 };
  
  KeyStonePad *kpad1 = new KeyStonePad(0, x0, y0);
  KeyStonePad *kpad2 = new KeyStonePad(1, x1, y0);
  
  kpad1->Draw();
  kpad2->Draw();

  c->SetEditable(false);
    
  c->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "KeyStonePad", 
             kpad1, "ExecuteEvent(Int_t,Int_t,Int_t,TObject*)");
  c->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "KeyStonePad", 
             kpad2, "ExecuteEvent(Int_t,Int_t,Int_t,TObject*)");

  kpad1->Connect("PadSelected(Int_t)", 0, 0, "PrintSelectedPadInfo(Int_t)");
  kpad2->Connect("PadSelected(Int_t)", 0, 0, "PrintSelectedPadInfo(Int_t)");
}
