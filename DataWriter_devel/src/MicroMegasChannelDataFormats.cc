#include "interface/MicroMegasChannelDataFormats.hh"

#include <TH1.h>
#include <cmath>
#include <iostream>
#include <algorithm>

TH1F * MicroMegasChannel::timeSamples(void) {

  TH1F *h = new TH1F("hPadSamples","pad samples",_adcVsTime.size(),0.5,_adcVsTime.size()+0.5);
  for( unsigned is = 0 ; is < _adcVsTime.size(); is++ ) {
    h->SetBinContent( is+1, _adcVsTime[is] );   
  }

  h->GetXaxis()->SetTitle("time");
  return h;
}


int MicroMegasChannel::maxCharge(void) {
  if( _qMax < 0 ) {
    std::vector<short>::const_iterator max = max_element(_adcVsTime.begin(), _adcVsTime.end());
    _qMax = *max;
    _tMax = max - _adcVsTime.begin();
  }
  return _qMax;
}


int MicroMegasChannel::maxCharge(int tmin, int tmax) {
  if (tmin>tmax) return -1;
  int max=-9999;
 for (int it=tmin; it<=tmax; it++){

    if (it<0) continue;
    if (it>_adcVsTime.size()) continue;
    if (_adcVsTime[it]>max) max=_adcVsTime[it];
  }
  return max;
 }


int MicroMegasChannel::tAtMax(void) {
  if( _tMax < 0 ) maxCharge();
  return _tMax;
}


double MicroMegasChannel::getMean(void) const {
  double x = 0;
  int    n = 0;
  for( unsigned is = 0 ; is < _adcVsTime.size(); is++ ) 
    if( _adcVsTime[is] > 0.01 ) { 
      x +=  _adcVsTime[is];
      n++;
    }
  if( n == 0 ) return 0;
  return x / n;
}

double MicroMegasChannel::getRMS(void) const {
  double mu = getMean();
  double x2 = 0;
  int    n = 0;
  for( unsigned is = 0 ; is < _adcVsTime.size(); is++ ) 
    if( _adcVsTime[is] > 0.01 ) { 
      x2 +=  _adcVsTime[is]*_adcVsTime[is];
      n++;
    }

  if( n <= 1 ) return 0;
  
  return sqrt((x2 - n*mu*mu) / (n-1));
}
