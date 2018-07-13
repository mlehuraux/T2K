#include "interface/geomManager.hh"
#include "interface/tpcDataAnalyzer.hh"
#include "interface/tpcPadDataFormats.hh"
#include "interface/utils.hh"
#include "interface/tpcPedCalc.hh"


#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include <TFile.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH2Poly.h>
#include <TRint.h>
#include <TTree.h>
#include <TH1.h>
#include <TChain.h>

#include <memory>

using namespace std;



int main( int nargc, char **argv ) {
  string dataFile = "unknown";
  string pedestalFile = "undefined";
  int interactive;
  double pedMean = 200; 
  int    pedRMS  = 5;
  bool doFitReso = false;
  namespace po = boost::program_options;
  po::options_description config("Configuration");
  config.add_options()
    ("help,h" , "help")
    ("pedFile"    , po::value<string>(&pedestalFile)->default_value("undefined"),"file with the pedestals" )
    ("pedMeanDef" , po::value<double>(&pedMean)->default_value(250),"pedestals default" )
    ("pedRMSDef " , po::value<int>   (&pedRMS) ->default_value(5)  ,"pedestals rms default" )
    ("f,filelist" , po::value<string>(&dataFile)->required(),"data filelist" )
    ("doFitReso"  , po::value<bool>(&doFitReso)->default_value(false),"do resolution fit" )    
    ("interactive,i","interactive mode" )
    ;
  
  po::variables_map vm;
  po::store(po::command_line_parser(nargc, argv).
	    options(config).run(), vm);
  po::notify(vm);
  
  if( vm.count("help") ) {
    cout << config << endl;
    cout << "Usage: ./bin/xmlReaderExe [options]" << endl;
    return 1;
  }
  
   if(    vm.count("interactive")  ) interactive = 1;
   else                              interactive = 0;
   
   
   auto_ptr<TRint> theApp(0);
   auto_ptr<TCanvas> canvas(0);
   if( interactive ) {
     theApp.reset( new TRint("App", 0, 0) );
     canvas.reset( new TCanvas("cMonitoring","cMonitoring",1000,900) );
     rootStyle();
   }
   
  string tmpFileForGeom = firstFileInList(dataFile);
  
  tpcPedestal pedestals;
  if( pedestalFile != "undefined" ) pedestals.calcPedestals(  pedestalFile, true);
  else                              pedestals.dummyPedestals( tmpFileForGeom, pedMean, pedRMS );
  
  ///   directly init geometry from the one stored in the root file
  femGeomManager geo;
  geo.initFromRootFile( tmpFileForGeom );

  
  TChain *tree = addInputFiles(dataFile,"padData");
  
  vector<vector<int> > *listOfSamples(0);
  vector<int>          *listOfChannels(0);
  tree->SetBranchAddress("physChannels", &listOfChannels );
  tree->SetBranchAddress("ADCvsTime"   , &listOfSamples );
  
  
  tpcDataAnalyzer ana;
  ana.setGeometry(geo);
  ana.setPedestalsMean( pedestals.getPedMean2D() );
  ana.setPedestalsRMS(  pedestals.getPedRMS2D()  );
  
  auto_ptr<TH1F> hChi2Max(     new TH1F("hChi2Max","chi2 plot", 64, -.5, 3.5) );
  auto_ptr<TH1F> hChi2Cluster( new TH1F("hChi2Cluster","chi2 plot", 64, -.5, 3.5) );
  hChi2Max->GetXaxis()->SetTitle( "#sigma [mm]" );
  hChi2Cluster->GetXaxis()->SetTitle( "#sigma [mm]" );
  hChi2Cluster->SetLineColor(kRed);
  
  
  cout << "  --- analyzing nEvents: " << tree->GetEntries() << endl;
  int nEvtSelected = 0;
  for( int ievt = 0 ; ievt < tree->GetEntries(); ievt++ ) {
    tree->GetEntry(ievt);
    vector<tpcPad> pads;
    
    for( unsigned ipad = 0; ipad < listOfChannels->size(); ipad++ ) {
      tpcPad p;
      pads.push_back(p);
      pads[pads.size()-1].setPad( (*listOfChannels)[ipad],(*listOfSamples)[ipad] );
    }
    
    // //// if monitoring mode - plot max charge
    ana.fillPadsWithMaxCharge( pads );
    
    double chi2(-1);
    int    ndof(1);
    if( ana.evtSelector(5,5) ){
      if( interactive ) {
	canvas->cd();	
	ana.drawEvt3D();
      }
      if( doFitReso &&  ana.evtSelector(5,10)) {
	double chi2Max(-1),chi2Clu(-1);
	double resMax     = ana.fitter(        chi2,ndof); chi2Max = chi2 / ndof;
	double resCluster = ana.fitterCluster( chi2,ndof); chi2Clu = chi2 / ndof;
	if( chi2Max < 3. ) {
	  hChi2Max    ->Fill( resMax ); 
	  hChi2Cluster->Fill( resCluster ); 
	  nEvtSelected++;
	}
      }
      if( interactive ) { 
	canvas->Update();	
	//getchar();
      }
    }    
  }
  if( doFitReso ) {
    auto_ptr<TCanvas> cChi2( new TCanvas("cChi2","chi2",800,700) );
    cChi2->cd();
    hChi2Max->DrawCopy();
    hChi2Cluster->DrawCopy("same");
    cChi2->Update();
    cout << " Number of events selected: " << nEvtSelected << endl;
    cChi2->Print("MicroTPC_resolution.pdf");
  }

  if( interactive )   theApp->Run();
  return 1;
}













