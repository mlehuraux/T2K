#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include <iostream>
#include "TH1I.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "../src/Pixel.h"
#include "../src/Mapping.h"
#include "../src/Pads.h"
#include "../src/DAQ.h"
#include "../src/T2KConstants.h"

using namespace std;

int main(int argc, char **argv)
{
    string input_file(argv[1]);
    string input_file_name = input_file.substr(0, input_file.size()-5);

    TFile * f1 = TFile::Open(( loc::rootfiles + input_file ).c_str(), "UPDATE");
    TTree * t1 = (TTree*)f1->Get("tree");
    cout << "File open" << endl;

    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples];
    Int_t MaxStripAmpl[n::cards][n::chips][n::bins];
    Int_t MaxStripPos[n::cards][n::chips][n::bins];
    Int_t eventNumber;
    Int_t ntot = t1->GetEntries();

    t1->SetBranchAddress("ADCAmpl", &ADCAmpl);
    t1->SetBranchAddress("eventNumber", &eventNumber);
    t1->SetBranchAddress("MaxStripAmpl", &MaxStripAmpl);
    t1->SetBranchAddress("MaxStripPos", &MaxStripPos);

    TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10,geom::wx,geom::wy);
    TH1I *histo = new TH1I("histo", "Time window", n::samples, 0, n::samples);
    gStyle->SetOptStat(0);

    for ( int i = 0; i < ntot; ++i)
    { //loop over events
        t1->GetEntry(i);
        for ( int p = 0; p < n::cards; ++p)
        { //cards ARC
    	    for ( int q = 0; q < n::chips; ++q)
            { // chip AFTER
                for ( int r = 0; r < n::bins; ++r)
                {
                    if (MaxStripAmpl[p][q][r]>0){histo->Fill(MaxStripPos[p][q][r]);}
                }
            }
        }
    }
    histo->SetMinimum(0.);
    histo->SetLineColor(4);
    histo->SetLineWidth(2);
    histo->GetXaxis()->SetTitle("Sample");
    histo->GetYaxis()->SetTitle("#Channels triggered");
    histo->Draw("hist");
    canvas->Update();
    canvas->SaveAs((loc::outputs+ "TimeWindow_" + input_file_name + ".gif").c_str());
    f1->Close();

}
