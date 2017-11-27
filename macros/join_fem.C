#define THIS_NAME join_fem
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "common_header.h"

const Float_t tpcPanelWidth   = 233.94;
const Float_t tpcPanelHeight  = 172.83;

void join_fem() {

  // LOOP OVER FILES
  for (Int_t fileID = 5148; fileID < 5162; ++fileID) {
    std::stringstream stream;
    stream << fileID;
    std::string strRUN = stream.str();

    TFile* outFile  = new TFile("data_root/multifem.root", "RECREATE");

    TTree* geom     = new TTree("geom", "geom");
    TTree* pad      = new TTree("pad", "pad");

    std::vector<std::vector<short> > *pad_charge;
    pad->Branch("PadCharge",    &pad_charge);

    // LOOP OVER FEMs
    for (Int_t femID = 0; femID < 7; ++femID) {
      stream.str("");
      stream << femID;
      std::string strFEM = stream.str();

      std::string file_name = "data_root/RUN_0" + strRUN + "_fem" + strFEM + ".root";
      TFile* inFile  = new TFile(file_name.c_str(), "READ");

      // READ BEAM CONFIG
      TTree* tconfig= (TTree*) inFile->Get("beam_config");
      Float_t driftZ;
      tconfig->SetBranchAddress("DriftZ", &driftZ);
      tconfig->GetEntry(0);

      // READ GEOMETRY
      vector<int> *iPad(0);
      vector<int> *jPad(0);
      TTree * tgeom= (TTree*) inFile->Get("femGeomTree");
      int Jmin, Imin, Jmax, Imax;
      vector<vector<double> > *x0Pad(0), *x1Pad(0), *x2Pad(0), *x3Pad(0);
      vector<vector<double> > *y0Pad(0), *y1Pad(0), *y2Pad(0), *y3Pad(0);
      tgeom->SetBranchAddress("jPad", &jPad );
      tgeom->SetBranchAddress("iPad", &iPad );
      tgeom->SetBranchAddress("jPadMin", &Jmin );
      tgeom->SetBranchAddress("iPadMin", &Imin );
      tgeom->SetBranchAddress("jPadMax", &Jmax );
      tgeom->SetBranchAddress("iPadMax", &Imax );
      tgeom->SetBranchAddress("x0Pad", &x0Pad );
      tgeom->SetBranchAddress("y0Pad", &y0Pad );
      tgeom->SetBranchAddress("x1Pad", &x0Pad );
      tgeom->SetBranchAddress("y1Pad", &y0Pad );
      tgeom->SetBranchAddress("x2Pad", &x0Pad );
      tgeom->SetBranchAddress("y2Pad", &y0Pad );
      tgeom->SetBranchAddress("x3Pad", &x0Pad );
      tgeom->SetBranchAddress("y3Pad", &y0Pad );

      tgeom->GetEntry(0); // put into memory geometry info

      // READ PAD DATA
      TTree *t = (TTree*) f->Get("padData");
      cout <<t->GetEntries() << " evts in file " << endl;

      vector<short>          *listOfChannels(0);
      vector<vector<short> > *listOfSamples(0);

      t->SetBranchAddress("PadphysChannels", &listOfChannels );
      t->SetBranchAddress("PadADCvsTime"   , &listOfSamples );

    }
  }
}

void GetCoordinates(Float_t** x, Float_t** y, Int_t iFem, Float_t** xOut, Float_t** yOut) {
  const double scale  = 1.;
  const double xscale = scale * 1./tpcPanelWidth;
  const double yscale = scale * 1./tpcPanelHeight;

  for (Int_t ic = 0; ic < 4; ++ic) {
    if (iFem==2) {
      xOut[ic] = x*cos(0.149488369/1.33) - y*sin(0.149488369/1.33) /*+ 0.5*/;
      yOut[ic] = y*cos(0.149488369/1.33) + x*sin(0.149488369/1.33) /*+ 0.5*/ - 1520*yscale;
    }
    if (iFem==3) {
      xOut[ic] = x*cos(0.003055244/1.33) - y*sin(0.003055244/1.33) /*+ 0.5*/;
      yOut[ic] = y*cos(0.003055244/1.33) + x*sin(0.003055244/1.33) /*+ 0.5*/ - 1520*yscale;
    }
    if (iFem==4) {
      xOut[ic] = x*cos(-0.143203347/1.33) - y*sin(-0.143203347/1.33) /*+ 0.5*/;
      yOut[ic] = y*cos(-0.143203347/1.33) + x*sin(-0.143203347/1.33) /*+ 0.5*/ - 1520*yscale;
    }
    if (iFem==0) {
      xOut[ic] = x*cos(0.088401845/1.33) - y*sin(0.088401845/1.33) /*+ 0.5*/;
      yOut[ic] = y*cos(0.088401845/1.33) + x*sin(0.088401845/1.33) /*+ 0.5*/ - 1520*yscale+175*yscale;
    }
    if (iFem==1) {
      xOut[ic] = x*cos(-0.05803128/1.33) - y*sin(-0.05803128/1.33) /*+ 0.5*/;
      yOut[ic] = y*cos(-0.05803128/1.33) + x*sin(-0.05803128/1.33) /*+ 0.5*/ - 1520*yscale+175*yscale;
    }
    if (iFem==5) {
      xOut[ic] = x*cos(0.05349526/1.33) - y*sin(0.05349526/1.33) /*+ 0.5*/;
      yOut[ic] = y*cos(0.05349526/1.33) + x*sin(0.05349526/1.33) /*+ 0.5*/ - 1520*yscale - 175*yscale;
    }
    if (iFem==6) {
      xOut[ic] = x*cos(-0.092937865/1.33) - y*sin(-0.092937865/1.33) /*+ 0.5*/;
      yOut[ic] = y*cos(-0.092937865/1.33) + x*sin(-0.092937865/1.33) /*+ 0.5*/ - 1520*yscale - 175*yscale;
    }

    xOut[ic] *= tpcPanelWidth;
    yOut[ic] *= tpcPanelHeight;
  }
}