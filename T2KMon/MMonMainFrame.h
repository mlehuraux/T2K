#ifndef __MMonMainFrame_h__
#define __MMonMainFrame_h__

//#include "TGWindow.h"
#include "TGFrame.h"
#include "TGMenu.h"
#include "TGButton.h"
#include "TGLayout.h"
#include "TGNumberEntry.h"
#include "TGTextEntry.h"
#include "TGProgressBar.h"
#include "TString.h"
#include "RQ_OBJECT.h"
#include "TTimer.h"

enum ETestCommandIdentifiers {
  M_OPEN_BROWSER,
  M_CANVAS_SELECT
};

enum EButtonIdentifier {
  B_START_ANALYSE,
  B_NEXT_EVENT,
  B_SKIP_TO_NEXT_THRESHOLD,
  B_STOP_ANALYSE,
  C_CANVAS
};

class MMonMainFrame
{
  RQ_OBJECT("MMonMainFrame");
	
 private:
  //Declaration
  TGMainFrame         *fMain;
  TGMenuBar           *fMenuBar;
  TGPopupMenu         *fMenuFile;
  TGButton            *fFillButton, *fStopButton;
  TGButton            *fNextButton, *fSkipButton;  // YHS
  TGVerticalFrame     *fHFrame;
  TGLayoutHints       *fMenuBarLayout, *fMenuBarItemLayout, *fCanvasLayout;
  TGLayoutHints       *fHFrameLayout, *fFillButtonLayout, *fStopButtonLayout;
  TGLayoutHints       *fFitprintLayout;
  TGNumberEntryField  *fT;
  TGCheckButton       *fRafraich;
  TGCheckButton       *fAffichMonitoring;
  TGTextEntry *fTL;
  TGHProgressBar *fHProg1;

  //Donnees Membres 
  TString   Filename;
  TGRadioButton *fR[10];
  TGCheckButton *fCheck[30];
	
 public:
  //Constructor
  MMonMainFrame(const TGWindow *p=0, UInt_t w=1, UInt_t h=1);
  //Destructor
  virtual ~MMonMainFrame();

  //GUI methods
  void HandleButton(Int_t id=-1);
  void HandleMenu(Int_t id);
  void CloseWindow();
	
  // 	//Other methods
  //         void MessageBox(const char* title, const char* msg, int button);
			
  ClassDef(MMonMainFrame,1)
};

#endif

    
