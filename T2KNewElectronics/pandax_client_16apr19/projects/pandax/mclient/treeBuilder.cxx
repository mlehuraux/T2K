// ROOT writing in the tree part
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/T2KConstants.h"

// Global part
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    // Create root output file structure
    TFile *output_file = new TFile(( input_dir + file_name + ext ).c_str(), "RECREATE");
    TTree *tree = new TTree();

    Int_t eventNumber;
    Int_t ADCAmpl[n::cards][n::chips][n::bins][n::samples];

    tree->Branch("eventNumber", &eventNumber, "eventNumber/I");
    tree->Branch("ADCAmpl", &ADCAmpl, TString::Format("ADCAmpl/I[%i][%i][%i][%i]", n::cards, n::chips, n::bins, n::samples));

    // Read in txt file_
    


    return 0;
}
