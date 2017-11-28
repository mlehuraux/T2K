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

  // LOOP OVER FILES
  for (Int_t fileID = 5149; fileID < 5161; ++fileID) {
    std::stringstream stream;
    stream << fileID;
    std::string strRUN = stream.str();

    // initialise files/trees/vars for all FEMs
    TFile* inFile[7];
    TTree* t[7];

    std::string file_name0  = "data_root/RUN_0" + strRUN + "_fem0.root";
    inFile[0]  = new TFile(file_name0.c_str(), "READ");
    if (!inFile[0]->IsOpen())
      cout << " WARNING problem in opening file " << file_name0 << endl;

    cout << endl;
    cout << "Run over: " << file_name0 << endl;

    TTree* tconfig  = (TTree*)inFile[0]->Get("beam_config");
    TTree* tgeom    = (TTree*) inFile[0]->Get("femGeomTree");

    t[0] = (TTree*)inFile[0]->Get("padData");

    vector<short>          *listOfChannels[7];
    vector<vector<short> > *listOfSamples[7];

    listOfChannels[0] = new vector<short>(0);
    listOfSamples[0] = new vector<vector<short> >(0);

    t[0]->SetBranchAddress("PadphysChannels", &listOfChannels[0] );
    t[0]->SetBranchAddress("PadADCvsTime"   , &listOfSamples[0] );


    std::string out_file_name = "data_root/RUN_0" + strRUN + "_multi.root";
    TFile* outFile  = new TFile(out_file_name.c_str(), "RECREATE");

    TTree* config   = tconfig->CloneTree();
    TTree* geom     = tgeom->CloneTree();
    config->Write();
    geom->Write();
    cout << endl;
    cout << "Creating file: " << out_file_name << endl;

    TTree* pad      = new TTree("pad", "pad");
    vector<vector<short> >  pad_charge[8];
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
    for (Int_t femID = 1; femID < 7; ++femID) {
      stream.str("");
      stream << femID;
      std::string strFEM = stream.str();
      std::string file_name = "data_root/RUN_0" + strRUN + "_fem" + strFEM + ".root";
      inFile[femID]  = new TFile(file_name.c_str(), "READ");

      // READ PAD DATA
      t[femID] = (TTree*) inFile[femID]->Get("padData");

      listOfChannels[femID] = new vector<short>(0);
      listOfSamples[femID] = new vector<vector<short> >(0);

      t[femID]->SetBranchAddress("PadphysChannels", &listOfChannels[femID] );
      t[femID]->SetBranchAddress("PadADCvsTime"   , &listOfSamples[femID] );
    }


    //************************************************************
    //************************************************************
    //****************** LOOP OVER EVENTS ************************
    //************************************************************
    //************************************************************
    int Nevents=t[0]->GetEntries();
    cout << "[          ] Nev="<<Nevents<<"\r[";
    for (int ievt=0; ievt < Nevents ; ievt++) {
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;

      // LOOP OVER FEMs
      for (Int_t femID = 0; femID < 7; ++femID) {
        //inFile[femID]->cd();
        t[0]->GetEntry(ievt);
        //cout << "OK" << endl;

        pad_charge[femID].clear();
        pad_charge[femID].resize(Imax+1);
        for (int z=0; z <= Imax; ++z)
          pad_charge[femID][z].resize(Jmax+1, 0);

        pad_charge[7].clear();
        pad_charge[7].resize(Imax+1);
        for (int z=0; z <= Imax; ++z)
          pad_charge[7][z].resize(Jmax+1, 0);

        //cout << pad_charge[0][0][0] << endl;
        //cout << pad_charge[0][5][31] << endl;


        //************************************************************
        //************************************************************
        //*****************LOOP OVER CHANNELS ************************
        //************************************************************
        //************************************************************

        for (uint ic=0; ic< listOfChannels[femID]->size(); ic++){
          int chan= (*listOfChannels[femID])[ic];
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

          int i = (*iPad)[chan];
          int j = (*jPad)[chan];

          pad_charge[7][i][j] += adcmax;

        } //loop over channels

        for (Int_t i = 0; i < 24; ++i) {\
          vector<short> line;
          for (Int_t j = 0; j < 72; ++j) {
            //cout << pad_charge[7][i][j] << " ";
            line.push_back(pad_charge[7][i][j]);
          }
          //cout << endl;
          pad_charge[femID].push_back(line);
        }


      } // loop over FEMs
      pad->Fill();
    } // loop over events

    outFile->cd();
    pad->Write();
    outFile->Close();
  }
  cout << endl;
  cout << "END" << endl;
  exit(1);
}