#ifndef PadSignal_h
#define PadSignal_h

#include "RQ_OBJECT.h"

#include "TPolyLine.h"
#include "Pixel.h"

class TCanvas;

class PadSignal
{
  RQ_OBJECT("PadSignal")

  public:

    PadSignal(Pixel& P);
    ~PadSignal() { delete fPolyLine; }

    // Getters
    int ID(){return fId;}
    int i(){return fi;}
    int j(){return fj;}
    TPolyLine *polyline(){return fPolyLine;}

    void SetCanvas( TCanvas* canv );

    // Mouse button event handler
    void ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel);

    // SIGNAL that
    void PadSelected(Int_t padId); //*SIGNAL*

  private:
    int fId;
    int fi;
    int fj;
    int fampl;
    TPolyLine *fPolyLine;
    static TCanvas *fPad;
  };

inline void PadSignal::SetCanvas( TCanvas* canv )
{
  if ( fPad ) return;
  fPad = canv;
}

#endif
