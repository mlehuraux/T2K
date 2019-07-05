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
#include "TH2I.h"

#include "../src/T2KConstants.h"

using namespace std;


void fast_display(string input_file, int start, int nevent)
{

    string input_file_name = input_file.substr(0, input_file.size()-5);

    TFile * f1 = TFile::Open(( loc::rootfiles + input_file ).c_str(), "UPDATE");
    TTree * t1 = (TTree*)f1->Get("tree");
    cout << "File open" << endl;

    Int_t eventNumber;
    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples];
    Int_t PadMaxAmpl[geom::nPadx][geom::nPady];
    Int_t PadAmpl[geom::nPadx][geom::nPady][n::samples];
    Int_t ntot = t1->GetEntries();

    t1->SetBranchAddress("ADCAmpl", &ADCAmpl);
    t1->SetBranchAddress("eventNumber", &eventNumber);
    //t1->SetBranchAddress("PadMaxAmpl", &PadMaxAmpl);
    t1->SetBranchAddress("PadAmpl", &PadAmpl);


    gStyle->SetPalette(kBird);
    gStyle->SetOptStat(0);

    for (int p=start; p<min(ntot, start+nevent); p++)
    {
        t1->GetEntry(p);
        TH2I *mypads = new TH2I("mypads","", geom::nPadx,0,geom::nPadx, geom::nPady,0,geom::nPady);
        for (int i=0; i < geom::nPadx; i++)
        {
            for (int j=0; j<geom::nPady; j++)
            {
                cout << i << "  " << j << "     " << PadMaxAmpl[i][j] << endl;
                mypads->Fill(i, j, PadMaxAmpl[i][j]);
                //if (PadAmpl[i][j][250]>0){mypads->Fill(i, j, PadAmpl[i][j][250]);}

            }
        }

        TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10,geom::wx,geom::wy);
        mypads->SetMinimum(-0.1);
        mypads->GetXaxis()->SetTitle("Pads on X axis");
        mypads->GetYaxis()->SetTitle("Pads on Y axis");
        mypads->Draw("COLZ");
        canvas->Update();
        canvas->SaveAs((loc::outputs+ input_file_name + "/fast_event_" + to_string(p) + ".gif").c_str());
        canvas->Close();
        canvas->Clear();
        mypads->Reset();
        delete canvas;
        delete mypads;
        /*for (int i=0; i < geom::nPadx; i++)
        {
            for (int j=0; j<geom::nPady; j++)
            {
                mypads->Fill(i, j, 0);
            }
        }*/
    }
    f1->Close();

}
