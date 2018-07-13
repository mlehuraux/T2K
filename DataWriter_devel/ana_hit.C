{

  gROOT->LoadMacro("ini.C");
  TFile *_file0 = TFile::Open("friend_tmp.root");

  for (int i=0; i<6 ;i++){
    
    GetTree()->Draw(Form("Nhit_forAsic[%d]>=58>>h",i));
    TH1F* h=  (TH1F*) gDirectory ->Get("h");
     cout << "Fraction of evts with 58 hits for Asic " << i << " " << h->GetMean() <<   " (" <<  h->GetMean()*h->GetEntries()<<  "evts)"<<endl;
  }
  cout << "-----------------" << endl;
 for (int i=0; i<6 ;i++)
   for (int j=5; j>i ;j--){
     //   cout << "i j " << i  << " " << j << endl;
     TString var=Form("(Nhit_forAsic[%d]>=58&&Nhit_forAsic[%d]>=58)>>hh",i,j);
     //  cout << var  << endl;
     GetTree()->Draw(var.Data());
 TH1F* h=  (TH1F*) gDirectory ->Get("hh");
 cout << "Fraction of evts with 58 hits for Asic " << i << " and " << j << " " << h->GetMean() <<   " (" <<  h->GetMean()*h->GetEntries()<<  "evts)"<<endl;
  }

}

