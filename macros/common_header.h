#ifndef _COMMON_HEADER
#define _COMMON_HEADER
#ifndef __CINT__
#include "LinkDef.h"
 
#include "TROOT.h"
#include "TRint.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF2.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TRandom.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TEventList.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TLeaf.h"

#include "TApplication.h"
#include "TSystem.h"


#include "TPaveStats.h"
#include "TProfile2D.h"
#include "TLorentzVector.h"

#include "TObjString.h"

#include "TMath.h"
#include "TLatex.h"
#include<iostream>
using namespace std;  // new header isostream instead od iostream.h but you have to add namespace...

#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include <iomanip>


#include "ini.C"

// compiler variables
// THIS_NAME = name of the void mainfunction() of the macro
// INTERACTIVE_OUTPUT to go to root prompt at the end of the execution
// OVERRIDE_OPTONS to manage yourself command line arguments  (eg -b for batch processing is therefore disabled)

extern void InitGui();
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
TROOT root("Higgs"," Example ", initfuncs);
#endif


//compatibility with old
#ifndef THIS_NAME
#define THIS_NAME ExecuteThisMacro
#endif

//try to avoid main in Aclic compiled code 
#ifndef __MAKECINT__

#ifndef __CINT__
void THIS_NAME();
int main(int argc, char* argv[]){ 
#ifdef INTERACTIVE_OUTPUT

#ifdef OVERRIDE_OPTIONS
TRint* theApp=new TRint("App",&argc,argv,0,-1);
#else
TRint* theApp=new TRint("App",&argc,argv);
#endif

#else
#ifdef OVERRIDE_OPTIONS
 TApplication* theApp=new TApplication("App",&argc,argv,0,-1);
#else
 TApplication* theApp=new TApplication("App",&argc,argv);
#endif
#endif



  //     
    THIS_NAME();
   theApp->Run();  
   exit(0);
   return 0;
}

#endif
 //CINT

#endif 
//makecint

#endif
// Common header
