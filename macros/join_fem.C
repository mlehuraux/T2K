#define THIS_NAME join_fem
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "common_header.h"

const int thr = 100;
const int NFem = 1;

void join_fem() {

  //************************************************************
  //************************************************************
  //****************** LOOP OVER FILES  ************************
  //************************************************************
  //************************************************************
  int events_raw        = 0;
  int events_not_empty  = 0;
  int events_long       = 0;
  int events_one_track  = 0;

  // LOOP OVER FILES
  for (Int_t fileID = 5149; fileID < 5161; ++fileID) {
    std::stringstream stream;
    stream << fileID;
    std::string strRUN = stream.str();

    // initialise files/trees/vars for all FEMs
    /*TFile* inFile[7];
    TTree* t[7];
*/


    std::string file_name0  = "data_root/RUN_0" + strRUN + "_fem0.root";
    TFile* f  = new TFile(file_name0.c_str(), "READ");
    if (!f->IsOpen())
      cout << " WARNING problem in opening file " << file_name0 << endl;

    cout << endl;
    cout << "Run over: " << file_name0 << endl;

    TTree* config;
    //config = (TTree*)f->Get("beam_config");
    TTree * tgeom= (TTree*) f->Get("femGeomTree");



    int Nevents1=2;
    TTree *t = (TTree*)f->Get("padData");


    vector<short>          *listOfChannels;
    //vector<vector<short> > *listOfSamples[7];

    t->SetBranchAddress("PadphysChannels", &listOfChannels );
    //t[0]->SetBranchAddress("PadADCvsTime"   , &listOfSamples[0] );

    vector<vector<short> >  pad_charge[7];

/*    std::string out_file_name = "data_root/RUN_0" + strRUN + "_multi.root";
    TFile* outFile  = new TFile(out_file_name.c_str(), "RECREATE");

    TTree* config   = tconfig->CloneTree();
    TTree* geom     = tgeom->CloneTree();
    config->Write();
    geom->Write();
    cout << endl;
    cout << "Creating file: " << out_file_name << endl;


    TTree* pad      = new TTree("pad", "pad");

    vector<vector<short> >  pad_charge[7];
    pad->Branch("pad0",     &pad_charge[0]);
    pad->Branch("pad1",     &pad_charge[1]);
    pad->Branch("pad2",     &pad_charge[2]);
    pad->Branch("pad3",     &pad_charge[3]);
    pad->Branch("pad4",     &pad_charge[4]);
    pad->Branch("pad5",     &pad_charge[5]);
    pad->Branch("pad6",     &pad_charge[6]);


    inFile[0]->cd();

    vector<int> *iPad(0);
    vector<int> *jPad(0);
    int Jmin, Imin, Jmax, Imax;
    tgeom->SetBranchAddress("jPad", &jPad );
    tgeom->SetBranchAddress("iPad", &iPad );
    tgeom->SetBranchAddress("jPadMin", &Jmin );
    tgeom->SetBranchAddress("iPadMin", &Imin );
    tgeom->SetBranchAddress("jPadMax", &Jmax );
    tgeom->SetBranchAddress("iPadMax", &Imax );
    tgeom->GetEntry(0); // put into memory geometry info

    // LOOP OVER FEMs
    /*for (Int_t femID = 1; femID < 7; ++femID) {
      stream.str("");
      stream << femID;
      std::string strFEM = stream.str();
      std::string file_name = "data_root/RUN_0" + strRUN + "_fem" + strFEM + ".root";
      inFile[femID]  = new TFile(file_name.c_str(), "READ");
      //inFile[femID]->ls();

      // READ PAD DATA
      t[femID] = (TTree*) inFile[femID]->Get("padData");
      //t[femID]->Print();

      //t[femID]->SetBranchAddress("PadphysChannels", &listOfChannels[femID] );
      //t[femID]->SetBranchAddress("PadADCvsTime"   , &listOfSamples[femID] );
    }*/


    //************************************************************
    //************************************************************
    //****************** LOOP OVER EVENTS ************************
    //************************************************************
    //************************************************************
    int Nevents=t->GetEntries();
    cout << "[          ] Nev="<<Nevents<<"\r[";
    cout << Nevents << endl;
    for (int ievt=0; ievt < Nevents ; ievt++) {
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;

      // LOOP OVER FEMs
      //for (Int_t femID = 0; femID < 7; ++femID) {
        //cout << "ev = " << ievt << "   FEM = " << femID << endl;
        //inFile[femID]->cd();
        t->GetEntry(ievt);
        cout << "OK" << endl;

        /*pad_charge[femID].clear();
        pad_charge[femID].resize(Jmax+1);
        for (int z=0; z <= Jmax; ++z)
          pad_charge[femID][z].resize(Imax+1, 0);


        //************************************************************
        //************************************************************
        //*****************LOOP OVER CHANNELS ************************
        //************************************************************
        //************************************************************

        for (uint ic=0; ic< listOfChannels->size(); ic++){
          int chan= (*listOfChannels)[ic];
          // find out the maximum
          float adcmax=-1;

          // one maximum per channel
          for (uint it = 0; it < (*listOfSamples[femID])[ic].size(); it++){
            int adc= (*listOfSamples[femID])[ic][it];
            if (adc>thr)
              if (adc>adcmax) {
                adcmax=adc;
              }
          }

          if (adcmax<0) continue; // remove noise

          pad_charge[femID][(*jPad)[chan]][(*iPad)[chan]] += adcmax;
        } //loop over channels*/
      //} // loop over FEMs
      //pad->Fill();
    } // loop over events

    //outFile->cd();
    //pad->Write();
    //outFile->Close();
  }
  cout << endl;
  cout << "END" << endl;
  exit(1);
}