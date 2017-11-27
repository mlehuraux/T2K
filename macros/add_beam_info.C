int add_beam_info(int RUN) {

  for (Int_t i = 5148; i < 5162; ++i) {
    std::stringstream stream;
    stream << i;
    std::string strRUN = stream.str();

    std::string file_name = "data_root/RUN_0" + strRUN + "_fem0.root";
    TFile* file = new TFile(file_name.c_str(), "UPDATE");
    std::cout << file_name << std::endl;

    TTree* config_tree = new TTree("beam_config", "beam");
  
    Float_t driftZ = GetDriftZ(i);
    config_tree->Branch("DriftZ", &driftZ);
  
    config_tree->Fill();
    config_tree->Write();
    file->Close();
  }

  return 1;
}

Float_t GetDriftZ(Int_t RUN) {
  Float_t driftZ;

  switch (RUN) {
    case 5148:
      driftZ = 0; break;
    case 5149:
      driftZ = 3; break;
    case 5150:
      driftZ = 3; break;
    case 5151:
      driftZ = 5; break;
    case 5152:
      driftZ = 10; break;
    case 5153:
      driftZ = 15; break;
    case 5154:
      driftZ = 20; break;
    case 5155:
      driftZ = 25; break;
    case 5156:
      driftZ = 30; break;
    case 5157:
      driftZ = 35; break;
    case 5158:
      driftZ = 40; break;
    case 5159:
      driftZ = 45; break;
    case 5160:
      driftZ = 50; break;
    case 5161:
      driftZ = 52.9; break;
  }

  return driftZ;
}