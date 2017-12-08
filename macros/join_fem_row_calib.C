#define THIS_NAME join_fem_ana
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "common_header.h"

vector< pair<Int_t, Int_t> > ScanPad(const vector<vector<short> >, Int_t , Int_t , Int_t , Int_t& , Int_t&, bool& );

void join_fem_ana() {
  bool DrawEnergy = false;
  bool DrawDrift  = false;
  Int_t run_start = 5149;
  Int_t run_end   = 5161;

  bool calibration = false;

  if ((run_start < 5162 && DrawEnergy) || (run_start > 5161 && DrawDrift)) {
    cout << "WRONG RUN!" << endl;
    exit(1);
  }

  vector<TH1F*> ChargePerRow[7];
  for (Int_t j = 0; j < 7; ++j) {
    ChargePerRow[j].resize(24, NULL);
    for (Int_t i = 0; i < 24; ++i)
      ChargePerRow[j][i] = new TH1F(Form("charge%i%i", i, j),"Charge per row", 150, 0. , 10000);
  }

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
  vector<float> row_calib[3];
  Int_t femId_ar[3] = {0, 3, 5};
  Int_t fem_ar_it = 0;
  while (true) {
    TString file = Form("data/fem%i.dat", femId_ar[fem_ar_it]);
    ifstream CalibFile(file);
    if ( !CalibFile ) {
      cerr << "ERROR: Can't open file for row calibration: " << file << endl;
      exit(1);
    }

    // Global FEM norm
    char   varName[20];
    double FEMnorm;
    CalibFile >> varName >> FEMnorm;

    while ( CalibFile.good() && !CalibFile.eof() ) {
      int row;
      float charge, avg, norm;
      CalibFile >> row >> charge >> avg >> norm;
      row_calib[fem_ar_it].push_back(norm);
    }

    // end of FEMs of interest
    if (fem_ar_it == 2)
      break;
    ++fem_ar_it;
  }

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

      Int_t Nrow[7];
      Int_t MaxSep[7];
      bool touchBorder[7];

      for (Int_t femId = 0; femId < 7; ++femId) {
        vector< pair<Int_t, Int_t > > temp = ScanPad(*pad_charge[femId], femId, Imax, Jmax, Nrow[femId], MaxSep[femId], touchBorder[femId]);

        for (vector< pair<Int_t, Int_t > >::iterator it = temp.begin(); it != temp.end(); ++it){
          Int_t row = (*it).second;
          /*if (row == 0 ||  row == Imax || (row == 12 && femId == 0) || (row == 1 && femId == 5) || (femId == 3 && (row == 3 || row == 9 || row == 11))) {
          /temp.erase(it);
            // VERY IMPORTANT!!!!
            --it;
            continue;
          }*/
          // norm
          Int_t fem_cal = -1;
          if (femId == 0)
            fem_cal = 0;
          else if (femId == 3)
            fem_cal = 1;
          else if (femId == 5)
            fem_cal = 2;
          if (calibration && fem_cal > -1) {
            (*it).first *= row_calib[fem_cal][row];
          }
        }

        // Simple selection
        Int_t cut_l = 22;
        if (femId == 0)
          cut_l = 21;
        else if (femId == 3)
          cut_l = 19;
        else if (femId == 5)
          cut_l = 21;

        if ((Int_t)temp.size() < cut_l)
            continue;

        if (MaxSep[femId] > 2)
          continue;

        for (UInt_t i = 0; i < temp.size(); ++i)
          ChargePerRow[femId][temp[i].second]->Fill(temp[i].first);
      } // end of loop over FEMs
    } // end event loop
  } // end file loop


  //************************************************************
  //************************************************************
  //****************** PLOT THE OUTPUT  ************************
  //************************************************************
  //************************************************************
  TGraphErrors* graphLandau[7];
  for (Int_t femId = 0; femId < 7; ++femId) {
    graphLandau[femId] = new TGraphErrors();
    for (Int_t row = 0; row <= Imax; ++row) {

      gStyle->SetOptStat("RMne");
      gStyle->SetOptFit(0111);
      TF1* f1 = new TF1("f1", "( (x < [1] ) ? [0] * TMath::Gaus(x, [1], [2]) : [0] * TMath::Gaus(x, [1], [3]))", 0, 10000);
      f1->SetParameters(2000, 600, 200, 700);
      Double_t fit_min = ChargePerRow[femId][row]->GetBinCenter(ChargePerRow[femId][row]->FindFirstBinAbove(ChargePerRow[femId][row]->GetMaximum()*0.3));
      Double_t fit_max = ChargePerRow[femId][row]->GetBinCenter(ChargePerRow[femId][row]->FindLastBinAbove(ChargePerRow[femId][row]->GetMaximum()*0.3));
      ChargePerRow[femId][row]->Fit("f1", "", "", fit_min, fit_max);
      TF1 *fit = ChargePerRow[femId][row]->GetFunction("f1");
      Float_t mean    = fit->GetParameter(1);
      Float_t mean_e  = fit->GetParError(1);
      if (mean < 0) {
        mean = 0;
      }
      if (mean < 200)
        mean_e  =0.;

      //cout << "FEM = " << femId << "   Row = " << row << "    " << mean << endl;

      int bin1 = ChargePerRow[femId][row]->FindFirstBinAbove(ChargePerRow[femId][row]->GetMaximum()*0.9);
      int bin2 = ChargePerRow[femId][row]->FindLastBinAbove(ChargePerRow[femId][row]->GetMaximum()*0.9);
      float mean_f = 0.5 * (ChargePerRow[femId][row]->GetBinCenter(bin1) + ChargePerRow[femId][row]->GetBinCenter(bin2));
      //float mean_e = 0.5 * ChargePerRow[femId][row]->GetNbinsX() * (bin2-bin1+1);

      Float_t ymax = 1.05 * ChargePerRow[femId][row]->GetMaximum();
      TLine *line1 = new TLine(mean, 0 ,mean_f,ymax);
      line1->SetLineColor(kRed);


      graphLandau[femId]->SetPoint(row, row, mean);
      graphLandau[femId]->SetPointError(row, 0, mean_e);
      //Float_t max = ChargePerRow[femId][row]->GetBinCenter(ChargePerRow[femId][row]->GetMaximumBin());
      //graphMax->SetPoint(i, i, max);
      //graphMax->SetPointError(i, 0, sqrt(max));

      ChargePerRow[femId][row]->Draw();
      //line1->Draw();
      c1->Print(Form("figure/row/ChargePerFem%iRow%i.png", femId, row));
    }

    Float_t avrg = 0;
    Int_t N_avg = 22;
    if (femId == 0)
      N_avg = 21;
    else if (femId == 3)
      N_avg = 19;
    else if (femId == 5)
      N_avg = 21;

    for (Int_t row = 0; row < 24; ++row) {
      if (row == 0 ||  row == Imax || (row == 12 && femId == 0) || (row == 1 && femId == 5) || (femId == 3 && (row == 3 || row == 9 || row == 11)))
        continue;
      Double_t x, y;
      graphLandau[femId]->GetPoint(row, x, y);
      avrg += y / N_avg;
    }
    cout << "*************** FEM = " << femId << "*******************" << endl;
    cout << " row    MPV      MPV_av     MPV_av/MPV" << endl;
    for (Int_t i = 0; i < 24; ++i) {
      Double_t x, y;
      graphLandau[femId]->GetPoint(i, x, y);
      cout << i << "    " << y << "    " << avrg << "     " << avrg / y << endl;
    }

    graphLandau[femId]->SetMarkerColor(4);
    graphLandau[femId]->SetMarkerSize(0.5);
    graphLandau[femId]->SetMarkerStyle(21);
    c1->SetGrid(1);
    graphLandau[femId]->GetXaxis()->SetRangeUser(-5, 25);
    graphLandau[femId]->Draw("ap");
    TLine *line1 = new TLine(0, avrg ,23,avrg);
    line1->Draw();
    c1->Print(Form("figure/FEM_calib/ChargeClusterLandauFem%i.png", femId));
  }
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
      cluster_charge.push_back(make_pair(ChargeCl, i));
      listofRow[i] = 1;
    }
  }  // end of PAD scan

  for (int i = 0; i <= Imax; i++)
    Nrow+=listofRow[i];

  return cluster_charge;
}