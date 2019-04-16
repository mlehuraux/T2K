
//
// How to reconstruct adc spectrum from zero suppresed data format
//
//  1. Padding
//
//  Q. How to interpret, eg, 252 222 223 231 
//
//     Is this 0 0 0 0 0 0 ... 0 252  222 223 231 0 0 0 0 0 ?
//
//  A. The value is still absolute value, which threshold is not subtracted.
//     Thus we have to subtract threshold value from the data !!!
//  
//     There is a threshold in CFG file. (eg) <threshold>0x12C</threshold>.

//
// Comments :
// 
//  (1) sampleCount>9  
//      <- considering padding there must be more than 10 if at the end
//         or 6 if at the beginning and that was the last above threshold
//      <- mostly data with sampleCount=8 is 0-0-0-0-0-0-0-0
//
//  (2) takes only sampleCount-2 bins instead of taking all counts !!!
// 
//  (3) check if it is timebin index with mask 0x1000
//
//  (4) binID and adc value takes only 9LSB : mask = 0x1FF
//
//  (5) when hit happens ( where wave form first go above the threshold )
//
//  (6) how many hits this channel/pad has so far
//
//  (7) SUBTRACT 250 from the value 
//      <-- means threshold is not subtracted yet !!!
//
//      But still better not to too tight.  That's why there are paddings.
//      Just shift to pseudo zero instead of to absolute zero.
//      
//  (8) Fill only that area.  Need to reset hist every event.
//
//  (9) Threshold is software threshold to handle pesudo zero.
//
//  (10) Simply fill the 2D Histo with max ADC value.
//
//         hPat->Fill(iCOL[iPhysChan],iROW[iPhysChan],MaxCharge[iPhysChan]);
//
//       There is no mapping except array indexing.
//

//________________________________________________________
int DecodeNextEvent(void) 
{
  // Read next header or quit of end of file
  if(fread(&head, sizeof(Header), 1, _fData) != 1) {
    fclose(_fData);
    return 0; // End of program
  }
	
  head.eventSize = ntohl(head.eventSize);
  head.eventNumb = ntohl(head.eventNumb);
  //head.eventTime = ntohl(head.eventTime);
	
  int rawDataSize = head.eventSize - sizeof(head);
  // 	cout << "Event number from header --> " << head.eventNumb << endl;
  // 	cout << " total rawdata size " << head.eventSize << endl ;
  // 	cout << "Rawdata size " << rawDataSize << endl ;
  // 	iFirstPacket=0;
  int nByteTot = rawDataSize ;
  int nByteRead=0;

  int countPads =0;

  while (nByteRead < nByteTot ) {

    // now read data packet header
    fread(&pHeader, sizeof(DataPacketHeader), 1, _fData);
    nByteRead+=sizeof(DataPacketHeader);

    // check sync fail
    if (GET_LOS_FLAG((ntohs(pHeader.hdr) ))) cout << " LOS FLAG SET " << endl  ;
    if (GET_SYNCH_FAIL((ntohs(pHeader.hdr) ))) cout << " SYNC FAIL FLAG SET " << endl  ;

    // compute asic#
    int tempAsic1 = GET_RB_ARG1(ntohs(pHeader.args));
    int tempAsic2 = GET_RB_ARG2(ntohs(pHeader.args));
    int channel = tempAsic1/6;
    int asicN = 10*(tempAsic1%6)/2 + tempAsic2;
    // 		cout << " channel " << channel << " asic " << asicN << endl;

    //get sample count
    int sampleCount = ntohs(pHeader.scnt);

    // take into account padding short int to get even total
    int sampleCountRead = sampleCount;
    if (sampleCount%2==1)  sampleCountRead++;

    // now read the data 
    fread(&pData, sizeof(unsigned short int), sampleCountRead, _fData);
    nByteRead+=sampleCountRead*sizeof(unsigned short int);
   
    // read end (crc stuff)
    fread(&pEnd,sizeof(DataPacketEnd), 1, _fData);
    nByteRead+=sizeof(DataPacketEnd);

    // compute physical channel (0-71)
    int physChannel=-10; 
    if (channel> 2 && channel < 15 ) {
      physChannel= channel -3; 
    } else if (channel> 15 && channel < 28 ) {
      physChannel= channel -4; 
    } else if (channel> 28 && channel < 53 ) {
      physChannel= channel -5; 
    } else if (channel> 53 && channel < 66 ) {
      physChannel= channel -6; 
    } else if (channel> 66  ) {
      physChannel= channel -7; 
    }
		
    int thisPhysChannel = asicN*72+physChannel;
    int tempCharge = 0; 
    //int tempNHit = 0;
    int thisCell = -999;
    int maxcharge = -50;
    //int maxTime = 510; 
 

    //only physical channels

    if (physChannel>-1 && physChannel<72 && sampleCount>9) {     // <-- (1)
      for (int ip=0; ip<sampleCount-2; ip++) {                   // <-- (2)
        unsigned short int datum = ntohs(pData.data[ip]);
        if (datum & 0x1000) {//4096                              // <-- (3)
          //cout << "new packet found cell " << (datum&0xFFF) << endl;//4095
          //thisCell = (datum&0xFFF);                            // <-- (4)
          thisCell = (datum&0x1FF);                              // <-- (4')
          //if (thisCell>0 && thisCell <511) {
          //timeHit[thisPhysChannel]= (datum&0xFFF); 
          timeHit[thisPhysChannel]= thisCell;                    // <-- (5)
          nHit[thisPhysChannel]++;                               // <-- (6)
          //}
        }
        else {
          int datumC = datum-250;                                // <-- (7)
          dataContent[ip][thisPhysChannel]=datumC;
          hADCvsTIME[thisPhysChannel]->Fill(ip,dataContent[ip][thisPhysChannel]);  // <-- (8)

          /** YHS : they are not stored to any where
          //compute sum of charge for all sample count for each channel
          if (datumC > 0 ) {
            tempCharge+=datumC;           // Integrate charge
          }
          if (datumC > maxcharge) {
            maxcharge = datumC;           // max charge
          }
          */
        }
      }

      //Generate Hit info
      _nhitPerChan=0;


      for (int ip=0; ip<sampleCount-2; ip++) {
        if (dataContent[ip][thisPhysChannel] > Threshold) {  // <-- (9)
					
          countPads++;
					
          //start oh Hit

          _hitTimeMult++;
          //find maximum
          if (dataContent[ip][thisPhysChannel]>_hitMaximum) {
            _hitMaximum = dataContent[ip][thisPhysChannel];
            _hitBinMaximum = ip;
          }
	
          //compute sum of charge for all sample count for each channel
          _hitSum+= dataContent[ip][thisPhysChannel];
          //end of Hit

          // PEEK next bin if it goes to below threshold
          if (dataContent[ip+1][thisPhysChannel] <= Threshold) {
            //record hit info
            HitTimeMultiplicity.push_back(_hitTimeMult);
            HitMaxCharge.push_back(_hitMaximum);
            HitBinMaxCharge.push_back(_hitBinMaximum);
            HitSum.push_back(_hitSum);
            HitChannelId.push_back(thisPhysChannel);
            //reset for new hit
            _hitTimeMult=0;
            _hitMaximum=-10;
            _hitSum=0;
            _hitBinMaximum=0;
            _nhitPerChan++;
          }
        }
      }
			
			 
      if(_nhitPerChan!=0) {
        //cout << "Event \t" << head.eventNumb 
        //     << "\t # Hit per chan = " << _nhitPerChan 
        //     << "\t for channel\t" << thisPhysChannel << endl;
      }
			
	
      if (maxcharge>Threshold) {
        if (tempCharge > MaxCharge[thisPhysChannel]) {
          MaxCharge[thisPhysChannel]= maxcharge;
        }
        // compute sum of ADC charge per event for each channel 
        // above threshold set at 100 ADC unit
        padtotcharge[thisPhysChannel]+= tempCharge; 
        evtpadcharge[thisPhysChannel][head.eventNumb] += tempCharge;
      }

    }//end of if physchanel

  }//end of while nByteRead

  //if(countPads>HitsOnPads) cout << "countPads :" << countPads << endl;;
  if(countPads>15) cout << "countPads :" << countPads << endl;;

  return countPads;
}



//-----------------------------------------------------------------------------
// Hit pattern :  max hit distribution : row vs col
// 
void FillRoHisto() {
	
  hPattern->Reset();		
  for (int iPhysChan=0; iPhysChan < kPhysChannelMax; iPhysChan++) 
  {
    if (MaxCharge[iPhysChan] > Threshold) {
      hRawCharge->Fill(MaxCharge[iPhysChan]);
    }
    hPattern->Fill(iCOLUMN[iPhysChan],iROW[iPhysChan],MaxCharge[iPhysChan]); //  <-- (10)
  }
}
