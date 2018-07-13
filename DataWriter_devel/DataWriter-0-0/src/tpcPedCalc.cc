#include "interface/geomManager.hh"
#include "interface/tpcPedCalc.hh"
#include "interface/tpcPadDataFormats.hh"

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>

#include <cmath>
#include <iostream>

using namespace std;

void tpcPedestal::calcPedestals( std::string inputFile, bool drawPed ) {
  
  femGeomManager geo;
  geo.initFromRootFile( inputFile );
  _pedestalsMean1D.resize( geo.nPad(), 0 );
  _pedestalsRMS1D .resize( geo.nPad(), 0 );

  _pedestalsMean2D.resize( geo.nRow(), vector<double>(geo.nCol(),0) );
  _pedestalsRMS2D .resize( geo.nRow(), vector<double>(geo.nCol(),0) );

  TFile *fIn = new TFile( inputFile.c_str(),"read");

  TTree *tree = (TTree*) fIn->Get("padData");
  vector<vector<int> > *listOfSamples(0);
  vector<int>          *listOfChannels(0);
  tree->SetBranchAddress("physChannels", &listOfChannels );
  tree->SetBranchAddress("ADCvsTime"   , &listOfSamples );


  
  vector<double> mu  ; mu  .resize(geo.nPad(),0);
  vector<double> rms ; rms .resize(geo.nPad(),0);
  vector<int>    nEvt; nEvt.resize(geo.nPad(),0);

  for( int ievt = 0 ; ievt < tree->GetEntries(); ievt++ ) {
    tree->GetEntry(ievt);
    vector<tpcPad> pads;

    for( unsigned ipad = 0; ipad < listOfChannels->size(); ipad++ ) {
      tpcPad p;
      pads.push_back(p);
      pads[pads.size()-1].setPad( (*listOfChannels)[ipad],(*listOfSamples)[ipad] );
    }

    
    for( unsigned ipad = 0; ipad < pads.size(); ipad++ ) {
      int iCh = pads[ipad].getPhysChannel();
      if( iCh < 0 ) continue;
      double mean = pads[ipad].getMean();
      if( mean > 0.1 ) {
	nEvt[iCh]++;
	mu[iCh]  += mean;
	rms[iCh] += pads[ipad].getRMS();    
      }
    }
  }

  for( unsigned ich = 0 ; ich < mu.size(); ich++ ) {
    _pedestalsMean1D[ich] = mu[ich] / nEvt[ich];
    _pedestalsRMS1D[ich]  = rms[ich]/ nEvt[ich];
    _pedestalsMean2D[geo.iPad(ich)][geo.jPad(ich)] = _pedestalsMean1D[ich];
    _pedestalsRMS2D[ geo.iPad(ich)][geo.jPad(ich)] = _pedestalsRMS1D[ ich];
  }

  fIn->Close();

  femGeomViewer pedMeanViewer("hPedestalMean",geo);
  femGeomViewer pedRMSViewer( "hPedestalRMS" ,geo);
  
  if( drawPed ) {
    for( unsigned ich = 0 ; ich < _pedestalsMean1D.size(); ich++ ) {
      int ipad = geo.iPad(ich);
      int jpad = geo.jPad(ich);
      pedMeanViewer.setBinContent(ipad,jpad,0,_pedestalsMean1D[ich] );
      pedRMSViewer.setBinContent( ipad,jpad,0,_pedestalsRMS1D[ ich] );
    }
    pedMeanViewer.setMinMax(0,500);
    pedRMSViewer .setMinMax(2.5,7.5);
    
    TCanvas *cMeanCanvas = new TCanvas("cPedestalMean","pedestal mean",900,800);
    TCanvas *cRMSCanvas  = new TCanvas("cPedestalRMS" ,"pedestal RMS" ,900,800);
    
    cMeanCanvas->cd(); pedMeanViewer.drawCopyXY("colz"); cMeanCanvas->Update();
    cRMSCanvas ->cd(); pedRMSViewer .drawCopyXY("colz"); cRMSCanvas ->Update();    

    delete cMeanCanvas;
    delete cRMSCanvas;
  }
}

void tpcPedestal::dummyPedestals( std::string inputFile, double ped, double rms ) {
  
  femGeomManager geo;
  geo.initFromRootFile( inputFile );
  _pedestalsMean1D.resize( geo.nPad(), 0 );
  _pedestalsRMS1D .resize( geo.nPad(), 0 );

  _pedestalsMean2D.resize( geo.nRow(), vector<double>(geo.nCol(),0) );
  _pedestalsRMS2D .resize( geo.nRow(), vector<double>(geo.nCol(),0) );

   for( unsigned ich = 0 ; ich < _pedestalsMean1D.size(); ich++ ) {
    _pedestalsMean1D[ich] = ped;
    _pedestalsRMS1D[ich]  = rms;
    _pedestalsMean2D[geo.iPad(ich)][geo.jPad(ich)] = _pedestalsMean1D[ich];
    _pedestalsRMS2D[ geo.iPad(ich)][geo.jPad(ich)] = _pedestalsRMS1D[ ich];
   }
}
