#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include <iostream>
#include "TH1.h"
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
    string input_file_name = input_file.substr(0, input_file.size()-5);

    DAQ daq;
    daq.loadDAQ();
    cout << "... DAQ loaded successfully" << endl;

    Mapping T2K;
    T2K.loadMapping();
    cout << "...Mapping loaded succesfully." << endl;

    TFile * f1 = TFile::Open(( loc::rootfiles + input_file ).c_str(), "UPDATE");
    TTree * t1 = (TTree*)f1->Get("tree");
    cout << "File open" << endl;

    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples]={};
    Int_t MaxAmpl[n::cards][n::chips][n::bins]={};
    Int_t MaxAmplTimeSample[n::cards][n::chips][n::bins]={};
    Int_t PadAmpl[geom::nPadx][geom::nPady][n::samples]={};
    Int_t PadMaxAmpl[geom::nPadx][geom::nPady]={};
    Int_t PadMaxAmplTimeSample[geom::nPadx][geom::nPady]={};

    Int_t ntot = t1->GetEntries();

    t1->SetBranchAddress("ADCAmpl", &ADCAmpl);

    TBranch *b1 = t1->Branch("MaxAmpl", &MaxAmpl, Form("MaxAmpl[%i][%i][%i]/I", n::cards, n::chips, n::bins));
    TBranch *b2 = t1->Branch("MaxAmplTimeSample", &MaxAmplTimeSample, Form("MaxAmplTimeSample[%i][%i][%i]/I", n::cards, n::chips, n::bins));
    TBranch *b3 = t1->Branch("PadAmpl", &PadAmpl, Form("PadAmpl[%i][%i][%i]/I", geom::nPadx, geom::nPady, n::samples));
    TBranch *b4 = t1->Branch("PadMaxAmpl", &PadMaxAmpl, Form("PadMaxAmpl[%i][%i]/I", geom::nPadx, geom::nPady));
    TBranch *b5 = t1->Branch("PadMaxAmplTimeSample", &PadMaxAmplTimeSample, Form("PadMaxAmplTimeSample[%i][%i]/I", geom::nPadx, geom::nPady));


    cout << "Tree OK" << endl;

    for ( int i = 0; i < ntot; ++i){ //loop over events
        t1->GetEntry(i);

        // Empty all branches
        // Elec
        for (int i = 0; i < n::cards; i++)
        {
            for (int j = 0; j < n::chips; j++)
            {
                for (int k = 0; k < n::bins; k++)
                {
                    MaxAmpl[i][j][k]=0;
                    MaxAmplTimeSample[i][j][k]=0;
                    for (int l = 0; l<n::samples; l++)
                    {
                        ADCAmpl[i][j][k][l] = 0;
                    }
                }
            }
        }
        // Pads
        for (int i = 0; i < geom::nPadx; i++)
        {
            for (int j = 0; j < geom::nPady; j++)
            {
                PadMaxAmpl[i][j]=0;
                PadMaxAmplTimeSample[i][j]=0;
                for (int k=0; k<n::samples; k++)
                {
                    PadAmpl[i][j][k]=0;
                }
            }
        }


        for ( int p = 0; p < n::cards; ++p){ //cards ARC
    	    for ( int q = 0; q < n::chips; ++q){ // chip AFTER
                for ( int r = 0; r < n::bins; ++r){
                    if (r!=15 && r!=28 && r!=53 && r!=66 && r>2 && r<79)
                    {
                        int maxindex = 1;
        		        for ( int s = 1; s < n::samples; ++s)
                        { // Samples
                            if (ADCAmpl[p][q][r][s] >= ADCAmpl[p][q][r][maxindex]) maxindex = s;
                            PadAmpl[T2K.i(p, q, daq.connector(r))][T2K.j(p,q,daq.connector(r))][s] = ADCAmpl[p][q][r][s];
        		        }
                        MaxAmpl[p][q][r] = ADCAmpl[p][q][r][maxindex];
                        MaxAmplTimeSample[p][q][r] = maxindex;
                        cout << maxindex << endl;
                        PadMaxAmpl[T2K.i(p, q, daq.connector(r))][T2K.j(p,q,daq.connector(r))] = ADCAmpl[p][q][r][maxindex];
                        PadMaxAmplTimeSample[T2K.i(p, q, daq.connector(r))][T2K.j(p,q,daq.connector(r))] = maxindex;
                    }
                }
            }
        }
        b1->Fill();
        b2->Fill();
        b3->Fill();
        b4->Fill();
        b5->Fill();
    }
    cout << "Tree filled" << endl;

    t1->Write("", TObject::kOverwrite);
    cout << "Tree written" << endl;

    f1->Close();
    return(0);

}
