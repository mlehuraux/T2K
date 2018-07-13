#include "interface/tpcDataAnalyzer.hh"

#include <TH2.h>
#include <TH2Poly.h>
#include <TPad.h>

#include <iostream>
#include <cmath>
using namespace std;

tpcDataAnalyzer::~tpcDataAnalyzer(void) { 
}


void tpcDataAnalyzer::fillPadsWithMaxCharge(  std::vector<tpcPad> & padsData ) {
  _padsVal .erase( _padsVal .begin(), _padsVal .end() );
  _padsTime.erase( _padsTime.begin(), _padsTime.end() );

  _padsVal. resize( _femGeomManager.nRow(), vector<double>(_femGeomManager.nCol(), 0) );
  _padsTime.resize( _femGeomManager.nRow(), vector<double>(_femGeomManager.nCol(),-1) );

  for( unsigned ipad = 0 ; ipad < padsData.size(); ipad++ ) {
    int physCh = padsData[ipad].getPhysChannel();
    int qmax   = padsData[ipad].maxCharge();
    int tmax   = padsData[ipad].tAtMax();
    double padPedestal = 0;
    if( _pedMean.size() > 0 ) padPedestal = _pedMean[_femGeomManager.iPad(physCh)][_femGeomManager.jPad(physCh)];

    _padsVal[ _femGeomManager.iPad(physCh)][ _femGeomManager.jPad(physCh)] = qmax - padPedestal;
    _padsTime[_femGeomManager.iPad(physCh)][ _femGeomManager.jPad(physCh)] = tmax;

  }
  
  
}

void tpcDataAnalyzer::drawEvt2D( void )  {
  
  _viewerMaxCharge.init( "maxCharge2D", _femGeomManager );
  
  for( unsigned irow = 0 ; irow < _padsVal.size(); irow++ ) 
  for( unsigned icol = 0 ; icol < _padsVal[0].size(); icol++ ) 
    _viewerMaxCharge.setBinContent( irow, icol,  _padsTime[irow][icol],  _padsVal[irow][icol]  ); 
  
  
  double max = 1000;
  double min = 0;
  if( _pedMean.size() > 0 ) {
    /// pedestal were subtracted so max can be lower
    max = 500;
    min = -50;
  }
  //// saturate real hist
  _viewerMaxCharge.setMinMax(min,max);
  _viewerMaxCharge.drawXY("colz");
  
}


#include <TCanvas.h>
void tpcDataAnalyzer::drawEvt3D( void )  {
  /// init pads;
  if( _pads3D.size() == 0 ) {
    /// create 3 different pads for the 3 different 3D viewer
    _pads3D.push_back( new TPad("padXY","pad XY",0.0,0.0,0.7,0.7) );
    _pads3D.push_back( new TPad("padXT","pad XT",0.0,0.7,0.7,1.0) );
    _pads3D.push_back( new TPad("padTY","pad TY",0.7,0.0,1.0,0.7) );
    for( int ipad = 0; ipad < 3; ipad++ ) {
      _pads3D[ipad]->SetRightMargin(0.05);
      _pads3D[ipad]->SetTopMargin(  0.05);
      _pads3D[ipad]->Draw();
    }  
  }


  _viewerMaxCharge.init( "maxCharge3D", _femGeomManager );
  
  for( unsigned irow = 0 ; irow < _padsVal.size(); irow++ ) 
  for( unsigned icol = 0 ; icol < _padsVal[0].size(); icol++ ) 
    _viewerMaxCharge.setBinContent( irow, icol,  _padsTime[irow][icol],  _padsVal[irow][icol]  ); 
  
  
  double max = 1000;
  double min = 0;
  if( _pedMean.size() > 0 ) {
    /// pedestal were subtracted so max can be lower
    max = 500;
    min = -50;
  }

  
  //// saturate real hist
  _viewerMaxCharge.setMinMax(min,max);
  _pads3D[0]->cd(); _viewerMaxCharge.drawXY("col"); 
  _pads3D[1]->cd(); _viewerMaxCharge.drawXT("col");
  _pads3D[2]->cd(); _viewerMaxCharge.drawTY("col");
  
}



bool tpcDataAnalyzer::evtSelector( double thresholdSigmas, int minNrow  ) {
  int numberOfRowTouched = 0;

  for( int irow = 0 ; irow < _femGeomManager.nRow(); irow++ ) 

    if( irow >= _femGeomManager.iPadMin() + 2 && irow <= _femGeomManager.iPadMax() - 2 ) {

      /// remove top and bottom rows to avoid edge effect
      int nPadTouchedInRow = 0;
      for( int icol = 0 ; icol < _femGeomManager.nCol(); icol++ ) 
	if( _padsVal[irow][icol] / _pedRMS[irow][icol] > thresholdSigmas ) nPadTouchedInRow++;
    
      if( nPadTouchedInRow >= 1 ) numberOfRowTouched++;
    }

  if( numberOfRowTouched >= minNrow ) return 1;
  return false;
}


#include <TF1.h>
#include <TCanvas.h>
double tpcDataAnalyzer::fitter( double & chi2_out, int &ndof_out) {

  //  femGeomViewer viewerUsedHits;
  //  viewerUsedHits.init( "h2DusedHits", _femGeomManager );


  double x_bar  = 0;
  double y_bar  = 0;
  double xy_bar = 0;
  double yy_bar = 0;

  vector<double> xMem, yMem;
  vector<int> iMem, jMem;

  for( int irow = 0 ; irow < _femGeomManager.nRow(); irow++ ) 

    if( irow >= _femGeomManager.iPadMin() + 2 && irow <= _femGeomManager.iPadMax() - 2 ) {
      
      /// find xMax
      double vMax = -1;
      int jMax = -1;
      for( int icol = 0 ; icol < _femGeomManager.nCol(); icol++ ) {
	if( _padsVal[irow][icol] / _pedRMS[irow][icol] > 20 && _padsVal[irow][icol] > vMax  ) {
	  vMax = _padsVal[irow][icol];
	  jMax = icol;
	}
      }

      if( jMax >= 0 ) {
	xMem.push_back( _femGeomManager.xPad(irow,jMax) );
	yMem.push_back( _femGeomManager.yPad(irow,jMax) );
	x_bar  += _femGeomManager.xPad(irow,jMax);
	y_bar  += _femGeomManager.yPad(irow,jMax);
	xy_bar += _femGeomManager.xPad(irow,jMax)*_femGeomManager.yPad(irow,jMax);
	yy_bar += _femGeomManager.yPad(irow,jMax)*_femGeomManager.yPad(irow,jMax);
      }
    }

  x_bar  /= xMem.size();
  y_bar  /= xMem.size();
  xy_bar /= xMem.size();
  yy_bar /= xMem.size();

  double a = (yy_bar - y_bar*y_bar) / (xy_bar - x_bar*y_bar);
  double b = y_bar - a * x_bar;

  double chi2 = 0;
  for( unsigned ip = 0 ; ip < xMem.size(); ip++ ) {
    double dchi = xMem[ip]-(yMem[ip]-b)/a;
    chi2 += dchi*dchi;
  }

  auto_ptr<TF1> fLin( new TF1("fLin","[0]*x+[1]",-200,200) );
  fLin->SetParameter(0,a);
  fLin->SetParameter(1,b);
  fLin->SetLineWidth(3);
  fLin->SetLineColor(kWhite);
  fLin->DrawClone("same");

  // /// view hist used in the fit
  // cout << " number of hits: " << iMem.size() << endl;
  // for( unsigned ii = 0 ; ii < iMem.size(); ii++ ) 
  //   viewerUsedHits.setBinContent( iMem[ii], jMem[ii],  50 ); 

  // cout << " chi2 / ndof= " << chi2 / xMem.size() << endl;
  
  // TCanvas * c = new TCanvas("usedless","used hits",800,700);
  // c->cd();viewerUsedHits.drawCopyXY("col");
  // c->Update();
    
  chi2_out = chi2;
  ndof_out = xMem.size()-2;
  if( chi2_out / ndof_out > 5 ) return 4.9999999;

  resoFromChi2Fit fitReso( chi2_out,ndof_out);
  double res = fitReso.getReso();
  return res > 5 ? 4.9999999 : res;
}



double tpcDataAnalyzer::fitterCluster( double & chi2_out, int &ndof_out ) {

  //  femGeomViewer viewerUsedHits;
  //  viewerUsedHits.init( "h2DusedHits", _femGeomManager );


  double x_bar  = 0;
  double y_bar  = 0;
  double xy_bar = 0;
  double yy_bar = 0;

  vector<double> xMem, yMem;
  for( int irow = 0 ; irow < _femGeomManager.nRow(); irow++ ) 

    if( irow >= _femGeomManager.iPadMin() + 2 && irow <= _femGeomManager.iPadMax() - 2 ) {
      
      /// find xMax
      double vMax = -1;
      int jMax = -1;
      for( int icol = 0 ; icol < _femGeomManager.nCol(); icol++ )
	if( icol >= _femGeomManager.jPadMin() + 5 && icol <= _femGeomManager.jPadMax() - 5 ) {
	if( _padsVal[irow][icol] / _pedRMS[irow][icol] > 20 && _padsVal[irow][icol] > vMax  ) {
	  vMax = _padsVal[irow][icol];
	  jMax = icol;
	}
      }


      if( jMax >= 0 ) {
	/// construct cluster around max
	double xCl = 0;
	double yCl = 0;
	double weight = 0;
	for( int icol = jMax - 2 ; icol <= jMax + 2 && icol <  _femGeomManager.nCol(); icol++ ) 
	  if( _padsVal[irow][icol] / _pedRMS[irow][icol] > 5 ) {
	  xCl += _padsVal[irow][icol]*_femGeomManager.xPad(irow,icol) ;
	  yCl += _padsVal[irow][icol]*_femGeomManager.yPad(irow,icol) ;
	  weight += _padsVal[irow][icol];
	}
	xCl /= weight;
	yCl /= weight;

	xMem.push_back( xCl );
	yMem.push_back( yCl );
	x_bar  += xCl;
	y_bar  += yCl;
	xy_bar += xCl*yCl;
	yy_bar += yCl*yCl;
      }
    }

  x_bar  /= xMem.size();
  y_bar  /= xMem.size();
  xy_bar /= xMem.size();
  yy_bar /= xMem.size();

  double a = (yy_bar - y_bar*y_bar) / (xy_bar - x_bar*y_bar);
  double b =  y_bar - a*x_bar;
  double theta =  atan( 1./a )*180./TMath::Pi();
  //  cout << " theta = " << atan( 1./a )*180./TMath::Pi() << endl;
  
  double chi2 = 0;
  for( unsigned ip = 0 ; ip < xMem.size(); ip++ ) {
    double dchi = xMem[ip]-(yMem[ip]-b)/a;
    chi2 += dchi*dchi;
  }

  auto_ptr<TF1> fLin( new TF1("fLinCl","[0]*x+[1]",-200,200) );
  fLin->SetParameter(0,a);
  fLin->SetParameter(1,b);
  fLin->SetLineWidth(4);
  fLin->SetLineColor(kYellow-10);
  fLin->SetLineStyle(kDashed);
  fLin->DrawClone("same");
  
  chi2_out = chi2;
  ndof_out = xMem.size()-2;
  if( chi2_out / ndof_out > 5 ) return 4.9999999;
  resoFromChi2Fit fitReso( chi2_out,ndof_out);
  double res = fitReso.getReso();
  if( fabs(theta) > 10 ) return 4.9999999; 
  return res > 5 ? 4.9999999 : res;
}





#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
double resoFromChi2Fit::getReso(void) {
  probFitter probFcn(_chi2,_ndof);
  string minimizerName = "Minuit2";  string minimizerAlgo  = "Migrad";

  ROOT::Math::Functor FCN( &probFcn, &probFitter::eval, probFcn.nPar() );
  ROOT::Math::Minimizer *minimizer =  ROOT::Math::Factory::CreateMinimizer(minimizerName.c_str(),minimizerAlgo.c_str());
  minimizer->SetFunction( FCN );
  double res0 = sqrt(_chi2/_ndof);
  if( res0 < 1e-2 ) res0 = 0.1;
  minimizer->SetLimitedVariable( 0, "reso", res0, 0.001, 0.05, 10 );
  // cout << "Minimizing with:  " << minimizerName << " (algo: " 
  //      << minimizerAlgo << "), tolerance: " << minimizer->Tolerance() << " , Up: " << minimizer->ErrorDef() 
  //      << " ==> precision: " << minimizer->Precision() << endl;
  minimizer->SetStrategy(0);
  minimizer->SetTolerance(10);
  minimizer->SetPrintLevel(-1);
  _status = minimizer->Minimize();
  _res = minimizer->X()[0];
  if( !_status ) _res = 999;
  delete minimizer;
  return _res;


}






