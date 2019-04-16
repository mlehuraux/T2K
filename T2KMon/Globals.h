bool gPauseFlag = kFALSE;
bool gSuivantFlag = kFALSE;
bool gVersion0_1 = kTRUE;
bool gAnalyseRapide = kFALSE;
bool gStop = kFALSE;
bool gDisplayCluster = kFALSE;
bool gRafraichir = kTRUE;
bool gFit = kFALSE;
bool gIsAnalysing = kFALSE;
bool gRootFile = kTRUE;
bool gBackUp = kTRUE;

float gProgress = 0;
float gLength = 1;
int gSpeed = 500;
int gIntervalBackUp = 10; // en seconde

TGFileInfo info;
TTimer *timer;

TString gStartDir ="";
