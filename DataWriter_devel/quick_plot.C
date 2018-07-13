{
gROOT->LoadMacro("ini.C");

TFile *_file0 = TFile::Open("friend_tmp.root");
TTree *  t0=  (TTree*) GetTree();
TTree *  t=  new TTree("read","read");
t->ReadFile("info_short.txt","Asic:thr:hit:hiterror:hit55:hit55error:hit0:hit0error");
t->AddFriend(t0);
TCanvas *c =new TCanvas ();
c->Divide(3,2);

TString var[3]={"hit:thr","hit0:thr","hit55:thr"};
TLegend * l[2];
for (int xy=0; xy<2; xy++){
l[xy]=new TLegend(0.7,0.7,1,1); 
for (int module=0; module<3;module++){
for (int iplot=0;iplot<3;iplot++){
int iasic=xy+module*2;
TString cut=Form("Asic==%d",iasic);
TString opt=""; if (module>0) opt="same";
gStyle->SetMarkerColor(module+1); gStyle->SetLineColor(module+1); t->UseCurrentStyle();
c->cd(iplot+xy*3+1);
 t->Draw(var[iplot],cut,opt);
TGraph * h= (TGraph*)gPad->GetPrimitive("Graph");h->SetName("toto") ;
 h->SetMarkerColor(3-module+1); h->SetMarkerStyle(module+22);
if (iplot==0) {l[xy]->AddEntry(h,Form("Asic %d",iasic),"P"); l[xy]->Draw();}
//htemp->SetMarkerColor(module+1); htemp->Draw("same");
}}
}
c->Print("raw_500ZS_optimized_thr_study.png");
}

