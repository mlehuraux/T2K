#ifndef tpcDataAnalyzer_hh__
#define tpcDataAnalyzer_hh__

#include "interface/geomManager.hh"
#include "interface/dataDecoder.hh"

#include <string>
#include <vector>
class TPad;

class tpcDataAnalyzer {
public:
  tpcDataAnalyzer(void) {  } 
  ~tpcDataAnalyzer(void);

  void setPedestalsMean( const std::vector<std::vector<double> > & peds ) { _pedMean = peds; }
  void setPedestalsRMS ( const std::vector<std::vector<double> > & peds ) { _pedRMS  = peds; }

  void setGeometry( const femGeomManager &geo ) { _femGeomManager = geo; }


  void fillPadsWithMaxCharge( std::vector<tpcPad> & pads);
  bool evtSelector( double thresholdInSigmas = 5, int minNrow = 15 );
  double fitter( double & chi2_out, int &ndof_out );
  double fitterCluster( double & chi2_out, int &ndof_out );


  void drawEvt2D( void );
  void drawEvt3D( void );

  

private:
  femGeomManager _femGeomManager;
  femGeomViewer  _viewerMaxCharge;

  std::vector<std::vector<double> > _padsVal;
  std::vector<std::vector<double> > _padsTime;

  std::vector<std::vector<double> > _pedMean;
  std::vector<std::vector<double> > _pedRMS ;
  
  /// pads for 3D plot
  std::vector<TPad* > _pads3D;
  

}; 

#include <TMath.h>

class probFitter {
public:
  probFitter( double chi2, int ndof ) : _chi2(chi2), _ndof(ndof) {}
  ~probFitter() {}
  double eval( const double *x ) { return fabs(TMath::Prob( _chi2/(x[0]*x[0]),_ndof ) - 0.5 ) ;}
  int nPar(void)  { return 1; }
  double Up(void) { return 1; }
private:
  double _chi2;
  int    _ndof;
};

class resoFromChi2Fit {
public:
  resoFromChi2Fit( double chi2, int ndof ) : _chi2(chi2), _ndof(ndof), _status(-1) {}
  double getReso(void);
  int    status(void) {return _status;}
private:
  double _chi2;
  int    _ndof;
  double  _res;
  int     _status;

};

#endif
