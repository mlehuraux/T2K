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
    pline->SetLineWidth(2);
    return(pline);
}

int main(int argc, char **argv)
{
    string input_file(argv[1]);
    string event(argv[2]);

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

    TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10,600,480);
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
            cout << i << "  " << j << "     " << P.card() << "   " << P.chip() << "  " << P.channel() << endl;
            if (P.channel()!=-99) // error somewhere in DAQ
            {
                //P.setAmp(ADCAmpl[P.card()][P.chip()][P.channel()][300]);
                P.setAmp(MaxStripAmpl[P.card()][P.chip()][P.channel()]);
            }
            if (P.ampl() > 300){padline(P,2)->Draw("f");}
            else{padline(P)->Draw("f");}
            padline(P)->Draw();
        }
    }

    p1->Modified();
    canvas->Update();
    canvas->SaveAs((loc::outputs+ + "event_" + event + ".pdf").c_str());
    f1->Close();

    return(0);
}
