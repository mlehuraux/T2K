#define THIS_NAME join_fem_ana
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "common_header.h"

const Float_t tpcPanelWidth   = 233.94;
const Float_t tpcPanelHeight  = 172.83;
const Float_t alpha           = 0.7;

vector< pair<Int_t, Int_t> > ScanPad(const vector<vector<short> >, Int_t , Int_t , Int_t , Int_t& , Int_t& );

void join_fem_ana() {

  // define output
  TGraph* graph = new TGraph();
  int point = 0;
  TH1F* ClusterNormCharge = new TH1F("cluster_norm_charge","Truncated mean energy deposit",150,0,4000);

  // Initialize the geometry
  // Similar for all the files actuallн
  std::string file_name0 = "data_root/RUN_05148_multi.root";
  TFile* inFile0  = new TFile(file_name0.c_str(), "READ");

  vector<int> *iPad(0);
  vector<int> *jPad(0);
  TTree * tgeom= (TTree*) inFile0->Get("femGeomTree");
  int Jmin, Imin, Jmax, Imax;
  // vectors are filled as x0_v[row][col] = x0
  // so calling of the vectors should be x0_v[i][j]
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

  // LOOP OVER FILES
  for (Int_t fileID = 5148; fileID < 5161; ++fileID) {
    std::stringstream stream;
    stream << fileID;
    std::string strRUN = stream.str();

    std::string file_name = "data_root/RUN_0" + strRUN + "_multi.root";
    cout << endl;
    cout << "Rinnung analysis over " << file_name << endl;
    TFile* inFile  = new TFile(file_name.c_str(), "READ");

    // READ BEAM CONFIG
    TTree* tconfig= (TTree*) inFile->Get("beam_config");
    Float_t driftZ;
    tconfig->SetBranchAddress("DriftZ", &driftZ);
    tconfig->GetEntry(0);

    // READ PAD DATA
    TTree *t = (TTree*) inFile->Get("pad");
    vector<vector<short> >  *pad_charge[7];
    for (Int_t it = 0; it < 7; ++it)
      pad_charge[it] = new vector<vector<short> >(0);

    t->SetBranchAddress("pad0",     &pad_charge[0]);
    t->SetBranchAddress("pad1",     &pad_charge[1]);
    t->SetBranchAddress("pad2",     &pad_charge[2]);
    t->SetBranchAddress("pad3",     &pad_charge[3]);
    t->SetBranchAddress("pad4",     &pad_charge[4]);
    t->SetBranchAddress("pad5",     &pad_charge[5]);
    t->SetBranchAddress("pad6",     &pad_charge[6]);

    //************************************************************
    //************************************************************
    //****************** LOOP OVER EVENTS ************************
    //************************************************************
    //************************************************************

    int Nevents=t->GetEntries();
    cout << "[          ] Nev="<<Nevents<<"\r[";
    for (int ievt=0; ievt < Nevents ; ievt++){
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;
      t->GetEntry(ievt);

      vector< pair<Int_t, Int_t> > cluster_charge;
      cluster_charge.clear();

      Int_t TotalLength = 0;

      Int_t Nrow = 0;
      Int_t MaxSep = 0;

      // Scan over pads  FEM 5
      vector< pair<Int_t, Int_t> > temp = ScanPad(*pad_charge[5], 5, Imax, Jmax, Nrow, MaxSep);
      cluster_charge.insert(cluster_charge.end(), temp.begin(), temp.end());
      //cout << cluster_charge.size() << endl;

      // cuts on FEM 5
      if (Nrow < 20)
        continue;

      if (MaxSep > 2)
        continue;

      // Scan over pads  FEM 3
      temp = ScanPad(*pad_charge[3], 3, Imax, Jmax, Nrow, MaxSep);
      cluster_charge.insert(cluster_charge.end(), temp.begin(), temp.end());

      // cuts on FEM 3
      if (Nrow < 20)
        continue;


      if (MaxSep > 2)
        continue;

      // Scan over pads  FEM 3
      vector< pair<Int_t, Int_t> > temp2 = ScanPad(*pad_charge[0], 0, Imax, Jmax, Nrow, MaxSep);
      cluster_charge.insert(cluster_charge.end(), temp2.begin(), temp2.end());

      // cuts on FEM 3
      if (Nrow < 20)
        continue;


      if (MaxSep > 2)
        continue;


      sort(cluster_charge.begin(), cluster_charge.end());
      Float_t norm_cluster = 0.;
      Int_t i_max = round(alpha * cluster_charge.size());
      for (int i = 0; i < std::min(i_max, int(cluster_charge.size())); ++i)
        norm_cluster += cluster_charge[i].first;

      norm_cluster *= 1 / (alpha * cluster_charge.size());

      ClusterNormCharge->Fill(norm_cluster);
    } // end event loop
  } // end file loop

  TCanvas *c1 = new TCanvas("c1","evadc",900,700);

  gStyle->SetOptStat("RMne");
  gStyle->SetOptFit(0111);
  ClusterNormCharge->Fit("gaus");
  ClusterNormCharge->Draw();
  c1->Print("figure/TruncEnergy_multi.png");
}

vector< pair<Int_t, Int_t> > ScanPad(const vector<vector<short> > pad, Int_t FEM, Int_t Imax, Int_t Jmax, Int_t& Nrow, Int_t& MaxSeparation) {

  MaxSeparation = 0;
  Nrow = 0;

  vector< pair<Int_t, Int_t> > cluster_charge;
  vector<int> listofRow;
  listofRow.resize(24, 0);

  for (int i = 0; i < pad.size(); ++i) {
    int prev = 1E6;
    int MaxSepRow = -1;
    int PadPerCl = 0;
    int ChargeCl = 0;
    for (int j = 0; j < pad[i].size(); ++j) {

      int adcmax = pad[i][j];
      //cout << i << "    " << j << "    " << adcmax << endl;
      if (adcmax < 0) {
        cout << "ALARM!" << endl;
        exit(1);
      }

      ChargeCl += adcmax;

      if (adcmax > 0) {
        if (j - prev - 1 > MaxSepRow)
          MaxSepRow = j - prev - 1;
        prev = j;
        ++PadPerCl;
      }
    }
    if (MaxSepRow > MaxSeparation)
      MaxSeparation = MaxSepRow;
    if (ChargeCl>0) {

      Int_t row_shift = 0;
      if (FEM < 5) row_shift += 24;
      if (FEM < 2) row_shift += 24;

      cluster_charge.push_back(make_pair(ChargeCl, i));
      listofRow[i] = 1;
    }
  }  // end of PAD scan

  for (int i = 0; i <= Imax; i++)
    Nrow+=listofRow[i];

  return cluster_charge;
}

void GetCoordinates(Double_t x[4], Double_t y[4], Int_t iFem, Double_t xOut[4], Double_t yOut[4]) {
  const double scale  = 1.;
  const double xscale = scale * 1./tpcPanelWidth;
  const double yscale = scale * 1./tpcPanelHeight;

  for (Int_t ic = 0; ic < 4; ++ic) {
    if (iFem==2) {
      xOut[ic] = x[ic]*cos(0.149488369/1.33) - y[ic]*sin(0.149488369/1.33);
      yOut[ic] = y[ic]*cos(0.149488369/1.33) + x[ic]*sin(0.149488369/1.33) - 1520*yscale;
    }
    if (iFem==3) {
      xOut[ic] = x[ic]*cos(0.003055244/1.33) - y[ic]*sin(0.003055244/1.33);
      yOut[ic] = y[ic]*cos(0.003055244/1.33) + x[ic]*sin(0.003055244/1.33) - 1520*yscale;
    }
    if (iFem==4) {
      xOut[ic] = x[ic]*cos(-0.143203347/1.33) - y[ic]*sin(-0.143203347/1.33);
      yOut[ic] = y[ic]*cos(-0.143203347/1.33) + x[ic]*sin(-0.143203347/1.33) - 1520*yscale;
    }
    if (iFem==0) {
      xOut[ic] = x[ic]*cos(0.088401845/1.33) - y[ic]*sin(0.088401845/1.33);
      yOut[ic] = y[ic]*cos(0.088401845/1.33) + x[ic]*sin(0.088401845/1.33) - 1520*yscale+175*yscale;
    }
    if (iFem==1) {
      xOut[ic] = x[ic]*cos(-0.05803128/1.33) - y[ic]*sin(-0.05803128/1.33);
      yOut[ic] = y[ic]*cos(-0.05803128/1.33) + x[ic]*sin(-0.05803128/1.33) - 1520*yscale+175*yscale;
    }
    if (iFem==5) {
      xOut[ic] = x[ic]*cos(0.05349526/1.33) - y[ic]*sin(0.05349526/1.33);
      yOut[ic] = y[ic]*cos(0.05349526/1.33) + x[ic]*sin(0.05349526/1.33) - 1520*yscale - 175*yscale;
    }
    if (iFem==6) {
      xOut[ic] = x[ic]*cos(-0.092937865/1.33) - y[ic]*sin(-0.092937865/1.33);
      yOut[ic] = y[ic]*cos(-0.092937865/1.33) + x[ic]*sin(-0.092937865/1.33) - 1520*yscale - 175*yscale;
    }

    xOut[ic] *= tpcPanelWidth;
    yOut[ic] *= tpcPanelHeight;
  }
}