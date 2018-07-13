#ifndef boboini
#define boboini

#ifndef __CINT__

#include <TChain.h>
#include <TFile.h>
#include <TPostScript.h>
#include <TH1.h>
#include <THStack.h>
#include <TStyle.h>
#include <TKey.h>

#include <TProfile.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TGraph.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TF1.h>
#include <TF2.h>
#include <TMath.h>


#include <fstream>

#include <iostream>


#include <sstream>
#include <string.h>
using namespace std;
#include <TGraphErrors.h>
#endif


//added 2012 09
#include <TStyle.h>
#include <TPostScript.h>
#include <TLeaf.h>

//int main(){return 1;};

///  initialisation of  Graphical style
///  This is far from being perfect and was set up a long time ago
class  Bobo_Initialisation {
 
private:
 
TStyle * _style1;
TStyle * _style2;
 public:
  Bobo_Initialisation(){
_style1=new TStyle("style1","Bobo Style ");
_style2=new TStyle("style2","Bobo Style with a divided pad");
// cout << " initialisation of bobo_initialisation" << endl;
};
 ~Bobo_Initialisation(){
   //   if (_style1!=NULL) delete _style1;  //commented out 5-Apri-2016 to avoid crash in TRint...
   //if (_style2!=NULL)  delete _style2;
};

  void CopyStyle(TStyle *stylein,TStyle *styleout){
    // assume styleout already exists
    TString title = styleout->GetTitle();
    TString name = styleout->GetName();
    stylein->Copy(*styleout); 
    /*  (TStyle*)styleout=stylein->Clone()
or     *styleout=*stylein;
copy only a partial set of all the Style attribute ????
 */


      styleout->SetName(name);
    styleout->SetTitle(title);
  
  }

  
  void  Print(){cout<<"Liste of Styles "<<endl;gROOT->GetListOfStyles()->ls();};
  void  Graphic(float size=1, int divided=0){
    CopyStyle(gStyle,_style1);  // modify only my own attribute
int myfont=132;
  
  _style1->SetPadBorderMode(0);
  _style1->SetPadTopMargin(0.1*size);
  _style1->SetPadLeftMargin(0.1*size);
  _style1->SetPadRightMargin(0.1*size);
  _style1->SetPadBottomMargin(0.15*size);

  
//_style1->SetFillStyle(1001);


_style1->SetCanvasColor(10);
_style1->SetPadColor(10);//same color otherwise some surp

 _style1->SetFrameFillColor(10); //added 31 march 2009 V5.22 ??



    //  _style1->SetfrickLength(0.03*size);



//_style1->SetTitleSize(0.05*size,"XYZ");
 _style1->SetLabelFont(myfont,"XYZ");

 _style1->SetTitleFontSize(0.08*size);

  _style1->SetLabelSize(0.04*size,"XYZ");
 
  _style1->SetTitleFont(myfont,"XYZ");


_style1->SetTitleXSize(0.05*size);
_style1->SetTitleYSize(0.05*size);
 _style1->SetTitleSize(0.05*size,"Z"); //BT oct14

 _style1->SetTitleYOffset(1-0.05*size);
 _style1->SetTitleXOffset(1-0.05*size);
 // _style1->SetTitleOffset(1-0.05*size,"Z");

 _style1->SetTitleColor(1); // bobo was 19;

_style1->SetTextSize(1*size);
_style1->SetTextFont(myfont);
  

//Stat
_style1->SetStatFont(myfont);
//_style1->SetStatFontSize(1*size);


_style1->SetStatW(0.25);//+(size-1)*.1);
_style1->SetStatH(0.2);

_style1->SetStatX(0.99);
_style1->SetStatY(0.99);
_style1->SetOptFit(101);

  

_style1->SetTitleW(0.6);
_style1->SetTitleH(0.06*size);
_style1->SetStatStyle(1001); //1 is opaqueon screen but transparent on ps
_style1->SetStatColor(19);


_style1->SetFitFormat("5.3f");

//    e = 1;  print errors (if e=1, v must be 1)
//    c = 1;  print Chisquare/Number of degress of freedom
//    p = 1;  print Probability

//Pad 
//_style1->SetPadColor(2);
//_style1->SetFillStyle(3006);
//_style1->SetHistFillStyle(3006);
//_style1->SetHistLineColor();

//Histo
_style1->SetHistFillColor(29);
_style1->SetHistFillColor(5);
 _style1->SetHistLineColor(1); //addd 6/3/2013, it looks the default is 602 nowadays in root, so put back to black instead of blue
_style1->SetHistLineWidth( ( int)  (1*size ));
_style1->SetFuncWidth( ( int) ( 1.3*size ) );
_style1->SetFrameLineColor(1); 



//_style1->SetPaperSize(10.5,15);
_style1->SetPaperSize(21,29.7);
//_style1->SetPaperSize(,28.7);

_style1->SetPaperSize(19,19);

_style1->SetPadColor(0);

CopyStyle(_style1,_style2);

// now define the difference

  _style2->SetPadTopMargin(0.01*size);
  _style2->SetPadRightMargin(0.02*size);

  _style2->SetPadLeftMargin(0.1*size);
  _style2->SetPadBottomMargin(0.11*size); //0.1 ->0.11 18/2/2008

 _style2->SetTitleYOffset(1+0.05*size);
 _style2->SetTitleXOffset(1+0.05*size);
  _style2->SetStatW(0.2+(size-1)*.1);

_style2->SetStatX(1-_style2->GetPadRightMargin());
_style2->SetStatY(1-_style2->GetPadTopMargin());

_style2->SetTitleH(0.04*size);


 _style2->SetStatStyle(0); // put it transparent
 
 if (divided==1){
gROOT->SetStyle("style2");
 }else{
gROOT->SetStyle("style1");
 }

 };

 
  void  Standard(float i=-1, TString version="v0"){

    if (i!=-1) Graphic(i);
  
 
  gStyle->SetHistLineWidth(3); 


        gStyle->SetPadColor(10);
        gStyle->SetCanvasColor(10);
     gStyle->SetFrameFillColor(10); //added 31 march 2009 V5.22

    // gStyle->SetPadColor(0);
    // gStyle->SetCanvasColor(0);
 
    gStyle->SetMarkerSize(1.4);
    gStyle->SetOptStat(0);
    gStyle->SetHistLineWidth(3);
    gStyle->SetOptTitle(0);
    gStyle->SetFuncWidth(3);
  

  gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerSize(1.2);
  
     gStyle->SetPalette(1);
    // soething to improve as a function of time and preserve backward compatibility

    if (version=="v1"){
      gStyle->SetPadTickX(1);
      gStyle->SetPadTickY(1);
   gStyle->SetHistFillStyle(0);
 
    }


 

    //if (i!=-1) Graphic(i);

  };///< save cut and paste and space




};
/*
List of graphical subroutine adapted from my old (sic) paw environment


 */


class Bobo_Pict2File {
public:
  Bobo_Pict2File(){_fname=""; ps=0;};
  ~Bobo_Pict2File(){};
  TPostScript * ps;
  TString _fname;
  void Open(TString fname="tmp", TString opt="ps"){
    if (!(opt == "ps" || opt == "eps")) { cout <<"No valid option (eps or ps)" <<opt<<endl; return;}
    fname+="."+opt;
    if (ps!=0){
      cout<<"Warning postscript already exist :"<<endl<<ps->GetName()
	  <<"What is going to happen ??"<<endl; 
    } 
    cout<<"opening PostScript "<<fname<<endl;
    if (opt=="eps") ps=new TPostScript(fname,113);
    if (opt=="ps")  ps=new TPostScript(fname,111);
    _fname=fname;
  };
  void Close(){ if (ps!=0){cout<<"closing "<<_fname<<endl;delete ps; ps=0;};}
  void NewPage(){if (ps!=0)  ps->NewPage();};
  void On(){if (ps!=0)  ps->On();};
  void Off(){if (ps!=0)  ps->Off();};
  void Save(TString fname="tmp", TString opt="eps"){// save the current canvas};
      Bobo_Pict2File * tmp;
      TCanvas *tmpc=(TCanvas *)gROOT->GetListOfCanvases()->Last();
    if (tmpc!=0){
   cout<<"saving the Canvas "<<tmpc->GetName()<<" : "<<tmpc->GetTitle()<<endl;
    tmp= new Bobo_Pict2File();
    tmp->Open(fname,opt);
    tmpc->Draw();
    tmp->Close();
    delete tmp;
    }    else { 
      cout <<"No Canvas to Save into a PostScript"<<endl;
    }
   


};

};





//end of class Declaration

namespace  Bobo_util
{
  void Norm(TH2F * histo, bool include_over_underflow=1);
  void Norm(TH1 * histo, bool include_over_underflow=1);

    Float_t Convolution(TH1F* h1, TF1* f1, float  startx=0 , float endx=-1,int overflow=0);
  



void TopLeftText(TString text= " ",float ipos=1,int step=20,float xoff=.2, float size=0){

TPaveText *leakingtext= new TPaveText( 
gStyle->GetPadLeftMargin()+xoff/step,
1-gStyle->GetPadTopMargin()-ipos/step,
1-gStyle->GetPadRightMargin(),
1-gStyle->GetPadTopMargin()-(ipos-1)/step,"NDC");
leakingtext->SetBorderSize(0);
TText *t=leakingtext->AddText(text);
t->SetTextAlign(11);
leakingtext->Draw();
leakingtext->SetFillStyle(0);
 if (size==0)leakingtext->SetTextSize(gStyle->GetTextSize()/25);
 else leakingtext->SetTextSize(size);

leakingtext->SetTextFont(gStyle->GetTextFont());

//leakingtext->SetFillColor(gPad->GetFillColor());

}

void atitle(TString xt="",TString yt="",TString zt=""){
    TObject * element=gPad->GetListOfPrimitives()->First();
    while (element){
      //            cout<<element->ClassName()<<strcmp(element->ClassName(),"TH")<<endl;
       if (strstr(element->ClassName(),"TH") && !strstr(element->ClassName(),"THS")){
      ((TH1*) element)->GetXaxis()->SetTitle(xt);
      ((TH1*) element)->GetYaxis()->SetTitle(yt);
      ((TH1*) element)->GetZaxis()->SetTitle(zt);
      //      ((TH1*) element)->Draw();
       }
  if (strstr(element->ClassName(),"TGraph")){
      ((TGraph*) element)->GetXaxis()->SetTitle(xt);
      ((TGraph*) element)->GetYaxis()->SetTitle(yt);
      //      ((TH1*) element)->Draw();
      
     } 
if (strstr(element->ClassName(),"TProfile")){
      ((TProfile*) element)->GetXaxis()->SetTitle(xt);
      ((TProfile*) element)->GetYaxis()->SetTitle(yt);
      //      ((TH1*) element)->Draw();
      

     }


 if (strstr(element->ClassName(),"THStack") ){
      ((THStack*) element)->GetXaxis()->SetTitle(xt);
      ((THStack*) element)->GetYaxis()->SetTitle(yt);
      //      ((THStack*) element)->GetZaxis()->SetTitle(zt);
      //      ((TH1*) element)->Draw();
       }

 if (strstr(element->ClassName(),"TF1")){
   ((TF1*) element)->GetXaxis()->SetTitle(xt);
   ((TF1*) element)->GetYaxis()->SetTitle(yt);
   //      ((TH1*) element)->Draw();
        }

 if (strstr(element->ClassName(),"TF2")){
   ((TF2*) element)->GetXaxis()->SetTitle(xt);
   ((TF2*) element)->GetYaxis()->SetTitle(yt);
   //      ((TH1*) element)->Draw();
        }


      element=gPad->GetListOfPrimitives()->After(element);

    }

      // ( (TH2*)(gPad->GetPrimitive("htemp")))->GetXaxis()->SetTitle(xt);
      //( (TH2*)(gPad->GetPrimitive("htemp")))->GetYaxis()->SetTitle(yt);
};

void  BinHalf(TH1F * histo){
int nbin=histo->GetNbinsX();
 int nbin2=nbin/2;
TString oldname=histo->GetName();
//TH1F  *out= new TH1F("binhalfhisto",histo->GetTitle(),nbin2,histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());      
TH1F  out("binhalfhisto",histo->GetTitle(),nbin2,histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());      
 for (int i=1; i<=nbin2 ; i++)    { //old fortan
    float y= histo->GetBinContent(i*2-1)+histo->GetBinContent(i*2); // edge effect
    float ey=sqrt((histo->GetBinError(i*2-1)*histo->GetBinError(i*2-1))
       + (histo->GetBinError(i*2) * histo->GetBinError(i*2) ));
    out.SetBinContent(i,y); 
    out.SetBinError(i,ey);  
}
 // delete histo;
  (*histo)= out;
 //  delete out;
 histo->SetName(oldname);
 
 }



// // Divide by 2 the binning in along X axis
// void  BinHalfX(TH2F * histo){
//   int nbin  = histo->GetNbinsX();
//   int nbiny = histo->GetNbinsY();
//   int nbin2=nbin/2;
//   TString oldname=histo->GetName();

//   TH2F  out("binhalfhisto",histo->GetTitle(),nbin2,histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax(),
// 	    nbiny,histo->GetYaxis()->GetXmin(),histo->GetYaxis()->GetXmax());
//   for (int j=0; j<nbiny+2 ; j++)    {
//     for (int i=1; i<=nbin2 ; i++)    { //old fortan
//       float y= histo->GetBinContent(i*2-1,j)+histo->GetBinContent(i*2,j); // edge effect
//       float ey=sqrt((histo->GetBinError(i*2-1,j)*histo->GetBinError(i*2-1,j))
// 		    + (histo->GetBinError(i*2,j) * histo->GetBinError(i*2,j) ));
//       out.SetBinContent(i,j,y); 
//       out.SetBinError(i,j,ey);  
//     }
//   }  (*histo)= out;
//   //  delete out;
//   histo->SetName(oldname);
// } 

// Divide by 2 the binning in along X axis
void  BinHalfX(TH2F * &histo){
  int nbin  = histo->GetNbinsX();
  int nbiny = histo->GetNbinsY();
  int nbin2=nbin/2;
  TString oldname=histo->GetName();

  TH2F * out= new TH2F("binhalfhisto",histo->GetTitle(),nbin2,histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax(),
	    nbiny,histo->GetYaxis()->GetXmin(),histo->GetYaxis()->GetXmax());
  for (int j=0; j<nbiny+2 ; j++)    {
    for (int i=1; i<=nbin2 ; i++)    { //old fortan
      float y= histo->GetBinContent(i*2-1,j)+histo->GetBinContent(i*2,j); // edge effect
      float ey=sqrt((histo->GetBinError(i*2-1,j)*histo->GetBinError(i*2-1,j))
		    + (histo->GetBinError(i*2,j) * histo->GetBinError(i*2,j) ));
      out->SetBinContent(i,j,y); 
      out->SetBinError(i,j,ey);  
    }
  } 
  delete histo; 
  histo= out;
  //  delete out;
  histo->SetName(oldname);
} 




//double the binning along X with linear extrpolation
void  BinDoubleX(TH2F * &histo){
  int nbin  = histo->GetNbinsX();
  int nbiny = histo->GetNbinsY();
  int nbin2=2*nbin-1;
  TString oldname=histo->GetName();

  float oldstep = histo->GetXaxis()->GetBinWidth(1);  
  TH2F * out= new TH2F("binhalfhisto",histo->GetTitle(),nbin2,histo->GetXaxis()->GetXmin()+oldstep/4,histo->GetXaxis()->GetXmax()-oldstep/4,
	    nbiny,histo->GetYaxis()->GetXmin(),histo->GetYaxis()->GetXmax());
  // cout << "juste a check " << oldstep << "=?" << newstep*2;
for (int j=0; j<nbiny+2 ; j++)    {
    for (int i=1; i<=nbin ; i++)    { //old fortan
      float z1=histo->GetBinContent(i,j);
      out->SetBinContent(2*i-1,j,z1);       
      if (i!=nbin){   //so that wi fill exatly 2n-1 bin
      float z2=histo->GetBinContent(i+1,j);
      float newz=(z1+z2)/2; //do the mean as
      //try extrapolating more than linear
      if (i>=2 && i<=nbin-2)  newz+=1./8*(histo->GetBinContent(i-1,j)+histo->GetBinContent(i+2,j)-histo->GetBinContent(i,j)-histo->GetBinContent(i+1,j));

      out->SetBinContent(2*i,j,newz);  
      }
    }
}
  delete histo; 
  histo= out;
  //  delete out;
  histo->SetName(oldname);
} 



//Swap the XY content of the 2d histo
void  SwapXY(TH2F * &histo){
  int nbin  = histo->GetNbinsX();
  int nbiny = histo->GetNbinsY();
  TString oldname=histo->GetName();

  TH2F * out=new TH2F("binhalfhisto",histo->GetTitle(),
		      nbiny,histo->GetYaxis()->GetXmin(),histo->GetYaxis()->GetXmax(),
		      nbin,histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());
  for (int j=0; j<nbiny+2 ; j++)    {
    for (int i=0; i<nbin+2 ; i++)    { //old fortan
      float y= histo->GetBinContent(i,j);
      float ey=histo->GetBinError(i,j);
      out->SetBinContent(j,i,y); 
      out->SetBinError(j,i,ey);  
    }
  }
  delete histo;
  histo=out;
  histo->SetName(oldname);
 
}


void  Translate(TProfile * histo, float step){
int nbin=histo->GetNbinsX();
TString oldname=histo->GetName();
//change this to const 
const Double_t * xx = new Double_t[nbin+1];
Double_t * x = new Double_t[nbin+1];
xx=histo->GetXaxis()->GetXbins()->GetArray();
 for (int i=0; i<nbin+1 ; i++) {
   cout<< x[i]<<endl;
    x[i]=xx[i]+step;

 }
 cout <<"Tranlating " << oldname << endl;
 TProfile *out =new TProfile("binhalfhisto",histo->GetTitle(),nbin,x);

			    //histo->GetXaxis()->GetXmin()+step,histo->GetXaxis()->GetXmax()+step,0,1);      
 for (int i=0; i<nbin+2 ; i++)    { //old fortan
    out->SetBinContent(i, histo->GetBinContent(i)); 
    out->SetBinError(i, histo->GetBinError(i)); 
    cout <<  out->GetBinContent(i) << endl;
}
 // delete histo;
  (*histo)= (*out);
 //  delete out;
 histo->SetName(oldname);
 delete []x;
 }


// test whether the error on an histogramme are consistent with bin to bin uncorrelation;
void HConsistency(TH1*h) {
 int ndof=0;
 float chisq=0;
 float x,y,ex,ey;
 for (int i=1 ;  i<h->GetNbinsX() ; i=i+2){
   x=h->GetBinContent(i);
   y=h->GetBinContent(i+1);
   ex=h->GetBinError(i);
   ey=h->GetBinError(i+1);
   if (ex>0 && ey>0){
     chisq+= (x-y)*(x-y)/(ex*ex+ey*ey);
   ndof+=1;
   }
 }
 if (ndof>0){
   cout << " consistency check " <<endl
	<< "chisq="<<chisq << endl
	<< "ndof="<< ndof << endl
	<<"Prob=" << TMath::Prob(chisq,ndof) << endl;
   
 }else{
   cout << "to many empty bins to check consistency " << endl;
 } 
}




  TH1F* AsymandSymPart(TH1 * h,int epsilon){
  //take into account overflows and underflows
int nbin=h->GetNbinsX();
 TH1F  *out;
 if (epsilon>0) out=(TH1F*)h->Clone("sympart_histo");else 
  out=(TH1F*)h->Clone("asympart_histo"); 
 float content=0;
 float error2=0;
 for (int i=0; i<nbin+2 ; i++)    {
   content=h->GetBinContent(i)+epsilon*h->GetBinContent(nbin+1-i);
   error2=h->GetBinError(i)*h->GetBinError(i)+h->GetBinError(nbin+1-i)*h->GetBinError(nbin+1-i);
   content*=0.5;
   error2*=0.25;
   out->SetBinContent(i,content);
   //  cout << i << " " << content << endl;
   //   out->SetBinContent(nbin+1-i,content);
   out->SetBinError(i,sqrt(error2));
   // out->SetBinError(nbin+1-i,sqrt(error2));
      }
 return out;
}

  TH1F* SymPart(TH1 * h){
    return  AsymandSymPart(h,1);
  }
  
  TH1F* AsymPart(TH1 * h){
    return  AsymandSymPart(h,-1);
  }


  TH1F* RelativeAsymPart(TH1 * h){
    TH1F* h1=AsymPart(h);
    TH1F* h2=SymPart(h);
    h1->Divide(h1,h2,1,1,"B");                                                                                                                      
        // Jan 2015. Special treatment for correct error computation                                                                                                                
    int nbin=h->GetNbinsX(); 
    for (int i=0; i<nbin+2 ; i++) {   
      float  Nplus=h->GetBinContent(nbin+1-i); 
      float  dNplus=h->GetBinError(nbin+1-i);
      float  Nminus=h->GetBinContent(nbin+1-i);   
      float  dNminus=h->GetBinError(i);                                                                                                                                           
      float error=0;  
      if (Nplus+Nminus>0)error=2/(Nplus+Nminus)/(Nplus+Nminus)
	  *sqrt(Nplus* Nplus* dNminus* dNminus+dNplus* dNplus* Nminus* Nminus);
	  h1->SetBinError(i,error);    
	  h1->SetBinError(nbin+1-i,error);	      
	  }                                                           
    delete h2;
    return h1;
  }


float Integral(TH1F * histo,int binomial=0){
  //take into account overflows and underflows
int nbin=histo->GetNbinsX();
TH1F  *out=(TH1F*)histo->Clone("integral_histo"); 
 float content=0;
 float error2=0;
 for (int i=0; i<nbin+2 ; i++)    { 
   content+=histo-> GetBinContent(i);
   error2+=histo->GetBinError(i)*histo->GetBinError(i);
    out->SetBinContent(i,content); 
    out->SetBinError(i,sqrt(error2));  
}
 for (int i=0; i<nbin+2;i++){
   histo->SetBinContent(i,out->GetBinContent(i));
   histo->SetBinError(i,out->GetBinError(i));
 }
 delete out;
 if (binomial==1){ //change the error since it is binomial error
  for (int i=0; i<histo->GetNbinsX()+2; i++){
  histo->SetBinError(i,histo->GetBinError(i)*sqrt(fabs(1-histo->GetBinContent(i))));
 }
}

 return histo->GetBinContent(nbin+1);
}

void ToIntegral(TH1F * histo,int binomial=0){
  Integral(histo,binomial);
}

float OneMinusIntegral(TH1F * histo){
  //take into account overflows and underflows
int nbin=histo->GetNbinsX();
TH1F  *out=(TH1F*)histo->Clone("integral_histo"); 
 float content=0;
 float error2=0;
 for (int i=nbin+1; i>=0 ; i--)    { 
   content+=histo-> GetBinContent(i);
   error2+=histo->GetBinError(i)*histo->GetBinError(i);
    out->SetBinContent(i,content); 
    out->SetBinError(i,sqrt(error2));  
}
 
 for (int i=nbin+1; i>=0 ; i--)    { 
   histo->SetBinContent(i,out->GetBinContent(i));
   histo->SetBinError(i,out->GetBinError(i));
 }
 delete out;
 return histo->GetBinContent(0);
}

void Scale(TGraphErrors *g  ,float scale){
  int n= g->GetN();
  Double_t*y= g->GetY();
  Double_t*ey= g->GetEY();

  for (int i=0; i<n ; i++){
    y[i]*=scale;
    ey[i]*=scale;
  
  }
}

void Scale(TH1  *histo,float scale){
  // DO not use this function
  // changinh overflow bin content also change Nbins !!!!! as of root 4.4
  /// cout << "in scale" << histo->GetBinError(0) << " sca=" << scale<< " content"<< histo->GetBinContent(0)<< endl;
    int n = histo->GetNbinsX()+2;
for (int i=0; i<n ;i++){
 
  //  if (i==0) cout << "error" << histo->GetBinError(i)  << " " << scale <<endl;
   // histo->SetBinError(i,histo->GetBinError(i)*scale);
  // if (i==0) cout << "error" << histo->GetBinError(i)  << " " << scale <<endl;
  // 
  // histo->SetBinContent(i,histo->GetBinContent(i)*scale);

  //     if (0);
     //     cout << "error " << i <<  " " << histo->GetBinError(i)  << " " << histo->GetNbinsX() << " " << scale <<endl;
   
 } 
  histo->Scale(scale);
//co << "in scale" << histo->GetBinError(0) <<endl;
 

}


void Scale(TH2F  *histo,float scale){
  cout << "in scale:  DO not use this function " << endl;
 for (int i=0; i<histo->GetNbinsX()+2;i++)
   for (int j=0; j<histo->GetNbinsY()+2; j++){

 
  // if (i==0) cout << "error" << histo->GetBinError(i)  << " " << scale <<endl;
  histo->SetBinError(i,j,histo->GetBinError(i,j)*scale);
     histo->SetBinContent(i,j,histo->GetBinContent(i,j)*scale);
   }
}



void Norm(TH1 * histo, bool include_over_underflow){//including overflow and underflow
 float scale=0;
 int imin=1;
 int N_i=histo->GetNbinsX()+1;

 if (include_over_underflow) {
   --imin;
   ++N_i;
   
 }

 for (int i=imin; i<N_i+1; i++){
   scale+=histo->GetBinContent(i);
 }
 // cout << " debug:TH1 total bin content " << scale << endl;
 //cout << " debug:TH1 total bin content (integral)" << histo->Integral() << endl;


  if (scale !=0 ) Scale(histo,1./scale);
  // cout << " debug: total bin content (integral) after scale" << histo->Integral() << endl;

}



void Norm(TH2F * histo, bool include_over_underflow){//including overflow and underflow
 float scale=0;
 int imin=1;
 int jmin=1;
 int N_i=histo->GetNbinsX()+1;
 int N_j=histo->GetNbinsY()+1;

 if (include_over_underflow) {
   --imin;
   --jmin;
   ++N_i;
   ++N_j;
   
 }

 for (int i=imin ; i<N_i+1 ; i++)
 for (int j=jmin ; j<N_j+1 ; j++){
   scale+=histo->GetBinContent(i,j);
 }
  if (scale !=0) Scale(histo,1./scale);
}


TGraphErrors * SigBkgEffi(TH1F * hsig,TH1F * hbkg, int integrer=1, int normalized=1)
{  

if (integrer==1) {
Integral(hsig); 
Integral(hbkg);
}
 int n=hsig->GetNbinsX(); 

 if (normalized==1){ 
   //   Norm(hsig);Norm(hbkg);
   Scale(hsig,1./hsig->GetBinContent(n+1));
   Scale(hbkg,1./hbkg->GetBinContent(n+1));
   cout<<" normalisation :"<<hbkg->GetBinContent(n+1)<<endl;
 }

 float x[10000]; float y[10000];
 float xe[10000]; float ye[10000];
 

  for (int j=0; j<n+1;j++){
    x[j]=hsig->GetBinContent(j)*1/hsig->GetBinContent(n+1);   
    y[j]=hbkg->GetBinContent(j)*1/hbkg->GetBinContent(n+1);   
    xe[j]=hsig->GetBinError(j)*1/hsig->GetBinContent(n+1);   
    ye[j]=hbkg->GetBinError(j)*1/hbkg->GetBinContent(n+1);   

}
  if (normalized==1){//binomial error
   for (int j=0; j<n+2;j++){
    xe[j]=hsig->GetBinError(j)*sqrt
      (1-pow(hsig->GetBinError(j),2)/pow(hsig->GetBinError(n+1),2));   
    ye[j]=hbkg->GetBinError(j)* (1-pow(hbkg->GetBinError(j),2)/pow(hbkg->GetBinError(n+1),2));   
    
    hsig->SetBinError(j,xe[j]);
    hbkg->SetBinError(j,ye[j]);

}
  

  }


  //  TGraph * g= new TGraph(n,x,y);
  TGraphErrors * g= new TGraphErrors(n,x,y,xe,ye);
  
  return g;
  }



TGraphErrors * h1vsh2(TH1F * hsig,TH1F * hbkg)
{  


 int n=hsig->GetNbinsX(); 



 float x[30000]; float y[30000];
 float xe[30000]; float ye[30000];
 

  for (int j=0; j<n+1;j++){
    x[j]=hsig->GetBinContent(j);
    y[j]=hbkg->GetBinContent(j);
    xe[j]=hsig->GetBinError(j);
    ye[j]=hbkg->GetBinError(j);

}
  //  TGraph * g= new TGraph(n,x,y);
  TGraphErrors * g= new TGraphErrors(n,x,y,xe,ye);
  
  return g;
  }

  void Multiply(TH1F * &histo,float scale){
int nbin=histo->GetNbinsX();
TString oldname=histo->GetName();
TH1F  *out= new TH1F("binhalfhisto",histo->GetTitle(),nbin,histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());      
 for (int i=1; i<=nbin ; i++)    { //old fortan
   float y= histo->GetBinContent(i)*scale;
   float ey=histo->GetBinError(i)*scale;
    out->SetBinContent(i,y); 
    out->SetBinError(i,ey);  
}
 delete histo;

 histo=out;
 // delete out;
 histo->SetName(oldname);
}


void StoreObject( TObject * histo=0,TString filename="tmphisto.root" ){
 TFile *dir = gDirectory->GetFile();
 TFile * fileout;
 if (histo==0){
 fileout = new TFile(filename,"recreate");
  cout<<"the file "<<filename<<" is set empty"<<endl;
 
}else
 { fileout = new TFile(filename,"update");

 // if (newname!="") histo->SetName(newname);
 histo->Write();
 gStyle->Write();
 }
 delete fileout;
 if (dir!=0) dir->cd();

}


void StoreCanvas( TCanvas * histo=0,TString filename="tmphisto.root",TString newname="" ){
 TFile *dir = gDirectory->GetFile();
 TFile * fileout;
 if (histo==0){
 fileout = new TFile(filename,"recreate");
  cout<<"the file "<<filename<<" is set empty"<<endl;
  delete fileout;
  return;
}else
 { fileout = new TFile(filename,"update");

  if (newname!="") histo->SetName(newname);

  if  (fileout->Get( histo->GetName()) ){// trying 
    char tmp[1000];
    //  int i;
    //    cout << sscanf (histo->GetName(),"%s %d", tmp,&i) << "sscanf result" << endl;
    //    cout <<"find number i "<< i << endl;
    //cout << "also find string " << tmp << endl;
    // don tknow how to parse the name.
    //use dirty method    

    //  cout << " WWWWWWWWWWWWWWWW The name is already in the file " << histo->GetName() << endl;
    sprintf(tmp,"%s%s",histo->GetName(),"a");
   delete fileout;
    if (dir!=0) dir->cd();
    StoreCanvas(histo,filename,tmp) ;
  }else{
 histo->Write();
 gStyle->Write();
  delete fileout;
 if (dir!=0) dir->cd();
  }

 
    if (dir!=0) dir->cd();
 // if (dir!=0) dir->cd();

 }
}

void StoreHisto( TH1 * histo=0,TString filename="tmphisto.root" ){
 TFile *dir = gDirectory->GetFile();
 TFile * fileout;
 if (histo==0){
 fileout = new TFile(filename,"recreate");
  cout<<"the file "<<filename<<" is set empty"<<endl;
 //TString toto=" rm -f "+filename;
 //toto=" echo salut gros con " ;
 // cout << toto  << endl;
 //  gSystem->Exec(toto);
 //  cout << "before return " << endl;
 
}else
 { fileout = new TFile(filename,"update");
 histo->Write();
 gStyle->Write();
 }
 delete fileout;
 if (dir!=0) dir->cd();

}






void StoreTree( TTree * histo=0,TString filename="tmphisto.root" ){
 TFile *dir = gDirectory->GetFile();
 TFile * fileout;
 if (histo==0){
 fileout = new TFile(filename,"recreate");
  cout<<"the file "<<filename<<" is set empty"<<endl;
 //TString toto=" rm -f "+filename;
 //toto=" echo salut gros con " ;
 // cout << toto  << endl;
 //  gSystem->Exec(toto);
 //  cout << "before return " << endl;
 
}else
 { fileout = new TFile(filename,"update");
 histo->Write();
 gStyle->Write();
 }
 delete fileout;
 if (dir!=0) dir->cd();

}




  TChain * MakeChain(const char * files=" *.root", TString tree="Global", int lim=200){
char temp[1000];
char tempfile[1000];

int pid=gSystem->GetPid();
TChain * mychain = new TChain(tree);
sprintf (tempfile,"myfiles.txt.%d",pid);
 cout<<" files "<<files<<endl;
sprintf(temp,"ls %s   |head -n %d > myfiles.txt.%d",files,lim,pid);
gSystem->Exec(temp);

char line[200];
ifstream in(tempfile);
 while (in>>line){
    cout<<"adding file "<<line<<endl;
   mychain->Add(line);
 }
 in.close();
 TString rm=tempfile;
rm="rm "+rm;
gSystem->Exec(rm);
 return mychain;
  }


float BinomialError(float  N,float n){
  if (N>n && N>0){
    float e=n/N;
      return sqrt(n*(1-e));
}else{
  cout<<"pb with binomial error (N,n)="<<N<<" "<<n<<endl;
  return 0;
}

}

TString  giftoeps(TString gif){
  char temp[400];
  char tempfile[400];
  TString outps;
  int pid=gSystem->GetPid();
  sprintf(temp," echo %s |sed s/\\.gif/\\.eps/g > tmp%d",gif.Data(),pid);
  gSystem->Exec(temp);
  sprintf(tempfile,"tmp%d",pid);  
  ifstream in(tempfile);
  in>>outps;
  in.close();
  sprintf(temp,"rm -f tmp%d",pid);
   gSystem->Exec(temp);
  // cout<<" fileout is "<< outps << endl;
  return outps;
}




TString LovelyFortran(float x){
  char temp[100];
  if ((int)x==x) {
    // this is an integer
    sprintf(temp," %d. ",(int)x);
  }else
    sprintf(temp," %f ",x);
  TString out=temp;
  return out;

};

float trunc(float x, int n){
  if (x==0) return x;
  int sign; 
 (x<0)? sign=-1:sign=1;
   int N= (int)floor(log10(sign*x))+1;
   return sign*floor(sign* x/pow(10.,N-n)+0.5)*pow(10.,N-n);  // 4-Jun-2014
   return sign*floor(sign* x/pow(10.,N-n))*pow(10.,N-n);  
}
float eta_xyz(float px, float py, float pz){
  float pt=sqrt(px*px+py*py);
  if (pt==0) return 100;
  float E= sqrt(pt*pt+pz*pz);
  float eta=0.5*log((E+pz)/(E-pz));
    return eta;
}


void DumpHistoContent(TH1* h){
  ostream &oh  =cout;
 oh <<  LovelyFortran(h->GetNbinsX())  <<   
   LovelyFortran(h->GetBinLowEdge(1)) <<
   LovelyFortran(h->GetBinLowEdge(h->GetNbinsX()+1)) << endl;
 
 for (int j=1; j<h->GetNbinsX()+1 ;j++){
   //   oh <<  LovelyFortran(j) <<  LovelyFortran( h->GetBinContent(j)) <<  LovelyFortran (h->GetBinError(j)) << endl;

   oh <<   LovelyFortran( h->GetBinContent(j)) <<  " " ;

 } oh <<  endl;
};

Bobo_Initialisation ini;
Bobo_Pict2File  Pict2File;



//gROOT->ForceStyle();

// ================================================

Float_t Convolution(TH1F* h1, TH1F* h2, float  startx , float endx, int overflow)  {

// ================================================

// h1 = distribution en eta
// h2 = efficacite versus eta

// First get the normalisation

//option overflow

  float ntot=0;
  float tot;
  int start;
  int end;
if (overflow) {
    start=0;
  end=h1->GetNbinsX()+1;
}else{
  start=1;
  end=h1->GetNbinsX();
}
//  if startx endx are set then they decide the binning
 if (startx < endx) {
   start=h1->FindBin(startx);
   end=h1->FindBin(endx);
 }

 tot= h1->Integral(start,end);


float prod=0;
  float err=0;
  for (Int_t xbin = start; xbin <= end; xbin++)
     {
       Float_t cont1 = (float)h1->GetBinContent(xbin)/tot;
       Float_t err1 =  h1->GetBinError(xbin)/tot;
       ntot = ntot + cont1;
       Float_t cont2 = (float)h2->GetBinContent(h2->FindBin(h1->GetBinCenter(xbin))); // works even if the binning are differetn
       Float_t err2 =  (float)h2->GetBinError(h2->FindBin(h1->GetBinCenter(xbin)));
            prod = prod + cont1 * cont2 ;
	    //    cout << "for bin " << h1->GetBinCenter(xbin) << " " << cont2 << " "  << cont1 << "  mean "<<prod/ntot<<endl;

  float dui = err1*err1*cont2*cont2 + cont1*cont1*err2*err2;
       err +=dui; 
    }
 
  ntot=1;
 prod = prod / (float)ntot;
 err = sqrt(err) / (float)ntot;

 cout << "--- convolution = " << prod << " +/- " << err << endl;
 // cout << endl;

 return prod;
}

// ================================================

Float_t Convolution(TH1F* h1, TF1* f1, float  startx , float endx,int overflow)  {

// ================================================

// h1 = distribution en eta
// h2 = efficacite versus eta

// First get the normalisation


  float ntot=0;
  float tot;
  int start;
  int end;
if (overflow) {
    start=0;
  end=h1->GetNbinsX()+1;
}else{
  start=1;
  end=h1->GetNbinsX();
}

//  if startx endx are set then they decide the binning
 if (startx < endx) {
   start=h1->FindBin(startx);
   end=h1->FindBin(endx);
 }

 tot= h1->Integral(start,end);


float prod=0;
  float err=0;

  for (Int_t xbin = start; xbin <= end; xbin++)
     {
       Float_t cont1 = (float)h1->GetBinContent(xbin)/tot;
       Float_t err1 =  h1->GetBinError(xbin)/tot;
       ntot = ntot + cont1;
        Float_t cont2 = (float)f1->Eval(h1->GetBinCenter(xbin)); 
            prod = prod + cont1 * cont2 ;
	    //   cout << "for bin " << h1->GetBinCenter(xbin) << " " << cont2 << " "  << cont1 << "  mean "<<prod/ntot<<endl;

	    float dui = err1*err1*cont2*cont2 ;
       err +=dui; 
    }
 
  ntot=1;
 prod = prod / (float)ntot;
 err = sqrt(err) / (float)ntot;

 //cout << "--- convolution = " << prod << " +/- " << err << endl;
 cout << endl;

 return prod;
}


/* d02004style.C

   This routine defines a set of functions to setup

         general, canvas and histogram styles

   recommended by D0. "zmumu_1.C" macro illustrates its usage.

                                 Created: January 2003, Avto Kharchilava
                                 Updated: January 2004,      ----
*/

// ... Global attributes go here ...
void global_style() {

   gStyle->SetCanvasColor(0);
   gStyle->SetCanvasBorderMode(0);
   gStyle->SetPadColor(0);
   gStyle->SetPadBorderMode(0);
   gStyle->SetFrameBorderMode(0);

   gStyle->SetTitleColor(1);   // 0
   gStyle->SetTitleBorderSize(1);
   gStyle->SetTitleX(0.10);
   gStyle->SetTitleY(0.94);
   gStyle->SetTitleW(0.5);
   gStyle->SetTitleH(0.06);

   gStyle->SetLabelOffset(1e-04);
   gStyle->SetLabelSize(0.2);

   gStyle->SetStatColor(0);
   gStyle->SetStatBorderSize(1);
   gStyle->SetStatX(0.90);
   gStyle->SetStatY(0.90);
   gStyle->SetStatW(0.30);
   gStyle->SetStatH(0.10);

   //   gStyle->SetErrorX(0.0);   // Horizontal error bar size
   //   gStyle->SetPaperSize(10.,12.);   // Printout size
};

// ... Canvas attributes ...
void canvas_style(TPad *c,
                  float left_margin=0.15,
		  float right_margin=0.05,
		  float top_margin=0.05,
		  float bottom_margin=0.15,
		  int canvas_color=0,
                  int frame_color=0) {

  c->SetLeftMargin(left_margin);
  c->SetRightMargin(right_margin);
  c->SetTopMargin(top_margin);
  c->SetBottomMargin(bottom_margin);
  c->SetFillColor(canvas_color);
  c->SetFrameFillColor(frame_color);

  c->SetBorderMode(0);
  c->SetBorderSize(1);
  c->SetFrameBorderMode(0);
}

// ... 1D histogram attributes; (2D to come) ...
// name changed 2011/12/09   was h1_style. COmpatibility issue with very old macros
void h1_style_D0(TH1 *h,
	    int line_width=3,
	    int line_color=1,
	    int line_style=1, 
	    int fill_style=1001,
		 int fill_color=18, //May 15 was 50
 	    float y_min=-1111.,
	    float y_max=-1111.,
	    int ndivx=510,
	    int ndivy=510,
	    int marker_style=20,
	    int marker_color=1,
	    float marker_size=1.2,
	    int optstat=0) {

   h->SetLineWidth(line_width);
   h->SetLineColor(line_color);
   h->SetLineStyle(line_style);
   h->SetFillColor(fill_color);
   h->SetFillStyle(fill_style);
   h->SetMaximum(y_max);
   h->SetMinimum(y_min);
   h->GetXaxis()->SetNdivisions(ndivx);
   h->GetYaxis()->SetNdivisions(ndivy);

   h->SetMarkerStyle(marker_style);
   h->SetMarkerColor(marker_color);
   h->SetMarkerSize(marker_size);
   h->SetStats(optstat);

   h->SetLabelFont(62,"X");       // 42
   h->SetLabelFont(62,"Y");       // 42
   h->SetLabelOffset(0.000,"X");  // D=0.005
   h->SetLabelOffset(0.005,"Y");  // D=0.005
   h->SetLabelSize(0.055,"X");
   h->SetLabelSize(0.055,"Y");
   h->SetTitleOffset(0.8,"X");
   h->SetTitleOffset(0.9,"Y");
   h->SetTitleSize(0.06,"X");
   h->SetTitleSize(0.06,"Y");
   h->SetTitle(0);
}

/// ... 2D histogram attributes; 
//static
 void h2_style_D0(TH1 *h,
	    int line_width=3,
	    int line_color=1,
	    int line_style=1, 
	    int fill_style=1001,
	    int fill_color=50,
	    float y_min=-1111.,
	    float y_max=-1111.,
	    int ndivx=510,
	    int ndivy=510,
	    int marker_style=20,
	    int marker_color=1,
	    float marker_size=1.2,
	    int optstat=0) {

  h->SetLineWidth(line_width);
  h->SetLineColor(line_color);
  h->SetLineStyle(line_style);
  h->SetFillColor(fill_color);
  h->SetFillStyle(fill_style);
   h->SetMaximum(y_max);
   h->SetMinimum(y_min);
   h->GetXaxis()->SetNdivisions(ndivx);
   h->GetYaxis()->SetNdivisions(ndivy);
   h->GetXaxis()->SetTitleColor(1);
   h->GetYaxis()->SetTitleColor(1);

   h->SetMarkerStyle(marker_style);
   h->SetMarkerColor(marker_color);
   h->SetMarkerSize(marker_size);
   h->SetStats(optstat);

   h->SetLabelFont(62,"X");       // 42
   h->SetLabelFont(62,"Y");       // 42
   h->SetLabelFont(62,"Z");       // 42
   h->SetLabelOffset(0.000,"X");  // D=0.005
   h->SetLabelOffset(0.005,"Y");  // D=0.005
   h->SetLabelOffset(0.0,"Z");  // D=0.005
   h->SetLabelSize(0.055,"X");
   h->SetLabelSize(0.055,"Y");
   h->SetLabelSize(0.055,"Z");
   h->SetTitleOffset(0.8,"X");
   h->SetTitleOffset(0.9,"Y");
   h->SetTitleOffset(0.0,"Z");
   h->SetTitleSize(0.06,"X");
   h->SetTitleSize(0.06,"Y");
   h->SetTitleSize(0.04,"Z");
   h->SetTitle(0);

}




float qs( float a, float b){
  return sqrt(a*a+b*b);
};

//sum of uncertainty for two measurement given by (dx1 dy1 corr1, dx2, dy2, cor2) Correlation can be either in % or in standard unit feb 2015
float qs22( float dx1, float dy1, float c1, float dx2, float dy2, float c2){
  float xx=dx1*dx1+dx2*dx2;
  float yy=dy1*dy1+dy2*dy2;
  float  exy=  fabs(dx1*dy1)*c1+fabs(dx2*dy2)*c2;
  if (xx+yy>0) exy = exy/sqrt(xx)/sqrt(yy);
  cout << " " << sqrt(xx) << " " << sqrt(yy) <<" "<< exy <<endl;; 
  return exy; //return combined correlation, as the first two outputs are just easy to obtainw with quadratic sum
};



float qs( float a, float b,float c){
  return qs(qs(a,b),c);

};
float qs( float a, float b,float c, float d){
  return qs(qs(a,b,c),d);
}
float qs( float a, float b,float c, float d, float e){
  return qs(qs(a,b,c),qs(d,e));
}

float invqs( float a, float b, float c){
  return 1/qs(1./a,1./b, 1./c);
};

float invqs( float a, float b, float c, float d){
  return 1/qs(1./a,1./b, 1./c, 1./d);
};


float invqs( float a, float b){
  return 1/qs(1./a,1./b);
};

// Just a quick helper to combine uncorrelated measurement easily
float CombineError(float a, float ae,float b=0, float be=0, float c=0, float ce=0, float d=0, float de=0, float e=0, float ee=0){
  // inverse all error
  if (ae>0) ae=1/ae/ae;
  if (be>0) be=1/be/be;
  if (ce>0) ce=1/ce/ce;
  if (de>0) de=1/de/de;
  if (ee>0) ee=1/ee/ee;
  float result=   a*ae + b*be + c*ce +d*de+ e*ee;
  float etot=  ae+be+ce+de+ee;
  if (etot>0)  result/=etot;
  if (etot>0) etot=sqrt(1/etot);
  cout << "Res=" << result << "+/-" << etot << endl;
  return result;
	     


}





void PrintAllCanvas(TString fname="tmp.root",TString fileout="allcanvas.ps",float scale=1){
  bool kbatch=gROOT->IsBatch();
 
  // Slava TIter next_mc(f_mc.GetListOfKeys()) ;
  // while (TKey* key = (TKey*) next_mc()) {
  FILE * _out = fopen("/tmp/toto.tmp","w");
fprintf(_out,"\\documentclass[a4paper]{article}\n\\usepackage{epsfig}\n\\hoffset=-2.5cm\n\\voffset=-1cm\n\\oddsidemargin=3cm\\evensidemargin=2cm\\textwidth=16cm\n\\begin{document}\\noindent\n");
 int nplot=1;
 char tmp[100];
  //TPostScript *ps=new TPostScript("test.ps",111); 
  // ps->Close();

TFile * f = new TFile(fname);
 
 TIter iter (f->GetListOfKeys()); 
 iter.Reset();  //not usefull here, reset the iterator
 

 //  TList*l = (TList*) f->GetListOfKeys();
 //  TObject* element=l->First();

 /// Pict2File.Open(fileout,"ps");

 TKey * next;  /// does not work with an object
 while (  (bool) (next  =  (TKey*) iter())) {

   //   if (strstr( next->ReadObj()->ClassName(),"TStyle"))
     //   gROOT->SetStyle(((TStyle*)next->ReadObj())->GetName());

    if (strstr( next->ReadObj()->ClassName(),"TCanvas")){
      cout << "find TCanvas " << next->ReadObj()->GetName()<< endl;
      gROOT->SetBatch(1); //force non graphical operation does not seem to work
 
      TCanvas * c = (TCanvas*) next->ReadObj();
      //      c->SetGrid(1);

      // Pict2File.On();
      //   Pict2File.NewPage();
      // ps->Open("test.ps");
      c->Draw();
      TString fileroot_eps = fileout;
      fileroot_eps.ReplaceAll("/","_");
      sprintf(tmp,"/tmp/plot_%s_%d.eps",fileroot_eps.Data(),nplot++);
      cout << " want to plot " << tmp << endl;
	c->Print(tmp);
	//	delete c; // test  15/6/2009
	float effectivescale;
	if (scale!=0){
	  int integer=((int) (1./scale))+1;
	 
	  effectivescale= 1./ integer;
	    }else
	  {effectivescale=1;}

	fprintf(_out," \\epsfig{figure=%s,width=%f\\textwidth}\n",tmp,effectivescale);
	if (scale==0)  
	  fprintf(_out,"\\newpage\n");	
	//	fprintf(_out," \\epsfig{figure=%s}\\\\\n",tmp);

      // ps->NewPage(); 
      // ps->Close();
      
      // c->Clear();

      //ps->On();
      
      //  delete c;
      //c->Print("toto.eps");
    }
 }
 //Pict2File.Close();
 //ps->Close();
 //delete ps;

 TString outname="/tmp/tmplatex";
 fprintf (_out,"\\end{document}");
  fclose(_out);

    sprintf (tmp," cd /tmp; mv /tmp/toto.tmp %s.tex",outname.Data());

    gSystem->Exec(tmp);
      gSystem->Exec("cd /tmp;  rm -f "+ outname +".dvi ; latex "+outname + ".tex ; dvips -o " + outname+".ps " + outname +".dvi");
  gSystem->Exec("mv "+outname +".ps "+fileout );

  gROOT->SetBatch(kbatch);
};
 


double* GetOptimalBinning(TH1F * hold, int nbin){
  TH1F * h= (TH1F*)hold->Clone("to_integrate");
  Norm(h);
  Integral(h);
  double * xbins = new double[nbin+1];
  xbins[0]=h->GetBinLowEdge(1);
  xbins[nbin]=h->GetBinLowEdge(h->GetNbinsX()+1);
  //debg
  //  cout << "boundaries"<< h->GetBinLowEdge(1)<< " " <<h->GetBinLowEdge(h->GetNbinsX()+1)<<endl;
 int nbinnew=1;
 //  int iold=0; int isecond=0 ; 
  int iold=2; int isecond=0 ; 
  //  bool ifound=0;
  for (int j=1; j< nbin; j++){ //each boundaries
   
    //  iold++;
    for (int i=iold; i < h->GetNbinsX()+2 ; i++){
      if (h->GetBinContent(i)>=1.0*j/nbin) {
	iold=i;  // I have found it;
			
	break;
      }
    }
    isecond=0;
    int itest=iold-1;
    //    debug
    //    cout << "in the histo bin="<<itest<< " edge=" << h->GetBinLowEdge(itest)<<  " (" << h->GetBinContent(itest)<< "<" << 1.0*j/nbin << ")" << endl; 
    if (itest>=0){
      for (int i=itest; i <  h->GetNbinsX()+1 ; i++){
	if (h->GetBinContent(itest)<h->GetBinContent(i)){
	  isecond=i;
	  iold=i;
	  //  cout << "second bin="<<isecond<<" edge=" << h->GetBinLowEdge(isecond)<<   " (" << h->GetBinContent(isecond)<< ">" << 1.0*j/nbin << ")"<< endl;;
	break;
	}
      }
    }
  
    //    cout << " found bin isecond " << isecond << endl;

    if (isecond!=0 ){


xbins[nbinnew]=h->GetBinLowEdge(itest) - (h->GetBinLowEdge(itest)-h->GetBinLowEdge(isecond))*(h->GetBinContent(itest)- j*1.0/nbin)/ (  h->GetBinContent(itest)-  h->GetBinContent(isecond));


    }else {
      //    cout << "here "<<itest<<endl;
      xbins[nbinnew]=h->GetBinLowEdge(itest);
    }
    
    if (xbins[nbinnew]<=xbins[nbinnew-1]){
      xbins[nbinnew]=h->GetBinLowEdge(iold);  
      iold++; // to have increasing binning  danger to run out of bin

    }

  
    //cout << h->GetBinLowEdge(itest) << " become " <<xbins[nbinnew]<<endl;;
    //  cout << "fineal choice "<< xbins[nbinnew] <<endl;
 nbinnew++;   
  }
  cout << "double x["<<nbinnew+1<<"]={";
  nbinnew++; // the last bin
  for (int i=0; i< nbinnew ; i++){
      cout << xbins[i] ;
      if (i<nbinnew-1) cout << ",";
      if (i>1){
		if ( xbins[i] <= xbins[i-1] ) cout << endl << " problem in the binning " << xbins[i-1] << " " << xbins[i];
      }
      

    }
  cout <<"};"<<endl;

  if  ( nbinnew!=nbin+1) cout << "GetOptimalBinning:: serious problem, I don't have the right number of bins :" << " expected size of array"
			    <<nbin+1<< " " << "Obtained="<< nbinnew<< endl;
  return xbins;
}




void VariableWidthNormalisation(TH1 * histo){/// transform the histogram into spectrum= divide by the bin width
  // float scale=0;
 for (int i=0; i<histo->GetNbinsX()+2; i++){
   histo->SetBinContent(i,histo-> GetBinContent(i)/ histo->GetBinWidth(i))  ;
   histo->SetBinError(i,histo-> GetBinError(i)/ histo->GetBinWidth(i))  ;
 }

}



void FitSliceXYZ(TH3 * h, TF1* f1){///< fit content=f(Z) for each (x,y) pair and return a 2d histogram    name_0, name_1,...  where 0,1,.. are the fit parameters of the function f1
  const int nparmax=100;
  if (f1==0){
    cout <<  "FitSliceXYZ::function not defined " << endl;
    return;

}
  
  int nparameter = f1->GetNpar();
  char tmp[100];
  if (nparameter >= nparmax) {
    cout << "FitSliceXYZ::too many parameters in function" << endl;
    return;
  }

  // first prepare the output histograms
  TH2F * hresult[nparmax]={NULL};
  for (int i=0 ; i<  nparameter;i++){
    sprintf(tmp,"%s_%d",h->GetName(),i);
    hresult[i]= new TH2F(tmp,tmp,
			 h->GetNbinsX(),
			 h->GetXaxis()->GetBinLowEdge(1),
			 h->GetXaxis()->GetBinLowEdge(h->GetNbinsX()+1),
			 h->GetNbinsY(),
			 h->GetYaxis()->GetBinLowEdge(1),
			 h->GetYaxis()->GetBinLowEdge(h->GetNbinsY()+1)
			 );

  }
  
  TH3F * hnew= (TH3F*) h->Clone("for_tmp_usage");
  for (int i=1 ; i < hnew->GetNbinsX()+1 ; i++){
    // deal with bin on X axis  
    // first copy into an TH2F

    hnew->GetXaxis()->SetRange(i,i);
    hnew->Project3D("ezy");  // here  z is the y-axis of the resulting histo...
    TH2F * tmphisto = (TH2F*)gDirectory->Get("for_tmp_usage_ezy");
    
//debug
 //    TCanvas * cnew = new TCanvas("cnew","cnew");
//     cnew->cd();
//     cout << "created cnew "<<endl;
//     tmphisto->Draw("colz") ; cnew->Update(); getchar(); //debug
    // then fit the 2d hist
    tmphisto->FitSlicesY(f1);
    // Get th parameters in the histograms
    
    for (int p=0 ; p<  nparameter;p++){
      //  cout << "param p " << p << endl;
      sprintf(tmp,"for_tmp_usage_ezy_%d",p);
      TH1F * h1 = (TH1F*)gDirectory->Get(tmp);
      //    h1 ->Draw("E");  cnew->Update(); getchar(); //debug


      for (int  inew=1 ; inew <h1->GetNbinsX()+1 ; inew++) {
	float  x = h1->GetBinContent(inew);
	float ex = h1->GetBinError(inew);
	hresult[p]->SetBinContent(i, inew,x);
	hresult[p]->SetBinError(i, inew,ex);
	//	cout << " low egde is " << h1->GetBinLowEdge(inew) << endl;
	//  cout << " ini.C x ex " << x << " " << ex << endl; 

	//     cout << " cecking the binning "<<  "expect " << hnew->GetNbinsY() << " see " << h1->GetNbinsX() << endl;
	
      }   // fill bin   i,inew

      delete h1;
    } // fill parameter p

 delete tmphisto;

  }

}


TString   GetShellResult(TString command)
{   TString result;
  FILE  * f = gSystem->OpenPipe(command,"r"); 
  
  char tmp[1000]="";
  //  while (fscanf(f,"%s",tmp)!=EOF){
 
  // do { result+=tmp; cout << "tot" ;}
  //while (fgets(tmp,sizeof(tmp),f))

  while (fgets(tmp,sizeof(tmp),f)){
    result+=tmp;
  }
  result.Chop();  //remove last carriage return
  fclose(f);
  return result;
}


void PrintLinLog(TString fig, TVirtualPad * pad=0){
  // first try to determine type of file
  const int ntype=4;
  int index=-1;
  int itype_found=-1;
  TString ftype[ntype]={".eps",".png",".gif",".jpg"};
  for (int itype=0 ; itype<ntype ; itype++){
    int index_suff=fig.Length()-4;
    index=fig.Index(ftype[itype],index_suff);
    if (index>=0) {
      itype_found=itype;
      fig.ReplaceAll(ftype[itype_found],"");
      //      cout << "fig " << fig << endl;
      break;
    }
    
  }
  
  if (index<0) {
    cerr << " ini::PrintLinLog  unknown file suffixe found in " << fig << endl;
    return;
  }
  TString lin_file= fig+"_lin"+ftype[itype_found];
  TString log_file= fig+"_log"+ftype[itype_found];
  //    cout << "lin file will be " <<lin_file << endl;
     //cout << "log file will be " <<log_file << endl;
  if (pad==0) return;
  //save ssstatus
  int logy=pad->GetLogy();

  pad->SetLogy(1);
  pad->Print(log_file);
  pad->SetLogy(0);
  pad->Print(lin_file);
  pad->SetLogy(logy);
  return;

}

TPaveText * TextToCanvas(stringstream &strstrm, int nmaxline=20){
  
  //  strstrm.seekg(ios_base::beg);
  string str;
  TPaveText *  info = new TPaveText(0,0,1,1,"NDC");
  // info->SetY2(0.1);
 info->SetTextAlign(13);
 info->SetTextFont(82);//courrier with fixed width
   info->SetFillColor(0);
  int nline=1;
  while (getline(strstrm,str,'\n')) { // test
    info->AddText(str.c_str());
        cout << str.c_str() << endl;
   nline++;
  }
  info->AddText(""); //one additionnal line
  

  //strstrm.seekg(ios_base::beg);
  //while (getline(strstrm,str,'\n')) { // test
  //    info->AddText(str.c_str());
  //       cout << str.c_str() << endl;
  //   nline++;
  //  }


   if (nline<nmaxline)   info->SetY1(1.-float(nline)/float(nmaxline));
  //  info->SetY2NDC(0.1);


  return info;
}


/*
/// In developemnt ?? don't emember what is was.
void tweak_prof(TChain *t, TString var, TString cut, TString opt){
  // 1d or 2d
  int nd=var.CountChar(':');  
  if (nd<=0 || nd>=3) {
    cerr << "Problem in syntax " << var  << " should correspond to 1d or 2d profile histo " << endl;
    return;
  }
  // now gets the histo name if any
  int index;
  TString histostring;  
 TString varstring=var;  
 index=var.Index(">>");
  if (index>0){
    histostring=var(index+2, var.Length()-index -2);
    varstring=var(0,index);
    //  cout << "histostring " <<  histostring << endl;
    //  cout << "varsring " <<  varstring << endl;
  }

 
  // now parse variable
  index=var.Index(":");
  TString lastvar;
  TString xyvar;
 if (index>0){
   lastvar=varstring(0,index);
   xyvar=varstring(index+1, varstring.Length()-index-1);
 }else
   {
     cerr << "issue with syntax of " << varstring << endl;
     return;
   }
 cout << " want to plot " << lastvar << " as a function of " << xyvar << " in " << histostring << endl;
  
 
 if (nd==1){

 }
}

*/

TTree * GetTree(int n=0, bool verbose=0){

  TDirectory *current_sourcedir =  gDirectory;
  current_sourcedir -> cd();
  TIter nextkey( current_sourcedir->GetListOfKeys() );
 TKey *key;
 int count=0;
 TObject *obj_out=0;
    while ( (key = (TKey*)nextkey())) {
      TObject *obj = key->ReadObj();
      if ( obj->IsA()->InheritsFrom( "TTree" ) ){
	if (verbose)cout << " Found TTree : " << obj -> GetName() << endl;
      if (count++ == n ) obj_out=obj;
      }}
    if( obj_out==0){
      if (verbose) cout << "GetTree found no tree at position " << n << ", returning 0 " << endl;
      return 0;
    }else
      {
	if (verbose)	cout << "GetTree returning: " <<  obj_out-> GetName()  <<endl;
	return (TTree*) obj_out;

      }
}


void BoboPrint( TTree * obj=0){
  if (obj==0) { cout<< "BoboPrint:: no input specified, trying to retrieve it myself."  ; obj=GetTree(0,1) ; }
  if (obj==0) { cout <<" BoboPrint:: no valied input, bye " << endl; return ;}
  if (! obj->IsA()->InheritsFrom( "TTree" ) ) { cout<<"BoboPrint not a tree as input " << endl ; return ;}
 TTree *tree = (TTree*)obj;  
 cout << "content for " <<  obj -> GetName() << " " << ((TTree*) obj )->GetTitle()<<  endl;
 if (tree){
   if (tree->GetListOfFriends())
     tree->GetListOfFriends()->ls();

 }

 TIterator *iter = tree->GetIteratorOnAllLeaves();
      TObject *obj_t = 0;
      while ((obj_t = (*iter)())) {
	//bricolage

	//    printf("\t name = %s title = %s class = %s\n",
	//	     obj_t->GetName(), obj_t->GetTitle(), obj_t->ClassName());
	if (obj_t->IsA() ->InheritsFrom( "TBranch" ))
	  cout << " Branch " <<": " << obj_t->GetName() << "" <<endl;
	if (obj_t->IsA() ->InheritsFrom( "TLeaf" )){
	  
	  TObject *  obj_b = ((TLeaf*)obj_t)->GetBranch();
	  printf("Branch: %-20s   Leaf:%-20s  %3s \n", obj_b->GetName(), obj_t->GetName(), ((TLeaf*)obj_t)->GetTypeName());
	  //	  cout << " Branch:"              Leaf " <<": " << obj_t->GetName() << "" <<endl;



	}

      }

}




TH1 * GetHistBobo(int n=0, bool verbose=0){

  TDirectory *current_sourcedir =  gDirectory;
  current_sourcedir -> cd();
  TIter nextkey( current_sourcedir->GetListOfKeys() );
 TKey *key;
 int count=0;
 TObject *obj_out=0;
    while ( (key = (TKey*)nextkey())) {
      TObject *obj = key->ReadObj();
      if ( obj->IsA()->InheritsFrom( "TH1" ) ){
	if (verbose)cout << " find TH1 : " << obj -> GetName() << endl;
      if (count == n ) obj_out=obj;
      count++;
      }
    }
    if( obj_out==0){
      if (verbose) cout << "GetHist found no tree at position " << n << ", returning 0 " << endl;
      return 0;
    }else
      {
	if (verbose)	cout << "GetHisto returning: " <<  obj_out-> GetName()  <<endl;
	return (TH1*) obj_out;

      }
}

// This is to have the same simpler behavior as paw famous vector/read
TH1F * FileToHist(TString fname, int nbin=100, float xmin=0,float xmax=0,TH1F * hinput=0){

  // I put things in a graph. This is dirty but simple.
  // At least this work in interactive mode..
  TGraph *g=new TGraph();
  int index=0;  //Graph index
  double x;
  cout << "Opening " << fname <<""<<endl;
  float xfound_min=xmin;
  float xfound_max=xmax;


  ifstream fin(fname);
  float mean=0;
  while (fin>>x){
    g->SetPoint(index,index,x); 
    index++;
    if (index==1) {
      xfound_min=x;//initialize
      xfound_max=x;
    }
    if (xfound_min>x) xfound_min=x; 
    if (xfound_max<x) xfound_max=x; 

    mean+= (x-mean)/index;
  }

  cout << "Obtained " << index << " values from file " << endl;
  // cout << "Obtained mean value:"<<mean<<endl;
  if (xmin==xmax&& xmax==0){  
    xmin=xfound_min+(xfound_min-xfound_max)*1.0001;
    xmax=xfound_max-(xfound_min-xfound_max)*1.0001 ; //strange things happened since I create artificially overflow and undeflow. Fixed Sep-10-2014
    cout <<"Creating histo with boudaries " << xmin <<";"<<xmax << endl;

} //found myself the bound
  //  TH1F* h=0; h=0;  
  if( hinput==0)
   hinput = new TH1F("histo_from_file","histo_from_file",nbin,xmin,xmax);
  //  else h=(TH1F*) hinput->Clone("histo_from_file");
  mean=0;
  for (int i=0 ; i< index ; i++){
    double xx,yy;
    g->GetPoint(i,xx,yy);
    hinput->Fill (yy );
    mean+= (yy-mean)/(i+1);
    //    cout << "fillig " << y << endl;
  }
  //  cout << "ReObtain mean value:"<<mean<<endl;
  //  cout << "Test:"<<hinput->GetMean()<<endl;


  delete g;
  return hinput;

}

  

double logFact(int N){
  if (N<=1) return 0;
  double result=0;
  for (int i=2; i<=N; i++)
    result+=log(i);
  return result;
  //  return log(N)+logFact(N-1);
}

double Poisson(double mu, int n){
  double logresult= -mu+n*log(mu)-logFact(n);
  return exp(logresult);
}


//return Prob(N<=Nbobs |mu)
double SumPoisson(double mu, int n){
  //  if (n <=0) return  Poisson(mu,n);
  //  return Poisson(mu,n) + SumPoisson(mu,n-1);
  double result=0;
  for (int i=0; i<=n; i++)
    result+=Poisson(mu,i);
      return result;


};
//return Prob(N>=Nbobs |mu)
double SumPoissonExcess(double mu, int n){
  double result=0;
  for (int i=0; i<n; i++)
    result+=Poisson(mu,i);
      return 1-result;
};


}//namespace


using namespace  Bobo_util;

#endif

