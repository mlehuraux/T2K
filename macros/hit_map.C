#define THIS_NAME hit_map
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "common_header.h"

void hit_map() {
  vector<TString> listOfFiles;
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
  }

  TCanvas *c1 = new TCanvas("c1","evadc",900,700);
  TString prefix = "~/T2K/figure/T2KTPC/";

  // define output
  TH2F* PadDisplayHIT=new TH2F("PadDisplay","I vs J of hits",38,-1.,37.,50,-1.,49.);
  TH2F* PadDisplayMAX=new TH2F("PadDisplay","I vs J of hits",38,-1.,37.,50,-1.,49.);

  //************************************************************
  //****************** LOOP OVER FILES  ************************
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

    cout << "J goes " << Jmin << "   " << Jmax << endl;
    cout << "I goes " << Imin << "   " << Imax << endl;

    gStyle->SetPalette(1);
    gStyle->SetOptStat(0);
    int Nevents=0;
    TTree *t = (TTree*) f->Get("padData");
    cout <<t->GetEntries() << " evts in file " << endl;

    vector<short>          *listOfChannels(0);
    vector<vector<short> > *listOfSamples(0);

    t->SetBranchAddress("PadphysChannels", &listOfChannels );
    t->SetBranchAddress("PadADCvsTime"   , &listOfSamples );

    if (Nevents<=0) Nevents=t->GetEntries();
    if (Nevents>t->GetEntries()) Nevents=t->GetEntries();

    cout << "[          ] Nev="<<Nevents<<"\r[";

    //************************************************************
    //****************** LOOP OVER EVENTS ************************
    //************************************************************
    for (int ievt=0; ievt < Nevents ; ievt++){
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;

      t->GetEntry(ievt);

      vector< vector<int> > PadDisplay;
      PadDisplay.resize(Jmax+1);
      for (int z=0; z <= Jmax; ++z)
        PadDisplay[z].resize(Imax+1, 0);

      //for (int i = 0; i < 24; i++)
      //  listofRow[i]=0;

      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];
        // find out the maximum
        float adcmax=-1;

        // one maximum per channel
        for (uint it = 0; it < (*listOfSamples)[ic].size(); it++){
          int adc= (*listOfSamples)[ic][it];
          if (adc>adcmax) {
            adcmax=adc;
            itmax=it;
          }
        }

        if (adcmax<0) continue; // remove noise

        //listofRow[(*iPad)[chan]]=1; //the row has been hit

        PadDisplay[(*jPad)[chan]][(*iPad)[chan]] = adcmax;
        if (adcmax > 0)
          PadDisplayHIT->Fill((*iPad)[chan], (*jPad)[chan], 1);
        PadDisplayMAX->Fill((*iPad)[chan], (*jPad)[chan], adcmax);
      } //loop over channels
    } // loop over events
  } // loop over files

  PadDisplayHIT->Draw("colz");
  c1->Print((prefix+"pad_scan_hit.pdf").Data());
  PadDisplayMAX->Draw("colz");
  c1->Print((prefix+"pad_scan_max.pdf").Data());
  return;
}