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

#include "../src/Pixel.h"
#include "../src/Mapping.h"
#include "../src/Pads.h"
#include "../src/DAQ.h"

using namespace std;


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
    string event(argv[2]);

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



    int eventid = stoi(event);

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
    Int_t MaxStripAmpl[n::cards][n::chips][n::bins];
    Int_t MaxStripPos[n::cards][n::chips][n::bins];
    Int_t ntot = t1->GetEntries();

    t1->SetBranchAddress("ADCAmpl", &ADCAmpl);
    t1->SetBranchAddress("eventNumber", &eventNumber);
    t1->SetBranchAddress("MaxStripAmpl", &MaxStripAmpl);
    t1->SetBranchAddress("MaxStripPos", &MaxStripPos);

    TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10,geom::wx,geom::wy);
    //canvas->SetWindowSize(geom::wx , geom::wy);
    //gPad->Range(0, 0, 1, 1);
    TPad *p1 = new TPad("p1", "p1", 0.1, 0.1, 0.9, 0.7);
    p1->Range(-0.5*geom::nPadx*geom::dx, -0.5*geom::nPady*geom::dy, 0.5*geom::nPadx*geom::dx, 0.5*geom::nPady*geom::dy);
    p1->Draw();
    p1->cd();

    t1->GetEntry(eventid);

    for (int i = 0; i < geom::nPadx; i++)
    {
        for (int j = 0; j < geom::nPady; j++)
        {
            Pixel P;
            P = padPlane.pad(i,j);
            //cout << i << "  " << j << "     " << P.card() << "   " << P.chip() << "  " << P.channel() << endl;
            if (P.channel()!=-99||P.channel()==65 || P.channel()==73) // error somewhere in DAQ
            {
                //P.setAmp(ADCAmpl[P.card()][P.chip()][P.channel()][300]);
                P.setAmp(MaxStripAmpl[P.card()][P.chip()][P.channel()]);
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
    canvas->SaveAs((loc::outputs+ + "event_" + event + ".gif").c_str());
    f1->Close();

    return(0);
}
