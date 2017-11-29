#define THIS_NAME alpha_var
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

void  alpha_var(){

  bool DEBUG = false;

  int thr=100;     //seuil
  int nbins=100;
  int Ndefaultevents=0;
  // int cut_adc=400000;
  bool ExitAtEnd=0;
  bool Eweight=0;
  bool doFit=0;
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
    else if (string( gApplication->Argv(iarg))=="-q"){
      ExitAtEnd=1;
    }
    else if (string( gApplication->Argv(iarg))=="-w"){
      Eweight=1;
    } else if (string( gApplication->Argv(iarg))=="-d"){
      DEBUG=true;
    } else if (string( gApplication->Argv(iarg))=="--fit"){
     doFit=1;
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
      printf("\n--fit perform exp+cte fit");
      printf("\n-n/--nEvents <max number of events>         (default = %d)", Ndefaultevents);
      printf("\n-q    quit at the end ");
      printf("\n-w    enable Energy weighting ");
      printf("\n-t/--threshold [threshold in ADC] (default = %d)", thr);
      printf("\n--nbins [nbins for time histo] (default = %d)", nbins);
      printf("\n");
      return;
    }
  } //Argument parsing

  TCanvas *c1 = new TCanvas("c1","evadc",900,700);

  //************************************************************
  //************************************************************
  //****************** LOOP OVER FILES  ************************
  //************************************************************
  //************************************************************
  vector< vector<int > > EventClusters;

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

    vector<int> *iPad(0);
    vector<int> *jPad(0);
    TTree * tgeom= (TTree*) f->Get("femGeomTree");
    tgeom->SetBranchAddress("jPad", &jPad );
    tgeom->SetBranchAddress("iPad", &iPad );
    tgeom->GetEntry(0); // put into memory geometry info
    cout << "reading geometry" << endl;
    cout << "jPad->size() " << jPad->size() <<endl;
    cout << "iPad->size() " << iPad->size() <<endl;

    int Imax = -1;
    int Imin = 10000000;
    int Jmax = -1;
    int Jmin = 10000000;

    for (unsigned long i = 0; i < jPad->size(); ++i) {
      if (Imax < (*iPad)[i])
        Imax = (*iPad)[i];
      if (Imin > (*iPad)[i])
        Imin = (*iPad)[i];
      if (Jmax < (*jPad)[i])
        Jmax = (*jPad)[i];
      if (Jmin > (*jPad)[i])
        Jmin = (*jPad)[i];
    }

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

      vector< vector<int> > PadDisplay;
      PadDisplay.resize(Jmax+1);
      for (int z=0; z <= Jmax; ++z)
        PadDisplay[z].resize(Imax+1, 0);

      for (int i = 0; i < 24; i++)
        listofRow[i]=0;

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

        if (adcmax<0) continue; // remove noise
        if (DEBUG) {
          cout << "channel " << chan << endl;
          cout << "i = " << (*iPad)[chan] << "   j = " << (*jPad)[chan]  << "    adc_max = " << adcmax << endl;
        }

        listofRow[(*iPad)[chan]]=1; //the row has been hit

        float weight=1;
        if (Eweight) weight=adcmax;

        PadDisplay[(*jPad)[chan]][(*iPad)[chan]] = adcmax;
      } //loop over channels

      int Nrow = 0;
      for (int i = 0; i < 24; i++)
        Nrow+=listofRow[i];

      if (Nrow < 1)
        continue;

      vector<int> cluster_charge;
      cluster_charge.clear();

      // Loop over pads i j
      int MaxSepEvent = 0;
      for (int i = 1; i <= Imax-1; ++i) {
        int prev = 1E6;
        int MaxSepRow = -1;
        int ChargeCl = 0;
        // int hit = 0;
        for (int j = 0; j <= Jmax; ++j) {
          int adcmax = PadDisplay[j][i];

          ChargeCl += adcmax;
          if (adcmax > 0) {
            if (j - prev - 1 > MaxSepRow)
              MaxSepRow = j - prev - 1;
            prev = j;
          }
        }

        if (MaxSepRow > MaxSepEvent)
          MaxSepEvent = MaxSepRow;

        if (ChargeCl>0)
          cluster_charge.push_back(ChargeCl);
      }  // end of PAD scan

      if (cluster_charge.size() < 2)
        continue;

      sort(cluster_charge.begin(), cluster_charge.begin()+cluster_charge.size()-1);

      // Selection
      // 1 cut    one track
      if (MaxSepEvent > 2)
        continue;

      // 2 cut    long track
      if (Nrow < 20)
        continue;

      // store the vector of clusters per event that passed the selection
      EventClusters.push_back(cluster_charge);

    } // end of loops over events
  } // end of loops over files

  // loop over alpha vars per optimization
  // define opt vars
  TH1F* BestResol;
  Float_t best_resolution = 1.;
  Float_t best_alpha = 1.;
  // loop over alpha
  Float_t min   = 0.3;
  Float_t max   = 0.9;
  Int_t   Nstep = 24;

   TGraphErrors* gr = new  TGraphErrors();

  Float_t step = (max- min) / Nstep;

  //************************************************************
  //************************************************************
  //*****************LOOP OVER ALPHA ;;;************************
  //************************************************************
  //************************************************************
  for (Int_t z = 0; z <= Nstep; ++z) {
    Float_t alpha = min + z * step;
    TH1F* ClusterNormCharge = new TH1F("cluster_norm_charge","Truncated mean energy deposit",2500,0,2500);

    for (Int_t j = 0; j < EventClusters.size(); ++j) {
      Float_t norm_cluster = 0.;
      Int_t i_max = round(alpha * EventClusters[j].size());
      Int_t v_size = (i_max < int(EventClusters[j].size())) ? i_max : int(EventClusters[j].size());
      for (int i = 0; i < v_size ; ++i)
        norm_cluster += EventClusters[j][i];

      norm_cluster *= 1 / (alpha * EventClusters[j].size());

      ClusterNormCharge->Fill(norm_cluster);
    }
    gStyle->SetOptStat("RMne");
    gStyle->SetOptFit(0111);
    ClusterNormCharge->Fit("gaus");
    TF1 *fit = ClusterNormCharge->GetFunction("gaus");
    Float_t constant  = fit->GetParameter(0);
    Float_t mean      = fit->GetParameter(1);
    Float_t sigma     = fit->GetParameter(2);

    Float_t mean_e     = fit->GetParError(1);
    Float_t sigma_e    = fit->GetParError(2);
    Float_t resol = sigma / mean;

    Float_t resol_e   = resol * sqrt(mean_e*mean_e/(mean*mean) + sigma_e*sigma_e/(sigma*sigma));
    //****************************************************************************
    int bin1 = ClusterNormCharge->FindFirstBinAbove(ClusterNormCharge->GetMaximum()/2);
    int bin2 = ClusterNormCharge->FindLastBinAbove(ClusterNormCharge->GetMaximum()/2);
    double fwhm = ClusterNormCharge->GetBinCenter(bin2) - ClusterNormCharge->GetBinCenter(bin1);
    //resol = 0.5 * fwhm / ClusterNormCharge->GetBinCenter(ClusterNormCharge->GetMaximumBin());


    cout << "****************************************************" << endl;
    cout << "alpha = " << alpha << endl;
    cout << "constant = " << constant << endl;
    cout << "mean     = " << mean << endl;
    cout << "sigma    = " << sigma << endl;
    cout << "resol    = " << resol << endl;
    cout << "resol_e  = " << resol_e << endl;



    gr->SetPoint(z, alpha, resol);

    gr->SetPointError(z, 0, resol_e);

    if (resol < best_resolution) {
      BestResol       = (TH1F*)ClusterNormCharge->Clone();
      best_alpha      = alpha;
      best_resolution = resol;
    }
  }


  gStyle->SetOptStat("RMne");
  gStyle->SetOptFit(0111);
  BestResol->Fit("gaus");
  BestResol->Draw();
  c1->Print("figure/TruncEnergyBest.png");
  gr->SetMarkerColor(4);
  gr->SetMarkerSize(0.5);
  gr->SetMarkerStyle(21);
  c1->SetGrid(1);
  gr->Draw("ap");
  c1->Print("figure/Resolution_alpha.png");


  cout << "****************************************************" << endl;
  cout << "alpha    = " << best_alpha << endl;
  cout << "resol    = " << best_resolution << endl;


  cout << "END" << endl;
}

