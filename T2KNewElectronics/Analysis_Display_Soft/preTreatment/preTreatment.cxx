#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include <iostream>
#include "TH1I.h"
#include "TCanvas.h"

#include "../src/Pixel.h"
#include "../src/Mapping.h"
#include "../src/Pads.h"
#include "../src/DAQ.h"
#include "../src/T2KConstants.h"

using namespace std;

int main(int argc, char **argv)
{
    string input_file(argv[1]);

    TFile * f1 = TFile::Open(( loc::rootfiles + input_file ).c_str(), "UPDATE");
    TTree * t1 = (TTree*)f1->Get("tree");
    cout << "File open" << endl;

    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples];
    Int_t MaxStripAmpl[n::cards][n::chips][n::bins];
    Int_t MaxStripPos[n::cards][n::chips][n::bins];
    Int_t ntot = t1->GetEntries();

    t1->SetBranchAddress("ADCAmpl", & ADCAmpl);

    TBranch *b1 = t1->Branch("MaxStripAmpl", &MaxStripAmpl, Form("MaxStripAmpl[%i][%i][%i]/I", n::cards, n::chips, n::bins));
    TBranch *b2 = t1->Branch("MaxStripPos", &MaxStripPos, Form("MaxStripAmpl[%i][%i][%i]/I", n::cards, n::chips, n::bins));

    cout << "Tree OK" << endl;

    for ( int i = 0; i < ntot; ++i){ //loop over events
        t1->GetEntry(i);
        for ( int p = 0; p < n::cards; ++p){ //cards ARC
    	    for ( int q = 0; q < n::chips; ++q){ // chip AFTER
                for ( int r = 0; r < n::bins; ++r){
                    TCanvas *canvas = new TCanvas("canvas", "canvas", 200,10, 2*geom::wx,geom::wy);
                    TH1I *signal = new TH1I("signal", "Signal", n::samples, 0, n::samples);
    		        int maxindex = 0;
    		        for ( int s = 0; s < n::samples; ++s){ // Samples
                        signal->Fill(s, ADCAmpl[p][q][r][s]);
                        if (ADCAmpl[p][q][r][s] >= ADCAmpl[p][q][r][maxindex]) maxindex = s;
    		        }
                    MaxStripAmpl[p][q][r] = ADCAmpl[p][q][r][maxindex];
                    MaxStripPos[p][q][r] = maxindex;
                    if (MaxStripAmpl[p][q][r] > 30 && i==10)
                    {
                        signal->SetMinimum(0.);
                        signal->SetLineColor(4);
                        signal->SetLineWidth(2);
                        signal->GetXaxis()->SetTitle("Sample");
                        signal->GetYaxis()->SetTitle("ADC");
                        signal->Draw("hist");
                        canvas->Update();
                        canvas->SaveAs((loc::outputs + "SignalsEvent10/channel_" + to_string(r) + ".gif").c_str());
                    }
                    delete signal;
                    delete canvas;
                }
            }
        }
        b1->Fill();
        b2->Fill();


    }
    cout << "Tree filled" << endl;

    t1->Write("", TObject::kOverwrite);
    cout << "Tree written" << endl;

    f1->Close();
    return(0);

}
