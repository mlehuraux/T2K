#define THIS_NAME ana
#define NOINTERACTIVE_OUTPUT
#define OVERRIDE_OPTIONS

#include "ilc/common_header.h"

void ana() {
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

  int thr  = 100.;
  double alpha = 0.7;
  TCanvas *c1 = new TCanvas("c1","evadc",900,700);
  TString prefix = "~/T2K/figure/T2KTPC/";
  TH2F* PadDisplayGeo=new TH2F("PadDisplayGeo","X vs Y of hits",50,-17.15,17.15, 38,-18.13,18.13);

  TH1F* ClusterCharge     = new TH1F("cluster_charge","Cluster charge",150,0,7000);
  TH1F* ClusterNormCharge = new TH1F("cluster_norm_charge","Truncated mean energy deposit",150,0,4000);

  TH1F* TimeMaximum       = new TH1F("time_max", "time of the maximum", 530, 0., 530.);
  TH1F* TimeMaximumL      = new TH1F("time_maxL", "time of the maximum", 100, 0., 530.);

  TH2F* PadDisplayHIT     = new TH2F("PadDisplay","I vs J of hits",38,-1.,37.,50,-1.,49.);
  TH2F* PadDisplayMAX     = new TH2F("PadDisplay","I vs J of hits",38,-1.,37.,50,-1.,49.);

  TH1F* TriggerX          = new TH1F("triggerX", "Trigger X", 40, -20., 20.);

  TH1F* PadsPerCluster    = new TH1F("PadsPerCluster", "ppc", 6, 0., 6.);

  Int_t total_events = 0;
  Int_t sel_events = 0;
  Int_t filled_events = 0;

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

      // clean for the next event
      PadDisplayGeo->Reset();

      vector< vector<int> > PadDisplay;
      PadDisplay.resize(Jmax+1);
      for (int z=0; z <= Jmax; ++z)
        PadDisplay[z].resize(Imax+1, 0);


      //************************************************************
      //*****************LOOP OVER CHANNELS ************************
      //************************************************************
      for (uint ic=0; ic< listOfChannels->size(); ic++){
        int chan= (*listOfChannels)[ic];
        // find out the maximum
        float adcmax=-1;
        Int_t it_max = -1;

        // one maximum per channel
        for (uint it = 0; it < (*listOfSamples)[ic].size(); it++){
          int adc= (*listOfSamples)[ic][it];
          if (adc>adcmax && adc > thr) {
            adcmax = adc;
            it_max = it;
          }
        }

        if (adcmax<0) continue; // remove noise
        if ((*iPad)[chan] == Imin || (*iPad)[chan] == Imax ||
            (*jPad)[chan] == Jmin || (*jPad)[chan] == Jmax)
            continue;

        PadDisplay[(*jPad)[chan]][(*iPad)[chan]] = adcmax;
        if (chan < 1728)
          PadDisplayGeo->Fill((*xPad)[chan],(*yPad)[chan], adcmax);

        TimeMaximum->Fill(it_max);
        TimeMaximumL->Fill(it_max);
      } //loop over channels
      ++total_events;

      // not empty
      if (PadDisplayGeo->Integral() == 0)
        continue;

      ++filled_events;

      PadDisplayGeo->Fit("pol1", "Q");
      TF1* fit = PadDisplayGeo->GetFunction("pol1");

      double quality = fit->GetChisquare() / fit->GetNDF();
      double k = fit->GetParameter(1);
      double b = fit->GetParameter(0);

      // track-like
      if (quality > 150.)
        continue;

      ++sel_events;

      // trigger position study
      TriggerX->Fill(-16.45*k + b);

      // detector uniformity study

      // charge per cluster study
      vector<Float_t > cluster_charge;
      cluster_charge.clear();
      for (Int_t it_j = 0; it_j < Jmax; ++it_j) {
        Int_t cluster = 0;
        Int_t pads_per_cluster = 0;
        for (Int_t it_i = 0; it_i < Imax; ++it_i) {
          cluster += PadDisplay[it_j][it_i];
          if (PadDisplay[it_j][it_i] > 0) {
            ++pads_per_cluster;
            PadDisplayHIT->Fill(it_i, it_j, 1);
          }
          PadDisplayMAX->Fill(it_i, it_j, PadDisplay[it_j][it_i]);
        }

        PadsPerCluster->Fill(pads_per_cluster);

        if (cluster != 0) {
          ClusterCharge->Fill(cluster);
          cluster_charge.push_back(cluster);
        }
      }

      sort(cluster_charge.begin(), cluster_charge.end());
      Float_t norm_cluster = 0.;
      Int_t i_max = round(alpha * cluster_charge.size());
      for (int i = 0; i < std::min(i_max, int(cluster_charge.size())); ++i)
        norm_cluster += cluster_charge[i];

      norm_cluster *= 1 / (alpha * cluster_charge.size());

      ClusterNormCharge->Fill(norm_cluster);


    } // loop over events
  } // loop over files
  cout << endl;
  gStyle->SetOptStat("RMne");

  ClusterCharge->Draw();
  c1->Print((prefix+"ClusterCharge.pdf").Data());


  TF1* f1 = new TF1("f1", "( (x < [1] ) ? [0] * TMath::Gaus(x, [1], [2]) : [0] * TMath::Gaus(x, [1], [3]))", 0, 1300);
  f1->SetParameters(200, 700, 50, 50);
  f1->SetParName(0, "Const");
  f1->SetParName(1, "Mean");
  f1->SetParName(2, "Left sigma");
  f1->SetParName(3, "Right sigma");
  ClusterNormCharge->Fit("f1");

  ClusterNormCharge->Draw();
  c1->Print((prefix+"ClusterChargeNorm.pdf").Data());

  TimeMaximum->Draw();
  c1->Print((prefix+"TimeMaximum.pdf").Data());
  TimeMaximumL->Draw();
  c1->Print((prefix+"TimeMaximumL.pdf").Data());

  gStyle->SetOptStat(0);

  PadDisplayHIT->Draw("colz");
  c1->Print((prefix+"pad_scan_hit_track.pdf").Data());
  PadDisplayMAX->Draw("colz");
  c1->Print((prefix+"pad_scan_max_track.pdf").Data());

  TriggerX->Draw();
  c1->Print((prefix+"TriggerX.pdf").Data());

  PadsPerCluster->Draw();
  c1->Print((prefix+"PadsPerCluster.pdf").Data());

  PadDisplayMAX->Divide(PadDisplayHIT);
  PadDisplayMAX->Draw("colz");
  c1->Print((prefix+"AvChargePerHit.pdf").Data());

  cout << "Total events number     : " << total_events << endl;
  cout << "Not empty events number : " << filled_events << endl;
  cout << "Track events number     : " << sel_events << endl;

  return;
}