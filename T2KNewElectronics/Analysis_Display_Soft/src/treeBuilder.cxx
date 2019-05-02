// ROOT writing in the tree part
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft/src/T2KConstants.h"
#include "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft/src/DAQ.h"

// Global part
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

void treeBuilder(string input_dir, string output_dir, string input_file)
{
    // Load DAQ conversion
    DAQ daq;
    daq.loadDAQ();

    // Create root output file structure
    TFile *output_file = new TFile(( output_dir + input_file + ".root" ).c_str(), "RECREATE");
    TTree *tree = new TTree("tree", "tree");

    Int_t nevent=-1;
    Int_t eventNumber, event;
    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples];


    tree->Branch("event", &event, "event/I");
    tree->Branch("eventNumber", &eventNumber, "eventNumber/I");
    tree->Branch("ADCAmpl", &ADCAmpl, TString::Format("ADCAmpl[%i][%i][%i][%i]/I", n::cards, n::chips, n::bins, n::samples));

    Int_t prevevtnum = -99;

    // Read in txt file
    ifstream file((input_dir + input_file + ".txt").c_str());
    cout << "File open : " << (input_dir + input_file + ".txt").c_str() << endl;
    while(!file.eof())
    {
        int evtnum, card, chip, bin, sample, amp;
        file >> evtnum >> card >> chip >> bin >> sample >> amp >> ws;
        if (evtnum != prevevtnum) // new event
        {
            eventNumber = evtnum;
            nevent++;

            // Initialize event to 0
            for (int i = 0; i < n::cards; i++)
            {
                for (int j = 0; j < n::chips; j++)
                {
                    for (int k = 0; k < n::bins; k++)
                    {
                        for (int l = 0; l < n::samples; l++)
                        {
                            ADCAmpl[i][j][daq.DAQchannel(k)][l]=0;
                        }
                    }
                }
            }


        }
        event = nevent;
        prevevtnum = evtnum;
        ADCAmpl[card][chip][daq.DAQchannel(bin)][sample]=amp;

        tree->Fill();
    }

    tree->Write();
    tree->Print();
    file.close();
    output_file->Write();
    output_file->Close();

}
