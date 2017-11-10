#define THIS_NAME pad_scan
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

void  pad_scan(){

  bool DEBUG = false;

  int thr=100;     //seuil
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
    else if (string( gApplication->Argv(iarg))=="-d"){
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

  // Dfinition des histos 
  TH1F* hout=new TH1F("htime","Time",nbins,0,600);
  hout->Sumw2();
  TH1F* hadc=new TH1F("hadc","hadc",500,0,5000);
  // TH1F* evadc=new TH1F("evadc","evadc",500,0,10000);
  // TH1F* adc=new TH1F("adc","adc",100,0,500000);
  //TH2F* PadDisplay=new TH2F("PadDisplay","I vs J of hits",72,-0.5,71+0.5,24,0-0.5,23+0.5);
  TH1F* htmin=new TH1F("htmin","tmin for non contained track",100,0,100);
  TH1F* htmax=new TH1F("htmax","tmax for non contained track",100,280,380);

  TH1F* RowNumber = new TH1F("nRow","Number of row", 24, 0. , 24);

  vector<TH1F*> MaximumSep;
  //vector<TH1F*> 
  MaximumSep.resize(24, NULL);
  for (int i = 0; i < 24; ++i) {
    MaximumSep[i] = new TH1F(Form("maxSep%i", i),"Maximum X separation between pads", 24, 0. , 24);
  }
  

  //TH2F* PadSelection=new TH2F("PadSelection","I vs J of hits",72,-0.5,71+0.5,24,0-0.5,23+0.5);

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


    // TODO Think about it
    /*for (uint i=0; i<jPad->size(); i++){
       cout << " i " << i << " jPad " << (*jPad)[i] << endl;
    }*/


    gStyle->SetPalette(1);
    gStyle->SetOptStat(0);
    int Nevents=Ndefaultevents;
    TTree *t = (TTree*) f->Get("padData");
    cout <<t->GetEntries() << " evts in file " << endl;

    vector<short>          *listOfChannels(0);
    vector<vector<short> > *listOfSamples(0);

    t->SetBranchAddress("PadphysChannels", &listOfChannels );
    t->SetBranchAddress("PadADCvsTime"   , &listOfSamples );

    // float good_evt = 0;
    // float adc_tot = 0;
    int nb_evt = 0;

 
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

      TH2F* PadDisplay=new TH2F("PadDisplay","I vs J of hits",72,-0.5,71+0.5,24,0-0.5,23+0.5);
    
      int adc_sum = 0;
      int adc_side = 0 ;
      int adc_up = 0 ;
      int adc_down = 0 ;
      int adc_calc = 0;
      
    
      //  PadDisplay->Reset();
      //PadSelection->Reset();

  
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
    
        /*
        if (((*jPad)[chan]<2) || ((*jPad)[chan]>69)) {
          adc_side += adcmax ;
        } else {adc_sum += adcmax;}
        if ((*iPad)[chan]<2) {
          adc_down += adcmax;
        } else if ((*iPad)[chan]>21) {
          adc_up += adcmax;
        } else {adc_calc += adcmax;}
        */
        adc_sum += adcmax;
   
        float weight=1;
        if (Eweight) weight=adcmax;
        if (adcmax>thr) {hout->Fill(itmax,weight); hadc->Fill(adcmax);}
          
        PadDisplay->Fill((*jPad)[chan],(*iPad)[chan],adcmax);
      } //loop over channels

      // Loop over pads i j
      for (int i = 0; i < Imax; ++i) {
        int prev = 1E6;
        int MaxSep = -1;
        // int hit = 0;
        for (int j = 0; j < Jmax; ++j) {
          if (PadDisplay->GetBinContent(i, j) > 0) {
            if (j - prev - 1 > MaxSep)
              MaxSep = j - prev - 1;
            prev = j;
          }
        }
        if (MaxSep >= 0)
          MaximumSep[i]->Fill(MaxSep);
      } 

      //determine number of rows:
      int Nrow = 0;
      for (int i = 0; i < 24; i++)
        Nrow+=listofRow[i];
      
      RowNumber->Fill(Nrow);

      /*if (Nrow > 15) {
  
        if (adc_side <= 0)  adc->Fill(adc_sum);
        //cout << " adcsum " << adc_sum << endl ;
        if ((adc_side<=0) && (adc_sum<cut_adc)){
          //cout << " good event " << endl ;
          adc_tot += adc_sum ;
          good_evt +=1 ;
          evadc-> Fill(ievt,adc_sum);
        }
      }*/
  
      //if (adc_side > 0){}
      //cout << ievt << " adc_side " << adc_side << endl ;
      if (adc_side <= 0){         
        //cout << " adc_down " << adc_down << " adc_up " << adc_up << endl; 
        if (((adc_up > 0) && (adc_down <= 0) && (adc_calc > adc_min)) || ((adc_up <= 0) && (adc_down > 0) && (adc_calc > adc_min))){
          cout << "evenement semi_contenu "<< ievt/2+1 << " adc_up " << adc_up << " adc_down " << adc_down << endl;
          nb_evt += 1;
      
          std::sort(listofT.begin(),listofT.end()); 
          int   tmin=listofT[0];
          int   tmax=listofT[listofT.size()-1];
  
          cout << "       Tmin Tmax of Track " << tmin <<   "  "  <<tmax << endl;;
          if (tmax >200) htmax->Fill(tmax);
          if (tmin <200) htmin->Fill(tmin);
  
          //gPad->Update();
          //getchar();
        }
      }
    

      if (ievt<10){
        cout << " event " << ievt  <<endl;
        //TCanvas* c1 = new TCanvas();
        //c1->Clear();
        PadDisplay->Draw("colz"); 
        //c1->Print("../pad.png");
        //PadSelection->Draw("colz");
        gPad->Update();
        cout << "Draw ADC?" << endl;
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
            cout << channel << endl;
            cout << (*listOfSamples)[it].size() << endl;

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
            //c1->Print("../adc.png");

            cout << "Continue?" << endl;
            char cont;
            cin >> cont;
            if (cont == 'n')
              cont_B = false;
          }
        }

        cout << " enter to go on" <<endl;
        getchar();
      }

      if (adc_sum > 0) {
      }
    }
  }
  if (ExitAtEnd) {getchar(); exit(0);}
}

