// ROOT writing in the tree part
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include "../src/Pixel.h"
#include "../src/Mapping.h"
#include "../src/Pads.h"
#include "../src/DAQ.h"
#include "../src/T2KConstants.h"

// Global part
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

void clear(int array[n::cards][n::chips][n::bins][n::samples])
{
    for (int i = 0; i < n::cards; i++)
    {
        for (int j = 0; j < n::chips; j++)
        {
            for (int k = 0; k<n::bins; k++)
            {
                for (int l = 0; l < n::samples; l++)
                {
                    array[i][j][k][l]=0;
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    // Read arguments to determine input dir, output dir, input file
    string input_file(argv[1]);

    // Load DAQ conversion
    DAQ daq;
    daq.loadDAQ();
    cout << "...DAQ loaded successfully" << endl;
    // Create root output file structure
    TFile *output_file = new TFile(( loc::rootfiles + input_file + ".root" ).c_str(), "RECREATE");
    TTree *tree = new TTree("tree", "tree");

    Int_t eventNumber;
    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples]={};
    //Int_t transfer[n::cards][n::chips][n::bins][n::samples];

    tree->Branch("eventNumber", &eventNumber, "eventNumber/I");
    tree->Branch("ADCAmpl", &ADCAmpl, TString::Format("ADCAmpl[%i][%i][%i][%i]/I", n::cards, n::chips, n::bins, n::samples));

    Int_t prevevtnum = -99;

    // Read in txt file
    ifstream file((loc::txt + input_file + ".txt").c_str());
    cout << "File open : " << (loc::txt + input_file + ".txt").c_str() << endl;
    while(!file.eof())
    {
        int evtnum, card, chip, bin, sample, amp;
        file >> evtnum >> card >> chip >> bin >> sample >> amp ;
        if (sample==-1){continue;}
        if (evtnum==prevevtnum)
        {
            //transfer[card][chip][daq.DAQchannel(bin)][sample] = amp;
            cout << card << "   " << chip << "   " << bin << "    " << sample << "  " << amp << endl;
            ADCAmpl[card][chip][bin][sample] = amp;
        }
        else
        {
            eventNumber = evtnum;
            //ADCAmpl = transfer;
            tree->Fill();
            //Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples] = {};
            clear(ADCAmpl);
            //transfer[card][chip][daq.DAQchannel(bin)][sample] = amp;
            //ADCAmpl.clear();
            ADCAmpl[card][chip][bin][sample] = amp;

        }

        prevevtnum = evtnum;


    }

    tree->Write("", TObject::kOverwrite);
    tree->Print();
    file.close();
    output_file->Write();
    output_file->Close();

    return(0);

}
