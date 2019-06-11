#ifndef T2KMainFrame_h
#define T2KMainFrame_h

#include "T2KConstants.h"
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>
#include <TApplication.h>

class T2KMainFrame {

  RQ_OBJECT("T2KMainFrame")

private:
  TGMainFrame          *fMain;
  TRootEmbeddedCanvas  *fEcanvas;
  TGHorizontalFrame    *hframe;
  TGTextButton         *next;
  TGTextButton         *prev;
  TGTextButton         *beam;
  TGTextButton         *cosmic;
  TGTextButton         *thresplus;
  TGTextButton         *thresminus;
  TGTextButton         *thresplusplus;
  TGTextButton         *exit;
  TGButtonGroup        *modes;
  TGTextButton         *fStart;


public:
  T2KMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~T2KMainFrame();
  void Monitor(int mode);
  void DrawNext(Int_t ev, int mode);
  void CloseWindow();
  void HandleButton(Int_t);
  void SetValue(Int_t); // *SIGNAL*
  void ChangeStartLabel();
  static void *loop(void * ptr);
};

void scan();

#endif
