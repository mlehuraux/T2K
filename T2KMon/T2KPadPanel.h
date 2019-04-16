
#include "RQ_OBJECT.h"

#include "TPolyLine.h"

class TCanvas;

//-----------------------------------------------------------------------------
// Primitive Class for T2K Pad :
//-----------------------------------
class T2KPadPanel
{
RQ_OBJECT("T2KPadPanel")

public:

  T2KPadPanel(int id=0, double *x=0, double *y=0);
  ~T2KPadPanel() { delete fPolyLine; }
 
  int GetID()           const { return fId; }
  int GetRowNumber()    const { return fId / 72; }
  int GetColumnNumber() const { return fId % 72; }

  void SetID(int id)     { fId = id; }
  void SetHistID(int id) { fHid = id; }

  void SetHistIDLeft(int id) { fHidLeft = id; }
  void SetHistIDRight(int id) { fHidRight = id; }
  
  void SetFillColor(int colorId);
  
  void Modified() {} 
  void Draw();
  
  void SetCanvas( TCanvas* canv );

  // Mouse button event handler
  void ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel);

  // SIGNAL that 
  void T2KPadPanelSelected(Int_t padId); //*SIGNAL*

private:
  int            fId;      // it's own id 
  int            fHid;
  int            fHidLeft;  // instead, we can keep column no and use it
  int            fHidRight;
  TPolyLine      *fPolyLine;
  static TCanvas *fPad;
};

inline void T2KPadPanel::SetFillColor( int color )
{
  fPolyLine->SetFillColor( color );
}

inline void T2KPadPanel::Draw()
{
  if ( fPolyLine != 0 ) {
    fPolyLine->Draw();
  }
}


inline void T2KPadPanel::SetCanvas( TCanvas* canv )
{
  if ( fPad ) return;
  fPad = canv;
}

void PrintSelectedPadInfo(Int_t);
