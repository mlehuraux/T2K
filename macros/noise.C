#define THIS_NAME noise
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "ilc/common_header.h"

void noise() {
  vector<TString> listOfFiles;
  TString file="default.root";
  TString prefix = "data/";
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
    } else if (string( gApplication->Argv(iarg))=="-o" || string( gApplication->Argv(iarg))=="--output" ){
      iarg++;
      prefix = gApplication->Argv(iarg);
    }
  }

  TH2D* mean = new TH2D("mean_n", "Mean noise", 38,-1.,37.,50,-1.,49.);
  TH2D* sigma = new TH2D("sigma_n", "Sigma noise", 38,-1.,37.,50,-1.,49.);

  TH1D* mean_1d = new TH1D("mean_1d", "", 1728, 0., 1728);
  TH1D* sigma_1d = new TH1D("sigma_1d", "", 1728, 0., 1728);

  TCanvas *c1 = new TCanvas("c1","evadc",900,700);

  Int_t total_events = 0;

  vector< vector<float> > mean_adc;
  vector< vector<float> > sigma_adc;
  mean_adc.resize(35+1);
  sigma_adc.resize(35+1);
  for (int z=0; z <= 35; ++z) {
    mean_adc[z].resize(47+1, 0);
    sigma_adc[z].resize(47+1, 0);
  }

  TH2F* PadDisplay=new TH2F("PadDisplay","Maximum adc",38,-1.,37.,50,-1.,49.);

  //************************************************************
  //****************** LOOP OVER FILES  ************************
  //************************************************************

  TFile*f=0;
  //for (uint ifile=0;ifile< listOfFiles.size();ifile++){
  int ifile = 0;
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

    // create histoes for noise storing
    t->GetEntry(0);
    TH1D* histo[1728];
    for (uint ic=0; ic< listOfChannels->size(); ic++){
      histo[ic] =  new TH1D(Form("noise_%i", ic), Form("noise%i_%i", (*iPad)[ic], (*jPad)[ic]),
          500, 0., 500.);
    }

    //************************************************************
    //****************** LOOP OVER EVENTS ************************
    //************************************************************
    for (int ievt=0; ievt < Nevents ; ievt++){
      if (ievt%(Nevents/10)==0)
        cout <<"."<<flush;

      t->GetEntry(ievt);
      ++total_events;

      PadDisplay->Reset();

      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];
        for (uint it = 0; it < 511; it++){
          int adc= (*listOfSamples)[ic][it];
          histo[chan]->Fill(adc);
          //cout << adc << endl;
        }
      } //loop over channels
    } // loop over events
  //} // loop over files
  cout << endl;

  ofstream ped_file;
  ped_file.open((prefix + "pedestals.txt").Data());

  TFile* output = new TFile((prefix + "pedestals.root").Data(), "recreate");
  output->cd();

  for (uint ic=0; ic< listOfChannels->size(); ic++){
    Float_t max = histo[ic]->GetMaximum();
    Float_t x_max = histo[ic]->GetBinCenter(histo[ic]->GetMaximumBin());
    Int_t start_bin = histo[ic]->FindFirstBinAbove(max / 2.);
    Int_t end_bin = histo[ic]->FindLastBinAbove(max / 2.);
    Float_t hwhm = histo[ic]->GetBinCenter(end_bin) - histo[ic]->GetBinCenter(start_bin);
    hwhm *= 0.5;

    histo[ic]->Fit("gaus", "Q", "", x_max - 2*hwhm, x_max + 2*hwhm);
    TF1* fit = histo[ic]->GetFunction("gaus");

    Float_t mean_t  = 0.;
    Float_t sigma_t = 0.;
    if (!fit)
      cout << "ALARM! No fit fuction for channel " << ic << endl;
    else {
      mean_t  = fit->GetParameter(1);
      sigma_t = fit->GetParameter(2);

      if (mean_t < 0 || sigma_t < 1 || mean_t > 1000 || sigma_t > 20) {
        histo[ic]->Fit("gaus", "Q", "", x_max - 2*hwhm, x_max + 2*hwhm);
        fit = histo[ic]->GetFunction("gaus");
        mean_t  = fit->GetParameter(1);
        sigma_t = fit->GetParameter(2);
      }
      if (mean_t < 0 || sigma_t < 1 || mean_t > 1000 || sigma_t > 20) {
        histo[ic]->Fit("gaus", "Q", "", x_max - 2*hwhm, x_max + 2*hwhm);
        fit = histo[ic]->GetFunction("gaus");
        mean_t  = fit->GetParameter(1);
        sigma_t = fit->GetParameter(2);
      }
      if (mean_t < 0 || sigma_t < 1 || mean_t > 1000 || sigma_t > 20) {
        histo[ic]->Fit("gaus", "Q", "", x_max - 2*hwhm, x_max + 2*hwhm);
        fit = histo[ic]->GetFunction("gaus");
        mean_t  = fit->GetParameter(1);
        sigma_t = fit->GetParameter(2);
      }
      if (mean_t < 0 || sigma_t < 1 || mean_t > 1000 || sigma_t > 20) {
        histo[ic]->Fit("gaus", "Q", "", x_max - 2*hwhm, x_max + 2*hwhm);
        fit = histo[ic]->GetFunction("gaus");
        mean_t  = fit->GetParameter(1);
        sigma_t = fit->GetParameter(2);
      }
    }

    mean->Fill((*iPad)[ic], (*jPad)[ic], mean_t);
    sigma->Fill((*iPad)[ic], (*jPad)[ic], sigma_t);

    mean_1d->Fill(ic, mean_t);
    sigma_1d->Fill(ic, sigma_t);

    ped_file << ic << "\t" << mean_t << "\t" << sigma_t  << "\t" << mean_t + 4 * sigma_t << endl;

    //cout << ic << "\t" << (*iPad)[ic] << "\t" << (*jPad)[ic] << "\t" << mean_t << "\t" << sigma_t << endl;
  }
  ped_file.close();
  output->cd();

  mean->SetDrawOption("colz");
  sigma->SetDrawOption("colz");
  mean->Write();
  sigma->Write();

  mean_1d->SetDrawOption("hist");
  sigma_1d->SetDrawOption("hist");
  mean_1d->Write();
  sigma_1d->Write();

  for (uint chan=0; chan< listOfChannels->size(); chan++){
    histo[chan]->Write();
  }

  output->Close();

  gStyle->SetOptStat(0);
  //mean->GetZaxis()->SetRangeUser(248., 258.);
  mean->Draw("colz");
  c1->Print((prefix+"Meann2D.pdf").Data());

  mean_1d->Draw("hist");
  c1->Print((prefix+"Meann1D.pdf").Data());

  //sigma->GetZaxis()->SetRangeUser(2., 10.);
  sigma->Draw("colz");
  c1->Print((prefix+"Sigma2D.pdf").Data());
  sigma_1d->Draw("hist");
  c1->Print((prefix+"Sigma1D.pdf").Data());

  exit(0);

  return;
}