#ifndef AfterFormats_hh__
#define AfterFormats_hh__
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
  unsigned short dcc_hdr;    // To change if the data after May 2010
  unsigned short hdr;
  unsigned short args;
  unsigned short ts_h;
  unsigned short ts_l;
  unsigned short ecnt;
  unsigned short scnt;
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

#endif
