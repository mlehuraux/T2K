int add_beam_info() {

  for (Int_t i = 5109; i < 5168; ++i) {
    std::stringstream stream;
    stream << i;
    std::string strRUN = stream.str();

    for (Int_t fem = 0; fem < 7; ++fem) {
      stream.str("");
      stream << fem;
      std::string strFEM = stream.str();

      std::string file_name = "data_root/RUN_0" + strRUN + "_fem" + strFEM + ".root";
      TFile* file = new TFile(file_name.c_str(), "UPDATE");
      std::cout << file_name << std::endl;

      TTree* config_tree = new TTree("beam_config", "beam");

      Float_t driftZ, peacking, Efield, energy;

      if (!GetRunInfo(i, driftZ, peacking, Efield, energy)) {
        std::cout << "ERROR with reading run info. Exit" << std::endl;
        return 1;
      }

      config_tree->Branch("DriftZ",   &driftZ);
      config_tree->Branch("peacking", &peacking);
      config_tree->Branch("Efield",   &Efield);

      config_tree->Fill();
      config_tree->Write("", TObject::kOverwrite);
      file->Close();
    }
  }

  return 1;
}

bool GetRunInfo(Int_t RUN, Float_t& driftZ, Float_t& peacking, Float_t& Efield, Float_t& energy) {
  driftZ = -1.;
  switch (RUN) {
    case 5109:
      driftZ = 50; peacking = 100.; Efield = 230; energy = 5; break;
    case 5110:
      driftZ = 52.9; peacking = 100.; Efield = 230; energy = 5; break;
    case 5111:
      driftZ = 52.9; peacking = 100.; Efield = 230; energy = 5; break;
    case 5112:
      driftZ = 45; peacking = 100.; Efield = 230; energy = 5; break;
    case 5113:
      driftZ = 40; peacking = 100.; Efield = 230; energy = 5; break;
    case 5114:
      driftZ = 35; peacking = 100.; Efield = 230; energy = 5; break;
    case 5115:
      driftZ = 30; peacking = 100.; Efield = 230; energy = 5; break;
    case 5116:
      driftZ = 25; peacking = 100.; Efield = 230; energy = 5; break;
    case 5117:
      driftZ = 20; peacking = 100.; Efield = 230; energy = 5; break;
    case 5118:
      driftZ = 15; peacking = 100.; Efield = 230; energy = 5; break;
    case 5119:
      driftZ = 10; peacking = 100.; Efield = 230; energy = 5; break;
    case 5120:
      driftZ = 5; peacking = 100.; Efield = 230; energy = 5; break;
    case 5121:
      driftZ = 3; peacking = 100.; Efield = 230; energy = 5; break;
    case 5122:
      driftZ = 0; peacking = 100.; Efield = 230; energy = 5; break;

    case 5123:
      driftZ = 3; peacking = 100.; Efield = 140; energy = 5; break;
    case 5124:
      driftZ = 5; peacking = 100.; Efield = 140; energy = 5; break;
    case 5125:
      driftZ = 10; peacking = 100.; Efield = 140; energy = 5; break;
    case 5126:
      driftZ = 15; peacking = 100.; Efield = 140; energy = 5; break;
    case 5127:
      driftZ = 20; peacking = 100.; Efield = 140; energy = 5; break;
    case 5128:
      driftZ = 25; peacking = 100.; Efield = 140; energy = 5; break;
    case 5129:
      driftZ = 30; peacking = 100.; Efield = 140; energy = 5; break;
    case 5130:
      driftZ = 35; peacking = 100.; Efield = 140; energy = 5; break;
    case 5131:
      driftZ = 40; peacking = 100.; Efield = 140; energy = 5; break;
    case 5132:
      driftZ = 45; peacking = 100.; Efield = 140; energy = 5; break;
    case 5133:
      driftZ = 50; peacking = 100.; Efield = 140; energy = 5; break;
    case 5134:
      driftZ = 52.9; peacking = 100.; Efield = 140; energy = 5; break;


    case 5135:
      driftZ = 52.9; peacking = 200.; Efield = 140; energy = 5; break;
    case 5136:
      driftZ = 50; peacking = 200.; Efield = 140; energy = 5; break;
    case 5137:
      driftZ = 52.9; peacking = 200.; Efield = 140; energy = 5; break;
    case 5138:
      driftZ = 45; peacking = 200.; Efield = 140; energy = 5; break;
    case 5139:
      driftZ = 40; peacking = 200.; Efield = 140; energy = 5; break;
    case 5140:
      driftZ = 35; peacking = 200.; Efield = 140; energy = 5; break;
    case 5141:
      driftZ = 30; peacking = 200.; Efield = 140; energy = 5; break;
    case 5142:
      driftZ = 25; peacking = 200.; Efield = 140; energy = 5; break;
    case 5143:
      driftZ = 20; peacking = 200.; Efield = 140; energy = 5; break;
    case 5144:
      driftZ = 15; peacking = 200.; Efield = 140; energy = 5; break;
    case 5145:
      driftZ = 10; peacking = 200.; Efield = 140; energy = 5; break;
    case 5146:
      driftZ = 5; peacking = 200.; Efield = 140; energy = 5; break;
    case 5147:
      driftZ = 3; peacking = 200.; Efield = 140; energy = 5; break;


    case 5148:
      driftZ = 0; peacking = 200.; Efield = 230; energy = 5; break;
    case 5149:
      driftZ = 3; peacking = 200.; Efield = 230; energy = 5; break;
    case 5150:
      driftZ = 3; peacking = 200.; Efield = 230; energy = 5; break;
    case 5151:
      driftZ = 5; peacking = 200.; Efield = 230; energy = 5; break;
    case 5152:
      driftZ = 10; peacking = 200.; Efield = 230; energy = 5; break;
    case 5153:
      driftZ = 15; peacking = 200.; Efield = 230; energy = 5; break;
    case 5154:
      driftZ = 20; peacking = 200.; Efield = 230; energy = 5; break;
    case 5155:
      driftZ = 25; peacking = 200.; Efield = 230; energy = 5; break;
    case 5156:
      driftZ = 30; peacking = 200.; Efield = 230; energy = 5; break;
    case 5157:
      driftZ = 35; peacking = 200.; Efield = 230; energy = 5; break;
    case 5158:
      driftZ = 40; peacking = 200.; Efield = 230; energy = 5; break;
    case 5159:
      driftZ = 45; peacking = 200.; Efield = 230; energy = 5; break;
    case 5160:
      driftZ = 50; peacking = 200.; Efield = 230; energy = 5; break;
    case 5161:
      driftZ = 52.9; peacking = 200.; Efield = 230; energy = 5; break;

    case 5162:
      driftZ = 5; peacking = 200.; Efield = 140; energy = 5; break;
    case 5163:
      driftZ = 5; peacking = 200.; Efield = 140; energy = 4; break;
    case 5164:
      driftZ = 5; peacking = 200.; Efield = 140; energy = 3; break;
    case 5165:
      driftZ = 5; peacking = 200.; Efield = 140; energy = 2; break;
    case 5166:
      driftZ = 5; peacking = 200.; Efield = 140; energy = 1; break;
    case 5167:
      driftZ = 5; peacking = 200.; Efield = 140; energy = 1; break;

  }


  if (driftZ != -1)
    return true;

  return false;
}