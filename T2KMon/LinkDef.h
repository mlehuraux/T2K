#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ main  MMonitoring;
#pragma link C++ class MMonSelectCanvasFrame+;
#pragma link C++ class MMonMainFrame+;
#pragma link C++ class T2KPadPanel+;

// Stand-alone Handlers defined in Handlers.h and implemented in main program
//#pragma link C++ function PrintSelectedPadInfo;
//#pragma link C++ function GetSelectedPadID;
#pragma link C++ function HandleResizeEvent;
//#pragma link C++ function ExecuteMaximizeEvent;
#endif
