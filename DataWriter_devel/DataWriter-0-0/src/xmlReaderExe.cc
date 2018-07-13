#include "interface/xmlReader.hh"


#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include <TFile.h>
using namespace std;


int main( int nargc, char **argv ) {

  string xmlFile;
  namespace po = boost::program_options;
  po::options_description config("Configuration");
  config.add_options()
    ("help,h" , "help")
    ("file,f" , po::value<string>(&xmlFile),"xmlFile" )
    //    ("interactive,i", po::value<bool>(&rootInteractive)->default_value(1),"root interactive" )
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

  tpcXmlConfig xmlConfig;
  xmlConfig.load( xmlFile );
  cout << xmlConfig << endl;

  TFile *f = new TFile( "test.root","recreate");
  xmlConfig.dumpToRootFile(f);
  f->Close();
}
