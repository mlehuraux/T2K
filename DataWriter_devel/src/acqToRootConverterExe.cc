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

void rootStyle(void);

int main( int nargc, char **argv ) {
  string xmlFile;
  string dataFile = "unknown";
  string electronicMap = "etc/geom/MapLayoutFile.txt";
  string geometryMap   = "etc/geom/keystonePadGeom.txt";
  string pedestals_file_name = "";
  bool DoZeroSuppression = false;

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
    ("file,f"  , po::value<string>(&dataFile)->required(),"data file" )
    ("interactive,i","interactive mode" )
    ("noxml","force to run without existing xml file" )
    ("tpcfem,t" , po::value<int>(&tpcfem  )->default_value(1),"FEM number for TPC pad (default 1)")
    ("multigenfem,m" , po::value<int>(&multigenfem  )->default_value(0),"FEM number for multigen (default 0)")
    ("Nevents,n" , po::value<int>(&Nevents)->default_value(0),"Max number of events (default 0 to process all)")
    ("pedestals,p", po::value<string>(&pedestals_file_name)->default_value(""),"file with pedestal information" );
  ;


  po::variables_map vm;
  po::store(po::command_line_parser(nargc, argv).
	    options(config).run(), vm);
  po::notify(vm);

  if (pedestals_file_name != "")
    DoZeroSuppression = true;

  if( vm.count("help") ) {
    cout << config << endl;
    cout << "Usage: ./bin/xmlReaderExe [options]" << endl;
    return 1;
  }

  if(    vm.count("interactive")  ) interactive = 1;
  else                              interactive = 0;

  if(    vm.count("noxml")  ) noxml = 1;


  vector<float>          mean_v;
  vector<float>          sigma_v;
  vector<float>          zero_v;

  mean_v.resize(1728);
  sigma_v.resize(1728);
  zero_v.resize(1728);

  if (DoZeroSuppression) {
    ifstream ped_file(pedestals_file_name.c_str());

    if ( !ped_file ) {
      cerr << "ERROR acqToRootConverterExe.cc: Can't open pedestal file : " << pedestals_file_name << endl;
      return -1;
    }

    int ch;
    float mean, sigma, zero;
    while ( ped_file.good() && !ped_file.eof() ) {
      ped_file >> ch >> mean >> sigma >> zero;
      mean_v[ch] = mean;
      sigma_v[ch] = sigma;
      zero_v[ch] = zero;
    }

    ped_file.close();
  }


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
  geomManager.setGeometryMap(  geometryMap  );


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

      //    vector<int> tmp;
	   vector<short> tmp_short;
	   tmp_short=pads[ipad].getADCvsTime() ;
	   //	  for (unsigned i=0; i<tmp.size(); i++){
	   // tmp_short.push_back(tmp[i]);
	   // } //interface changed on april 2017. no conversion needed
	   listOfPadSamples.push_back( tmp_short);
	   //      listOfPadSamples.push_back(  pads[ipad].getADCvsTime() );
    }

    // INFO probably not the best place but avoids pedestal file opening in the decoder
    // Apply zero suppression
    if (DoZeroSuppression) {
      for (uint ipad = 0; ipad < pads.size(); ++ipad) {
      // DEFINE ZERO LEVEL HERE
      // could be defined with precomputed zero or calc mean+N*sigma here;
      // TODO add check mean != 0 and sigma != 0
        Int_t zero_level = zero_v[listOfPadChannels[ipad]];
        //zero_level = int(mean_v[listOfPadChannels[ipad]] + 4. * sigma_v[listOfPadChannels[ipad]] + 0.5);
        for (uint itime = 0; itime < listOfPadSamples[ipad].size(); ++itime)
          listOfPadSamples[ipad][itime] -= zero_level;
      }
    }
    // zero suppression done


    for( unsigned img = 0; img < multigens.size(); img++ ) {
      listOfMGChannels.push_back( multigens[img].getPhysChannel() );
      listOfMGSamples.push_back(  multigens[img].getADCvsTime() );
    }


    if ( !decoder.endOfDataFile() ) //  condition to avoid the last empty event
     t->Fill();


    //// if monitoring mode - plot max charge
    if (interactive) {
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






