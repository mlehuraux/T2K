#define THIS_NAME pad_charge
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "common_header.h"

/*

make SRC="mydict.C tpc_gain_study.C"  EXE=test.exe
 ./test.exe  -f  ./RUN_05206.00000.root


*/

/*tpc_gain_study
What we need to study:
1) maximum separation in the row. for 2 tracks substraction. How often?
2) Number of pads per cluster
3) Qmax (sum over cluster) vs row
4) Qmax destribution
*/

void  pad_charge(){

  bool DEBUG = false;

  int thr=100;     //seuil
  int nbins=100;
  int Ndefaultevents=0;
  // int cut_adc=400000;
  vector<TString> listOfFiles;
  //  TFile::Open("/media/ilctpc/FCC_TPC_3/MiniTPC-FCC//Test_1705/17_05_11_data/RUN_00008.00004.root");
  //  TFile::Open("RUN_00003.00000.root");
  TString file="default.root";
  for (int iarg=0; iarg<gApplication->Argc(); iarg++){
    if (string( gApplication->Argv(iarg))=="-f" || string( gApplication->Argv(iarg))=="--file" ){
      iarg++;
      file=gApplication->Argv(iarg);
      if (file.Contains(".root")) {

        cout << "adding filename" <<" " << file << endl;
        listOfFiles.push_back(file);
      } else {
        fstream fList(file);
        if (fList.good()) {
          while (fList.good()) {
            string filename;
            getline(fList, filename);
            if (fList.eof()) break;
            listOfFiles.push_back(filename);
          }
        }
      }
    }
    else if (string( gApplication->Argv(iarg))=="-d"){
      DEBUG=true;
    }
 else if (string( gApplication->Argv(iarg))=="-b"){
      gROOT->SetBatch(1);

    }

  else if (string( gApplication->Argv(iarg))=="-t"||string( gApplication->Argv(iarg))=="--threshold"){
    iarg++;
    thr = strtol(gApplication->Argv(iarg), NULL, 0);
  }

    else if (string( gApplication->Argv(iarg))=="-n"||string( gApplication->Argv(iarg))=="-nEvents"){
      iarg++;
    Ndefaultevents = strtol(gApplication->Argv(iarg), NULL, 0);
    }
    else if (string( gApplication->Argv(iarg))=="--nbins"){
      iarg++;
      nbins = strtol(gApplication->Argv(iarg), NULL, 0);
       }
    else if (string( gApplication->Argv(iarg))=="-h" || string( gApplication->Argv(iarg))=="--help" ){
      cout << "this is help" <<endl;
      printf("Usage: %s [OPTIONS]\n\n", gApplication->Argv(0));
      printf("\n-f/--file <input file with .acq extension>         (default = %s)", file.Data());
      printf("\n-n/--nEvents <max number of events>         (default = %d)", Ndefaultevents);
      printf("\n-q    quit at the end ");
      printf("\n-t/--threshold [threshold in ADC] (default = %d)", thr);
      printf("\n--nbins [nbins for time histo] (default = %d)", nbins);
      printf("\n");
      return;
    }
  } //Argument parsing

  TCanvas *c1 = new TCanvas("c1","evadc",900,700);

  // Histoes

  TH1F* ClusterCharge     = new TH1F("cluster_charge","Cluster charge",100,0,10000);
  TH1F* ClusterNormCharge = new TH1F("cluster_norm_charge","Truncated mean energy deposit",150,0,4000);

  TH1F* PadPerCluster     = new TH1F("pads_per_cluster","Pads per cluster",10,0,10);

  TH1F* NrowHist          = new TH1F("n_row", "number of rows per event", 26, 0, 26);
  TH1F* NcolHist          = new TH1F("n_row", "number of columns per event", 73, 0, 73);

  vector<TH1F*> ChargePerRow;
  //vector<TH1F*>
  ChargePerRow.resize(24, NULL);
  for (int i = 0; i < 24; ++i) {
    ChargePerRow[i] = new TH1F(Form("charge%i", i),"Charge per row", 250, 0. , 10000);
  }

  TH1F* MaximumSepEvent = new TH1F("maxSep","Maximum X separation between pads", 73, 0. , 73);

  Float_t alpha = 0.7;

  TGraphErrors* DriftScanResol = new TGraphErrors();
  TH1F* ClusterNormChargeFile = new TH1F("cluster_norm_charge_file","Truncated mean energy deposit",150,0,4000);

  //************************************************************
  //************************************************************
  //****************** LOOP OVER FILES  ************************
  //************************************************************
  //************************************************************
  int events_raw        = 0;
  int events_not_empty  = 0;
  int events_long       = 0;
  int events_one_track  = 0;

  TFile*f=0;
  for (uint ifile=0;ifile< listOfFiles.size();ifile++){
    file = listOfFiles[ifile];
    if (f!=0) f->Close();
    cout << "opening file " << file <<  endl;
    f= new TFile(file);
    if (!f->IsOpen()) {
      cout << " WARNING problem in opening file " << file << endl;
      return;
    }

    TTree* tconfig= (TTree*) f->Get("beam_config");
    Float_t driftZ;
    tconfig->SetBranchAddress("DriftZ", &driftZ);
    tconfig->GetEntry(0);

    vector<int> *iPad(0);
    vector<int> *jPad(0);
    TTree * tgeom= (TTree*) f->Get("femGeomTree");
    int Jmin, Imin, Jmax, Imax;
    vector<vector<double> > *x0Pad(0);//, *x1Pad, *x2Pad, *x3Pad;
    vector<vector<double> > *y0Pad(0);//, *y1Pad, *y2Pad, *y3Pad;
    tgeom->SetBranchAddress("jPad", &jPad );
    tgeom->SetBranchAddress("iPad", &iPad );
    tgeom->SetBranchAddress("jPadMin", &Jmin );
    tgeom->SetBranchAddress("iPadMin", &Imin );
    tgeom->SetBranchAddress("jPadMax", &Jmax );
    tgeom->SetBranchAddress("iPadMax", &Imax );
    tgeom->SetBranchAddress("x0Pad", &x0Pad );
    //tgeom->SetBranchAddress("x1Pad", &x1Pad );
    //tgeom->SetBranchAddress("x2Pad", &x2Pad );
    //tgeom->SetBranchAddress("x3Pad", &x3Pad );

    tgeom->SetBranchAddress("y0Pad", &y0Pad );
    //tgeom->SetBranchAddress("y1Pad", &y1Pad );
    //tgeom->SetBranchAddress("y2Pad", &y2Pad );
    //tgeom->SetBranchAddress("y3Pad", &y3Pad );

    tgeom->GetEntry(0); // put into memory geometry info
    cout << "reading geometry" << endl;
    cout << "jPad->size() " << jPad->size() <<endl;
    cout << "iPad->size() " << iPad->size() <<endl;

    if (DEBUG)
      cout << "Imax = " << Imax << " Imin = " << Imin << " Jmax = " << Jmax << " Jmin = " << Jmin << endl;

    gStyle->SetPalette(1);
    gStyle->SetOptStat(0);
    int Nevents=Ndefaultevents;
    TTree *t = (TTree*) f->Get("padData");
    cout <<t->GetEntries() << " evts in file " << endl;

    vector<short>          *listOfChannels(0);
    vector<vector<short> > *listOfSamples(0);

    t->SetBranchAddress("PadphysChannels", &listOfChannels );
    t->SetBranchAddress("PadADCvsTime"   , &listOfSamples );

    if (Nevents<=0) Nevents=t->GetEntries();
    if (Nevents>t->GetEntries()) Nevents=t->GetEntries();

    cout << "[          ] Nev="<<Nevents<<"\r[";

    // to win time, disable MG data
    t->SetBranchStatus("MGADCvsTime",0);
    t->SetBranchStatus("MGphysChannels",0);

    int listofRow[24]; //to count how many rows have been hit
    int listofCol[72];

    vector <int> listofT ;

    //************************************************************
    //************************************************************
    //****************** LOOP OVER EVENTS ************************
    //************************************************************
    //************************************************************

    for (int ievt=0; ievt < Nevents ; ievt++){
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;

      if (DEBUG)
        cout << "event i " << ievt << endl;

      t->GetEntry(ievt);

      ++events_raw;

      vector< vector<int> > PadDisplay;
      PadDisplay.resize(Jmax+1);
      for (int z=0; z <= Jmax; ++z)
        PadDisplay[z].resize(Imax+1, 0);
      //TH2F* PadDisplay1=new TH2F("PadDisplay","I vs J of hits",72,-0.5,71+0.5,24,0-0.5,23+0.5);

      int adc_sum = 0;

      for (int i = 0; i < 24; i++)
        listofRow[i]=0;

      for (int i = 0; i < 72; i++)
        listofCol[i]=0;

      listofT.clear();

      //************************************************************
      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      //************************************************************

      if (DEBUG)
        cout << "channel size " << listOfChannels->size() << endl;
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];
        // find out the maximum
        float adcmax=-1;
        int itmax=-1;

        // one maximum per channel
        for (uint it = 0; it < (*listOfSamples)[ic].size(); it++){
          int adc= (*listOfSamples)[ic][it];
          // cout << " ADC channel " << ic << " time " << it << " adc " << adc << endl ;}
          if (adc>thr)
            if (adc>adcmax) {
              adcmax=adc;
              itmax=it;
            }
        }
        if (DEBUG) {

          int i = (*iPad)[chan];
          int j = (*jPad)[chan];

          if (i == 10 && j == 10) {
            cout << "channel " << chan << endl;
            cout << "i  = " << (*iPad)[chan]  << "   j  = " << (*jPad)[chan]  << "    adc_max = " << adcmax << endl;
            cout << "x0 = " << (*x0Pad)[i][j] << "   y0 = " << (*y0Pad)[i][j] << endl;
            exit(0);
          }
        }

        if (adcmax<0) continue; // remove noise

        listofT.push_back(itmax);
        listofRow[(*iPad)[chan]]=1; //the row has been hit
        listofCol[(*jPad)[chan]]=1; //the column has been hit

        adc_sum += adcmax;

        PadDisplay[(*jPad)[chan]][(*iPad)[chan]] += adcmax;
      } //loop over channels

      int Nrow = 0;
      for (int i = 0; i < 24; i++)
        Nrow+=listofRow[i];

      int Ncol = 0;
      for (int i = 0; i < 72; i++)
        Ncol+=listofCol[i];

      if (Nrow < 1)
        continue;
      ++events_not_empty;

      vector< pair<Float_t, Int_t> > cluster_charge;
      cluster_charge.clear();
      vector<Float_t> pads_per_cluster;
      pads_per_cluster.clear();

      // Loop over pads i j
      int MaxSepEvent = 0;
      for (int i = 0; i <= Imax; ++i) {
        int prev = 1E6;
        int MaxSepRow = -1;
        int PadPerCl = 0;
        int ChargeCl = 0;
        // int hit = 0;
        for (int j = 0; j <= Jmax; ++j) {
          int adcmax = PadDisplay[j][i];

          ChargeCl += adcmax;
          if (adcmax > 0) {
            if (j - prev - 1 > MaxSepRow)
              MaxSepRow = j - prev - 1;
            prev = j;
            ++PadPerCl;
          }
        }
        if (MaxSepRow > MaxSepEvent)
          MaxSepEvent = MaxSepRow;

        if (ChargeCl>0) {
          cluster_charge.push_back(make_pair(ChargeCl, i));
          pads_per_cluster.push_back(PadPerCl);
        }


      }  // end of PAD scan
      sort(cluster_charge.begin(), cluster_charge.end());
      Float_t norm_cluster = 0.;
      Int_t i_max = round(alpha * cluster_charge.size());
      for (int i = 0; i < std::min(i_max, int(cluster_charge.size())); ++i)
        norm_cluster += cluster_charge[i].first;

      norm_cluster *= 1 / (alpha * cluster_charge.size());

      MaximumSepEvent->Fill(MaxSepEvent);
      for (UInt_t i = 0; i < cluster_charge.size(); ++i) {
        ClusterCharge->Fill(cluster_charge[i].first);
      }

      if (MaxSepEvent > 2)
        continue;
      ++events_one_track;
      NrowHist->Fill(Nrow);
      NcolHist->Fill(Ncol);
      if (Nrow < 20)
        continue;
      /*if (Ncol < 40)
        continue;
        */
      ++events_long;

      ClusterNormCharge->Fill(norm_cluster);
      ClusterNormChargeFile->Fill(norm_cluster);

      for (UInt_t i = 0; i < cluster_charge.size(); ++i) {
        ChargePerRow[cluster_charge[i].second]->Fill(cluster_charge[i].first);
        PadPerCluster->Fill(pads_per_cluster[i]);
      }

    } // end of loops over events

    // Fill scan over files
    ClusterNormChargeFile->Fit("gaus");
    TF1 *fit = ClusterNormChargeFile->GetFunction("gaus");
    Float_t mean      = fit->GetParameter(1);
    Float_t sigma     = fit->GetParameter(2);

    Float_t mean_e     = fit->GetParError(1);
    Float_t sigma_e    = fit->GetParError(2);
    Float_t resol = sigma / mean;

    Float_t resol_e   = resol * sqrt(mean_e*mean_e/(mean*mean) + sigma_e*sigma_e/(sigma*sigma));

    DriftScanResol->SetPoint(ifile, driftZ, resol);
    DriftScanResol->SetPointError(ifile, 0, resol_e);

    ClusterNormChargeFile->Reset();
  } // end of loops over files

  cout << "Total events number     : " << events_raw << endl;
  cout << "Not empty events number : " << events_not_empty << endl;
  cout << "One track events number : " << events_one_track << endl;
  cout << "Long track events number: " << events_long << endl;

  MaximumSepEvent->Draw();
  c1->Print("figure/MaxSep.png");

  gStyle->SetOptStat("RMne");
  gStyle->SetOptFit(0111);
  ClusterCharge->Fit("landau");
  ClusterCharge->Draw();
  c1->Print("figure/ClusterCharge.png");

  PadPerCluster->Draw();
  c1->Print("figure/PadPerCluster.png");

  NrowHist->Draw();
  c1->Print("figure/TrackNrow.png");

  NcolHist->Draw();
  c1->Print("figure/TrackNcol.png");

  gStyle->SetOptStat("RMne");
  gStyle->SetOptFit(0111);
  ClusterNormCharge->Fit("gaus");
  ClusterNormCharge->Draw();
  c1->Print("figure/TruncEnergy.png");

  int bin1 = ClusterNormCharge->FindFirstBinAbove(ClusterNormCharge->GetMaximum()/2);
  int bin2 = ClusterNormCharge->FindLastBinAbove(ClusterNormCharge->GetMaximum()/2);
  double fwhm = ClusterNormCharge->GetBinCenter(bin2) - ClusterNormCharge->GetBinCenter(bin1);
  cout << "max = " << ClusterNormCharge->GetMaximum() << "   FWHM = " << fwhm << std::endl;
  cout << "resol = " << fwhm / ClusterNormCharge->GetMaximum() << endl;

  TGraphErrors* graphMax = new TGraphErrors();
  TGraphErrors* graphLandau = new TGraphErrors();
  for (Int_t i = 0; i < 24; ++i) {
    gStyle->SetOptStat("RMne");
    gStyle->SetOptFit(0111);
    ChargePerRow[i]->Fit("landau");
    TF1 *fit = ChargePerRow[i]->GetFunction("landau");
    Float_t mean = fit->GetParameter(1);
    cout << i << "    " << mean << endl;
    graphLandau->SetPoint(i, i, mean);
    graphLandau->SetPointError(i, 0, sqrt(mean));
    Float_t max = ChargePerRow[i]->GetBinCenter(ChargePerRow[i]->GetMaximumBin());
    graphMax->SetPoint(i, i, max);
    graphMax->SetPointError(i, 0, sqrt(max));

    ChargePerRow[i]->Draw();
    c1->Print(Form("figure/row/ChargePerRow%i.png", i));
  }
  graphMax->SetMarkerColor(4);
  graphMax->SetMarkerSize(0.5);
  graphMax->SetMarkerStyle(21);
  c1->SetGrid(1);
  graphMax->Draw("ap");
  c1->Print("figure/ChargeClusterMax.png");

  graphLandau->SetMarkerColor(4);
  graphLandau->SetMarkerSize(0.5);
  graphLandau->SetMarkerStyle(21);
  c1->SetGrid(1);
  graphLandau->Draw("ap");
  c1->Print("figure/ChargeClusterLandau.png");

  DriftScanResol->SetMarkerColor(4);
  DriftScanResol->SetMarkerSize(0.5);
  DriftScanResol->SetMarkerStyle(21);
  c1->SetGrid(1);
  DriftScanResol->Draw("ap");
  c1->Print("figure/Resolution_Drift.png");

  cout << "END" << endl;
}

