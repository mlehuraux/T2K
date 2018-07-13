#ifndef tpcPedCalc_hh__
#define tpcPedCalc_hh__

#include <vector>
#include <string>

class tpcPedestal {
public:
  tpcPedestal(void) {};
  void calcPedestals( std::string inputPedestalFile, bool drawPed = false );
  void dummyPedestals( std::string inputFile, double ped = 250, double rms = 5 ); /// set a default value in case no pedestal file available
  // std::vector<double> getPedestals(void) const {return _pedestalsMean1D;}
  std::vector<double>& getPedestals(void)  {return _pedestalsMean1D;} //< referenc is faster, but danger to be modifed
 std::vector<double>& getRMS(void)  {return _pedestalsRMS1D;} //< referenc is faster, but danger to be modifed


  std::vector<std::vector<double> > getPedMean2D(void) const { return _pedestalsMean2D; }
  std::vector<std::vector<double> > getPedRMS2D(void)  const { return _pedestalsRMS2D; }

private:
  std::vector<double> _pedestalsMean1D;
  std::vector<double> _pedestalsRMS1D;

  std::vector<std::vector<double> > _pedestalsMean2D;
  std::vector<std::vector<double> > _pedestalsRMS2D;
};


#endif

