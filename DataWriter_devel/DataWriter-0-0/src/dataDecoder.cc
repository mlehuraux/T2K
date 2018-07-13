#include "interface/dataDecoder.hh"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>


using namespace std;
tpcDataDecoder::tpcDataDecoder( std::string filename, int tpcfem, int mgfem )  { _dataFileName = filename; _first = true; _last = false; _iEvt = 0; _tpcfem=tpcfem, _mgfem=mgfem;}

void tpcDataDecoder::decodeNextEvent(void) {
  int Ntime = 530;
  int buffer = -1;
  unsigned short pData[Ntime];

  /// clear data
  _padData.clear();
  _mgData.clear();

  if( _last ) return;

  if( _first ) {
    cout << "OomonAcqReader OpenFile opening file " << _dataFileName << endl;
    _fData = fopen(_dataFileName.c_str(),"rb");

    // check if this input datafile exist
    if( _fData == NULL ){
      cout<<"Can't open data file : " << _dataFileName <<endl;
      exit(1);
      return ;// End of program
    }

    // Read Run UID characters
    char runUid[21];
    buffer=fread(runUid, 1, 20, _fData);
    runUid[20] = '\0'; // Null terminated C string
    cout << "File UID is "<< runUid << endl;

    _first = false;
  }

  bool DEBUG =  false;

  // if( _iEvt%50 == 0 ) cout << " converting event: " << _iEvt << endl;
  if( _iEvt%1 == 0 ) cout << "converting event: " << _iEvt<<"\r"<<flush ;
  // Read Event Header
  if(fread(&head, sizeof(Header), 1, _fData) != 1) {
    fclose(_fData);
    _last = true;
    return ; /*return 0; End of program*/
  }

  head.eventSize = ntohl(head.eventSize); // size of current event
  head.eventNumb = ntohl(head.eventNumb); // current event number

  int eventSize = head.eventSize;
  int eventNumb = head.eventNumb;

  //   int evN = head.eventNumb;
  if (DEBUG) cout <<  "Event number: " << eventNumb << endl;

  int rawDataSize = eventSize - sizeof(head) - sizeof(pEnd);
  int nByteTot    = rawDataSize ;
  int nByteRead   = 0;

  //cout << "New Event : size = 0x" << hex << eventSize << ", "
  //     << eventNumb << dec << endl;

  while (nByteRead < nByteTot ) // nByteTot represents the size of the full event, not only the current packet
  {
    // Read Packet Header of the current packet
    buffer     = fread(&pHeader, sizeof(DataPacketHeader), 1, _fData); //!!!!!
    nByteRead += sizeof(DataPacketHeader);

    // # of data words in the packet except header and trailer
    int kTrailerSize = 2;
    int kPktHdrSize  = sizeof(pHeader) / sizeof(short); // 8
    int nTotPktWords = ntohs(pHeader.size) / sizeof(unsigned short);

    int nlinespacket = nTotPktWords - kPktHdrSize - kTrailerSize;

    //cout << "New Packet : size=0x" << hex << ntohs(pHeader.size) << dec << endl;

    // Check sync fail
    if (GET_LOS_FLAG  ((ntohs(pHeader.hdr) ))) cout<<"LOS FLAG SET"<< endl;
    if (GET_SYNCH_FAIL((ntohs(pHeader.hdr) ))) cout<<"SYNC FAIL FLAG SET"<< endl;
    // Check compress mode
    int compressed = GET_RB_COMPRESS(ntohs(pHeader.args));
    //int tempAsic1  = GET_RB_ARG1(ntohs(pHeader.args));
    //int tempAsic2  = GET_RB_ARG2(ntohs(pHeader.args));

    int version   = GET_VERSION  (ntohs(pHeader.dcc_hdr)); // old is 0, new is 1
    int dccFlag   = GET_FLAG     (ntohs(pHeader.dcc_hdr));
    int replyType = GET_RT       (ntohs(pHeader.dcc_hdr));
    int femId     = GET_FEM_INDEX(ntohs(pHeader.dcc_hdr));
    if (DEBUG) cout << "femId: " <<  femId  << endl;

    //     int FrameType = GET_FRAME_TY_V2(ntohs(pHeader.dcc_hdr));
    //     cout << "FrameType: " <<  FrameType << endl;
    //     cout << "RT: "   <<  RT << endl;
    if (DEBUG) cout << "Version: " << version << endl;
    if (DEBUG) cout << "Flag: "    << dccFlag << endl;

    // Check dcc flags
    if ( version != 1 ) {
      cout << "WARNING: This is NOT a version 2 format data" << endl;
    }

    // If EoE or Non-FEM data, read out all remaining packet data and
    // exit from the loop since there is nothing to be done.
    if ( dccFlag==2 || replyType==1)
    {
      int nWordsToRead = nTotPktWords - kPktHdrSize; // - kTrailerSize;
      buffer = fread(&pData, sizeof(unsigned short), nWordsToRead, _fData);
      nByteRead += sizeof(unsigned short) * nWordsToRead;
      break;
    }

    // compute asic#
    //int channelN = tempAsic1/6;
    //int asicN    = 10*(tempAsic1%6)/2 + tempAsic2;
    // cout << "asicN, channelN: " << asicN << ", " << channelN << endl;

    // get sample count of the first channel
    //int tempAsic1  = GET_RB_ARG1(ntohs(pHeader.args));
    //int tempAsic2  = GET_RB_ARG2(ntohs(pHeader.args));
    //int sampleCount = ntohs(pHeader.scnt);

    //    if (DEBUG)  cout << "Word count: " << sampleCount << "\t" << asicN << "\t" << channelN << endl;
    // now read the data (should use the sample count !!!!
    // take into account padding short int to get even total

    int physChannel=-10;
    int bin_index;

    bool firstChan  = true;
    int  nlinesread = 0;
    while ( nlinesread < nlinespacket )
    {
      // Get args and sample count
      unsigned short args, sampleCount;
      if (firstChan)
      {
        //cout << "--- First Chan Dawa Block ----" << endl;
        args        = ntohs(pHeader.args);
        sampleCount = ntohs(pHeader.scnt);
        firstChan   = false;
      }
      else
      {
        //cout << "--- New Chan Dawa Block ----" << endl;
        NewDataHeader newData;
        buffer = fread(&newData, sizeof(unsigned short int), 2, _fData);
        nlinesread += 2;
        nByteRead  += 2*sizeof(unsigned short int);


        if ( ((ntohs(newData.args)      & 0xE000) != 0x2000) ||
             ((ntohs(newData.wordcount) & 0xF000) != 0x4000))
        { cout << "something wrong happened... exiting" << endl; exit(0); }

        args        = ntohs(newData.args)      & 0x1FFF; // No need but...
        sampleCount = ntohs(newData.wordcount) & 0x0FFF;
      }

      int arg1 = GET_RB_ARG1(args);
      int arg2 = GET_RB_ARG2(args);

      // compute asic#
      int channelN = arg1/6;
      int asicN    = 10*(arg1%6)/2 + arg2;

      // From now on only data words until next channel data.
      // Thus read the data words.
      int sampleCountRead = sampleCount;
      if (sampleCount%2 == 1)
        sampleCountRead++;

      buffer = fread(&pData, sizeof(unsigned short), sampleCountRead, _fData);
      nlinesread += sampleCountRead;

      nByteRead += sampleCountRead * sizeof(unsigned short);

      //unsigned short binIndex = ntohs(pData.data[0]);
      unsigned short binIndex = ntohs(pData[0]);
/*
      if (compressed && ((binIndex & 0xFE00) != 0x1000) ) {
        cout << "Not a bin index word: 0x" << hex
             << ntohs(pData[0]) << " " << ntohs(pData[1]) << " " << ntohs(pData[2]) << dec << endl;
        exit(1);
      }
*/
      bin_index = GET_BIN_INDEX(binIndex);
      //cout << "Bin index: " << bin_index << endl;
      //cout << "new word count: " << sampleCount << endl;

      int t0 = 0;  // will be updated for compressed data.  (?? BT ?????)

      //
      // Handle data
      //
      bool chstatus = false;

      // compute physical channel (0-71)
      //TODO:  tmp dirty code. Sort out an automatic switch based on some geometry file  FEC/FEM or Asic number =>   multigen=xx Tpcfem=1-xx
      bool Tpcfem=(femId==_tpcfem); // micromegas TPC pads
      bool multigen=(femId==_mgfem);  // micromegas multigen strips




      if (Tpcfem){


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

      }else

	if (multigen){

	  // BT from T2KDataReader.cxx
	  if(channelN>12 && channelN<79)
	{
	  //if((channel%2)==1 && channel > 10 && channel < 44) strip=72-(int) (( physChannel+1)/2)-7;

	  if(channelN==13) {  physChannel = channelN; chstatus = true;}
	  else if( channelN>=16 && channelN<28 ) { physChannel = channelN-2; chstatus = true;}
	  else if( channelN>=29 && channelN<53 ) { physChannel = channelN-3; chstatus = true;} //< Bobo modified t avoid channel 15 being transformed to 0
	  //	  else if(channelN<54) { physChannel = channelN-3; chstatus = true;}
	  else if( channelN>=54&& channelN<66) {  physChannel = channelN-4; chstatus = true;}
	  else if (channelN>=67) { physChannel =  channelN-5; chstatus = true;};

	}
	  physChannel=physChannel-12;
	  //Keep one additionnal channel to study noise
	  if (channelN==14) { physChannel=0; chstatus=true;}


	  //BOBO FIXME  TEMPORARY DEBUG 	  physChannel=channelN; chstatus = true;
	}//end multigen decoding




      int thisPhysChannel = asicN*72+physChannel;   // 2011/05/12
      //int thisPhysChannel = hwMap->getPadId(args);  // 2011/05/12
      if ((thisPhysChannel < 0) || (thisPhysChannel >= 1728)) { //Todo this may work for TPC pad, but what happens when same databaknk contains both multigen and TPC pad !!!

        //cout << "thisPhysChan=" << thisPhysChannel << ", "
        //     << "args=" << args << endl;
	    //bobo test what happens when reading unphysical channel
	    continue;
      }


      //      vector<int> dataADC(sampleCount,0);
      vector<int> dataADC(Ntime,0);

      if (physChannel>=0 && physChannel<72 && chstatus)
      {

        // // Reset dataADC
        // for (Int_t itime=0; itime<Ntime; itime++) {dataADC[itime]=0;}

        //unsigned short int timebin0 = ntohs(pData.data[0]);
        //cout << "timebin0: " << (timebin0 & 0x1FF)  << endl;
        //int t0 = (timebin0 & 0x1FF);
        if(compressed) 			////compressed data
        {
          // Calculate time bin of the first sample of the first pulse
          const int kPreThr = 10; // # of sample before threshold
          int nskip;          // in case of scnt < 10,
          if (bin_index < kPreThr) {
            t0    = 0;            // time bin id of the first valid sample = 0
            nskip = kPreThr - t0; // first few samples are just paddings
          }
          else {
            t0    = bin_index - kPreThr;
            nskip = 0;           // all samples are valid --> no need to skip
          }

          //
          int iT = 0; // distance from the t0
          if (sampleCount<=Ntime) {
            for (int ismpl=nskip+1; ismpl<sampleCount-1; ismpl++)
            {
              //unsigned short int datum = ntohs(pData.data[ismpl]);
              unsigned short int datum = ntohs(pData[ismpl]);

              if ((datum & 0x1000) == 0)   // Check Bin Index Word mask
              {
                // Just in case
                if ((datum & 0xF000) != 0) {
                  cout << "*** ERROR: Wrong sample word: 0x" << hex
                       << datum << dec << ": nskip=" << nskip << endl;
                }

                const int kOffset = 250;

                int index  = t0 + iT;
                int datumC = datum - kOffset;
                dataADC[index] = datumC;

                iT++;

              }
              else  // This is Bin index word
              {
                iT        = 0;  // reset distance from the t0
                bin_index = GET_BIN_INDEX(datum);
                t0        = bin_index - kPreThr;
              }
            } // sample words walking
          } // IF
        }
        else {  // Non-Compressed
          // NOTE: There is NO bin index word in non-compressed data !
          for ( int ismpl=1; ismpl<sampleCount-1; ismpl++)
          {
            dataADC[ismpl]= ntohs(pData[ismpl]);
          }
        }  //end of boucle over data

	//-----------------------------------

	//store data in vectors
	if (multigen){
	  MultigenStrip mychannel;
	  _mgData.push_back( mychannel );
	  (_mgData)[_mgData.size()-1].setPad( thisPhysChannel, dataADC );
	}	else if (Tpcfem) {
	  tpcPad  mychannel;
	  _padData.push_back( mychannel );
	  (_padData)[_padData.size()-1].setPad( thisPhysChannel, dataADC );

	}
	else{ cerr << "undefined data type " << endl; exit(1);}

      }

	// Now try to store result in
	//just a test


	/*

	std::vector< MicroMegasChannel> *data=0;
	if (multigen) data=& _mgData;
	else if (Tpcfem) data=& _padData;
	else{ cerr << "undefined data type " << endl; exit(1);}
	MicroMegasChannel mychannel;
	data->push_back( mychannel );
	(*data)[data->size()-1].setPad( thisPhysChannel, dataADC );

      }
	*/

    }

    // Read the trailer words
    buffer = fread(&pEnd, sizeof(DataPacketEnd), 1, _fData);
    nByteRead += sizeof(DataPacketEnd);

  }//end main channel

  _iEvt++;

}




