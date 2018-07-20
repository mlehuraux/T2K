#define THIS_NAME noise
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "ilc/common_header.h"

void noise() {
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

  TH2D* mean = new TH2D("mean_n", "Mean noise", 38,-1.,37.,50,-1.,49.);
  TH2D* sigma = new TH2D("sigma_n", "Sigma noise", 38,-1.,37.,50,-1.,49.);

  TCanvas *c1 = new TCanvas("c1","evadc",900,700);
  TString prefix = "~/T2K/figure/T2KTPC/";

  Int_t total_events = 0;
  Int_t sel_events = 0;

  vector< vector<float> > mean_adc;
  vector< vector<float> > sigma_adc;
  mean_adc.resize(35+1);
  sigma_adc.resize(35+1);
  for (int z=0; z <= 35; ++z) {
    mean_adc[z].resize(47+1, 0);
    sigma_adc[z].resize(47+1, 0);
  }

  TH2F* PadDisplay=new TH2F("PadDisplay","Maximum adc",38,-1.,37.,50,-1.,49.);
  TH2F* MeanDisplay=new TH2F("MeanDisplay","Mean noise",38,-1.,37.,50,-1.,49.);
  TH2F* SigmaDisplay=new TH2F("SigmaDisplay","Sigma noise",38,-1.,37.,50,-1.,49.);

  //************************************************************
  //****************** LOOP OVER FILES  ************************
  //************************************************************

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

    cout << "Imax = " << Imax << " Imin = " << Imin << " Jmax = " << Jmax << " Jmin = " << Jmin << endl;
    cout << "ImaxI = " << Imax << " IminI = " << Imin << " JmaxI = " << Jmax << " JminI = " << Jmin << endl;

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
      ++total_events;

      PadDisplay->Reset();
      MeanDisplay->Reset();
      MeanDisplay->Reset();

      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      bool event = false;
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];
        int adc_max = -1;
        for (uint it = 0; it < 511; it++){
          int adc= (*listOfSamples)[ic][it];
          if (adc > 400) {
            event = true;
            break;
          }
          if (adc > adc_max)
            adc_max = adc;
        }
        if (event) break;
        PadDisplay->Fill((*iPad)[chan], (*jPad)[chan], adc_max);
      } //loop over channels

      if (event)
        continue;
      ++sel_events;

      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];

        TH1D* histo =  new TH1D(Form("noise%i_%i", ievt, ic), Form("noise%i_%i", (*iPad)[chan], (*jPad)[chan]),
          120, 200., 320.);
        for (uint it = 0; it < listOfChannels->size(); it++){
          int adc= (*listOfSamples)[ic][it];
          histo->Fill(adc);
        }
        histo->Fit("gaus", "Q");
        TF1 *fit = histo->GetFunction("gaus");
        Float_t m     = fit->GetParameter(1);
        Float_t s     = fit->GetParameter(2);
        /*histo->Draw();
        gPad->Update();
        char k[20];
        cin >> k;*/
        mean_adc[(*iPad)[chan]][(*jPad)[chan]] += m;
        sigma_adc[(*iPad)[chan]][(*jPad)[chan]] += s;

        MeanDisplay->Fill((*iPad)[chan], (*jPad)[chan], m);
        SigmaDisplay->Fill((*iPad)[chan], (*jPad)[chan], s);

        histo->Reset();
        delete histo;
      } // 2nd loop over channel

      /*{
        c1->cd();
        PadDisplay->Draw("colz");
        gPad->Update();
        TCanvas *c2 = new TCanvas("c2","evadc",900,700);
        c2->cd();
        MeanDisplay->GetZaxis()->SetRangeUser(240., 265.);
        MeanDisplay->Draw("colz");
        gPad->Update();
        TCanvas *c3 = new TCanvas("c3","evadc",900,700);
        c3->cd();
        SigmaDisplay->GetZaxis()->SetRangeUser(4., 10.);
        SigmaDisplay->Draw("colz");
        gPad->Update();
        char k[67];
        cin >> k;
      }*/
    } // loop over events
  } // loop over files
  cout << endl;

  for (int it_j = 0; it_j <= 47; ++it_j) {
    for (int it_i = 0; it_i <= 35; ++it_i) {
      mean->Fill(it_i, it_j, mean_adc[it_i][it_j] / sel_events);
      sigma->Fill(it_i, it_j, sigma_adc[it_i][it_j] / sel_events);
    }
  }
  TFile* output = new TFile("data/pedestals.root", "recreate");
  output->cd();
  mean->Write();
  output->Close();

  gStyle->SetOptStat(0);
  mean->GetZaxis()->SetRangeUser(248., 258.);
  mean->Draw("colz");
  c1->Print((prefix+"MeannNoise.pdf").Data());

  sigma->GetZaxis()->SetRangeUser(2., 10.);
  sigma->Draw("colz");
  c1->Print((prefix+"SigmaNoise.pdf").Data());

  cout << "Total events number     : " << total_events << endl;
  cout << "Sel events number       : " << sel_events << endl;



  return;
}