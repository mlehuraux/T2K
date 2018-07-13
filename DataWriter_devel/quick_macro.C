{
TFile *_file0 = TFile::Open("friend_tmp.root");
gROOT->LoadMacro("ini.C");
cout <<"======== " <<"Analyze: "<<"RUN_00019.00000.root :" << endl;
for (int i=0;i<6;i++){
GetTree()->Draw(Form("Nhit_forAsic[%d]>>h1",i));
GetTree()->Draw(Form("Nhit_forAsic[%d]==0>>h2",i));
 GetTree()->Draw(Form("Nhit_forAsic[%d]>=55>>h",i));
 TH1F*h =(TH1F*)gDirectory->Get("h");
 TH1F*h1 =(TH1F*)gDirectory->Get("h1");
 TH1F*h2 =(TH1F*)gDirectory->Get("h2");


cout<< Form("thr=1180; Hit info for Asic=%d ;",i);
cout <<" average=" << Form("%2.1f+/-%2.1f",(h1->GetMean()),(h1->GetMeanError())) << ";";
cout << " more than 55 hits "<<Form("%d%%+/-%d%%",int(100*h->GetMean()),int(100*h->GetMeanError()))<< ";";
cout << " 0 hits "<<Form("%d%%+/-%d%%",int(100*h2->GetMean()),int(100*h2->GetMeanError()))<< ";";

cout<<endl;
cout <<  "short info " <<  i << " "<< 1180 << " " << h1->GetMean() << " " <<h1->GetMeanError() << " " << h->GetMean() << " " << h->GetMeanError() << " " << h2->GetMean() << " " << h2->GetMeanError() << endl;
delete h1;delete h; delete h2;

}
}
.q
