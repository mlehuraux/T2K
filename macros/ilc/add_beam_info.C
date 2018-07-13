int add_beam_info() {

  for (Int_t i = 4098; i <= 4109; ++i) {
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

      Float_t driftZ, peacking, Efield, energy, x;

      if (!GetRunInfo(i, driftZ, peacking, Efield, energy, x)) {
        std::cout << "ERROR with reading run info. Exit" << std::endl;
        return 1;
      }

      config_tree->Branch("DriftZ",   &driftZ);
      config_tree->Branch("peacking", &peacking);
      config_tree->Branch("Efield",   &Efield);
      config_tree->Branch("energy",   &energy);
      config_tree->Branch("x",        &x);

      config_tree->Fill();
      config_tree->Write("", TObject::kOverwrite);
      file->Close();
    }
  }

  return 1;
}

bool GetRunInfo(Int_t RUN, Float_t& driftZ, Float_t& peacking, Float_t& Efield, Float_t& energy, Float_t& x) {
  driftZ = -1.;

  switch (RUN) {

    case 4028:
      driftZ =  8.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4029:
      driftZ = 18.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4030:
      driftZ = 23.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4031:
      driftZ =  8.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4032:
      driftZ = 33.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4033:
      driftZ = 38.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4034:
      driftZ = 43.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4035:
      driftZ = 48.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4036:
      driftZ = 53.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4037:
      driftZ = 58.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4038:
      driftZ = 13.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4039:
      driftZ = 10.68;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4040:
      driftZ =  5.68;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4041:
      driftZ =  3.18;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;
    case 4042:
      driftZ =  0.68;     peacking = 100.; Efield = 230; energy = 5; x = 23.; break;

      case 4043:
      driftZ =  0.68;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4044:
      driftZ =  3.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4045:
      driftZ =  5.68;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4046:
      driftZ =  8.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4047:
      driftZ = 10.68;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4048:
      driftZ = 13.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4049:
      driftZ = 18.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4050:
      driftZ = 23.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4052:
      driftZ = 28.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4053:
      driftZ = 33.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4054:
      driftZ = 38.68;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4055:
      driftZ = 43.68;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4056:
      driftZ = 48.18;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4057:
      driftZ = 53.68;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4058:
      driftZ = 55.68;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;

    case 4059:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 23.; break;
    case 4060:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 92.; break;
    case 4061:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 92.; break;
    case 4062:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 162.; break;
    case 4063:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 232.; break;
    case 4064:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 232.; break;
    case 4065:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 232.; break;
    case 4066:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 214.; break;
    case 4067:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 162.; break;
    case 4068:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 162.; break;
    case 4069:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 147.; break;
    case 4070:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 147.; break;
    case 4071:
      driftZ = 20.;     peacking = 100.; Efield = 140; energy = 5; x = 147.; break;

    case 4098:
      driftZ =  57;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4099:
      driftZ =  55;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4100:
      driftZ =  50;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4101:
      driftZ =  45;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4102:
      driftZ =  40;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4103:
      driftZ =  35;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4104:
      driftZ =  30;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4105:
      driftZ =  25;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4106:
      driftZ =  20;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4107:
      driftZ =  15;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4108:
      driftZ =  10;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;
    case 4109:
      driftZ =  5;     peacking = 100.; Efield = 230; energy = 5; x = 147.; break;

    case 5073:
      driftZ = 5.;    peacking = 100.; Efield = 230; energy = 5; x = 30.; break;
    case 5074:
      driftZ = 5.;    peacking = 100.; Efield = 230; energy = 5; x = 40.; break;
    case 5075:
      driftZ = 5.;    peacking = 100.; Efield = 230; energy = 5; x = 20.; break;
    case 5076:
      driftZ = 5.;     peacking = 100.; Efield = 230; energy = 5; x = 10.; break;
    case 5077:
      driftZ = 5.;     peacking = 100.; Efield = 230; energy = 5; x = 0.; break;
    case 5078:
      driftZ = 5.;     peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5079:
      driftZ = 5.;     peacking = 100.; Efield = 230; energy = 5; x = -20.; break;
    case 5080:
      driftZ = 5.;     peacking = 100.; Efield = 230; energy = 5; x = -30.; break;
    case 5081:
      driftZ = 5.;     peacking = 100.; Efield = 230; energy = 5; x = -40.; break;
    case 5082:
      driftZ = 5.;     peacking = 100.; Efield = 230; energy = 5; x = -40.; break;

    case 5083:
      driftZ = 5.;    peacking = 100.; Efield = 230; energy = 5; x = -40.; break;
    case 5084:
      driftZ = 5.;    peacking = 200.; Efield = 230; energy = 5; x = -40.; break;
    case 5085:
      driftZ = 5.;    peacking = 400.; Efield = 230; energy = 5; x = -40.; break;
    case 5086:
      driftZ = 5.;    peacking = 500.; Efield = 230; energy = 5; x = -40.; break;
    case 5087:
      driftZ = 5.;    peacking = 600.; Efield = 230; energy = 5; x = -40.; break;
    case 5088:
      driftZ = 5.;    peacking = 700.; Efield = 230; energy = 5; x = -40.; break;
    case 5089:
      driftZ = 5.;    peacking = 900.; Efield = 230; energy = 5; x = -40.; break;
    case 5090:
      driftZ = 5.;    peacking = 1000.; Efield = 230; energy = 5; x = -40.; break;

    case 5091:
      driftZ = 20.;   peacking = 100.; Efield = 230; energy = 5; x = -40.; break;
    case 5092:
      driftZ = 20.;   peacking = 200.; Efield = 230; energy = 5; x = -40.; break;
    case 5093:
      driftZ = 20.;   peacking = 400.; Efield = 230; energy = 5; x = -40.; break;
    case 5094:
      driftZ = 20.;   peacking = 500.; Efield = 230; energy = 5; x = -40.; break;
    case 5095:
      driftZ = 20.;   peacking = 600.; Efield = 230; energy = 5; x = -40.; break;
    case 5096:
      driftZ = 20.;   peacking = 700.; Efield = 230; energy = 5; x = -40.; break;
    case 5097:
      driftZ = 20.;   peacking = 900.; Efield = 230; energy = 5; x = -40.; break;
    case 5098:
      driftZ = 20.;   peacking = 1000.; Efield = 230; energy = 5; x = -40.; break;

    case 5099:
      driftZ = 50.;   peacking = 100.; Efield = 230; energy = 5; x = -40.; break;
    case 5100:
      driftZ = 50.;   peacking = 200.; Efield = 230; energy = 5; x = -40.; break;
    case 5101:
      driftZ = 50.;   peacking = 400.; Efield = 230; energy = 5; x = -40.; break;
    case 5102:
      driftZ = 50.;   peacking = 500.; Efield = 230; energy = 5; x = -40.; break;
    case 5103:
      driftZ = 50.;   peacking = 600.; Efield = 230; energy = 5; x = -40.; break;
    case 5104:
      driftZ = 50.;   peacking = 700.; Efield = 230; energy = 5; x = -40.; break;
    case 5105:
      driftZ = 50.;   peacking = 900.; Efield = 230; energy = 5; x = -40.; break;
    case 5106:
      driftZ = 50.;   peacking = 1000.; Efield = 230; energy = 5; x = -40.; break;

    case 5109:
      driftZ = 50;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5110:
      driftZ = 52.9;  peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5111:
      driftZ = 52.9;  peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5112:
      driftZ = 45;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5113:
      driftZ = 40;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5114:
      driftZ = 35;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5115:
      driftZ = 30;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5116:
      driftZ = 25;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5117:
      driftZ = 20;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5118:
      driftZ = 15;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5119:
      driftZ = 10;    peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5120:
      driftZ = 5;     peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5121:
      driftZ = 3;     peacking = 100.; Efield = 230; energy = 5; x = -10.; break;
    case 5122:
      driftZ = 0;     peacking = 100.; Efield = 230; energy = 5; x = -10.; break;

    case 5123:
      driftZ = 3;     peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5124:
      driftZ = 5;     peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5125:
      driftZ = 10;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5126:
      driftZ = 15;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5127:
      driftZ = 20;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5128:
      driftZ = 25;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5129:
      driftZ = 30;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5130:
      driftZ = 35;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5131:
      driftZ = 40;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5132:
      driftZ = 45;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5133:
      driftZ = 50;    peacking = 100.; Efield = 140; energy = 5; x = -10.; break;
    case 5134:
      driftZ = 52.9;  peacking = 100.; Efield = 140; energy = 5; x = -10.; break;


    case 5135:
      driftZ = 52.9;  peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5136:
      driftZ = 50;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5137:
      driftZ = 52.9;  peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5138:
      driftZ = 45;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5139:
      driftZ = 40;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5140:
      driftZ = 35;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5141:
      driftZ = 30;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5142:
      driftZ = 25;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5143:
      driftZ = 20;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5144:
      driftZ = 15;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5145:
      driftZ = 10;    peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5146:
      driftZ = 5;     peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5147:
      driftZ = 3;     peacking = 200.; Efield = 140; energy = 5; x = -10.; break;


    case 5148:
      driftZ = 0;     peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5149:
      driftZ = 3;     peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5150:
      driftZ = 3;     peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5151:
      driftZ = 5;     peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5152:
      driftZ = 10;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5153:
      driftZ = 15;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5154:
      driftZ = 20;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5155:
      driftZ = 25;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5156:
      driftZ = 30;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5157:
      driftZ = 35;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5158:
      driftZ = 40;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5159:
      driftZ = 45;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5160:
      driftZ = 50;    peacking = 200.; Efield = 230; energy = 5; x = -10.; break;
    case 5161:
      driftZ = 52.9;  peacking = 200.; Efield = 230; energy = 5; x = -10.; break;

    case 5162:
      driftZ = 5;     peacking = 200.; Efield = 140; energy = 5; x = -10.; break;
    case 5163:
      driftZ = 5;     peacking = 200.; Efield = 140; energy = 4; x = -10.; break;
    case 5164:
      driftZ = 5;     peacking = 200.; Efield = 140; energy = 3; x = -10.; break;
    case 5165:
      driftZ = 5;     peacking = 200.; Efield = 140; energy = 2; x = -10.; break;
    case 5166:
      driftZ = 5;     peacking = 200.; Efield = 140; energy = 1; x = -10.; break;
    case 5167:
      driftZ = 5;     peacking = 200.; Efield = 140; energy = 1; x = -10.; break;

  }


  if (driftZ != -1)
    return true;

  return false;
}