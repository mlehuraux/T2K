#ifndef xmlReader_hh__
#define xmlReader_hh__


#include <iostream>
#include <string>
#include <set>


class TFile;

class tpcXmlConfig {
public:
  std::string trigger;  // trigger type
  std::string acqType;  // acquisition type: pedestal, standard, calibration

  // pedestal
  bool  pedActive; 
  bool  isPedCalc;
  float pedMean;

  // samples
  float sampleTimeWidth; // sample time length in nanoseconds
  int  nSamples       ; // number of samples
  int   divideScaClock ; // timeSampleWidth = divideScaClock / 100MHz 

  // shaping
  float peakingTime;

  void load(const std::string &filename);
  void dumpToRootFile(TFile *, const std::string &treeName = "runConfig" );
};


std::ostream& operator<<(std::ostream& out, const tpcXmlConfig & conf);

#endif
