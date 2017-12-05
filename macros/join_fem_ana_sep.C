#define THIS_NAME join_fem_ana
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "common_header.h"

const Float_t tpcPanelWidth   = 233.94;
const Float_t tpcPanelHeight  = 172.83;
const Float_t alpha           = 0.7;

vector< pair<Int_t, Int_t> > ScanPad(const vector<vector<short> >, Int_t , Int_t , Int_t , Int_t& , Int_t&, bool& );
void GetCoordinates(Double_t* , Double_t* , Int_t , Double_t* , Double_t* );

void join_fem_ana() {
  bool DrawEnergy = false;
  bool DrawDrift  = false;
  Int_t run_start = 5149;
  Int_t run_end   = 5161;

  if ((run_start < 5162 && DrawEnergy) || (run_start > 5161 && DrawDrift)) {
    cout << "WRONG RUN!" << endl;
    exit(1);
  }

  // define output
  //TGraph* graph = new TGraph();
  //int point = 0;
  TH1F* ClusterNormCharge[7];
  for (Int_t femId = 0; femId < 7; ++femId)
    ClusterNormCharge[femId]  = new TH1F(Form("cluster_norm_charge%i", femId) ,"Truncated mean energy deposit",250,0,2000);
  TH1F* ClusterNormChargeFile   = new TH1F("cluster_norm_charge_file","Truncated mean energy deposit",250,0,2000);
  vector< vector<pair<Int_t, Int_t> > > EventClusters;
  TGraphErrors* DriftScanResol  = new TGraphErrors();
  TGraphErrors* EnergyScanResol = new TGraphErrors();

  Int_t N_events_raw = 0;
  Int_t N_events_fem5 = 0;
  Int_t N_events_Middle = 0;
  Int_t N_events_top = 0;
  Int_t N_events_border = 0;
  Int_t N_events_matched = 0;

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
  TCanvas *c1 = new TCanvas("c1","evadc",900,700);

  //************************************************************
  //************************************************************
  //****************** LOOP OVER FILEWS ************************
  //************************************************************
  //************************************************************
  string postfix = "";
  for (Int_t fileID = run_start; fileID <= run_end; ++fileID) {
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

    int Nevents=t->GetEntries();
    cout << "[          ] Nev="<<Nevents<<"\r[";
    for (int ievt=0; ievt < Nevents ; ievt++){
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;
      t->GetEntry(ievt);

      vector< pair<Int_t, Int_t> > cluster_charge;
      cluster_charge.clear();

      Int_t TotalLength = 0;

      Int_t Nrow[7];
      Int_t MaxSep[7];
      bool touchBorder[7];


      // Scan over pads
      for (Int_t femId = 0; femId < 7; ++femId) {
        vector< pair<Int_t, Int_t > > temp = ScanPad(*pad_charge[femId], femId, Imax, Jmax, Nrow[femId], MaxSep[femId], touchBorder[femId]);

        // cuts on FEM 5
        if (Nrow[femId] < 20)
          continue;
        if (MaxSep[femId] > 2)
          continue;
        N_events_fem5++;

        cluster_charge.insert(cluster_charge.end(), temp.begin(), temp.end());

        sort(cluster_charge.begin(), cluster_charge.end());
        Float_t norm_cluster = 0.;
        Int_t i_max = round(alpha * cluster_charge.size());
        for (int i = 0; i < std::min(i_max, int(cluster_charge.size())); ++i)
          norm_cluster += cluster_charge[i].first;

        norm_cluster *= 1 / (alpha * cluster_charge.size());

        // general resolution
        ClusterNormCharge[femId]->Fill(norm_cluster);
        // for scan over vars
        // ClusterNormChargeFile->Fill(norm_cluster);
        // for alpha variation
        // EventClusters.push_back(cluster_charge);
      } // end of FEM loop
    } // end event loop
    // fill the file vars
    // distance / energy
    /*gStyle->SetOptStat("RMne");
    gStyle->SetOptFit(0111);
    ClusterNormChargeFile->Fit("gaus");
    TF1 *fit = ClusterNormChargeFile->GetFunction("gaus");
    Float_t mean      = fit->GetParameter(1);
    Float_t sigma     = fit->GetParameter(2);

    Float_t mean_e     = fit->GetParError(1);
    Float_t sigma_e    = fit->GetParError(2);
    Float_t resol = sigma / mean;

    Float_t resol_e   = resol * sqrt(mean_e*mean_e/(mean*mean) + sigma_e*sigma_e/(sigma*sigma));

    DriftScanResol->SetPoint(fileID - run_start, driftZ, resol);
    DriftScanResol->SetPointError(fileID - run_start, 0, resol_e);

    EnergyScanResol->SetPoint(fileID - run_start, energy, resol);
    EnergyScanResol->SetPointError(fileID - run_start, 0, resol_e);
    c1->Print(Form("figure/file/TruncEnergy%i.png", fileID));

    cout << "File Scan. N = " << fileID - run_start << " Drift = " << driftZ << "   Energy = " << energy << "  Resol = " << resol << endl;

    ClusterNormChargeFile->Reset();*/
  } // end file loop

  //************************************************************
  //************************************************************
  //************* VARY THE ALPHA VALUE  ************************
  //************************************************************
  //************************************************************
  /*Float_t best_resolution = 1.;
  Float_t best_alpha = 1.;
  // loop over alpha
  Float_t min   = 0.3;
  Float_t max   = 0.9;
  Int_t   Nstep = 24;
  Float_t step = (max- min) / Nstep;

  TGraphErrors* alpha_gaussian  = new  TGraphErrors();
  TGraphErrors* alpha_fwhw      = new  TGraphErrors();

  for (Int_t z = 0; z <= Nstep; ++z) {
    Float_t alpha = min + z * step;
    TH1F* ClusterNormCharge = new TH1F("cluster_norm_charge","Truncated mean energy deposit",250,0,2000);

    for (UInt_t j = 0; j < EventClusters.size(); ++j) {
      Float_t norm_cluster = 0.;
      Int_t i_max = round(alpha * EventClusters[j].size());
      Int_t v_size = (i_max < int(EventClusters[j].size())) ? i_max : int(EventClusters[j].size());
      for (int i = 0; i < v_size ; ++i)
        norm_cluster += EventClusters[j][i].first;

      norm_cluster *= 1 / (alpha * EventClusters[j].size());

      ClusterNormCharge->Fill(norm_cluster);
    }

    gStyle->SetOptStat("RMne");
    gStyle->SetOptFit(0111);
    ClusterNormCharge->Fit("gaus");
    TF1 *fit = ClusterNormCharge->GetFunction("gaus");
    Float_t mean      = fit->GetParameter(1);
    Float_t sigma     = fit->GetParameter(2);

    Float_t mean_e     = fit->GetParError(1);
    Float_t sigma_e    = fit->GetParError(2);
    Float_t resol = sigma / mean;

    Float_t resol_e   = resol * sqrt(mean_e*mean_e/(mean*mean) + sigma_e*sigma_e/(sigma*sigma));

    int bin1 = ClusterNormCharge->FindFirstBinAbove(ClusterNormCharge->GetMaximum()/2);
    int bin2 = ClusterNormCharge->FindLastBinAbove(ClusterNormCharge->GetMaximum()/2);
    double fwhm = ClusterNormCharge->GetBinCenter(bin2) - ClusterNormCharge->GetBinCenter(bin1);
    Float_t resol_fwhm = 0.5 * fwhm / ClusterNormCharge->GetBinCenter(ClusterNormCharge->GetMaximumBin());

    ClusterNormCharge->Draw();
    c1->Print(Form("figure/alpha/%i.png", z));
    alpha_gaussian->SetPoint(z, alpha, resol);
    alpha_fwhw->SetPoint(z, alpha, resol_fwhm);

    alpha_gaussian->SetPointError(z, 0, resol_e);

    if (resol < best_resolution) {
      best_alpha      = alpha;
      best_resolution = resol;
    }
  }


  gStyle->SetOptStat("RMne");
  gStyle->SetOptFit(0111);
  alpha_gaussian->SetMarkerColor(4);
  alpha_gaussian->SetMarkerSize(0.5);
  alpha_gaussian->SetMarkerStyle(21);
  c1->SetGrid(1);
  alpha_gaussian->Draw("ap");
  c1->Print("figure/Resolution_alpha_gaus.png");

  alpha_fwhw->SetMarkerColor(4);
  alpha_fwhw->SetMarkerSize(0.5);
  alpha_fwhw->SetMarkerStyle(21);
  alpha_fwhw->Draw("ap");
  c1->Print("figure/Resolution_alpha_fwhm.png");*/
  //************************************************************
  //************************************************************
  //****************** PLOT THE OUTPUT  ************************
  //************************************************************
  //************************************************************

  /*DriftScanResol->SetMarkerColor(4);
  DriftScanResol->SetMarkerSize(0.5);
  DriftScanResol->SetMarkerStyle(21);
  DriftScanResol->GetYaxis()->SetRangeUser(0., 0.11);
  c1->SetGrid(1);
  DriftScanResol->Draw("ap");
  if (DrawDrift)
    c1->Print("figure/Resolution_Drift_multi.png");

  EnergyScanResol->SetMarkerColor(4);
  EnergyScanResol->SetMarkerSize(0.5);
  EnergyScanResol->SetMarkerStyle(21);
  EnergyScanResol->GetYaxis()->SetRangeUser(0., 0.11);
  c1->SetGrid(1);
  EnergyScanResol->Draw("ap");
  if (DrawEnergy)
    c1->Print("figure/Resolution_Energy_multi.png");*/

  for (Int_t femId = 0; femId < 7; ++femId) {
    gStyle->SetOptStat("RMne");
    gStyle->SetOptFit(0111);
    ClusterNormCharge[femId]->Fit("gaus");
    TF1 *fit = ClusterNormCharge[femId]->GetFunction("gaus");
    Float_t mean      = fit->GetParameter(1);
    Float_t sigma     = fit->GetParameter(2);
    ClusterNormCharge[femId]->Draw();
    c1->Print((Form("figure/TruncEnergy_FEM%i", femId) + postfix + ".png").c_str());
    int bin1 = ClusterNormCharge[femId]->FindFirstBinAbove(ClusterNormCharge[femId]->GetMaximum()/2);
    int bin2 = ClusterNormCharge[femId]->FindLastBinAbove(ClusterNormCharge[femId]->GetMaximum()/2);
    double fwhm = ClusterNormCharge[femId]->GetBinCenter(bin2) - ClusterNormCharge[femId]->GetBinCenter(bin1);
    cout << "****************************************************" << endl;
    cout << "Present resolution" << endl;
    cout << "FEM = " << femId << endl;
    cout << "resol FWHM = " << 0.5 * fwhm / ClusterNormCharge[femId]->GetBinCenter(ClusterNormCharge[femId]->GetMaximumBin()) << endl;
    cout << "resol GAUS = " << sigma / mean << endl;
  }

  /*cout << "****************************************************" << endl;
  cout << "Initial N events   : " << N_events_raw << endl;
  cout << "Events in fem5     : " << N_events_fem5 << endl;
  cout << "Events with middle : " << N_events_Middle << endl;
  cout << "Events with top    : " << N_events_top << endl;
  cout << "Events w/o borders : " << N_events_border << endl;
  cout << "Events matched     : " << N_events_matched << endl;*/

  /*cout << "****************************************************" << endl;
  cout << "Results of scannning over alpha. Gaussian method of resolution calculating" << endl;
  cout << "alpha    = " << best_alpha << endl;
  cout << "resol    = " << best_resolution << endl;
  cout << "****************************************************" << endl;*/
}

vector< pair<Int_t, Int_t> > ScanPad(const vector<vector<short> > pad, Int_t FEM, Int_t Imax, Int_t Jmax, Int_t& Nrow, Int_t& MaxSeparation, bool& touchBorder) {

  MaxSeparation = 0;
  Nrow = 0;
  touchBorder = false;

  vector< pair<Int_t, Int_t> > cluster_charge;
  vector<int> listofRow;
  listofRow.resize(24, 0);

  for (UInt_t i = 0; i < pad.size(); ++i) {
    int prev = 1E6;
    int MaxSepRow = -1;
    int ChargeCl = 0;
    for (Int_t j = 0; j < (int)pad[i].size(); ++j) {

      int adcmax = pad[i][j];
      //if (FEM == 0)
      //  cout << i << "    " << j << "    " << adcmax << endl;
      if (adcmax < 0) {
        cout << "ALARM!" << endl;
        exit(1);
      }

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