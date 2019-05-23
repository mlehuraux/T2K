#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "TCanvas.h"
#include "TPad.h"
#include "TF1.h"
#include "TBox.h"
#include "TPolyLine.h"
#include "TColor.h"
#include "TFrame.h"
#include "TAttLine.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include "TH1I.h"

#include "../src/Pixel.h"
#include "../src/Mapping.h"
#include "../src/Pads.h"
#include "../src/DAQ.h"

using namespace std;

// Gaussian function
Double_t fgauss(Double_t *x, Double_t *par)
{
    Double_t arg = 0;
    if (par[2] != 0) arg = (x[0] - par[1])/par[2];

    Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg);
    return fitval;
}

TPolyLine *padline(Pixel& P, int color=602)
{
    Float_t x[4] = {geom::convx*(P.coordx()-0.5*geom::dx), geom::convx*(P.coordx()-0.5*geom::dx), geom::convx*(P.coordx()+0.5*geom::dx), geom::convx*(P.coordx()+0.5*geom::dx)};
    Float_t y[4] = {geom::convy*(P.coordy()-0.5*geom::dy), geom::convy*(P.coordy()+0.5*geom::dy), geom::convy*(P.coordy()+0.5*geom::dy), geom::convy*(P.coordy()-0.5*geom::dy)};
    TPolyLine *pline = new TPolyLine(4,x,y);
    pline->SetFillColor(color);
    pline->SetLineColor(kGray);
    pline->SetLineWidth(1);
    return(pline);
}



int main(int argc, char **argv)
{
    string input_file(argv[1]);
    string start(argv[2]);
    string nevent(argv[3]);

    string input_file_name = input_file.substr(0, input_file.size()-5);

    // My colors---------------------------------
    Int_t MyPalette[20];
    const Int_t NRGBs = 5;
    const Int_t NCont = 20;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    gStyle->SetNumberContours(NCont);
    Int_t FI = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    for (int i=0;i<20;i++)
    {
        MyPalette[i] = FI+i;
    }

    int istart = stoi(start);
    int inevent = stoi(nevent);


    DAQ daq;
    daq.loadDAQ();
    cout << "... DAQ loaded successfully" << endl;

    Mapping T2K;
    T2K.loadMapping();
    cout << "...Mapping loaded succesfully." << endl;

    Pads padPlane;
    padPlane.loadPadPlane(daq, T2K);
    cout << "...Pad plane loaded succesfully." << endl;

    TFile * f1 = TFile::Open(( loc::rootfiles + input_file ).c_str(), "UPDATE");
    TTree * t1 = (TTree*)f1->Get("tree");
    cout << "File open" << endl;

    Int_t eventNumber;
    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples];
    Int_t MaxAmpl[n::cards][n::chips][n::bins];
    Int_t MaxAmplTimeSample[n::cards][n::chips][n::bins];
    Int_t ntot = t1->GetEntries();

    t1->SetBranchAddress("ADCAmpl", &ADCAmpl);
    t1->SetBranchAddress("eventNumber", &eventNumber);
    t1->SetBranchAddress("MaxAmpl", &MaxAmpl);
    t1->SetBranchAddress("MaxAmplTimeSample", &MaxAmplTimeSample);

    for (int p=istart; p<min(ntot, istart+inevent); p++)
    {
        TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10,geom::wx,geom::wy);
        //canvas->SetWindowSize(geom::wx , geom::wy);
        //gPad->Range(0, 0, 1, 1);
        TPad *p1 = new TPad("p1", "p1", 0.01, 0.01, 0.99, 0.99);
        p1->Range(-0.5*geom::nPadx*geom::dx, -0.5*geom::nPady*geom::dy, 0.5*geom::nPadx*geom::dx, 0.5*geom::nPady*geom::dy);
        p1->Draw();
        p1->cd();
        t1->GetEntry(p);

        // Initialize track
        //Int_t track[geom::nPady][geom::nPadx];

        for (int i = 0; i < geom::nPadx; i++)
        {
            for (int j = 0; j < geom::nPady; j++)
            {
                Pixel P;
                P = padPlane.pad(i,j);
                //cout << i << "  " << j << "     " << P.card() << "   " << P.chip() << "  " << P.channel() << endl;
                //if (P.channel()!=-99||P.channel()==65 || P.channel()==73) // error somewhere in DAQ
                if (P.channel()!=15&&P.channel()!=28&&P.channel()!=53&&P.channel()!=66&&P.channel()<79&&P.channel()>2) // error somewhere in DAQ
                {
                    //P.setAmp(ADCAmpl[P.card()][P.chip()][P.channel()][300]);
                    P.setAmp(MaxAmpl[P.card()][P.chip()][P.channel()]);
                    //track[j][i]=MaxAmpl[P.card()][P.chip()][P.channel()];
                }
                int color = 2*floor(float(P.ampl())/2000*NCont);
                //cout << color << endl;
                if (NCont-1 < color){color=NCont-1;}
                //if (P.ampl() > 0){padline(P,2)->Draw("f");}
                if (P.ampl() > 0){padline(P,MyPalette[color])->Draw("f");}
                else{padline(P)->Draw("f");}
                padline(P)->Draw();
            }
        }
        p1->Modified();
        canvas->Update();
        canvas->SaveAs((loc::outputs+ input_file_name + "/event_" + to_string(p) + ".gif").c_str());
        delete canvas;
    }

    f1->Close();
/*
    TCanvas *canvas1 = new TCanvas("canvas1", "canvas1",300, 1200);
    canvas1->Divide(1, geom::nPady, 0, 0);

    for (int j = 0; j < geom::nPady; j++)
    {
        canvas1->cd(geom::nPady-j);
        TH1I *h = new TH1I("h", "", geom::nPadx, 0, geom::nPadx/2);
        gStyle->SetOptStat(0);
        for (int i = 0; i < geom::nPadx; i++)
        {
            h->Fill(i, track[j][i]);
        }

        TF1 *fitFcn1 = new TF1("fitFcn1",fgauss, -10., geom::nPadx/2, 3);
        fitFcn1->SetNpx(500);
        fitFcn1->SetLineColor(kRed);
        Double_t mean1 = (Double_t) h->GetMean();
        Double_t rms1  = (Double_t) max(h->GetRMS(), 0.5);
        Double_t max1  = (Double_t) h->GetMaximum();
        fitFcn1->SetParameters( max1, mean1, rms1 );
        fitFcn1->SetParNames( "Constant", "Mean Value", "Sigma" );
        h->Fit(fitFcn1,"","", -5., 5.);
        h->SetMinimum(0.);
        h->SetLineColor(4);
        h->SetLineWidth(2);
        h->Draw("hist");
        fitFcn1->Draw("same");
        canvas1->Update();
    }
    canvas1->SaveAs((loc::outputs+ input_file_name + "/track_" + event + ".gif").c_str());
*/
    return(0);
}
