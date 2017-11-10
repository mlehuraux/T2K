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

  //TCanvas *c1 = new TCanvas("c1","evadc",900,700);

  // Histoes

  TH1F* ClusterCharge = new TH1F("cluster_charge","Cluster charge",100,0,10000);
  TH1F* PadPerCluster = new TH1F("pads_per_cluster","Pads per cluster",10,0,10);

  vector<TH1F*> MaximumSep;
  //vector<TH1F*> 
  MaximumSep.resize(24, NULL);
  for (int i = 0; i < 24; ++i) {
    MaximumSep[i] = new TH1F(Form("maxSep%i", i),"Maximum X separation between pads", 70, 0. , 70);
  }

  TH1F* MaximumSepEvent = new TH1F("maxSep","Maximum X separation between pads", 70, 0. , 70);
  

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

      TH2F* PadDisplay = new TH2F("PadDisplay","I vs J of hits",72,-0.5,71+0.5,24,0-0.5,23+0.5);
    
      int adc_sum = 0;
  
      for (int i = 0; i < 24; i++)
        listofRow[i]=0; 

      listofT.clear();

      TH1F* ClusterCharge_temp = new TH1F("cluster_charge_temp","Cluster charge",100,0,10000);
      TH1F* PadPerCluster_temp = new TH1F("pads_per_cluster_temp","Pads per cluster",10,0,10);

      //************************************************************
      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      //************************************************************

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
    
        adc_sum += adcmax;
   
        float weight=1;
        if (Eweight) weight=adcmax;
          
        PadDisplay->Fill((*jPad)[chan],(*iPad)[chan],adcmax);
      } //loop over channels

      int Nrow = 0;
      for (int i = 0; i < 24; i++)
        Nrow+=listofRow[i];

      if (Nrow < 1)
        continue;
      ++events_not_empty;

      // Loop over pads i j
      int MaxSepEvent = 0;
      for (int i = 0; i <= Imax; ++i) {
        int prev = 1E6;
        int MaxSepRow = -1;
        int PadPerCl = 0;
        int ChargeCl = 0;
        // int hit = 0;
        for (int j = 0; j <= Jmax; ++j) {
          int adcmax = PadDisplay->GetBinContent(j, i);
          ChargeCl += adcmax;
          if (adcmax > 0) {
            if (j - prev - 1 > MaxSepRow)
              MaxSepRow = j - prev - 1;
            prev = j;
            ++PadPerCl;
          }
        }
        if (MaxSepRow >= 0)
          MaximumSep[i]->Fill(MaxSepRow);
        if (MaxSepRow > MaxSepEvent)
          MaxSepEvent = MaxSepRow;

        if (ChargeCl>0) {
          //cout << ChargeCl << endl;
          ClusterCharge_temp->Fill(ChargeCl);  
          PadPerCluster_temp->Fill(PadPerCl);
        }
      }  // end of PAD scan

      MaximumSepEvent->Fill(MaxSepEvent);

      if (MaxSepEvent > 2)
        continue;
      ++events_one_track;

      if (Nrow < 18)
        continue;
      ++events_long;

      cout << "temp_max " << ClusterCharge_temp->GetMaximum() << endl;
      ClusterCharge->Add(ClusterCharge_temp);
      cout << "sum_max " << ClusterCharge->GetMaximum() << endl;
      PadPerCluster->Add(PadPerCluster_temp);

    } // end of loops over events
  } // end of loops over files

  cout << "Total events number     : " << events_raw << endl;
  cout << "Not empty events number : " << events_not_empty << endl;
  cout << "One track events number : " << events_one_track << endl;
  cout << "Long track events number: " << events_long << endl;

  //MaximumSepEvent->Draw();
  ClusterCharge->Draw();
  //PadPerCluster->Draw();

  cout << "END" << endl;
  if (ExitAtEnd) {getchar(); exit(0);}
}

