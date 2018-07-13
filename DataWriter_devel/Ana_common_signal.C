// Study Noise vs signal vs signal max
void Ana_common_signal()
{  gROOT->LoadMacro("ini.C");

  TFile *_file0 = TFile::Open("friend_tmp.root");
  //  TString date=GetShellResult("date '+\%Y\%m\%d_\%H\%M\%S'");
TString date=GetShellResult("echo -n `basename $PWD|cut -f2 -d_`_;date '+\%Y\%m\%d_\%H\%M\%S'");

 for (int j=2;j<4;j++){


 for (int i=0; i<3 ; i++){



 gStyle->SetOptStat(0);
gStyle->SetOptStat(1111);
gStyle->SetOptFit(1);

   TString v1,v2,v3;
   float range=2000;
   if (i==0) { v2=Form("qmax_forAsic[%d]-250",j); v1=Form("Noise_forAsic[%d]",j); v3=Form("qmax_forAsic[%d]>252",j);}
					   
   if (i==1) { v1=Form("qmax_forAsic[%d]-250",j); v2=Form("Signal_forAsic[%d]",j); v3=Form("qmax_forAsic[%d]>252",j); range=2000;}
				   
     if (i==2) { v1=Form("Noise_forAsic[%d]",j); v2=Form("Signal_forAsic[%d]",j); v3=Form("qmax_forAsic[%d]>252",j); range=2000;}

   GetTree()->Draw(v1+":"+v2+">>hhh",v3,"box"); //

     TH2F* hhh=(TH2F*)  gDirectory ->Get("hhh");   //
     cout << hhh << endl;
     float rangex=hhh->GetRMS(1)+hhh->GetMean(1);
     float rangey=hhh->GetRMS(2)+hhh->GetMean(2);
       delete hhh;
     GetTree()->Draw(v1+":"+v2+Form(">>hh(40,0,%f,40,0,%f)",2.*rangex,2.*rangey),v3,"box"); //
   
     TH2F* hh=(TH2F*)  gDirectory ->Get("hh");   //
    cout << "rangexy " << rangex << " " << rangey << endl;
     cout << Form(">>hh(40,0,%f,40,0,%f)",rangex,rangey) << endl;
// GetTree()->Draw(v1+":"+v2+">>h",v3,"prof ");
//gDirectory ->Get("h");
 TProfile* h= (TProfile*)hh->ProfileX("h");//
hh->SetLineColor(3); 
 h->SetLineColor(1);
 
h->Fit("pol1","","",252-250,rangex); h->GetFunction("pol1")->Draw("same");hh->Draw("same box"); h->Draw("same");
 atitle(v2,v1);
 hh->SetName(Form("hh%d%d",i,j));
 h->SetName(Form("h%d%d",i,j));


 cout << " v1 " <<v1 << " v2 " << v2 << endl;
  gPad->Update();
 long int k=gSystem->Now();
 gPad->Print(Form("Noise_Asic_%d_%d_%s.png",j,i,date.Data()));

 /*
 GetTree()->Draw(v1,v3);
 gPad->Print(Form("Charge_study_%s_%d_%s.png",v1.ReplaceAll("[","").ReplaceAll("]","").Data(),j,date.Data()));

 GetTree()->Draw(v2,v3);
 gPad->Print(Form("Charge_study_%s_%d_%s.png",v2.ReplaceAll("[","").ReplaceAll("]","").Data(),j,date.Data()));
 */


 //getchar();
 //delete h; delete hh;
}


 }

}
