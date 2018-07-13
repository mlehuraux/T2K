#ifndef dataDecoder_hh__
#define dataDecoder_hh__

//Librairies for reading acquisition files
#include <arpa/inet.h>
#include "interface/mygblink.h"
#include "interface/tpcPadDataFormats.hh"
#include "interface/MultigenStripDataFormats.hh"
#include "interface/AfterDataFormats.hh"

#include <string> 


///--------------------------------------------------------
/// Class handling decoding
///--------------------------------------------------------

class tpcDataDecoder {
public:
  tpcDataDecoder( std::string filename, int tpcfem, int mgfem);

  /// not ideal,  std::vector<tpcPad> is re-copied... can take some time, need to be moved to auto-pointer .
void decodeNextEvent(void);

  bool endOfDataFile(void) { return _last; }
  std::vector<tpcPad> & GetPadData(void){ return _padData;}
  std::vector<MultigenStrip> & GetMGData(void){ return _mgData;}
  

private:
  std::string _dataFileName;
  bool _first;
  bool _last;
  int _iEvt;
  FILE * _fData;
  int _tpcfem; ///< Front End Module id for TPC pad
  int _mgfem; ///< Fron End Module id for Multigen 
  Header head;
  DataPacketHeader pHeader;  
  DataPacketEnd    pEnd; 

  std::vector<tpcPad>  _padData;
  std::vector<MultigenStrip>  _mgData;
  
};






#endif

