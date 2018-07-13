

#include "interface/tpcPadDataFormats.hh"
#include "interface/geomManager.hh"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include <TFile.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH2Poly.h>
#include <TRint.h>
#include <TTree.h>
#include <TH1.h>
#include <TPaveText.h>
#include <TChain.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TRandom3.h>

#include "interface/tpcPedCalc.hh"

using namespace std;


void KeyPressed(); //my Keypress function

int main( int nargc, char **argv ) {
  string dataFile = "unknown";
int interactive=0;
 bool quickTest=0;;
 int debuglevel=0;
 bool  do_common_noise_subtraction=0;
 bool exit_at_the_end=0;
 int threshold=400;
 float Nsigma=0;
 int window_min=1; int window_max=150;
 int N_To_Process;
 int asic_detail;
 string dataPed="";
string outdir="";
 bool compute_pedestal=0;
 int display_event=0; //event display. Not implemented yet
 int selected_event=-1;
 int MaximumForDisplay=1000;
 namespace po = boost::program_options; 

  int AsicMap[24]={0};   
  // temporary AsicMap for Run 13 November  asic 5 &4 were swapped
  for (int i=0; i< 24 ; i++)AsicMap[i]=i;
  // This was for November 15 now we are in december  AsicMap[5]=4;AsicMap[4]=5;

 


po::options_description config("Configuration");
 
 config.add_options()
   ("help,h" , "help")  
   ("file,f"  , po::value<string>(&dataFile)->required(),"data file" )
   ("pedestal,p"  , po::value<string>(&dataPed) ->default_value("dummy"),"pedestal file" )
   ("debug"  , po::value<int>(&debuglevel) ->default_value(0),"debug" )
   ("interactive,i","set interactive mode " )
   ("threshold,thr"  , po::value<int>(&threshold) ->default_value(0),"charge max threshold for some cuts, defined in ADC count from 0" )
   ("zero_suppresion,z"  , po::value<float>(&Nsigma) ->default_value(0), "zero suppression with argument giving sigmas from pedestal file (it is recommended to use threshold in ADC set to 0" )     // BT Feb 2016. This options does not really work as the channel are not stable enough due to sensitivity to external noise.
   ("Nevent,N"  , po::value<int>(&N_To_Process) ->default_value(999999),"Number of events to process" )
   ("details"  , po::value<int>(&asic_detail) ->default_value(-1),"asic# to scrutinize" )

   ("MaximumForDisplay", po::value<int>(&MaximumForDisplay) ->default_value(1000),"Scale for displaying details on Asic" )
   ("dir,d", po::value<string>(&outdir) ->default_value("."),"output directory" )
   ("quick"  ,"quick test with 10 events" )    
   ("quit,q"  ,"quit at the end" )    
   ("display"  ,"display event" )    
   ("noise_removal"  ,"common noise removal" )  
   ("event,e", po::value<int>(&selected_event) ->default_value(-1),"focus on aparticular event")
   ("batch,b"  ,"root batch" )    
   ;
 
  po::variables_map vm;
  po::store(po::command_line_parser(nargc, argv).
	    options(config).run(), vm);
  po::notify(vm);
  
  
  if( vm.count("help") ) {
    cout << config << endl;
    cout << "Usage: " << endl;
    return 1;
  }
  if( vm.count("quick"))  quickTest=1;
 if( vm.count("quit"))  exit_at_the_end=1;
 if( vm.count("batch"))  gROOT->SetBatch();
 if( vm.count("interactive")) interactive=1;
if( vm.count("display")) display_event=1;

 if( dataPed!="dummy")  compute_pedestal=1;


  if( vm.count("noise_removal"))  do_common_noise_subtraction=1;

   auto_ptr<TRint> theApp(0);
   auto_ptr<TCanvas> canvas(0);
  auto_ptr<TCanvas> canvas_display(0);
   auto_ptr<TCanvas> c_reco(0);

   auto_ptr<TCanvas> c_basic_asic(0);
   if(1 ) { //was interactive FIXME
     theApp.reset( new TRint("App", 0, 0) );
     // canvas.reset( new TCanvas("cMonitoring","cMonitoring",400,400) );
    canvas.reset( new TCanvas("cMonitoring","cMonitoring",900,700) );
    c_reco.reset( new TCanvas("creco","reconstruciton",600,600) );
   c_basic_asic.reset( new TCanvas("asicbasic","Asic infos",1200,600) );
   canvas_display.reset( new TCanvas("display","display",600,600) );
 
  // rootStyle();
    canvas->SetBorderMode(2); canvas->SetBorderSize(3);
    canvas->Draw(); canvas->Clear(); canvas->SetFillColor(5);
        canvas->Divide(9,7,0.001,0.001,10);
    //72  79

    //canvas->Divide(9,9,0.001,0.001,10);

    c_basic_asic->Divide(6,3,0.001,0.001,10);
     //     canvas->Divide(10,10);

     canvas->Update();
   }

   femGeomManager geo;
  geo.initFromRootFile(dataFile );

  cout << "output in " << outdir << endl;

  //Getting run number  cout << "testing run number "<< endl;
  int run_number=0;

    char dummy1[1000];
    sprintf(dummy1,"%s",dataFile.c_str());
    sprintf(dummy1,"%s",basename(dummy1)); 
    cout<<"file test:" << dummy1 << endl;
    sscanf(dummy1,"RUN_%d",&run_number);
    //  cout << "found run number " << run <<" " << dummy1<<" :" << dummy2<< endl;
  
  tpcPedestal  ped;
  if (compute_pedestal) {
    cout << "computing pedestal from" << dataPed << endl; //"/home/tuchming//saclay1/miniTPC/dataTPC/data_Nov_2015/Test_13_2/RUN_00000.00000.root"
 ped.calcPedestals(dataPed);
 cout << "done" << endl;
  }


  TTree* tree = (TTree*) (TFile::Open(dataFile.c_str()))->Get("padData");

  if (!tree) cerr << "Error Getting Tree from File " << dataFile << endl;

  gSystem->ChangeDirectory(outdir.c_str());

  vector<vector<int> > *listOfSamples(0);
  vector<int>          *listOfChannels(0);
  tree->SetBranchAddress("MGphysChannels", &listOfChannels );
  tree->SetBranchAddress("MGADCvsTime"   , &listOfSamples );
  TFile* f= new TFile("friend_tmp.root","RECREATE");
  TTree* T= new TTree("friend","friend");

  //Add new branches for analysis
  int goodEvent_forAsic[6] ; //
  int goodEvent;
  int  tmax_forAsic[6];
  int  qmax_forAsic[6];
  int  qmean_forAsic[6];
  int  qrms_forAsic[6];
  int  Nhit_forAsic[6];
  
  int  Noise_forAsic[6];
  int  Signal_forAsic[6];


  T->Branch("goodEvent_forAsic",goodEvent_forAsic,"goodEvent_forAsic[6]/I");
  T->Branch("tmax_forAsic",tmax_forAsic,"tmax_forAsic[6]/I");
  T->Branch("qmax_forAsic",qmax_forAsic,"qmax_forAsic[6]/I");
  T->Branch("qmean_forAsic",qmean_forAsic,"qmean_forAsic[6]/I");
  T->Branch("qrms_forAsic",qrms_forAsic,"qrms_forAsic[6]/I");
  T->Branch("goodEvent",&goodEvent,"goodEvent/I");
  T->Branch("Nhit_forAsic",Nhit_forAsic,"Nhit_forAsic[6]/I");

  T->Branch("Signal_forAsic",Signal_forAsic,"Signal_forAsic[6]/I"); // summed signal after noise correction
  T->Branch("Noise_forAsic",Noise_forAsic,"Noise_forAsic[6]/I");   // noise level

 int  Cluster_forAsic[6];
 int  PosCluster_forAsic[6];
 int  LengthCluster_forAsic[6];
 T->Branch("NCluster_forAsic",Cluster_forAsic,"NCluster_forAsic[6]/I");
 T->Branch("PosCluster_forAsic",PosCluster_forAsic,"PosCluster_forAsic[6]/I");
 T->Branch("LengthCluster_forAsic",LengthCluster_forAsic,"LengthCluster_forAsic[6]/I");



 // Mapping Samtec, DAQ T2K - Boris 15 Apri 2016
   int mmap[62]={0,61,59,60,57,58,55,56,53,54,51,52,49,50,47,48,45,46,43,44,41,42,39,40,37,38,35,36,33,34,2,1,4,3,6,5,8,7,10,9,12,11,14,13,16,15,18,17,20,19,22,21,24,23,26,25,28,27,30,29,32,31};

//--Variables for cluster reconstruction in the MultiGen
 int p=61; int n=1024;
  int MultiplexSeries[]={30,10,15,19,5,20,27,22,11,24,18,12,9,6,3,4,1,16,8,2,23,21,7,25,14,28,17,26,13,29};
  int Detector[n]; // strip to channel correspondance
   for(int i=0;i<(p-1)/2;i++)
    {
      for(int j=0;j<p;j++)
        {
          if(i*p+j<n)
            {
              Detector[i*p+j]=(0+MultiplexSeries[i]*j)%p;   //from 0 to 60 included
	      // This says that the strip number XXX is connected to channel Detector[XX]
              //if(Detector[i*p+j]==0) Detector[i*p+j]=p;//strip number 61, not 0
            }
        }
    }


   // Temporary ana

   TH1F* hfec=new TH1F("hfec","fec",10,-0.5,9.5);
   TH1F* hasic=new TH1F("hasic","asic",20,-0.5,19.5);
   TH1F* hstrip=new TH1F("hstrip","strip",100,-0.5,99.5);
   TH1F* hcharge=new TH1F("hcharge","charge",200,-0.5,2999.5);

   TH1F* hcharge_asic[6];  for (int i=0; i<6; i++)hcharge_asic[i]=new TH1F(Form("hcharge_asic%d",i),Form("charge Asic %d",i),200,0,2000);

   TH1F* hchargeY=new TH1F("hchargeY","charge Y",200,-0.5,2999.5);
   TH1F* hchargeX=new TH1F("hchargeX","charge X",200,-0.5,2999.5);
   TH1F* htmax=new TH1F("htmax","tmax",500,.5,500.5);
   TH1F* htmax_asic[6];   for (int i=0; i<6; i++)htmax_asic[i]=new TH1F(Form("htmax%d",i),Form("tmax Asic %d",i),100,0,500);

   TH1F* hfec_f=new TH1F("hfec_f","fec",10,-0.5,9.5);
   TH1F* hasic_f=new TH1F("hasic_f","asic",20,-0.5,19.5);
  
   TH2F*   h_strip_vs_Asic=new TH2F("h_strip_vs_Asic","strip vs Asic",24,-0.5,23.5,64,-0.5,63.5);

 TH1F* hstrip_f=new TH1F("hstrip_f","strip",100,-0.5,99.5);
   TH1F* htmax_f=new TH1F("htmax_f","tmax_f",500,.5,500.5);
   
   TH2F* h_charge_vs_tmax=new TH2F("h_charge_vs_tmax"," h_charge_vs_tmax",50,0,500,60,0,3000);
   TH2F* h_charge_vs_tmax_asic[6]; for (int i=0; i<6; i++)h_charge_vs_tmax_asic[i]=new TH2F(Form("h_charge_vs_tmax%d",i),Form("h_charge_vs_tmax Asic %d",i),50,0,500,40,0,2000);


 TH1F* hchargewindow=new TH1F("hchargewindow","charge in window",100,-0.5,4999.5);


 //Reco histograms

 TH2F*   h_Nhit_vs_Asic=new TH2F("h_Nhit_vs_Asic","Nhit vs Asic",6,-0.5,5.5,50,-0.5,49.5);
 TH1F*   h_Nhit_stripX=new TH1F("h_Nhit_X","Nhit X",124,-0.5,123.5);
 TH1F*   h_Nhit_stripY=new TH1F("h_Nhit_Y","Nhit Y",124,-0.5,123.5);

 if (quickTest){ N_To_Process=10; cout << " performing quick test with " <<  N_To_Process << " events at most"<<endl; }
 int Nevent= tree->GetEntries();
 if   (N_To_Process>0) { if (Nevent>N_To_Process)  Nevent=N_To_Process; } 
//BOBO debug. I don't know why I have 401 events
//Fixed now
 cout << "   [           ]"<< " Nevents="<<Nevent<< "\r   [";
  for( int ievt = 0 ; ievt < Nevent; ievt++ ) {

    gSystem->ProcessEvents() ; //Addition 4/19/2016   // This help refreshing display

    //  %%     if (i%(Nevent/10)==0) cout <<"."<<flush;
    if(Nevent>=10){ if (ievt%(Nevent/10)==0) cout <<"[42m=[m"<<flush;} else cout <<"[42m=[m"<<flush;
    
    if (selected_event>-1 && ievt!=selected_event) continue;
   
    if (debuglevel>2) cout << "event# " << ievt << endl;

    tree->GetEntry(ievt);
  // Initialisation of tree variables
    for (int i=0; i<6;i++) {  goodEvent_forAsic[i]=1;tmax_forAsic[i]=-1;qmax_forAsic[i]=-1; Cluster_forAsic[i]=Nhit_forAsic[i]=qrms_forAsic[i]=qmean_forAsic[i]=-1;  Noise_forAsic[i]=Signal_forAsic[i]=LengthCluster_forAsic[i]=0; PosCluster_forAsic[i]=-1; }
  goodEvent=1;
  

  // read the pads
    vector<tpcPad> pads;
    int AsicMax=-1;
    for( unsigned ipad = 0; ipad < listOfChannels->size(); ipad++ ) {
      tpcPad p;
      pads.push_back(p);
      pads[pads.size()-1].setPad( (*listOfChannels)[ipad],(*listOfSamples)[ipad]);
      // Obtain the number of Asic automatically
      if (AsicMap[geo.asic(pads[pads.size()-1].getPhysChannel())]>  AsicMax)AsicMax=AsicMap[geo.asic(pads[pads.size()-1].getPhysChannel())];
    }

  
    // Pedestal 
    if (compute_pedestal){
      for( unsigned ipad = 0; ipad < listOfChannels->size(); ipad++ ) {
	//pedestal subtraction
		const int &channel= pads[ipad].getPhysChannel();   
	int Ntime= pads[pads.size()-1].getADCvsTime().size() ;
	for( unsigned itime=0; itime <  Ntime; itime++ ){
	  if ( pads[ipad].getADCvsTime () [itime] >0.01 )  pads[ipad].getADCvsTime () [itime]-= (ped.getPedestals()[channel] -250); //again hardcoded 250
	}
      } // loop over channel to fill data bank
    }

   // common noise subtraction
    //For now assume everybody has same number of time samples.  
    vector<tpcPad> pads_uncorrected;
    if ( do_common_noise_subtraction) { // copy the original pads
      pads_uncorrected=pads;
    }
 
    /////////////////////////
   //common noise sutraction
if ( do_common_noise_subtraction){   
  if (debuglevel>2) cout << "looping over nsample " <<  pads[pads.size()-1].getADCvsTime().size() <<endl;
  int Ntime= pads[pads.size()-1].getADCvsTime().size() ;


  //  int &channel,&asic,&charge;
for( unsigned itime=0; itime <  Ntime; itime++ ){
  vector <int> sample[AsicMax+1];
  for( unsigned ipad = 0; ipad < pads.size(); ipad++ ) {
     if (itime >=pads[ipad].getADCvsTime ().size()) cout << "problem" << endl;
    // if (debuglevel>2) cout << "ipad " << ipad << endl;
   const int &channel= pads[ipad].getPhysChannel();       	 
   const int& asic= AsicMap[geo.asic(channel)];//  sample[asic][itime]=0;
   const int& charge=pads[ipad].getADCvsTime () [itime];
      if (charge>0)  sample[asic].push_back(charge); // Fill table of all charge for each asic. Consider only non 0 (0 considered as==empty)
     }

  vector <int> noise_estimate(AsicMax+1,0);
  //sort the vector to determin median
  for (int iasic=0; iasic<AsicMax+1; iasic++){
   if (sample[iasic].size()==0) continue; // do not consider empty asic
    std::sort(sample[iasic].begin(), sample[iasic].end()); 
    if (debuglevel>5)  cout << "median value for asic " << iasic << " is " <<   (sample[iasic])[sample[iasic].size()/2] << endl;
    noise_estimate[iasic]=(sample[iasic])[sample[iasic].size()/2] ;

 }
  // Now subtract the median value for each strip
  for( unsigned ipad = 0; ipad < pads.size(); ipad++ ) {
    const int & channel= pads[ipad].getPhysChannel(); 
    const int & asic= AsicMap[geo.asic(channel)];
    // as things are sorted estimate the median value for that asic, the median bin is sample[asic].size()/2  
     if (pads[ipad].getADCvsTime () [itime] >0 ) // Correct only non 0 charge
      pads[ipad].getADCvsTime () [itime] -= ( noise_estimate[asic] -250); // Beware of hardcoded 250
    
  }//loop over strip (or pad)
  
  
  
  for( unsigned ipad = 0; ipad < pads.size(); ipad++ ) {
    pads[ipad].ResetMax();
  }
 }// loop over the time
 }// end of common noise subtraction
////////////////////////////////////
  
//zero suppression
    if (compute_pedestal) if (Nsigma>0) {
      for( unsigned ipad = 0; ipad < listOfChannels->size(); ipad++ ) {
		const int &channel= pads[ipad].getPhysChannel();   
	int Ntime= pads[pads.size()-1].getADCvsTime().size() ;
	for( unsigned itime=0; itime <  Ntime; itime++ ){
        if ( pads[ipad].getADCvsTime () [itime] >0.01 )  if  ( pads[ipad].getADCvsTime () [itime]  <  250 + Nsigma * ped.getRMS()[channel] ) pads[ipad].getADCvsTime () [itime]=250 ; //again hardcoded 250   zerosuppresion
	}
	cout <<" RMS for channel " << channel << " pad " <<ipad << "  " <<ped.getRMS()[channel] << endl;
      } // loop over channel to fill data bank
      }


  
// Check plot, with chessboard pattern
 if (asic_detail>=0){
   string name_to_save="";
   string name_to_save2="";
   for( unsigned ipad = 0; ipad < pads.size(); ipad++ ) {
  
   
   int myasic=asic_detail; // The ASIC TO DRAW
   const int & channel= pads[ipad].getPhysChannel(); 
   const int & asic= AsicMap[geo.asic(channel)];

   if (asic==myasic){
     gPad->SetTitle(Form("Display: Run:%05d Evt:%03d, ASIC:%d",run_number,ievt,asic));
     name_to_save=Form("Display_run%05d_ev%03d_asic%1d.eps",run_number,ievt,myasic);
     name_to_save2=Form("Display_run%05d_ev%03d_asic%1d.png",run_number,ievt,myasic);
	     gStyle->SetOptTitle(1);
	     //          cout << "cding " << geo.padInAsic(channel) << endl;
	     int ican=1+geo.padInAsic(channel);
	     canvas->cd(ican); ;
	     gPad->SetTopMargin(0);
	     gPad->SetRightMargin(0);
	     //   cout << "ican " << ican << endl;
	     if (ican%9!=1) gPad->SetLeftMargin(0);
	     if (ican/9!=6) gPad->SetBottomMargin(0);
	     gPad->SetBorderMode(2); gPad->SetBorderSize(3);//gPad->PaintBorder(3,1);
	   TH1F * h= pads[ipad].timeSamples();
	   h->SetTitle(Form("Ev%d Asic %d, chan %d",ievt,asic,ican-1));
	   
	   h->SetMaximum(MaximumForDisplay);
	   h->SetName("toberemoved"); //trick to avoid warning from root because of same object name

	  
	  //	   gStyle->SetTitleSize(0.1,"X");

	   float charge=pads[ipad].maxCharge(window_min,window_max);
	   // cout << "charge for pad " << geo.padInAsic(channel) << " :" <<charge << endl;;
	   if ( do_common_noise_subtraction){  
	     TH1F * h0= pads_uncorrected[ipad].timeSamples();
	     h0->SetMaximum(MaximumForDisplay);;
	     h0->SetTitle(Form("Ev%d Asic %d, chan %d",ievt,asic,ican-1));
	     h0->SetLineColor(2);h0->DrawCopy(""); h->DrawCopy("same");
	     h0->Delete();
	   }else {
	     h->DrawCopy();
	   }
	   h->Delete();
	   gStyle->SetTitleFontSize(0.1);
	   // TPaveText*  l = new TPaveText(0.05,0.9,0.2,1,"NDC");
	   //  l->SetTextSize(0.2); 
	   // 	   l->AddText(Form("%d",+geo.padInAsic(channel)));								
	   // l->Draw();

	   
   }// right asic	   	 
   }
	   canvas->Update();



	   if (name_to_save!="")	{   canvas->Print(name_to_save.c_str());    canvas->Print(name_to_save2.c_str());}
	   cout << "saving Picture:" << name_to_save.c_str() << " " << name_to_save2.c_str() << endl;
	   gStyle->SetOptTitle(0); if( interactive )  KeyPressed(); //getchar();
 }

    // SHORT ANALYSIS (xross-check) begin
     int thr=threshold;  
     for( unsigned ipad = 1; ipad < pads.size(); ipad++ ) { //start at one to remove unphysical channel
     // quick plot of all channel (takes some time looks like infinite loop)


       /*
       if (pads[ipad].tAtMax()<1){
	 cout << "pad with 0 maximum " ;
	 for (int itime=0;itime <30;itime++){
	   cout << pads[ipad].getADCvsTime () [itime]<< " : " ;
	 }
	 cout << endl;
	 }*/


       //Graphical view 
       if (0 && ipad>15&& ipad<30)
	 //if (pads[ipad].tAtMax()<1)
	 {
	   TH1F * h= pads[ipad].timeSamples();
	   h->SetMaximum(3000);	h->SetMinimum(0);h->DrawCopy();
	   h->Delete();
	   
	   if ( do_common_noise_subtraction){  
	     TH1F * h0= pads_uncorrected[ipad].timeSamples();
	     h0->SetLineColor(2);h0->DrawCopy("same");
	     h0->Delete();
	   }
	   canvas->Update();
	 }
       //Draw all pad from a given asic

   


     
     


       

       int channel= pads[ipad].getPhysChannel(); 
       const int & asic= AsicMap[geo.asic(channel)];      	 
       int strip = geo.padInAsic(channel);   
       strip=mmap[strip]; //maping, fixme
       float charge=pads[ipad].maxCharge();
       float tmax=pads[ipad].tAtMax();
       hcharge->Fill(charge);
       if (asic<6)hcharge_asic[asic]->Fill(charge);

       if ( asic==0 || asic==2 || asic==4 ) hchargeX->Fill(charge); else  hchargeY->Fill(charge);
       hfec->Fill(geo.fec(channel));
       hasic->Fill(AsicMap[geo.asic(channel)]);
	
       hstrip->Fill(strip);
       htmax->Fill(tmax);  
       h_charge_vs_tmax->Fill(tmax,charge);
       if (asic<6) htmax_asic[asic]->Fill(tmax);
       if (asic<6) h_charge_vs_tmax_asic[asic]->Fill(tmax,charge);
     
 
       // tree variables  . BT Feb 2016. They are bugged, this is actually the variables for the last channel of the asic!!!
       tmax_forAsic[asic]=tmax;
       if (charge> qmax_forAsic[asic])  qmax_forAsic[asic]=charge;
       qmean_forAsic[asic]+=pads[ipad].getMean()/61;
       qrms_forAsic[asic]+=pads[ipad].getRMS()/61;

       //crash here on Feb08 2016
       //      cout << "ipad is " << ipad <<  "/" << pads.size() <<  endl;
    
       hchargewindow->Fill(pads[ipad].maxCharge(window_min,window_max));

 // hchargewindow->Fill(pads[ipad].maxCharge(60,110));
       float newcharge=pads[ipad].maxCharge(window_min,window_max);
       if (charge>=thr)h_strip_vs_Asic->Fill(AsicMap[geo.asic(channel)],strip);

       if (charge>=thr){
	   //	   if (strip==14 &&AsicMap[geo.asic(channel)==0 ) cout << " strip 14 for " << channel << endl;



	 hfec_f->Fill(geo.fec(channel));
	 hasic_f->Fill(AsicMap[geo.asic(channel)]);
	 hstrip_f->Fill(strip);
	 htmax_f->Fill(tmax);
	 }

     }//loop over pad
    

  // Now try to plot all channel

     // TRy to find hit.
     // Reconstruciton part

     vector <int> Nhit(AsicMax+1,0);
     vector <int> N_noisehit(AsicMax+1,0);
     for( unsigned ipad = 0; ipad < pads_uncorrected.size(); ipad++ ) {
       int channel= pads[ipad].getPhysChannel();       	 
       float charge=pads[ipad].maxCharge(window_min,window_max);
            float charge_uncor=pads_uncorrected[ipad].maxCharge(window_min,window_max);


       // Bobo Warning temporarry cut //remove unused asic
       if (AsicMap[geo.asic(channel)]>=6) continue;
       if (charge >= thr) Nhit[AsicMap[geo.asic(channel)]]++;


             if (charge>=thr) Signal_forAsic[AsicMap[geo.asic(channel)]]+=charge-250;   //hardcoded
       else{
	 //consider this is a unreposnsive event where I can study the Noise
	 N_noisehit	[AsicMap[geo.asic(channel)]] ++;
	 Noise_forAsic[AsicMap[geo.asic(channel)]]+=(charge_uncor-250)/ N_noisehit[AsicMap[geo.asic(channel)]]; // compute the mean response


       }


     } // loop over pads
     for ( int iasic=0 ; iasic <=AsicMax;iasic++){
       if (iasic>=6) continue; 
       h_Nhit_vs_Asic->Fill(iasic,Nhit[iasic]);
       Nhit_forAsic[iasic]=Nhit[iasic];
       goodEvent_forAsic[iasic]=Nhit[iasic]<50;
       if  (!goodEvent_forAsic[iasic])goodEvent=0;

       if ( iasic==0 || iasic==2 || iasic==4 )  h_Nhit_stripX->Fill(Nhit[iasic]); // last should be 4 but there was a cable swap in November
	
       if ( iasic==1 || iasic==3 || iasic==5 )  h_Nhit_stripY->Fill(Nhit[iasic]); // last should be 5 but there was a cable swap in November
     }

     if  (display_event){ //  event display
       //    TH2F * detector_display = new TH2F ("h_detector","XY view",1024/4,-0.5,1023.5,1024/4,-0.5,1023.5);
   TH2F * detector_display = new TH2F ("h_detector","XY view",1024,-0.5,1023.5,1024,-0.5,1023.5);
   TH1F * detector_displayX = new TH1F ("h_detectorX","X view",1024,-0.5,1023.5);
   TH1F * detector_displayY = new TH1F ("h_detectorY","Y view",1024,-0.5,1023.5);

       int asicX=0;
       int asicY=1;

       for( unsigned ipad = 0; ipad < pads.size(); ipad++ ){
	 const int & channel= pads[ipad].getPhysChannel(); 
	 const int & asic= AsicMap[geo.asic(channel)];
	 if (asic!=asicX && asic!=asicY) continue;
         	 
          int  multigen_channel = geo.padInAsic(channel);     
	  // mapping  FIXME
	  multigen_channel=mmap[multigen_channel];

	  float charge=pads[ipad].maxCharge(window_min,window_max);
	  //Fill x and y

	  for (int istrip=0; istrip < 1024 ; istrip++){ 
	    if( Detector[istrip]==  multigen_channel -1){ 
	      if (asic==asicX) detector_displayX ->Fill(istrip,(charge>thr));
	      if (asic==asicY) detector_displayY ->Fill(istrip,(charge>thr));
	    }
	  }
	    

	  //BEWARE   x,y coordinate, vs  strip  number,  vs  histogram bin number
	  //if (charge <thr) continue;
	  //  cout << "adding charge " << charge << endl;
	  for (int istrip=0; istrip < 1024 ; istrip++){ 
	    //  if( Detector[istrip]== 61-  multigen_channel){  //FIXME
	       if( Detector[istrip]==   multigen_channel -1){
	      //multigen channel goes from  1 to 61
              // Detector[istrip] goes from 0 to 60
 // Test if this physical strip correspond to the detector channel The first physical strip is at 1  so multigen_channel-1 goes from 0 to 60 as  //
	      for (int jstrip=0; jstrip<1024; jstrip++){ // fill the full line of squares, as a strip is a line
		float x=jstrip; float y=jstrip;
		// now decide if it is X or Y coordinnate
		if (asic==asicX) x=istrip;   // The X layer measure the Y coordinate !!!
		if (asic==asicY) y=istrip;
		detector_display ->Fill(x,y,(charge>thr));//(charge-250));
	      } // end filling the full strip 
	    } ; //test this a detector channel
	  }// end testing each possible physical strip
       } // end looping over all detector channels
     
       // clustering treatment  in x   look for 3 neighboors
       if(0)         for (int ibin=1+1 ; ibin <= 1024-1  ; ibin++){ 
	 if 	(detector_display->GetBinContent (ibin,1)  > 0  && 	detector_display->GetBinContent (ibin-1,1)>0 && 	detector_display->GetBinContent (ibin+1,1)>0){
	  float x= detector_display->GetXaxis()->FindBin(ibin);
	   for (int jbin=1 ; jbin <= 1024  ; jbin++){
	     float y=detector_display->GetYaxis()->FindBin(jbin);
	     detector_display->Fill(x,y,1)  ;   // BEWARE that here ibin and x are same coordinate
	     detector_display->Fill(x-1,y,1)  ;   // BEWARE that here ibin and x are same coordinate
	     detector_display->Fill(x+1,y,1)  ;   // BEWARE that here ibin and x are same coordinate
	   }
	 }
       }
       
       // clustering treatment  in x   look for 3 neighboors
      if(0)   for (int jbin=1+1 ; jbin <= 1024-1  ; jbin++){ 
	  if 	(detector_display->GetBinContent (1,jbin)  > 0  && 	detector_display->GetBinContent (1,jbin-1)>0 && 	detector_display->GetBinContent (1,jbin+1)>0){
	  float y= detector_display->GetYaxis()->FindBin(jbin);
	   for (int ibin=1 ; ibin <= 1024  ; ibin++){
	     float x=detector_display->GetXaxis()->FindBin(ibin);
	     detector_display->Fill(x,y,1)  ;   // BEWARE that here ibin and x are same coordinate
	     detector_display->Fill(x,y-1,1)  ;   // BEWARE that here ibin and x are same coordinate
	     detector_display->Fill(x,y+1,1)  ;   // BEWARE that here ibin and x are same coordinate
	   }
	 }
       }
       

       canvas_display->cd();
       detector_display ->Draw("colz");
  

       //     TH1F * hproj=(TH1F*) detector_display ->ProjectionX("projdisplay",1,1) ;// detector_display ->GetNbinsY()+1); //issue with two many bins
       //  for (int i=0;i<  detector_display ->GetNbinsY()+1;i++)hproj->SetBinContent(i,detector_display->GetBinContent (i,1));
       //  hproj->Draw();;

       /*
       canvas_display->Clear();
       gStyle->SetOptTitle(1);
       canvas_display->Divide(1,2);
       canvas_display->cd(1);
       detector_displayX->Draw();
       canvas_display->cd(2);
       detector_displayY->Draw();
       */
     

       canvas_display->Update();


       if( interactive )KeyPressed ;// getchar();

      
     }


       // New test of clustering algorithm


       for (int ThisAsic=0 ; ThisAsic<6; ThisAsic++)
	 //	  for (int itest_shift=-3; itest_shift<6; itest_shift++)
	 { 
	   //   cout << " TEST SHIFT " << itest_shift << endl;

       int        clusize_min=2;
	 if (ThisAsic%2 == 0) clusize_min=2 ;  // For layer X (// to resistive strip) 
	 if (ThisAsic%2 == 1) clusize_min=3 ;  // For layer Y 


 if (debuglevel>0)cout <<"==============" << "  Asic " << ThisAsic << "   " <<"==============" <<  endl;
		 // First get list of Hits
       float Strip_Charge[1024]={0};
       int StripClusterIndex[1024]; // This array says if StripClusterIndex[i])=k  says that the strip#i belongs to cluster #k
       for (int i=0;i<1024;i++) StripClusterIndex[i]=-1;//initialisation

       //For debugging purpose, I generate randomly a permutation.
       int Permutation[61];
       for (int i=0 ; i<61; i++) Permutation[i]=i;
       std::random_shuffle(Permutation, Permutation + 61); // shuffle C-style array
       

       for( unsigned ipad = 0; ipad < pads.size(); ipad++ ){
	 const int &  channel=pads[ipad].getPhysChannel(); 
	 const int & asic= AsicMap[geo.asic(channel)];
       	 if (asic!=ThisAsic) continue; // The the relevant ASIC
	 float charge=pads[ipad].maxCharge();
	 if (charge < thr) continue;
	 int   multigen_channel = geo.padInAsic(channel);    // from 1 to 61  
	 // test:: randomize everything
	 //	 multigen_channel=Permutation[multigen_channel-1]+1;


	 // try mapping
	 if(0) { //mapping id "simon"
	   int tmpchan=	  multigen_channel; int strip=0;
	if (tmpchan<=1) strip=tmpchan;
	else if(tmpchan>15 && tmpchan<48) strip=tmpchan;  // 32 strip concerned (inner)
	     else strip= (tmpchan + 1 - (2*(tmpchan%2)));   // I presume outer strip are not concerned
	multigen_channel=strip;

	 }
	 
	 // Random mapping
	 if (0){
	   multigen_channel = 1 +  Permutation[multigen_channel -1];
	 }

	 // Try mapping with sametec

	 if (0){
	   int strip=-99;
	   int tmpchan=  multigen_channel  +12 -2 -4;
	// VICTORY
	if((tmpchan%2)==0 && tmpchan<74 && tmpchan>0) strip=72-(int) ((tmpchan+1)/2)-7;    //rotation around 32
	if((tmpchan%2)==1 && tmpchan<72) strip=(int) ((tmpchan+1)/2)-3;
	
	strip-=1;
	if (strip==0) strip=61;
	multigen_channel=strip;
	
	}
       
	 // Try mapping with sametec #2

	 if (0){
	   int strip=-99;
	   int tmpchan=  multigen_channel  +12 -2 -4;
	// VICTORY
	if((tmpchan%2)==0 && tmpchan<74 && tmpchan>0) strip=72-(int) ((tmpchan+1)/2)-7;    //rotation around 32
	if((tmpchan%2)==1 && tmpchan<72) strip=(int) ((tmpchan+1)/2)-3;
		multigen_channel=strip;

	}
       

	 // Try my mapping

	 if (1){
	   int strip=-99;


	    int mmap[62]={0,61,59,60,57,58,55,56,53,54,51,52,49,50,47,48,45,46,43,44,41,42,39,40,37,38,35,36,33,34,2,1,4,3,6,5,8,7,10,9,12,11,14,13,16,15,18,17,20,19,22,21,24,23,26,25,28,27,30,29,32,31};
	    // int mmap[62]={0,61,59,60,57,58,55,56,53,54,51,52,49,50,47,48,45,46,43,44,41,42,39,40,37,38,35,36,33,34,2,0,4,1,6,3,8,5,10,7,12,9,14,11,16,13,18,15,20,17,22,19,24,21,26,23,28,25,30,27,32,29};
  strip=mmap[multigen_channel];  // 1... 61
	   
  multigen_channel=strip;
	 }


	 // Inversion 
	 if (0) {   // 1...61 -> 1..61
	   multigen_channel =  62 - multigen_channel ;

	 }


	 if (debuglevel>0)	 cout << "channel " << channel <<" multigen channel "<< multigen_channel  << " " << "charge=" << charge << endl;
	 for (int istrip=0; istrip < 1024 ; istrip++){ 
	     if( Detector[istrip]==  multigen_channel -1 )   //  from 0 to 60
	   //   if( Detector[istrip]== 61- multigen_channel  )   //  from 0 to 60 
	     Strip_Charge[istrip]=charge;
	 }
       }

       //debug
     for (int istrip=0; istrip < 1024-1 ; istrip++){
       //   if (Strip_Charge[istrip]>thr){ cout << " istrip " << istrip << " :" << Strip_Charge[istrip] << endl;}
       }
  

       // Now cluster things
       int N_cluster=0;
       for (int istrip=0; istrip < 1024-2 ; istrip++){
	 if (Strip_Charge[istrip]>thr){  // found a seed now find cluster end
	   int cluster_size=1;
	   while ( istrip+cluster_size <1024 && Strip_Charge[istrip+cluster_size] >thr ) { //change thr to 0
	     cluster_size++;
	   }
	   //   cout << "debug " << " found a cluster size of " << cluster_size << endl;
	   if (cluster_size>=clusize_min) { // find 3 adjacent neighbours  istrip ,  istrip+1 ,... istrip-1+ clustersize
	     for (int i=istrip;  i <istrip+cluster_size ; i++) { // also  i<=istrip-1+clustersize
	       StripClusterIndex[i]=N_cluster; // To which cluster the strip i belongs
	       if (LengthCluster_forAsic[ThisAsic] < cluster_size){
		 LengthCluster_forAsic[ThisAsic]= cluster_size; // Keep the longest cluster
		 PosCluster_forAsic[ThisAsic]=i; //store the position of the longest cluster
		 
	       }
	     }		 
		    N_cluster++;
		  istrip+=cluster_size-1; // the -1 is not useful:  in principal this one has been already tested 
		  }	   // More than 3 neighbours
	   }//end positive charge
	 }//end loop
  
       if (debuglevel>0) cout << " N clusters found " << N_cluster << endl;
	 Cluster_forAsic[ThisAsic]=N_cluster;// for Tree
	 //		for (int icluster=0; icluster<N_cluster; icluster++) for (int istrip=0; istrip < 1024-1 ; istrip++) if ( StripClusterIndex[istrip]==icluster)	PosCluster_forAsic[ThisAsic]=istrip;// Look at possition of last cluster of the list

if (debuglevel>0) {
	   for (int icluster=0; icluster<N_cluster; icluster++){
	     cout << " Cluster #" << icluster;
	     for (int istrip=0; istrip < 1024-1 ; istrip++){
	       if ( StripClusterIndex[istrip]==icluster)  cout << " : " << istrip << " (" << Strip_Charge[istrip]<< ") "   ;
	     }
	     cout << endl;
	   }
	 }
	 
	 }// testing algorithm
       
       //
       //       output
 

  
  T->Fill(); //add variable
      


	    
  //  cout <<  listOfChannels->size() << endl;
  
	    }	    //end loop events

  T->Write();
  tree->AddFriend(T);
  gStyle->SetOptTitle(1);
  gStyle->SetPalette(1);
  TCanvas *  Canvas_check=new TCanvas("check","Data",700,800);
  Canvas_check->Draw();
  Canvas_check->Divide(3,4);
  Canvas_check->cd(1);hfec->Draw(); hfec_f->Draw("same");
  Canvas_check->cd(2);hasic->Draw();hasic_f->Draw("same");
  Canvas_check->cd(3);hstrip->Draw(); hstrip_f->Draw("same");
  Canvas_check->cd(4);htmax->Draw(); htmax_f->Draw("same");
  Canvas_check->cd(5);hcharge->Draw();
  Canvas_check->cd(6);hchargeX->Draw();
  Canvas_check->cd(7);hchargeY->Draw();
  Canvas_check->cd(8); h_charge_vs_tmax->Draw("colz");
  Canvas_check->cd(9); hchargewindow->Draw("");
  Canvas_check->cd(10); h_strip_vs_Asic->Draw("colz");
  Canvas_check->Update();
  canvas->Update();

  c_basic_asic->cd();
  for (int iasic=0;iasic<6;iasic++){
    c_basic_asic->cd(iasic + 1); hcharge_asic[iasic]->Draw();
    c_basic_asic->cd(iasic + 6 + 1); htmax_asic[iasic]->Draw();
    c_basic_asic->cd(iasic + 12 + 1);  h_charge_vs_tmax_asic[iasic]->Draw("colz");
  }
  c_basic_asic->Update();
  cout <<endl;
  
  
  c_reco->Divide(2,2);

 c_reco->cd(2);
 h_Nhit_vs_Asic->SetTitle("Hits per Asic"); h_Nhit_vs_Asic->ProjectionY("proj",0, h_Nhit_vs_Asic ->GetNbinsY()+1)->Draw();;
 c_reco->cd(3);
 h_Nhit_stripX->Draw();
 c_reco->cd(4);
 h_Nhit_stripY->Draw();
  c_reco->cd(2);
  h_Nhit_vs_Asic->DrawCopy("box");
  c_reco->Update();
  

  //  getchar();

  if (exit_at_the_end) exit(0);
  if( interactive || 1 ) { theApp->Run();}
  return 0;
  }

// added 4/19/2016
#include "Getline.h"
void KeyPressed(){

TTimer  *timer = new TTimer("gSystem->ProcessEvents();", 50, kFALSE);
    const char    *input;
  
    Bool_t done = kFALSE;
do {
       timer->TurnOn();
       timer->Reset();
       // Now let's read the input, we can use here any
       // stdio or iostream reading methods. like std::cin >> myinputl;
        input = Getline("Type <return> to continue, q <return> to exit: "); 
	if (TString(input)=="q\n") exit(0);
        timer->TurnOff();
	 if (input) done = kTRUE;
    } while (!done);

 delete timer;
 cout << "in Key pressed " << TString(input) << "ok"<<endl;;
}

// GARBAGE
/*
cp /media/WD_SAVE_2014/FCC/Test_Nov15/Test_13_2/RUN_00005.00000.root .
bin/rootHodoAnalyzerExe -f RUN_00005.00000.root

*/
