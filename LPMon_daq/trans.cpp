//
//
// NOTE:
//  - Compress mode can be obtained by parsing contents of the CFG file,
//    which is stored in LCIO Run Header.
//    (eg) <compress>false</compress>           <--- compress = 0
//

// Include standard librairies
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
//#include <arpa/inet.h>
#include <byteswap.h>

#define ntohs(x)  __bswap_16 (x)
#define ntohl(x)  __bswap_32 (x)

// Include LCIO
#include <lcio.h>

#include <EVENT/LCIO.h>
#include <EVENT/LCCollection.h>
#include <EVENT/TrackerRawData.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/TrackerDataImpl.h>
#include <IMPL/LCRunHeaderImpl.h>
#include <IMPL/LCEventImpl.h>
#include <IMPL/TrackerRawDataImpl.h>
#include <UTIL/LCTOOLS.h>
#include <IO/LCWriter.h>

// Include my librairies
#include "mygblink.h"


struct EventHeader {
  int eventSize;
  int eventNumb;
};

struct DataPacketHeader {
  unsigned short size;
  unsigned short hdr;
  unsigned short args;
  unsigned short ts_h;
  unsigned short ts_l;
  unsigned short ecnt;
  unsigned short scnt;
};

struct DataPacketData {
  unsigned short data[512];
};

struct DataPacketEnd {  // not for end users
  unsigned short crc1;
  unsigned short crc2;
};

using namespace std;
using namespace lcio;

//----------------------------------------------------------
// Variables declaration zone
//----------------------------------------------------------

FILE* fileData;
FILE* fileLayout;

char acqFilename[80];
char lcioFilename[80];
char cfgFilename[80];

char RunNumber[80];

int asic, row, column, pad;

const int kChannelMax      = 1896;
const int kPhysChannelMax  = 1728;

double ped[kChannelMax];
double pedrms[kChannelMax];
double phyped[kPhysChannelMax];
double phypedrms[kPhysChannelMax];

int       sampleChan[kChannelMax];
const int kNumOfTimeBins = 512;

int eventContent[kChannelMax][kNumOfTimeBins];

const int kNumRows    = 24;
const int kNumColumns = 72;
int       PadMap[kNumRows][kNumColumns]; 

//int ic    = 0;
//int board = 0;
//int page  = 0;

int pedestalmode = 0;
int pedestal_run = 0;
int nEvent       = 100; //Set as default
int eventloop;

// Structures definition
EventHeader      head;
DataPacketHeader pHeader;
DataPacketData   pData;
DataPacketEnd    pEnd;


// Histogrammes definition
const int kNumOfChannelPerChip  = 79; // 72 for adc, 7 for other chip functions
const int kNumOfPhysChanPerChip = 72;

// YHS
int NumSampleCounts[kChannelMax];
int readback_mode     = 0; 
int readback_compress = 0;   // 0 = non comp,  1 = compressed

char RunUID[21];   // Run UID in the begining of the ACQ file : time stamp


//----------------------------------------------------------
// Open Acquisition File
//----------------------------------------------------------
int openFile(void) 
{
  char key[] = "0123456789";
  strcat(RunNumber, strpbrk (acqFilename, key));
  RunNumber[5] = '\0';

  cout << " Opening Data File " << endl;
  strcat(acqFilename,".acq");
  cout << acqFilename  << endl;
  fileData = fopen(acqFilename, "rb");
  if(fileData==NULL)
  {
    cout<<" error opening files, "<<acqFilename<<" does not match !!!"<<endl;
    return(-1);
  }

  strcpy (cfgFilename, acqFilename);

  int temp = strcspn (cfgFilename, "R") ; // Note If Path has a  R in it; death

  cfgFilename[temp] = '\0'; 

  strcat(cfgFilename,"CFG_");
  strcat(cfgFilename,RunNumber);
  strcat(cfgFilename,".xml");


  // Read Run UID characters
  //char runUid[21];
  fread(RunUID, 1, 20, fileData);
  RunUID[20] = '\0'; // Null terminated C string

  return 1; 
}

//----------------------------------------------------------
// Open Layout File Function
//----------------------------------------------------------
int openLayoutFile(void) 
{
  // Now opening layout file
  fileLayout = fopen("MapLayoutFile.txt", "r");
  if(fileLayout==NULL)
  {
    cout<<" error opening files, MapLayoutFile.txt does not match !!!"<<endl;
    return(-1);
  }
 
  // Read layout headerint;
  char str[80] ;

  //skip first lignes
  cout<< "********\n";
  fgets( str,80 , fileLayout);//printf( "%s", str);
  fgets( str,80 , fileLayout);//printf( "%s", str);
  fgets( str,80 , fileLayout);//printf( "%s", str);
  fgets( str,80 , fileLayout);//printf( "%s", str);
  fgets( str,80 , fileLayout);//printf( "%s", str);
  fgets( str,80 , fileLayout);//printf( "%s", str);
  fgets( str,80 , fileLayout);//printf( "%s", str);
  //fgets( str,80 , fileLayout);printf( "%s", str);
  cout<< "********\n";	
  return 1; 
}

//----------------------------------------------------------
// Acquisition Reading Function
//----------------------------------------------------------
int decodeNextEvent(void)
{
  // Read next header or quit of end of file
  if(fread(&head, sizeof(EventHeader), 1, fileData) != 1) 
  {
    fclose(fileData);
    return 0; // End of program
  }

  //-------------------
  // Read header
  //-------------------
  head.eventSize  = ntohl(head.eventSize);
  head.eventNumb  = ntohl(head.eventNumb);
  int rawDataSize = head.eventSize - sizeof(head);
  // 	cout << " reading event number " << head.eventNumb << endl;
  // 	cout << " rawdata size " << rawDataSize << endl ;

  //iFirstPacket=0;
  int nByteTot  = rawDataSize ;
  int nByteRead = 0;

  //Raw data loop
  while (nByteRead < nByteTot ) 
  {
    // Now read data packet header
    fread(&pHeader, sizeof(DataPacketHeader), 1, fileData);
    nByteRead += sizeof(DataPacketHeader);

    //-------------------------------------------------------
    // Total Packet Size in bytes including header and crc32
    //-------------------------------------------------------
    unsigned short packetSize = ntohs(pHeader.size);

    //---------------------------
    // Check Packet Header Flags
    //---------------------------
    // Excerpt from FEM design note: p50
    //
    //    When set, the LOS flag indicates that the ADC of one or several 
    //      FEC(s) lost synchronization during digitization. 
    //
    //    When set, the USYNCH flag indicates that the ADC of one or several
    //      FEC(s) could not gain synchronization for the digitization of that 
    //      event. 
    //
    //    The FEC flags (6-bit wide field) indicate the faulty FEC. 
    //    
    //    If any of the LOS or USYNCH flag is set, all ADC samples from the 
    //    faulty FEC(s) must be discarded. 
    //
    unsigned short hdr = ntohs(pHeader.hdr);

    if ( GET_LOS_FLAG(hdr))   { cout << " LOS FLAG SET "       << endl; }
    if ( GET_SYNCH_FAIL(hdr)) { cout << " SYNC FAIL FLAG SET " << endl; }
    if ( GET_FEC_ERROR(hdr))  { cout << " FEC_ERROR"           << endl; }

    //---------------------
    // Read Back ARGS word
    //---------------------
    // Excerpt from FEM design report: p51
    //
    //  In readout Mode 0 and without data compression, 
    //    the number of samples may vary from 4 to 512 depending on the number 
    //    of SCA cells being digitized and the cell offset being programmed. 
    //
    //  In readout Mode 1, the number of samples is determined by the 
    //    configuration of the look up table used to address samples (and the 
    //   eventual data compression). 
    //
    //  The maximum size of a data packet is limited to 2047 16-bit words, 
    //  including header and CRC32.
    //
    unsigned short args = ntohs(pHeader.args);
    
    readback_mode     = GET_RB_MODE(args);
    readback_compress = GET_RB_COMPRESS(args);

    //cout << "mode="     << readback_mode     << ", "
    //     << "compress=" << readback_compress << endl;


    // Compute asic number
    int tempAsic1 = GET_RB_ARG1(args);  // 0,6,12,...  2,8,14,... 4,10,16,...
    int tempAsic2 = GET_RB_ARG2(args);  // 0 ~ 9
    int asicChan  = tempAsic1/6;        // local channel id in each ASIC
    int asicN     = 5*(tempAsic1%6) + tempAsic2;
    //cout << "\n ic " << asicChan << " asic " << asicN << endl;


    //---------------------------
    // Get event type and counts
    //---------------------------
    //unsigned short ecnt = ntohs(pHeader.ecnt);
    //int evtType   = GET_EVENT_TYPE(ecnt);
    //int evtNumber = GET_EVENT_COUNT(ecnt);
    //cout << " evt count " << evtNumber << endl ;

    //------------------
    // Get Sample count
    //------------------
    int sampleCount = ntohs(pHeader.scnt);
    //cout << " sample count " << sampleCount << endl ;

    // There will be an additional byte if sample count is odd to align data 
    // to short int boundary. It thus number of bytes to read may be different
    // from the sample count.
    int sampleCountRead;
    int packetHeaderSize = sizeof(DataPacketHeader);
    int packetEndSize    = sizeof(DataPacketEnd);
    sampleCountRead = (packetSize - packetHeaderSize - packetEndSize) / 2;
    //sampleCountRead = sampleCount;
    //if (sampleCount%2==1) { 
    //  sampleCountRead++;
    //}
    
    //----------------------
    // Now read ADC samples 
    //----------------------
    //  bit12   : flag to distinguish ADC value and time bin. 
    //            if set, content is time bin number, NOT adc value
    //
    //  content : 9 LSB (i.e., 0x1FF)
    //
    //  data format :   timeBin (0x1xxx)
    //                + 10 padding samples before the bin
    //                + samples for above threshold
    //                + 4 padding samples 
    //
    //  resultant adc samples :
    //   adc[timebin-10],  adc[timebin-9], ..., adc[timebin-1],
    //   adc[timebin],     adc[timebin+1], ..., adc[timebin+N],  (N=0,1,2,...)
    //   adc[timebin+N+1], adc[timebin+N+2], adc[timebin+N+3], adc[timebin+N+4]
    //
    fread(&pData, sizeof(unsigned short int), sampleCountRead, fileData);
    nByteRead += sampleCountRead*sizeof(unsigned short int);

    //----------------------
    // Read end (crc stuff)
    //----------------------
    fread(&pEnd,sizeof(DataPacketEnd), 1, fileData);
    nByteRead += sizeof(DataPacketEnd);

    //-----------------------------------------------------------------------
    // Store channel data by using global channel id including non-phys chan
    //-----------------------------------------------------------------------
    // NOTE: Handle physical channels (0-71) when writing data to LCIO file !!!
    //
    
    // Calculate global channel id
    int thisChannel = asicN*kNumOfChannelPerChip + asicChan;
    //cout << "channel = " << thisChannel;
     
    NumSampleCounts[thisChannel] = sampleCount;

    // NOTE : Defer decode sample to the analysis stage.
    //        We need to keep sample encoding for zero supressed data format!
    //
    for (int ip=0; ip<sampleCount; ip++) {
      // Last time bin is for ASIC functions so don't take it
      if ( (ip < kNumOfTimeBins-1) && (thisChannel < kChannelMax) ) {
        eventContent[thisChannel][ip] = ntohs(pData.data[ip]);
      }
    }

  } // End of while "nByteRead"

  return 1;
}


//-----------------------------------------------------------------------------
int EncodeThisEvent(int runNumber, LCWriter* lcWrt)
{
  // Create new lcio event
  LCEventImpl*  evt = new LCEventImpl() ;
  evt->setRunNumber( runNumber   ) ;
  evt->setEventNumber( head.eventNumb ) ;
  evt->setDetectorName( "PROTOTYPE-TEST" ) ;

  // Create a collection of ADC samples to put into the event
  IMPL::LCCollectionVec* rawADCCollection;
  rawADCCollection = new IMPL::LCCollectionVec( LCIO::TRACKERRAWDATA ); 
  rawADCCollection->setTransient( false );  // we want to store it to a file

  // Second Collection for RESET channels.
  IMPL::LCCollectionVec* resetChanCollection;
  resetChanCollection = new IMPL::LCCollectionVec( LCIO::TRACKERRAWDATA ); 
  resetChanCollection->setTransient( false );  // we want to store it to a file
      
  // Third Collection for NOISE channels
  IMPL::LCCollectionVec* noiseChanCollection;
  noiseChanCollection = new IMPL::LCCollectionVec( LCIO::TRACKERRAWDATA ); 
  noiseChanCollection->setTransient( false );  // we want to store it to a file
  
  // For each channel (more channels then pads), 
  // creat a data vector, and re-store data 
  for (int chn=0; chn<kChannelMax; chn++) 
  {
    // Make a raw Data area
    TrackerRawDataImpl* rawData = new TrackerRawDataImpl; 
	      
    rawData->setCellID0( chn );
    rawData->setCellID1( 0 );    // module number
    rawData->setTime(static_cast<int>(0));

    EVENT::ShortVec ADCValues(NumSampleCounts[chn]);

    // Store only available ADC's
    for(int ip=0; ip<NumSampleCounts[chn]; ip++) // T'was  ip < NumOfTimeBins
    {  
      ADCValues[ip] = eventContent[chn][ip];
    }

    rawData->setADCValues(ADCValues);

    int asicChan = chn % kNumOfChannelPerChip;
    if ( asicChan==0 || asicChan==1  || asicChan==2 ) {
      resetChanCollection->push_back(rawData);
    }
    //else if (asicChan==16 || asicChan==29 || asicChan==54 || asicChan==67 ) {
    else if ( asicChan==15 || asicChan==28 || asicChan==53 || asicChan==66 ) {
      noiseChanCollection->push_back(rawData);
    }
    else {  
      // We need to modify channel number to physical one
      // David's mapping method to compute physical channel (0-71)
      int physChan = -10;
      if ( asicChan> 2 && asicChan < 15 ) 
        physChan = asicChan - 3;
      else if ( asicChan> 15 && asicChan < 28 )
        physChan = asicChan - 4;
      else if ( asicChan> 28 && asicChan < 53 )
        physChan = asicChan - 5;
      else if ( asicChan> 53 && asicChan < 66 )
        physChan = asicChan - 6;
      else if ( asicChan> 66)
        physChan = asicChan - 7;
      
      int asicN        = (chn - asicChan) / kNumOfChannelPerChip;  
      int thisPhysChan = physChan + asicN*kNumOfPhysChanPerChip;

      rawData->setCellID0( thisPhysChan );
      rawADCCollection->push_back(rawData);
    }
  }

  // add the collection of raw data's to the event
  ostringstream name;
  name << "ADC_RAW_DATA";  // to make this appear before other coll's in dump
  evt->addCollection( rawADCCollection , name.str() );      

  ostringstream name2;
  name2 << "RESET_CHAN_DATA";
  evt->addCollection( resetChanCollection , name2.str() );  
  
  ostringstream name3;
  name3 << "NOISE_CHAN_DATA";
  evt->addCollection( noiseChanCollection , name3.str() );  
  
  LCTOOLS::dumpEvent (evt);		
  
  try{
    lcWrt->writeEvent(evt);
  }
  catch(IOException& e ){    
    cout << e.what() << endl ;
  }
  
  delete evt;
  return 1;
}

//-----------------------------------------------------------------------------
int EncodeHeader(LCWriter* lcWrt, int& runNumber)
{
  // make run number/ get run number
  runNumber = atoi(RunNumber);

  // Open file for reading in of xml file
  ifstream cfgFile(cfgFilename);

  //Create a RUN Header for the output
  LCRunHeaderImpl* runHdr = new LCRunHeaderImpl ; 

  runHdr->setRunNumber( runNumber ) ;

  string detName("AfterLargePrototypeResist")  ;
  runHdr->setDetectorName( detName ) ;

  stringstream description ; 
  description << " run: " << runNumber 
              << " Large Prototype for ILC" ; // Stephen Fix this later
  runHdr->setDescription( description.str()  ) ;

  string tpcName("DriftChamberLP") ;
  runHdr->addActiveSubdetector( tpcName ) ;
  
  // Add RunUID string to keep time stamp : (eg) R2008.10.03-12:38:31
  runHdr->parameters().setValue( "RunUID", RunUID );
  
  // Store contents of CFG file to run header
  int lncount = 0;
  string line;
  stringstream key;
  while(getline(cfgFile,line)) {
    key << "L" << setfill('0') << setw(5) << lncount++;
    runHdr->parameters().setValue( key.str() , line ); 
    key.str("");  // reset the buffer
  }
  
  //runHdr->parameters().setValue( "ZNote: ",
  //                               "Line numbers are in %05d format, "
  //                               "so that they are properly aligned"); 
  
  //Write something, the header, to the file.
  
  lcWrt->writeRunHeader( runHdr ) ;
  //LCTOOLS::dumpRunHeader(runHdr);
  delete runHdr;
  
  return 1;
}


//-------------------------------------------------------
// Main
//-------------------------------------------------------

//int main(int argc,char **argv) 
int trans()
{
  //int status;
  int loop;
  int runNumber=0;
  int runNumberR = -1;

  strcpy(acqFilename,"RUN_00130.00000.acq");

  //Get options
  openFile();
	
  // Create an output place
  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter()  ;
  lcWrt->setCompressionLevel( 0 ) ;


  //lcioFilename = substr(acqFilename,13);
  strcpy(lcioFilename,"RUN");
  strcat(lcioFilename, strrchr(acqFilename, '_'));
  lcioFilename[15] = '\0';

  lcWrt->open( lcioFilename , LCIO::WRITE_NEW ) ;

  //create and write header
  EncodeHeader(lcWrt,runNumber);

  eventloop = 0;	
  while (true) 
  {
    // Reset event
    for (int ich=0; ich<kChannelMax; ich++) {
      for (int ip=0; ip<kNumOfTimeBins; ip++) {
        eventContent[ich][ip] = -999; 
      }
    }
	
    // Get event
    if (decodeNextEvent() != 1) {
      cout << " !!! end of file reached " << endl;
      break;
    }
    else {
      eventloop++;
      // encode and resave
      EncodeThisEvent(runNumber,lcWrt);
    }

  }  // End of main event loop !!!

  cout << "Transfered " << eventloop << " events \n";
  return 1; 
}
