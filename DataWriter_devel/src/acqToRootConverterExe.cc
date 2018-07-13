#include "interface/xmlReader.hh"
#include "interface/geomManager.hh"
#include "interface/dataDecoder.hh"
#include "interface/tpcDataAnalyzer.hh"
#include "interface/utils.hh"


#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <TFile.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH2Poly.h>

#include <TRint.h>
#include <TTree.h>

using namespace std;

/*
TODO:    multigen/TPC geometry read from cmd line -> should it be switched ??
*/
void rootStyle(void);

int main( int nargc, char **argv ) {
  string xmlFile;
  string dataFile = "unknown";
  string electronicMap = "etc/geom/MapLayoutFile.txt";
  string geometryMap   = "etc/geom/keystonePadGeom.txt";

  int tpcfem=1;
  int multigenfem=0;
  int Nevents=-1;
  int interactive;
  int noxml=0;
  namespace po = boost::program_options;
  po::options_description config("Configuration");
  config.add_options()
    ("help,h" , "help")
    ("xmlFile" , po::value<string>(&xmlFile),"xmlFile" )
    ("elecMap" , po::value<string>(&electronicMap)->default_value("etc/geom/MapLayoutFile.txt"  ),"file with the electronic map layout" )
    ("geomMap" , po::value<string>(&geometryMap  )->default_value("etc/geom/keystonePadGeom.txt"),"file with the geometry map layout" )
    ("f,file"  , po::value<string>(&dataFile)->required(),"data file" )
    ("interactive,i","interactive mode" )
    ("noxml","force to run without existing xml file" )
    ("tpcfem,t" , po::value<int>(&tpcfem  )->default_value(1),"FEM number for TPC pad (default 1)")
    ("multigenfem,m" , po::value<int>(&multigenfem  )->default_value(0),"FEM number for multigen (default 0)")
    ("Nevents,n" , po::value<int>(&Nevents)->default_value(0),"Max number of events (default 0 to process all)");
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

   if(    vm.count("noxml")  ) noxml = 1;




  TRint *theApp   = 0;
  TCanvas *canvas = 0;
  if( interactive ) {
    theApp = new TRint("App", 0, 0);
    canvas = new TCanvas("cMonitoring","cMonitoring",800,600);
    rootStyle();
  }

  /////// get filename and path to work only on filename
  boost::filesystem::path datapath(dataFile);
  boost::filesystem::path datafileDirName  = datapath.parent_path();
  boost::filesystem::path datafileFileName = datapath.filename();


  ////// Decode xml file using input file name
  vector<string> tmpXml;
  boost::split( tmpXml, datafileFileName.string() , boost::is_any_of("."), boost::token_compress_on );

  for( unsigned itmp = 0 ; itmp < tmpXml.size(); itmp++ )
    if( boost::find_first(tmpXml[itmp], "RUN") ) {
      xmlFile = tmpXml[itmp];
      boost::replace_first(xmlFile,"RUN","CFG");
      xmlFile += ".xml";
    }
  xmlFile = datafileDirName.string() + "/" + xmlFile;
  tpcXmlConfig xmlConfig;
  if (noxml==0){
  cout << " Opening config xml file = " << xmlFile << endl;

  xmlConfig.load( xmlFile );
  cout << xmlConfig << endl;

  }
  ///// setup geometry
  femGeomManager geomManager;
  geomManager.setElectronicMap(electronicMap);
  //geomManager.setGeometryMap(  geometryMap  );


  ////// opening output file
  string outputRootFile = dataFile;
  boost::replace_first(outputRootFile,".acq",".root");
  TFile *fOut = new TFile( outputRootFile.c_str(),"recreate");
  if (noxml==0)xmlConfig  .dumpToRootFile(fOut);
  geomManager.dumpToRootFile(fOut);

  TTree *t = new TTree("padData","pad data");
  /*vector<vector<int> > listOfPadSamples;
  vector<int>          listOfPadChannels;
  vector<vector<int> > listOfMGSamples;
  vector<int>          listOfMGChannels;*/

  //Test 2/22/ short

  vector<vector<short> > listOfPadSamples;
  vector<short>          listOfPadChannels;
  vector<vector<short> > listOfMGSamples;
  vector<short>          listOfMGChannels;



  t->Branch("MGphysChannels",  &listOfMGChannels );
  t->Branch("MGADCvsTime"   ,  &listOfMGSamples );
  t->Branch("PadphysChannels", &listOfPadChannels );
  t->Branch("PadADCvsTime"   , &listOfPadSamples );

  ///// prepare data decoder
  tpcDataDecoder decoder(dataFile,tpcfem,multigenfem);
  tpcDataAnalyzer ana;
  ana.setGeometry(geomManager);

  int nprocessed=0;
  while( !decoder.endOfDataFile() && ( nprocessed<Nevents || Nevents<=0) ) {
    //
    nprocessed++;
    decoder.decodeNextEvent();
    vector<tpcPad> & pads = decoder.GetPadData();
    vector<MultigenStrip> & multigens = decoder.GetMGData();
    /*   listOfMGChannels.erase( listOfMGChannels.begin(), listOfMGChannels.end() );
    listOfPadChannels.erase( listOfPadChannels.begin(), listOfMGChannels.end() );
    listOfMGSamples.erase(  listOfMGSamples.begin(), listOfMGSamples.end() );
    listOfPadSamples.erase(  listOfPadSamples.begin(), listOfPadSamples.end() );*/

    listOfPadChannels.clear();
    listOfMGChannels.clear();
    listOfMGSamples.clear();
    listOfPadSamples.clear();

    for( unsigned ipad = 0; ipad < pads.size(); ipad++ ) {
      listOfPadChannels.push_back( pads[ipad].getPhysChannel() );
      //conversion to short //FIXME once interface are changed

      //    vector<int> tmp;
	 vector<short> tmp_short;
	  tmp_short=pads[ipad].getADCvsTime() ;
	  //	  for (unsigned i=0; i<tmp.size(); i++){
	  // tmp_short.push_back(tmp[i]);
	  // } //interface changed on april 2017. no conversion needed
	  listOfPadSamples.push_back( tmp_short);
	  //      listOfPadSamples.push_back(  pads[ipad].getADCvsTime() );
    }

 for( unsigned img = 0; img < multigens.size(); img++ ) {
      listOfMGChannels.push_back( multigens[img].getPhysChannel() );

      //   vector<int> tmp;
      vector<short>tmp_short;
      tmp_short=  multigens[img].getADCvsTime() ;
      //     for (unsigned i=0; i<tmp.size(); i++){
      //	tmp_short.push_back(tmp[i]);
      //      }
	 listOfMGSamples.push_back( tmp_short);
      //listOfMGSamples.push_back(  multigens[img].getADCvsTime() );
    }


    if ( !decoder.endOfDataFile() ) //  condition to avoid the last empty event
     t->Fill();


    //// if monitoring mode - plot max charge
    if( interactive ) {
  	canvas->cd();
	ana.fillPadsWithMaxCharge( pads );
	ana.drawEvt2D();
	canvas->Update();
    }
  }
  cout << " ----> Closing output file: " << fOut->GetName() << endl;
  cout << " ---->              Nevent: " << t->GetEntries() << endl;
  t->Write();
  fOut->Close();

  if( canvas ) delete canvas;
  return 1;
}






