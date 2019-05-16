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
#include "TH2I.h"

#include "../src/Pixel.h"
#include "../src/Mapping.h"
#include "../src/Pads.h"
#include "../src/DAQ.h"

using namespace std;


void fast_display(string input_file, int start, int nevent)
{

    string input_file_name = input_file.substr(0, input_file.size()-5);

    TFile * f1 = TFile::Open(( loc::rootfiles + input_file ).c_str(), "UPDATE");
    TTree * t1 = (TTree*)f1->Get("tree");
    cout << "File open" << endl;

    Int_t eventNumber;
    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples];
    Int_t MaxStripAmpl[n::cards][n::chips][n::bins];
    Int_t MaxStripPos[n::cards][n::chips][n::bins];
    Int_t PadAmpl[geom::nPadx][geom::nPady];
    Int_t ntot = t1->GetEntries();

    t1->SetBranchAddress("ADCAmpl", &ADCAmpl);
    t1->SetBranchAddress("eventNumber", &eventNumber);
    t1->SetBranchAddress("MaxStripAmpl", &MaxStripAmpl);
    t1->SetBranchAddress("MaxStripPos", &MaxStripPos);
    t1->SetBranchAddress("PadAmpl", &PadAmpl);

    gStyle->SetPalette(kBird);
    gStyle->SetOptStat(0);

    for (int p=start; p<min(ntot, start+nevent); p++)
    {
        t1->GetEntry(p);
        TH2I *pads = new TH2I("pads","", geom::nPadx,0,geom::nPadx, geom::nPady,0,geom::nPady);
        for (int i=0; i < geom::nPadx; i++)
        {
            for (int j=0; j<geom::nPady; j++)
            {
                pads->Fill(i, j, PadAmpl[i][j]);
            }
        }

        TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10,geom::wx,geom::wy);
        pads->SetMinimum(-0.1);
        pads->GetXaxis()->SetTitle(" Pads on X axis");
        pads->GetYaxis()->SetTitle("Pads on Y axis");
        pads->Draw("COLZ");
        canvas->Update();
        canvas->SaveAs((loc::outputs+ input_file_name + "/fast_event_" + to_string(p) + ".gif").c_str());
        canvas->Close();
        delete canvas;
        delete pads;
    }
    f1->Close();

}
