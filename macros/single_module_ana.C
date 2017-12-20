#define THIS_NAME join_fem_ana
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "common_header.h"

const Float_t tpcPanelWidth   = 233.94;
const Float_t tpcPanelHeight  = 172.83;
const Float_t alpha           = 0.7;

vector< pair<Int_t, Int_t> > ScanPad(const vector<vector<short> >, Int_t , Int_t , Int_t , Int_t& , Int_t&, bool&, TH2F& );
void GetCoordinates(Double_t* , Double_t* , Int_t , Double_t* , Double_t* );

void join_fem_ana() {
  bool DrawEnergy = false;
  bool DrawDrift  = false;
  Int_t run_start = 4098;
  Int_t run_end   = 4109;

  bool calibration = false;

  Int_t moduleID = 3;

  if ((run_start < 5162 && DrawEnergy) || (run_start > 5161 && DrawDrift)) {
    cout << "WRONG RUN!" << endl;
    exit(1);
  }

  // define output
  //TGraph* graph = new TGraph();
  //int point = 0;
  TH1F* ClusterNormCharge = new TH1F("cluster_norm_charge" ,"Truncated mean energy deposit",200,0,1300);

  vector< vector<pair<Int_t, Int_t> > > EventClusters;

  TH2F* PadDisplay = new TH2F("PadDisplay" ,"I vs J of ADC_{max}",72,-0.5,71+0.5,24,0-0.5,23+0.5);

  Int_t N_events_raw       = 0;
  Int_t N_events_long      = 0;
  Int_t N_events_one_track = 0;

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
  vector<vector<double> > *x0Pad(0);//, *x1Pad(0), *x2Pad(0), *x3Pad(0);
  vector<vector<double> > *y0Pad(0);//, *y1Pad(0), *y2Pad(0), *y3Pad(0);
  tgeom->SetBranchAddress("jPad", &jPad );
  tgeom->SetBranchAddress("iPad", &iPad );
  tgeom->SetBranchAddress("jPadMin", &Jmin );
  tgeom->SetBranchAddress("iPadMin", &Imin );
  tgeom->SetBranchAddress("jPadMax", &Jmax );
  tgeom->SetBranchAddress("iPadMax", &Imax );
  tgeom->SetBranchAddress("x0Pad", &x0Pad );
  tgeom->SetBranchAddress("y0Pad", &y0Pad );
  /*tgeom->SetBranchAddress("x1Pad", &x0Pad );
  tgeom->SetBranchAddress("y1Pad", &y0Pad );
  tgeom->SetBranchAddress("x2Pad", &x0Pad );
  tgeom->SetBranchAddress("y2Pad", &y0Pad );
  tgeom->SetBranchAddress("x3Pad", &x0Pad );
  tgeom->SetBranchAddress("y3Pad", &y0Pad );*/

  tgeom->GetEntry(0); // put into memory geometry info
  // Initialize row by row and FEM by FEM callibration
  vector<float> row_calib;
  TString file = "data/fem3_no_field.dat";
  ifstream CalibFile(file);
  if ( !CalibFile ) {
    cerr << "ERROR: Can't open file for row calibration: " << file << endl;
    exit(1);
  }

  // Global FEM norm
  char   varName[20];
  float FEMnorm;
    CalibFile >> varName >> FEMnorm;

  while ( CalibFile.good() && !CalibFile.eof() ) {
    int row;
    float charge, avg, norm;
    CalibFile >> row >> charge >> avg >> norm;

    row_calib.push_back(norm);
  }

  TCanvas *c1 = new TCanvas("c1","evadc",900,700);

  //************************************************************
  //************************************************************
  //****************** LOOP OVER FILEWS ************************
  //************************************************************
  //************************************************************
  string postfix = "";
  for (Int_t fileID = run_start; fileID <= run_end; ++fileID) {

    if (fileID == 5082 || fileID == 4051) {
      cout << "skipping empty run  "  << fileID << endl;
      continue;
    }
    std::stringstream stream;
    stream << fileID;
    std::string strRUN = stream.str();

    std::string file_name = "data_root/RUN_0" + strRUN + "_multi.root";
    cout << endl;
    cout << "Rinnung analysis over " << file_name << endl;
    TFile* inFile  = new TFile(file_name.c_str(), "READ");

    // READ BEAM CONFIG
    TTree* tconfig= (TTree*) inFile->Get("beam_config");
    Float_t driftZ, peacking, energy, Efield;
    tconfig->SetBranchAddress("DriftZ", &driftZ);
    tconfig->SetBranchAddress("peacking", &peacking);
    tconfig->SetBranchAddress("energy", &energy);
    tconfig->SetBranchAddress("Efield", &Efield);
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
    Int_t pseudo_module_length = 0;
    vector< pair<Int_t, Int_t > > pseudo_track;
    Float_t norm_cluster = 0.;

    int Nevents=t->GetEntries();
    cout << "[          ] Nev="<<Nevents<<"\r[";
    for (int ievt=0; ievt < Nevents ; ievt++){
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;
      t->GetEntry(ievt);

      Int_t Nrow[7];
      Int_t MaxSep[7];
      bool touchBorder[7];

      // Scan over pads

      vector< pair<Int_t, Int_t > > temp;
      temp.clear();
      temp = ScanPad(*pad_charge[moduleID], moduleID, Imax, Jmax, Nrow[moduleID], MaxSep[moduleID], touchBorder[moduleID], *PadDisplay);
      ++N_events_raw;

      for (vector< pair<Int_t, Int_t > >::iterator it = temp.begin(); it != temp.end(); ++it){
        Int_t row = (*it).second;
        if (row == 0 ||  row == Imax) {
          temp.erase(it);
          // VERY IMPORTANT!!!!
          --it;
          continue;
        }
        // norm
        if (calibration) {
          (*it).first *= row_calib[row];
        }
      }

      // Simple selection
      Int_t cut_l = 22;

      if ((Int_t)temp.size() < cut_l)
          continue;
      ++N_events_long;

      if (MaxSep[moduleID] > 2)
        continue;
      ++N_events_one_track;

      pseudo_track.insert(pseudo_track.end(), temp.begin(), temp.end());
      ++pseudo_module_length;

      if (pseudo_module_length == 8) {
        sort(pseudo_track.begin(), pseudo_track.end());
        Int_t i_max = round(alpha * pseudo_track.size());
        for (int i = 0; i < std::min(i_max, int(pseudo_track.size())); ++i)
           norm_cluster += pseudo_track[i].first;

        norm_cluster *= 1 / (alpha * pseudo_track.size());

        // general resolution
        //cout << norm_cluster << endl;
        ClusterNormCharge->Fill(norm_cluster);
        pseudo_track.clear();
        norm_cluster = 0;
        pseudo_module_length = 0;
      }
    } // end event loop
  } // end file loop

  gStyle->SetFillColor(1);
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  PadDisplay->Draw("colz");
  c1->Print("figure/FEM3_simple.png");

  gStyle->SetOptStat("RMne");
  gStyle->SetOptFit(0111);
  Double_t fit_min = ClusterNormCharge->GetBinCenter(ClusterNormCharge->FindFirstBinAbove(ClusterNormCharge->GetMaximum()*0.1));
    Double_t fit_max = ClusterNormCharge->GetBinCenter(ClusterNormCharge->FindLastBinAbove(ClusterNormCharge->GetMaximum()*0.1));
  TH1F* ClusterNormCharge1 = (TH1F*)ClusterNormCharge->Clone();
  ClusterNormCharge->Fit("gaus", "", "", fit_min, fit_max);
  TF1 *fit = ClusterNormCharge->GetFunction("gaus");
  Float_t mean      = fit->GetParameter(1);
  Float_t sigma     = fit->GetParameter(2);
  Float_t mean_e     = fit->GetParError(1);
  Float_t sigma_e    = fit->GetParError(2);
  Float_t resol = sigma / mean;

  Float_t resol_e   = resol * sqrt(mean_e*mean_e/(mean*mean) + sigma_e*sigma_e/(sigma*sigma));

  ClusterNormCharge->Draw();
  c1->Update();

  TPaveStats *ps = (TPaveStats*)c1->GetPrimitive("stats");
  ps->SetName("mystats");
  TList *listOfLines = ps->GetListOfLines();
  TText *tconst = ps->GetLineWith("RMS");
  TLatex *myt = new TLatex(0,0,Form("Resol = %f #pm %f %%", (Float_t)0.01*round(resol*10000), resol_e));
  myt ->SetTextFont(tconst->GetTextFont());
  myt ->SetTextSize(tconst->GetTextSize());
  listOfLines->Add(myt);
  ClusterNormCharge->SetStats(0);
  c1->Modified();


  c1->Print(("figure/TruncEnergy_FEM3_simple" + postfix + ".png").c_str());

  TF1* f1 = new TF1("f1", "( (x < [1] ) ? [0] * TMath::Gaus(x, [1], [2]) : [0] * TMath::Gaus(x, [1], [3]))", 0, 1300);
  f1->SetParameters(200, 700, 50, 50);
  f1->SetParName(0, "Const");
  f1->SetParName(1, "Mean");
  f1->SetParName(2, "Left sigma");
  f1->SetParName(3, "Right sigma");
  ClusterNormCharge1->Fit("f1");
  ClusterNormCharge1->Draw();
  TF1 *fit1 = ClusterNormCharge1->GetFunction("f1");
  mean      = fit1->GetParameter(1);
  Float_t sigmaL     = fit1->GetParameter(2);
  Float_t sigmaR     = fit1->GetParameter(3);

  Float_t resolL = sigmaL / mean;
  Float_t resolR = sigmaR / mean;

  c1->Update();

  TPaveStats *ps1 = (TPaveStats*)c1->GetPrimitive("stats");
  ps1->SetName("mystats");
  TList *listOfLines1 = ps1->GetListOfLines();
  TText *tconst1 = ps1->GetLineWith("RMS");
  TLatex *myt1 = new TLatex(0,0,Form("resol L = %f %%", (Float_t)0.01*round(resolL*10000)));
  myt1 ->SetTextFont(tconst1->GetTextFont());
  myt1 ->SetTextSize(tconst1->GetTextSize());
  TLatex *myt2 = new TLatex(0,0,Form("resol R = %f %% ", (Float_t)0.01*round(resolR*10000)));
  myt2 ->SetTextFont(tconst1->GetTextFont());
  myt2 ->SetTextSize(tconst1->GetTextSize());
  listOfLines1->Add(myt1);
  listOfLines1->Add(myt2);
  ClusterNormCharge1->SetStats(0);
  c1->Modified();

  c1->Print(Form("figure/TruncEnergy_Simple_AssymFit.png"));


  int bin1 = ClusterNormCharge->FindFirstBinAbove(ClusterNormCharge->GetMaximum()/2);
  int bin2 = ClusterNormCharge->FindLastBinAbove(ClusterNormCharge->GetMaximum()/2);
  double fwhm = ClusterNormCharge->GetBinCenter(bin2) - ClusterNormCharge->GetBinCenter(bin1);
  cout << "****************************************************" << endl;
  cout << "Present resolution" << endl;
  cout << "resol FWHM = " << 0.5 * fwhm / ClusterNormCharge->GetBinCenter(ClusterNormCharge->GetMaximumBin()) << endl;
  cout << "resol GAUS = " << sigma / mean << endl;

  cout << "****************************************************" << endl;
  cout << "Initial N events   : " << N_events_raw << endl;
  cout << "Events long        : " << N_events_long << endl;
  cout << "Events one track   : " << N_events_one_track << endl;
}

vector< pair<Int_t, Int_t> > ScanPad(const vector<vector<short> > pad, Int_t FEM, Int_t Imax, Int_t Jmax, Int_t& Nrow, Int_t& MaxSeparation, bool& touchBorder, TH2F& PadDisplay) {

  MaxSeparation = 0;
  Nrow = 0;
  touchBorder = false;

  vector< pair<Int_t, Int_t> > cluster_charge;
  cluster_charge.clear();
  vector<int> listofRow;
  listofRow.resize(24, 0);

  for (UInt_t i = 0; i < pad.size(); ++i) {
    int prev = 1E6;
    int MaxSepRow = -1;
    int ChargeCl = 0;
    for (Int_t j = 0; j < (int)pad[i].size(); ++j) {

      int adcmax = pad[i][j];

      if (adcmax < 0) {
        cout << "ALARM!" << endl;
        exit(1);
      }

      //if (FEM == 5 && i < 9 && adcmax > 300)
        //cout << "J = " << j << endl;

      PadDisplay.Fill(j, i, adcmax);

      ChargeCl += adcmax;

      if (adcmax > 0) {
        if (j - prev - 1 > MaxSepRow)
          MaxSepRow = j - prev - 1;
        prev = j;
        if (j == 0 || j == (int)pad[i].size() - 1)
          touchBorder = true;
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
    x[ic] *= xscale;
    y[ic]  = (y[ic] + 1520) * yscale;

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