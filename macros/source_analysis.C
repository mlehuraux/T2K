#define THIS_NAME source_analysis
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "ilc/common_header.h"

void source_analysis() {
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

  TH1F* ClusterCharge     = new TH1F("cluster_charge","Cluster charge",100,0,5000);
  TH2F* PadDisplayH       = new TH2F("PadDisplay","I vs J of hits",38,-1.,37.,50,-1.,49.);

  Int_t total_events = 0;
  Int_t sel_events = 0;
  Int_t filled_events = 0;
  Int_t doted_events = 0;

  TFile* ped_file = new TFile("data/pedestals.root", "READ");
  TH2F* pedestals = (TH2F*)ped_file->Get("mean_n");

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

    gStyle->SetPalette(1);
    //gStyle->SetOptStat(0);
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
      PadDisplayH->Reset();

      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];
        // find out the maximum
        float adcmax=-1;
        // one maximum per channel
        for (uint it = 0; it < (*listOfSamples)[ic].size(); it++){
          float ped = pedestals->GetBinContent((*iPad)[chan]+2,(*jPad)[chan]+2);
          float adc = (*listOfSamples)[ic][it] - ped;
          if (adc>adcmax) {
            adcmax = adc;
          }
        }

        //PadDisplay[(*jPad)[chan]][(*iPad)[chan]] = adcmax;
        PadDisplayH->Fill((*iPad)[chan],(*jPad)[chan],adcmax);

      } //loop over channels
      ++total_events;

      if (PadDisplayH->GetMaximum() < 20)
        continue;

      ++filled_events;

      Int_t locmax, locmay, locmaz;
      Int_t thr = 400;
      PadDisplayH->GetMaximumBin(locmax, locmay, locmaz);

      if (abs(locmax-17.5) > 12 || abs(locmay-22.5) > 15)
        continue;

      ++doted_events;

      if (PadDisplayH->GetBinContent(locmax+2, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay+3) > thr ||
          PadDisplayH->GetBinContent(locmax+2, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax+2, locmay+3) > thr ||
          PadDisplayH->GetBinContent(locmax+1, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax+1, locmay+3) > thr ||
          PadDisplayH->GetBinContent(locmax+2, locmay+1) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay+1) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay+3) > thr ||

          PadDisplayH->GetBinContent(locmax-2, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay+3) > thr ||
          PadDisplayH->GetBinContent(locmax-2, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax-2, locmay+3) > thr ||
          PadDisplayH->GetBinContent(locmax-1, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax-1, locmay+3) > thr ||
          PadDisplayH->GetBinContent(locmax-2, locmay+1) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay+1) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay+2) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay+3) > thr ||

          PadDisplayH->GetBinContent(locmax+2, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay-3) > thr ||
          PadDisplayH->GetBinContent(locmax+2, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax+2, locmay-3) > thr ||
          PadDisplayH->GetBinContent(locmax+1, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax+1, locmay-3) > thr ||
          PadDisplayH->GetBinContent(locmax+2, locmay-1) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay-1) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax+3, locmay-3) > thr ||

          PadDisplayH->GetBinContent(locmax-2, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax, locmay-3) > thr ||
          PadDisplayH->GetBinContent(locmax-2, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax-2, locmay-3) > thr ||
          PadDisplayH->GetBinContent(locmax-1, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax-1, locmay-3) > thr ||
          PadDisplayH->GetBinContent(locmax-2, locmay-1) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay-1) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay-2) > thr ||
          PadDisplayH->GetBinContent(locmax-3, locmay-3) > thr) continue;

      ++sel_events;

      Int_t charge = PadDisplayH->GetBinContent(locmax, locmay);
      Int_t thr2 = 0.;
      Int_t c;
      c = PadDisplayH->GetBinContent(locmax+1, locmay); if (c > thr2) charge += c;
      c = PadDisplayH->GetBinContent(locmax+1, locmay+1); if (c > thr2) charge += c;
      c = PadDisplayH->GetBinContent(locmax, locmay+1); if (c > thr2) charge += c;
      c = PadDisplayH->GetBinContent(locmax-1, locmay-1); if (c > thr2) charge += c;
      c = PadDisplayH->GetBinContent(locmax+1, locmay-1); if (c > thr2) charge += c;
      c = PadDisplayH->GetBinContent(locmax-1, locmay+1); if (c > thr2) charge += c;
      c = PadDisplayH->GetBinContent(locmax, locmay-1); if (c > thr2) charge += c;
      c = PadDisplayH->GetBinContent(locmax-1, locmay); if (c > thr2) charge += c;

      ClusterCharge->Fill(charge);

      /*
      gStyle->SetOptStat(0);
      PadDisplayH->Draw("colz");
      gPad->Update();
      cout << endl;
      cout << locmax << "  " << locmay << endl;
      cout << PadDisplayH->GetBinContent(locmax, locmay) << endl;
      cout << PadDisplayH->GetBinContent(locmax, locmay-1) << endl;
      cout << PadDisplayH->GetBinContent(locmax, locmay+1) << endl;
      cout << "charge = " << charge << endl;
      char k[5];
      cin >> k;
      */


    } // loop over events
  } // loop over files
  cout << endl;

  gStyle->SetOptStat("rmne");
  gStyle->SetOptFit();

  ClusterCharge->Fit("gaus", "", "", 1500., 3000.);

  ClusterCharge->Draw();
  c1->Print((prefix+"SourceCharge.pdf").Data());

  cout << "Total events number     : " << total_events << endl;
  cout << "Not empty events        : " << filled_events << endl;
  cout << "Centered events         : " << doted_events << endl;
  cout << "Track events number     : " << sel_events << endl;

}