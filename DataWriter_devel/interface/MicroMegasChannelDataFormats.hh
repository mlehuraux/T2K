#ifndef MicroMegasChannelDataFormats__hh__
#define MicroMegasChannelDataFormats__hh__

#include <vector>


class TH1F;

class MicroMegasChannel {
public:
  MicroMegasChannel(void) :  _iPhysChannel(-1), _qMax(-999), _tMax(-1) {};
  // should become setchannel  
 void setPad( int iphysChannel, const std::vector<short> & samples) { _iPhysChannel = iphysChannel; _adcVsTime = samples; }

  int maxCharge(void);
  int tAtMax(void);

  int maxCharge(int tmin, int tmax);//< max charge found between tmin and tmax

  int getPhysChannel(void) const {return _iPhysChannel;}
  std::vector<short> &getADCvsTime() { return _adcVsTime; } //< return a reference. The vector can therefore be modified.. Carefull
  //std::vector<int> getADCvsTime() const { return _adcVsTime; }

  double getMean() const;
  double getRMS()  const;
  void ResetMax() { _qMax=-999;_tMax=-1;}; //< Reset the maximum charge and timemax for recompatation
  TH1F * timeSamples(void);

protected:

  std::vector<short> _adcVsTime;
  int _iPhysChannel;
  int    _qMax;
  int    _tMax;
  //double _qTot;



};





#endif
