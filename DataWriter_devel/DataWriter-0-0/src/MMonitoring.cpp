//--------------------------------------------------------
//                        MMonitoring.cc
// This program could be used for online monitoring of
// LPTPC MICROMEGAS modules during tests phase.
//
// Author : D. Attié
// Date : 30.06.2010 
//
// Modified from J. Beucher
// Date : 20.08.2008
//--------------------------------------------------------

//Standard librairies
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

//Librairies for reading acquisition files
#include <arpa/inet.h>
#include "interface/mygblink.h"

//ROOT librairies
//#include <TROOT.h>
//#include <TRint.h>
//#include <TApplication.h>
#include <TFile.h>
//#include <TStyle.h>
//#include <TCanvas.h>
//#include <TRootCanvas.h>
#include <TH1.h>
#include <TTree.h>
//#include <TH2.h>
//#include <TH3.h>
//#include <TF1.h>
//#include <TProfile.h>
//#include <TGWindow.h>
//#include <TGButton.h>
//#include <TGFrame.h>
//#include <TGFileDialog.h>
//#include <TSystem.h>
//#include <TGMsgBox.h>
//#include <TBrowser.h>
//#include "TMath.h"
//#include "TPolyLine.h"
//#include "RQ_OBJECT.h"
//#include "TRootCanvas.h"

//Other
//#include "MMonMainFrame.h"
//#include "MMonSelectCanvasFrame.h"
//#include "Globals.h"
//#include "KeyStonePad.h"

//using namespace std;

static int first=0;
static unsigned int evt_number;
static unsigned int data_struct[1800][100];
static TFile *f=new TFile("output.root","RECREATE");
static TTree *nt=new TTree("nt","Data Ntuple");
static TTree *nt_layout =new TTree("nt_layout","Detector Geometry Layout");

const int kPhysChannelMax  = 1728;
const int kASIC            = 24;

const int Ndet   =  24;				// number of detectors
const int Nasic  =   2;				// number of ASIC per detector
const int Nstrip =  72;				// number of strips per detector
const int Ntime  = 530;				// number of time samples

///--------------------------------------------------------
/// Structure acquisition data
///--------------------------------------------------------
struct Header
{
  int eventSize;
  int eventNumb;
};

struct EventHeader 
{
  int eventSize;
  int eventNumb;
  //int eventTime;  
};

struct DataPacketHeader 
{
  unsigned short size;
  unsigned short dcc_hdr;			// To change if the data after May 2010
  unsigned short hdr;
  unsigned short args;
  unsigned short ts_h;
  unsigned short ts_l;
  unsigned short ecnt;
  unsigned short scnt;
};

struct DataPacketData
{
  unsigned short data[Ntime];
};

struct NewDataHeader
{
  unsigned short args;
  unsigned short wordcount;
};

struct DataPacketEnd 
{
  unsigned short crc1;
  unsigned short crc2;
};

///--------------------------------------------------------
/// Declaration zone
///--------------------------------------------------------

int  COMPRESS=1; 				// 0: compress false; 1: compress true
int  evNumber; 				// current event number 
double Time_inloop;				// time in the current loop time
int Nloop; 					// number of loops
double Time;					// time of current event
int compteur;
int sampleCountMax;
bool event_flag=false;
bool select_flag=false;

int buffer;
int NbEntries;

bool fStop;
bool fEvenTargetReach;
bool fAffichageMonitoring;
bool fDefaultFile;
bool fFinAnalyse;
bool fRecovering;
bool fWriteBackup;

long int curPos=0;

FILE* _fData;
FILE* _fLayout;
char str[80];

int dataADC[Ntime];

int asic_fec, row, column, pad;
int iROW[kPhysChannelMax];
int iCOLUMN[kPhysChannelMax];
int iPAD[kPhysChannelMax];
int iCHAN[kPhysChannelMax];			// Map for pad ID ( col+row*nCOL ) to chan ID

int MaxCharge[kPhysChannelMax];
int padtotcharge[kPhysChannelMax];
int nHit[kPhysChannelMax];
int timeHit[kPhysChannelMax];

int dataContent[kPhysChannelMax][511];
float evtpadcharge[kPhysChannelMax][100000]; 

/// Define a hit list:  pad ch nslice time max 
int nhit; 
int hitInfo[5][1000];
int _hitTimeMult=0;
int _hitMaximum=-10;
int _hitBinMaximum=0;
int _hitSum=0;
int _nhitPerChan=0;

int board = 0;
int channel = 0;
int page = 0;

int Version; 				// version of the daq
int nEvent = 1000000;
int Threshold = 40;			// 40
int RefreshEvent = 100;

int HitsOnPads = 0;			// 5

/// Define data structures 
// EventHeader      head;
// DataPacketHeader pHeader;
// DataPacketData   pData;
// DataPacketEnd    pEnd;



Header head;
DataPacketHeader pHeader;  
DataPacketData pData;
DataPacketEnd pEnd; 

//void Init();
void DoMonitoring();
void DrawXvsTime();  //wwx

//-----------------------------------------------------------------------------
// Some other globals : YHS
//
bool fNextEvent           = false; // if set, display next event
bool fSkipToNextThreshold = false; // if set, skip until next evt over thrshld

bool compressMod_flag= (bool) COMPRESS;

const int kPhysChanPerAsic = 72;
const int kMaxCols         = 72;

class KeyStonePad;
const int kNPoints = 4;

double tpcPanelWidth  = 0.0;
double tpcPanelHeight = 0.0;

double x[kPhysChannelMax][kNPoints];
double y[kPhysChannelMax][kNPoints];

int      ReadGeomMap();
//void     FillPadPanelWithColor();

unsigned int cPadPanelWidth  = 0;
unsigned int cPadPanelHeight = 0;
unsigned int xPadding = 0;  // decoration for left and right side of the window
unsigned int yPadding = 0;  // decoration for top and bottom of the window (menu,..)


//________________________________________________________
///Mapping
//________________________________________________________
int OpenLayoutFile(char * layoutfilename) 
{
  char * c_tmp;
  
  // Now opening layout file
  _fLayout = fopen(layoutfilename, "r");

  if( ! _fLayout ) {
    printf(" Can't open file : MapLayoutFile.txt\n");
    return(-1);
  }
   
  // Read layout headerint;
  //skip first lignes
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);
  c_tmp=fgets( str,80 , _fLayout);//printf( "%s", str);

  return 1;
}

//________________________________________________________
int CloseLayoutFile()
{
  fclose(_fLayout);
  return 1; 
}

//________________________________________________________
void ReadMapping_T2K() 
{
  //now get mapping
  int iASIC=0;
  int iFEC=0;
  int pad=0;
  
  for (int i=0 ; i<kPhysChannelMax; i++){
    buffer=fscanf(_fLayout, "%d %d %d %d", &asic_fec, &row, &column, &pad);
    if(column%6==0 && column!=0 && row==0) {
      iFEC++;
    }
    iASIC = asic_fec+4*iFEC;
    iPAD[i] = pad+72*iASIC;
    iROW[iPAD[i]] = row;
    iCOLUMN[iPAD[i]] = column;

     printf("PhysChan: %d",i);
     printf("\tFEC: %d",iFEC); 
     printf("\tASIC: %d", iASIC); 
     printf( "\tPad: %d", iPAD[i]);
     printf( "\trow,col: %d,%d\n" ,iROW[iPAD[i]], iCOLUMN[iPAD[i]]);

  }
}

//--------------------------------------------------------------------
// 
void ReadMapping() 
{
  // now get mapping
  int iASIC=0;
  int iFEC=0;
  int pad=0;

  for (int i=0 ; i<kPhysChannelMax; i++) 
  {
    buffer=fscanf(_fLayout, "%d %d %d %d", &asic_fec, &row, &column, &pad);

    if (column>=60)              iFEC=0;
    if (column>=48 && column<60) iFEC=1;
    if (column>=36 && column<48) iFEC=2;
    if (column>=24 && column<36) iFEC=3;
    if (column>=12 && column<24) iFEC=4;
    if (column<12)               iFEC=5;

    // Comment by YHS
    //
    // The Map : (Row, Col) --> PhysChan
    //
    // NOTE: PhysChan is the most important one,
    //       which is the one that we can get from the ACQ data file, and
    //       which should be used to find (row, col) of the pad from it !
    //
    //
    //
    // NOTE : 2 pads denoted by -1 are being used for HV.
    //        This results in two missing channel numbers : 72 and 73 !!!
    //
    iASIC = asic_fec + 4*iFEC;
      
    if  ( pad != -1 ) {
      iPAD[i] = pad + kPhysChanPerAsic*iASIC;
    }
    else {
      //stringstream iss;
      //iss << "  Pad at row,col: " << setw(2) << row << "," << setw(2) << column
      //    << " is for HV :  ";

      // NOTE: Modify this when mapping is being changed !
      //
      if ( column == 65 ) iPAD[i] = 72;
      if ( column == 64 ) iPAD[i] = 73;

      //iss << "Channel ID is assinged manually to " << iPAD[i];
      //missingChannelInfo.push_back( iss.str() );
    }

    iROW[iPAD[i]]    = row;
    iCOLUMN[iPAD[i]] = column;

    // Copy of iPAD with different name
    int padid = column + row * kMaxCols;
    iCHAN[padid] = iPAD[i];  // this is the map that we need 

    //cout << "Line: "    << setw(4) << i       << "  "
    //     << "FEC: "     << setw(2) << iFEC    << "  "
    //     << "ASIC: "    << setw(2) << iASIC   << "  " 
    //     << "Chan: "    << setw(4) << iPAD[i] << "  "
    //     << "row,col: " << setw(2) << iROW[iPAD[i]] 
    //     << ","         << setw(2) << iCOLUMN[iPAD[i]] 
    //     << "     "     <<  asic_fec << "," << row << "," << column 
    //     << endl;
  }

  printf ("\n");
  printf ( "NOTE: \n" );
  //for ( string::size_type i=0; i<missingChannelInfo.size(); i++) {
  //  printf ("%d\n" , missingChannelInfo[i] );
  //}
  printf("  --> Modify this when mapping is changed <---\n") ;

}

//--------------------------------------------------------
//
int ReadGeomMap()
{
  char filename[] = "etc/geom/keystonePadGeom.txt";
  //char filename[] = "rectCorners.txt";

  ifstream padGeomFile( filename );

  
  if ( !padGeomFile ) {
    printf("Can't open file :%s\n",filename);
    //cout << "Can't open file : " << filename << endl;
    return -1;
  }

  char   varName[20];
  double width;
  double ymin, ymax;

// Variables needed for the Root ntuple
  
  int tab_id[kPhysChannelMax], tab_row[kPhysChannelMax], tab_col[kPhysChannelMax];
  int nPhysChannelMax;
  float TPCPanelWidth, TPCPanelHeight, TPC_ymin, TPC_ymax;
  float pad_x0[kPhysChannelMax], pad_x1[kPhysChannelMax], pad_x2[kPhysChannelMax], pad_x3[kPhysChannelMax];
  float pad_y0[kPhysChannelMax], pad_y1[kPhysChannelMax], pad_y2[kPhysChannelMax], pad_y3[kPhysChannelMax];
  padGeomFile >> varName >> width;  // half width of the pad panel
  printf("varName : %s, width=%f\n",varName,width);
  padGeomFile >> varName >> ymin;
  padGeomFile >> varName >> ymax;

  nt_layout->Branch("TPCPanelWidth",&TPCPanelWidth,"TPCPanelWidth/F");
  nt_layout->Branch("TPCPanelHeight",&TPCPanelHeight,"TPCPanelHeight/F");
  nt_layout->Branch("TPC_ymin",&TPC_ymin,"TPC_ymin/F");
  nt_layout->Branch("TPC_ymax",&TPC_ymax,"TPC_ymax/F");
  nt_layout->Branch("nPhysChannels",&nPhysChannelMax,"nPhysChannels/I");
  nt_layout->Branch("id",tab_id,"id[1728]/I");
  nt_layout->Branch("row",tab_row,"row[1728]/I");
  nt_layout->Branch("col",tab_col,"col[1728]/I");
  nt_layout->Branch("pad_x0",pad_x0,"pad_x0[1728]/F");
  nt_layout->Branch("pad_x1",pad_x1,"pad_x1[1728]/F");
  nt_layout->Branch("pad_x2",pad_x2,"pad_x2[1728]/F");
  nt_layout->Branch("pad_x3",pad_x3,"pad_x3[1728]/F");
  nt_layout->Branch("pad_y0",pad_y0,"pad_y0[1728]/F");
  nt_layout->Branch("pad_y1",pad_y1,"pad_y1[1728]/F");
  nt_layout->Branch("pad_y2",pad_y2,"pad_y2[1728]/F");
  nt_layout->Branch("pad_y3",pad_y3,"pad_y3[1728]/F");

  tpcPanelWidth  = 2. * width;         // total width  of the pad array
  tpcPanelHeight = ymax - ymin;        // total height of the pad array

  TPCPanelWidth=(float)tpcPanelWidth;
  TPCPanelHeight=(float)tpcPanelHeight;
  TPC_ymin=(float)ymin;
  TPC_ymax=(float)ymax;

  //cout << "Pad Panel: w,ymin,ymax = " 
  //     << width << ", " << ymin << ", " << ymax << endl;

  for (int i=0; i<kPhysChannelMax; i++) {
    int id, row, col;
    double x0, y0, x1, y1, x2, y2, x3, y3;
    // padGeomFile >> id >> row >> col ;
    padGeomFile >> row >> col >> x0 >> y0 >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
    id = i;
    x[id][0] = x0;    y[id][0] = y0;
    x[id][1] = x1;    y[id][1] = y1;
    x[id][2] = x2;    y[id][2] = y2;
    x[id][3] = x3;    y[id][3] = y3;

    tab_col[i]=col; tab_row[i]=row;
    tab_id[i]=i;
    pad_x0[i]=x0; pad_y0[i]=y0;
    pad_x1[i]=x1; pad_y1[i]=y1;
    pad_x2[i]=x2; pad_y2[i]=y2;
    pad_x3[i]=x3; pad_y3[i]=y3;
  }

  // OK, fill the layout ntuple
  nPhysChannelMax=kPhysChannelMax;
  nt_layout->Fill();

  padGeomFile.close();

  return 0;
}



//________________________________________________________
///Data acquisiton reader
//________________________________________________________
int OpenDataFile(char * fileName) 
{
  printf("OomonAcqReader OpenFile opening file %s \n",fileName); 
  _fData = fopen(fileName,"rb");
  // check if this input datafile exist
  if(_fData==NULL){
    //cout<<"Can't open data file : " << fileName <<endl;
    return(-1);// End of program
  }
  
  // Read Run UID characters
  char runUid[21];
  buffer=fread(runUid, 1, 20, _fData);
  runUid[20] = '\0'; // Null terminated C string
  printf("File UID is %s \n",runUid); 
  return 1; 
}

//________________________________________________________
int CloseDataFile() {
  fclose(_fData);
  return 1; 
}


//________________________________________________________
int DecodeNextEvent(void) 
{

  if (first == 0) 
    {
    printf ("First event\n");
    first=1;
    }

  bool DEBUG = false;
  
  int countPads =0;
  
  Time=0; Time_inloop=0; Nloop=0;

  if(fread(&head, sizeof(Header), 1, _fData) != 1) {fclose(_fData); return -1; /*return 0; End of program*/}
  
  head.eventSize = ntohl(head.eventSize); // size of current event
  head.eventNumb = ntohl(head.eventNumb); // current event number
  
  evt_number=head.eventNumb;

  /* Initialisation de la table de données */
  {
  int i,j;
  for (i=0; i<1800; i++) for (j=0; j<100; j++) data_struct[i][j]=0;
  }

//   int evN = head.eventNumb;
  if (DEBUG) printf ("Event number: %d\n",  head.eventNumb );
  int rawDataSize = head.eventSize - sizeof(head) - sizeof(pEnd);
  int nByteTot = rawDataSize ; 
  int nByteRead=0;
  sampleCountMax=0;

  while (nByteRead < nByteTot ) { // nByteTot represents the size of the full event, not only the current packet
    // now read data packet header 
    buffer = fread(&pHeader, sizeof(DataPacketHeader), 1, _fData);
    nByteRead+=sizeof(DataPacketHeader); 
    int  nlinespacket = (ntohs(pHeader.size)-2-12-2-4)/sizeof(short); // number of lines to be read in the current packet
//     cout << "Payload size (-20/2): " << nlinespacket << endl;
    // check sync fail
    if (GET_LOS_FLAG((ntohs(pHeader.hdr) ))) printf("LOS FLAG SET\n");
    if (GET_SYNCH_FAIL((ntohs(pHeader.hdr) ))) printf("SYNC FAIL FLAG SET\n");
    int tempAsic1 = GET_RB_ARG1(ntohs(pHeader.args)); 
    int tempAsic2 = GET_RB_ARG2(ntohs(pHeader.args)); 
    Version = GET_VERSION(ntohs(pHeader.dcc_hdr)); // old is 0, new is 1
    int Flag = GET_FLAG(ntohs(pHeader.dcc_hdr)); 
//     int RT = GET_RT(ntohs(pHeader.dcc_hdr)); 
//     int FrameType = GET_FRAME_TY_V2(ntohs(pHeader.dcc_hdr));
//     cout << "FrameType: " <<  FrameType << endl;
//     cout << "RT: "   <<  RT << endl;
    if (DEBUG) printf("Version: %d\n" , Version);
    if (DEBUG) printf("Flag: %d\n",Flag);
    // compute asic# 
    int channelN = tempAsic1/6; 
    int asicN = 10*(tempAsic1%6)/2 + tempAsic2;
    // cout << "asicN, channelN: " << asicN << ", " << channelN << endl;
    //  get sample count
    int sampleCount = ntohs(pHeader.scnt);
    if (DEBUG) printf( "Word count: %d\n" ,sampleCount );
    // now read the data (should use the sample count !!!!
    // take into account padding short int to get even total
    
    int physChannel=-10; 

    int nlinesread=0;
    while(nlinesread<nlinespacket) {
      if(nlinesread!=0 && Flag!=2) { // some data have already been read in the packet
//      if(nlinesread!=0 && Flag!=2 && RT==0) { // some data have already been read in the packet
//      if((FrameType & 3)==0) { // some data have already been read in the packet
//      if(nlinesread!=0 && (FrameType & 3)==0) { // some data have already been read in the packet
          NewDataHeader newData;
          buffer = fread(&newData, sizeof(unsigned short int), 2, _fData); 
    	  nlinesread+=2;
	  nByteRead+=2*sizeof(unsigned short int); 
	  if(GET_TYPE1(ntohs(newData.args))==0) {printf ("something wrong happened... exiting\n") ; exit(0);}
	  tempAsic1 = GET_RB_ARG1(ntohs(newData.args)); 
	  tempAsic2 = GET_RB_ARG2(ntohs(newData.args)); 
	  // compute asic# 
	  channelN = tempAsic1/6;
	  asicN = 10*(tempAsic1%6)/2 + tempAsic2;
	  sampleCount = GET_NEWWORDCOUNT(ntohs(newData.wordcount));
	  // cout << "new word count: " << sampleCount << endl;
      }
      
      bool chstatus = false;
     
      // compute physical channel (0-71)
      if (channelN> 2 && channelN < 15 ) {
        physChannel= channelN -3; chstatus = true;
      } else if (channelN> 15 && channelN < 28 ) {
        physChannel= channelN -4; chstatus = true;
      } else if (channelN> 28 && channelN < 53 ) {
        physChannel= channelN -5; chstatus = true;
      } else if (channelN> 53 && channelN < 66 ) {
        physChannel= channelN -6; chstatus = true;
      } else if (channelN> 66  ) {
        physChannel= channelN -7; chstatus = true;
      }
      
      int maxcharge = -50;
      int tempCharge = 0;
     
      int thisPhysChannel = asicN*72+physChannel;
 
      int sampleCountRead = sampleCount;
      if (sampleCount%2==1) sampleCountRead++;
 
      if(Flag!=2 && sampleCount!=0) {
        buffer = fread(&pData, sizeof(unsigned short int), sampleCountRead, _fData);
        nByteRead+=sampleCountRead*sizeof(unsigned short int);
        nlinesread+=sampleCountRead;
      } else {
        buffer = fread(&pData, sizeof(unsigned short int), nlinespacket, _fData);
        nByteRead+=nlinespacket*sizeof(unsigned short int);
        nlinesread+=nlinespacket;
      }
      
      if (physChannel>=0 && physChannel<72 && chstatus) {

        if(Flag!=2) {
	  int thisCell = -999;
	  timeHit[channelN] = 0;
	  
	  for (int itime=0; itime<Ntime; itime++) {dataADC[itime]=0;}
	  
	  if(compressMod_flag) {
	    if (sampleCount<=Ntime) {
	      for (int itime=0; itime<sampleCount-2; itime++) {
		unsigned short int datum = ntohs(pData.data[itime]);
		if (datum & 0x1000) {//4096
		  thisCell = (datum & 0xFFF);
		  if (thisCell>0 && thisCell<511) {
		    timeHit[thisPhysChannel]= (datum&0xFFF);
		    dataContent[thisPhysChannel][itime] = timeHit[thisPhysChannel];	// 12 LS
		    if (datum > 0) {
		      tempCharge+=datum;
		    }
		    if (datum > maxcharge) {
		      maxcharge = datum;
		    }
		  }
	        } else {
		  if (datum != 0) {  // Dans le cas du mode COMPRESS
//>>>		    dataADC[itime] = datum - 250;
//>>>		    int datumC = datum - 250; 
		    dataADC[itime] = datum;
		    int datumC = datum; 
		    if (datumC > 0) {
		      tempCharge+=datumC;
		    }
		    if (datumC > maxcharge) {
		      maxcharge = datumC;
		    }
		    dataContent[thisPhysChannel][itime] = dataADC[itime];
//  		      if (itime != 11) hADCvsTIME[thisPhysChannel]->Fill(itime, dataADC[itime]);
 		    //hADCvsTIME[thisPhysChannel]->Fill(itime, dataADC[itime]);
		  }
	        }
	      } //end of boucle over data
	    }
	  } else {
	    for (int itime=0; itime<sampleCount; itime++) {
	      dataADC[itime]= ntohs(pData.data[itime]);
	      //hADCvsTIME[thisPhysChannel]->Fill(itime, dataADC[itime]);
	      if (dataADC[itime] > 0) {
	        tempCharge+=dataADC[itime];
	      }
	      if (dataADC[itime] > maxcharge) {
	        maxcharge = dataADC[itime];
	      }
	    }
	  }
        }  //end of boucle over data	
   
        _nhitPerChan=0;
        _hitTimeMult=0;

        countPads++;
        {
        int i;
        // C'est là que ca se passe... ARRETE
        //if (countPads == 1) 
        //for (i=0; i<100; i++) printf("evt=%d pad=%d isamp=%d samp=%d\n",head.eventNumb, countPads,i,dataADC[i]);
        //if (countPads == 100) 
        //for (i=0; i<100; i++) printf("evt=%d npad=%d ipad=%d isamp=%d samp=%d\n",head.eventNumb, countPads,physChannel,i,dataADC[i]);
        //printf("Event : %d, pad=%d iPad=%d row=%d col=%d x=%f y=%f\n",head.eventNumb, thisPhysChannel, 
        //          iPAD[thisPhysChannel],iROW[iPAD[thisPhysChannel]], iCOLUMN[iPAD[thisPhysChannel]], x[iPAD[thisPhysChannel]][0],y[iPAD[thisPhysChannel]][0]);
        if (head.eventNumb == 0)
          {
          int i;
          //for (i=0;i<kPhysChannelMax; i++)
          //  printf("Physchan = %d Pad=%d row=%d col=%d\n",i,iPAD[i],iROW[iPAD[i]], iCOLUMN[iPAD[i]]);
          }
        {
        int i, ipad, amax;
        amax=-100;
        for (i=0; i<100; i++) 
            {
            data_struct[thisPhysChannel][i]=dataADC[i];
            if (dataADC[i]>amax) amax=dataADC[i];
            //printf("evt=%d pad=%d isamp=%d samp=%d\n",head.eventNumb,countPads,i,dataADC[i]);
            }
        for (i=0; i< kPhysChannelMax; i++) if (iPAD[i]==thisPhysChannel) ipad=i;
        printf("%d %d %d %d %d %f %f %d\n",head.eventNumb, thisPhysChannel, 
                  iPAD[thisPhysChannel],iROW[iPAD[thisPhysChannel]], iCOLUMN[iPAD[thisPhysChannel]], x[iPAD[thisPhysChannel]][0],y[iPAD[thisPhysChannel]][0],amax);
        }
        }

	
        for (int ip=0; ip<sampleCount; ip++) {
	  if (dataContent[thisPhysChannel][ip] > Threshold) {
            //start oh Hit
            _hitTimeMult++;
            //find maximum
            if (dataContent[thisPhysChannel][ip]>_hitMaximum) {
              _hitMaximum = dataContent[thisPhysChannel][ip];
              _hitBinMaximum = ip;
            }
            //compute sum of charge for all sample count for each channel
            _hitSum+= dataContent[thisPhysChannel][ip];
            //end of Hit
            if (dataContent[thisPhysChannel][ip+1] <= Threshold) {
              //record hit info
              //HitTimeMultiplicity.push_back(_hitTimeMult);
              //HitMaxCharge.push_back(_hitMaximum);
              //HitBinMaxCharge.push_back(_hitBinMaximum);
              //HitSum.push_back(_hitSum);
              //HitChannelId.push_back(thisPhysChannel);
              //reset for new hit
              _hitTimeMult=0;
              _hitMaximum=-10;
              _hitSum=0;
              _hitBinMaximum=0;
              _nhitPerChan++;
// 	      cout << "_nhitPerChan: " << _nhitPerChan << endl;
// 	      cout << "thisPhysChannel: " << thisPhysChannel << endl;
	    }
	  }
        }
      
//       cout << "tempCharge, maxcharge, MaxCharge[thisPhysChannel]: " << tempCharge << ", " << maxcharge << ", " << MaxCharge[thisPhysChannel] << endl;
      
        if (thisPhysChannel != -10 && maxcharge>Threshold) {
	  if (tempCharge > MaxCharge[thisPhysChannel]) {
 	    MaxCharge[thisPhysChannel]= maxcharge;
	  }
	  //compute sum of ADC charge per event for each channel above threshold set at 100 ADC unit
//  	    padtotcharge[thisPhysChannel]+= tempCharge;
// 	    evtpadcharge[thisPhysChannel][evN] += tempCharge;
        }
      }
    }

    buffer = fread(&pEnd,sizeof(DataPacketEnd), 1, _fData);
    nByteRead+=sizeof(DataPacketEnd);
  
  }//end main channel


  if(Time_inloop<=1e-8*((ntohs(pHeader.ts_h))*pow(2,16)+ ntohs(pHeader.ts_l))) Time_inloop= 1e-8*((ntohs(pHeader.ts_h))*pow(2,16)+ ntohs(pHeader.ts_l));
  else {
    Nloop++;
    Time_inloop= 1e-8*((ntohs(pHeader.ts_h))*pow(2,16)+ ntohs(pHeader.ts_l));
  }
  Time = 1e-8*pow(2,32)*Nloop+Time_inloop;
  evNumber++; 
  select_flag=false;
  event_flag=false;
  sampleCountMax=Ntime;
  
  float maxbin = 0; 
  for ( int hid=0; hid<kPhysChannelMax; hid++ ) {
    //if (hADCvsTIME[hid]->GetMaximumBin() >  maxbin) maxbin = hADCvsTIME[hid]->GetMaximumBin();
  }
  //if ( 1 < maxbin) hMaxHitTimeBin->Fill(maxbin);

//   cout << "maxbin: " << maxbin << endl;
//   cout << "countPads: " << countPads << endl;

  //printf("Number of pads : %d for evt %d\n",countPads,evNumber);
  nt->Fill();
  return countPads;
}


///Event loop (DoMonitoring function)
//________________________________________________________
void DoMonitoring() 

{
  int iEvent = 0;
  static int iEventLoop = 0;
  // int ThlEvents=5;

  //Loop to check monitoring stop decided by user
  // while (!fStop && !fEvenTargetReach) {
  // gSystem->ProcessEvents();

  //Main event loop
  while (!fStop && !fEvenTargetReach) {
    //reset event
    for (int ich=0; ich<kPhysChannelMax; ich++) {
      timeHit[ich]=0;
      padtotcharge[ich]=0;
      MaxCharge[ich]=0;
      nHit[ich]=0;
      //hADCvsTIME[ich]->Reset();
    }
    nhit = 0;
    //reset hit vector
    //decode next event
    //	//get event time
    //	int eventTimeH = ntohs(pHeader.ts_h);
    //	int eventTimeL = ntohs(pHeader.ts_l);
    //	cout << "eventTimeH =\t" << eventTimeH << "\t eventTimeL=\t" << eventTimeL << endl;

    HitsOnPads = DecodeNextEvent();
    if ( HitsOnPads == -1 ) {
      printf("EOF reached\n");
      nt->Write();
      nt_layout->Write();
      printf("Ntuple filled at EOF\n");
      f->Close();
      printf("File closed at EOF\n");
      break;
    }

    //FillHitHisto();

    //get event number
    iEvent = head.eventNumb;
    if (iEvent!=iEventLoop) {
      //cout << "Event maybe lost, check this !" << endl;
      //cout << " iEvent, iEventLoop = " << iEvent << ", " << iEventLoop << endl;
      //break;
    }
    //Update histo
    if (iEvent%RefreshEvent==0 && iEvent!=0){
      //printf("Number of analyzed events so far %d\n",iEvent);
      //cout << "Number of analyzed events so far " << iEventLoop << endl;
      //UpdateCanvases(true);
      //gSystem->ProcessEvents();
    }
    if (iEvent==nEvent) {
      printf("Number of event target to be monitored is reached\n");
      fEvenTargetReach = true;
    }

//    if (HitsOnPads>ThlEvents) {
      
      //FillRoHisto();
      //cPermanent[0]->Modified();
      //cPermanent[0]->Update();

      // PadPanel
//       FillPadPanelWithColor();
//       cPadPanel->Modified();
//       cPadPanel->Update();
      if (iEvent==nEvent && nEvent!=1000000) {
        //FillPadPanelWithColor();
        //cPadPanel->Modified();
        //cPadPanel->Update();
        
	char gifName[20];
        sprintf(gifName , "./GIF/%4.5d.gif", head.eventNumb);
        //cPadPanel->Print(gifName);
      } else {
        //FillPadPanelWithColor();
        //cPadPanel->Modified();
        //cPadPanel->Update();

	char gifName[20];
        sprintf(gifName , "./GIF/%4.5d.gif", head.eventNumb);
        //cPadPanel->Print(gifName);
      }


      //gSystem->ProcessEvents();

      if ( fSkipToNextThreshold )
        fEvenTargetReach = true;  // make it to stop
//    }

    iEventLoop ++;

  }//End of event loop

  // 	iLoop++;
  // 	}//End of while (fStop)

  //UpdateCanvases(true);
  if ( fStop )
    printf("Monitoring is finished\n");
}

//________________________________________________________
///Main
//________________________________________________________
int main(int argc, char **argv) {

  int loop;

  char Filename_acq[255];
  strcpy(Filename_acq,"./RUN.acq");
  char FilenameLayout_txt[255];

// Variables to be used for the root-tuple


//  TTree *nt=new TTree("nt","Data Ntuple");
//  TFile *f=new TFile("output.root", "RECREATE");

  nt->Branch("evt",&evt_number,"evt/I");
  nt->Branch("data",data_struct,"data_struct[1800][100]/I");
  //evt_number=1;
  //nt->Fill();
  //evt_number=2;
  //nt->Fill();
  //nt->Write();
  //f->Close();

  strcpy(FilenameLayout_txt,"etc/geom/MapLayoutFile.txt");

  for (loop = 1; loop < argc; loop++) {
    if (argv[loop][0] == '-') {
      switch (argv[loop][1]) {
      case 'n':
        nEvent = strtol(argv[loop+1], NULL, 0);
        loop++;
        break;
      case 'r':
        RefreshEvent = strtol(argv[loop+1], NULL, 0);
        loop++;
        break;
      case 't':
        Threshold = strtol(argv[loop+1], NULL, 0);
        loop++;
        break;
      case 'f':
        strcpy(Filename_acq, argv[loop+1]);
        loop++;
        break;
      case 'h':
        printf("Usage: %s [OPTIONS]\n\n", argv[0]);
        printf("Options:\n");
        printf("\n-n [number of events] (default = %d)", nEvent);
        printf("\n-r [number of events before histogram updating] (default = %d)", RefreshEvent);
        printf("\n-t [threshold in ADC] (default = %d)", Threshold);
        printf("\n-f <input file without .acq extension>         (default = %s)", Filename_acq);
        printf("\n");
        return 0;
        break;
      }
    }
  }

  //TH1F *hh=new TH1F("TRUC","TRUC",10,0.,10.);
  
  //Prepare mapping
  printf("Opening layout file ...\n");
  OpenLayoutFile(FilenameLayout_txt);
  if ( ! _fLayout ) 
    return 1;

  printf("Prepare mapping ...\n");
  ReadMapping();
  printf("--------------------------------\n");

  printf("Reading geomtry info ...\n");
  int status = ReadGeomMap();
  if ( status != 0 ) 
    return 1;
  printf("--------------------------------\n");
  
  //Open data file
  OpenDataFile(Filename_acq);
  if ( ! _fData )
    return 1;
  printf("--------------------------------\n");

  /* Do Monitoring */
  DoMonitoring();

  printf("On ressort de DoMonitoring\n");

  //Close files
  CloseLayoutFile();
  printf("Layout File closed\n");
  //CloseDataFile(); ! Not needed, closing already done in DoMonitoring
  printf("Data File closed\n");

  //nt->Write();

  printf("Ntuple written once more\n");

  //f->Close();

  printf("Output file closed\n");

  return (0);
}
