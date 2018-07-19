#define THIS_NAME pad_scan
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "ilc/common_header.h"

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

void  pad_scan(){

  bool DEBUG = false;

  int thr=0.;     //seuil
  int nbins=100;
  int Ndefaultevents=0;
  // int cut_adc=400000;
  int adc_min = 200;
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
      cout << "adding filename" <<" " << file << endl;
      listOfFiles.push_back(file);
    }
    else if (string( gApplication->Argv(iarg))=="-q"){
      ExitAtEnd=1;
    }
    else if (string( gApplication->Argv(iarg))=="-w"){
      Eweight=1;
    }else if (string( gApplication->Argv(iarg))=="-d"){
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

  TH1F* RowNumber = new TH1F("nRow","Number of row", 24, 0. , 24);

  vector<TH1F*> MaximumSep;
  MaximumSep.resize(24, NULL);
  for (int i = 0; i < 24; ++i) {
    MaximumSep[i] = new TH1F(Form("maxSep%i", i),"Maximum X separation between pads", 24, 0. , 24);
  }

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

    vector<double> *xPad(0);
    vector<double> *yPad(0);

    vector<double> *dxPad(0);
    vector<double> *dyPad(0);

    TTree * tgeom= (TTree*) f->Get("femGeomTree");
    tgeom->SetBranchAddress("jPad", &jPad );
    tgeom->SetBranchAddress("iPad", &iPad );

    tgeom->SetBranchAddress("xPad", &xPad );
    tgeom->SetBranchAddress("yPad", &yPad );

    tgeom->SetBranchAddress("dxPad", &dxPad );
    tgeom->SetBranchAddress("dyPad", &dyPad );

    tgeom->GetEntry(0); // put into memory geometry info
    cout << "reading geometry" << endl;
    cout << "jPad->size() " << jPad->size() <<endl;
    cout << "iPad->size() " << iPad->size() <<endl;

    int Imax = -1;
    int Imin = 10000000;
    int Jmax = -1;
    int Jmin = 10000000;

    int ImaxI, IminI, JmaxI, JminI;

    for (unsigned long i = 0; i < jPad->size(); ++i) {
      if (Imax < (*iPad)[i]) {
        Imax = (*iPad)[i];
        ImaxI = i;
      }
      if (Imin > (*iPad)[i]) {
        Imin = (*iPad)[i];
        IminI = i;
      }
      if (Jmax < (*jPad)[i]) {
        Jmax = (*jPad)[i];
        JmaxI = i;
      }
      if (Jmin > (*jPad)[i]) {
        Jmin = (*jPad)[i];
        JminI = i;
      }
    }

    if (DEBUG) {
      cout << "Imax = " << Imax << " Imin = " << Imin << " Jmax = " << Jmax << " Jmin = " << Jmin << endl;
      cout << "ImaxI = " << Imax << " IminI = " << Imin << " JmaxI = " << Jmax << " JminI = " << Jmin << endl;
    }

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

    int listofRow[24]; //to count how many rows have been hit

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

      TH2F* PadDisplay=new TH2F("PadDisplay","I vs J of hits",38,-1.,37.,50,-1.,49.);
      TH2F* PadDisplayGeo=new TH2F("PadDisplayGeo","X vs Y of hits",50,-17.15,17.15, 38,-18.13,18.13);

      for (int i = 0; i < 24; i++)
        listofRow[i]=0;

      listofT.clear();

      cout << "channel size " << listOfChannels->size() << endl;
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];
        if (DEBUG) {
          cout << "channel " << chan << endl;
          cout << "i = " << (*iPad)[chan] << "   j = " << (*jPad)[chan] << endl;
        }
        // find out the maximum
        float adcmax=-1;
        int itmax=-1;

        if (DEBUG)
          cout << "ADC size " << (*listOfSamples)[ic].size() <<endl;

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
        listofT.push_back(itmax);
        listofRow[(*iPad)[chan]]=1; //the row has been hit

        PadDisplay->Fill((*iPad)[chan],(*jPad)[chan],adcmax);
        if (ic > 1727)
          continue;
        // ommit the borders
        if ((*iPad)[chan] == Imin || (*iPad)[chan] == Imax ||
            (*jPad)[chan] == Jmin || (*jPad)[chan] == Jmax)
            continue;
        PadDisplayGeo->Fill((*xPad)[chan],(*yPad)[chan], adcmax);
      } //loop over channels

      //determine number of rows:
      int Nrow = 0;
      for (int i = 0; i < 24; i++)
        Nrow+=listofRow[i];

      RowNumber->Fill(Nrow);

      TCanvas* c1 = new TCanvas("can1", "can1");
      c1->cd();
      PadDisplay->Draw("colz");
      gPad->Update();
      //TCanvas* c2 = new TCanvas("can2", "can2");
      PadDisplayGeo->Fit("pol1");
      TF1* fit = PadDisplayGeo->GetFunction("pol1");
      if(fit)
        cout << "Chi1/NDOF                 = " << fit->GetChisquare() / fit->GetNDF() << endl;
      //PadDisplayGeo->Draw("colz");
      //gPad->Update();
      /*cout << "Draw ADC?" << endl;
      char res;
      cin >> res;
      if (res == 'y') {
        bool cont_B = true;
        while (cont_B) {
          int j_int = 0;
          int i_int = 0;
          cout << "Enter j" << endl;
          cin >> j_int;
          cout << "Enter i" << endl;
          cin >> i_int;

          cout << j_int <<  "     " << i_int << endl;
          int channel = -1;
          for (unsigned long i = 0; i < iPad->size(); ++ i) {
            if ((*iPad)[i] == i_int && (*jPad)[i] == j_int) {
              channel = i;
              break;
            }
          }

          int it = -1;
          for (uint ic=0; ic< listOfChannels->size(); ic++){
            if (channel == (*listOfChannels)[ic])
              it = ic;
          }
          cout << "channel " << channel << endl;
          cout << "channel number " << (*listOfSamples)[it].size() << endl;

          TH1F* adc1=new TH1F("adc1","adc",(*listOfSamples)[it].size(),0,(*listOfSamples)[it].size());

          for (unsigned long i = 0; i < (*listOfSamples)[it].size(); ++i) {
            adc1->SetBinContent(i, 0);
            if ((*listOfSamples)[it][i] > 0){
              Double_t content = (*listOfSamples)[it][i];
              cout << it << "   " <<  i << "  from "  << (*listOfSamples)[it].size() << "    " << content << endl;
              adc1->SetBinContent(i, content);
            }
          }
          adc1->Draw();
          gPad->Update();

          cout << "Continue?" << endl;
          char cont;
          cin >> cont;
          if (cont == 'n')
            cont_B = false;
        }
      }*/

      PadDisplay->Reset();

      cout << " enter to go on" <<endl;
      char k[25];
      cin >> k;
    }
  }
  if (ExitAtEnd) {getchar(); exit(0);}
}

