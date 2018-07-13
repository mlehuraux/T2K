#include "interface/xmlReader.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

void tpcXmlConfig::load(const std::string &filename) {
    // Create an empty property tree object
    using boost::property_tree::ptree;
    ptree pt;

    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    read_xml(filename, pt);

    trigger = pt.get<std::string>("Daq.Run.Trigger.mode");    

    std::stringstream ssHexConv;
    BOOST_FOREACH(ptree::value_type &dcc, pt.get_child("Daq.DCC") ) 
      if( dcc.first == "FEM" &&  dcc.second.get<string>("<xmlattr>.id") == "*" ) {
	/// only read generic FEM config
	acqType  = dcc.second.get<string>("acqType");
	stringstream nSamplesHex   (dcc.second.get<string>("Module.nTimeBuckets"))    ; nSamplesHex    >> std::hex >> nSamples;
	stringstream divScaClockHex(dcc.second.get<string>("Module.divideScaClockBy")); divScaClockHex >> std::hex >> divideScaClock;
	sampleTimeWidth = divideScaClock * 10; /// sampleTimeWIdth = 1 / ( 100MHz / divideScaClock ) * 1e9 (nanosecond)
	
	/// pedestal 
	pedActive = dcc.second.get<bool>( "Module.pedestalActive");
	isPedCalc = dcc.second.get<bool>( "Module.isCalculatedPedestal");
	pedMean   = dcc.second.get<float>("Module.pedestalMean" ); 
	

	BOOST_FOREACH(ptree::value_type &fem, dcc.second)
	  if( fem.first == "FEC" &&  fem.second.get<string>("<xmlattr>.id") == "*" ) {
	    /// only read generic FEC config(i.e. ID = "*" )

	    BOOST_FOREACH(ptree::value_type &fec, fem.second ) 
	      if( fec.first == "ASIC" && fec.second.get<string>("<xmlattr>.id") == "*" ) {
	    /// only read generic ASIC config(i.e. ID = "*" )	    
		peakingTime = fec.second.get<float>("peakingTime");
	      }
	  }
      }

    /// remove potential all newline in string variables
    boost::replace_all(trigger, "\n", "");
    boost::replace_all(acqType, "\n", "");
}

std::ostream& operator<<(std::ostream& out, const tpcXmlConfig & conf) {

  out << "================  tpc config ============== "       << endl;
  out << " RUN trigger type     : " << conf.trigger           << endl;
  out << " FEM acquisition type : " << conf.acqType           << endl;
  out << " FEM # of samples     : " << conf.nSamples          << endl;
  out << " FEM time sample (ns) : " << conf.sampleTimeWidth   << endl;  
  out << " FEM Zero suppression : " << conf.pedActive         << endl;  
  if( conf.pedActive ) {
    out << " FEM mean pedestal (if ZS) : " << conf.pedMean    << endl;  
    out << " FEM is pedestal calculated: " << conf.isPedCalc  << endl;  
  }
  out << " ASIC peaking TIME    : " << conf.peakingTime       << endl;

  return out ;
}


#include <TFile.h>
#include <TTree.h>

void tpcXmlConfig::dumpToRootFile(TFile *f, const std::string &treeName ) {
  if( f == 0 || !f->IsOpen() || f->IsZombie() ) {
    cerr << " ERROR[ tpcXmlConfig::dumpToRootFile ] output root file is not opened properly... " << endl;
    return;
  }
  
  f->cd();
  TTree *t = new TTree(treeName.c_str(),"TPC run configuration" );
  t->Branch("trigger","std::string", &trigger );
  t->Branch("acqType","std::string", &acqType );
  t->Branch("nSamples",&nSamples,"nSamples/I");
  t->Branch("sampleTimeWidth",&sampleTimeWidth,"sampleTimeWidth/F");
  t->Branch("pedActive"  , &pedActive,"pedActive/B");
  t->Branch("isPedCalc"  , &isPedCalc,"isPedCalc/B");
  t->Branch("pedMean"    , &pedMean  ,"pedMean/F");
  t->Branch("peakingTime", &peakingTime,"peakingTime/F"  );
  
  t->Fill();
  t->Write(treeName.c_str(),TObject::kOverwrite);
 
}






