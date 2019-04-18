#include "T2KConstants.h"

class Mapping {
  pair<int, int> pix[n::arc][n::chip][n::bins];
  public:
  // Setters
  void loadMapping();
  // Getters
  int padi(int arc, int chip, int bin);
  int padj(int arc, int chip, int bin);
  int padx(int arc, int chip, int bin);
  int pady(int arc, int chip, int bin);
  void refresh();
};

void Mapping::loadMapping() {
	for ( int i = 0; i < 3; ++i){ //asic
   	  for ( int j = 0; j < 64; ++j){ // pin
		for ( int k = 0; k < 9; ++k){ // pixel
		  status[j][i][k]=0;
		  colors[j][i][k]=16;
		  alphas[j][i][k]=1;
		  pix[j][i][k] = make_pair(99,99);
		}
	  }
	}
  std::ifstream file ("MapChanLayout.txt");
  while(!file.eof()){
    int i, j, pin, asic, count;
    file >> asic >> pin >> count >> i >> j;
    pix[pin][asic][count]=make_pair(i,j);
  }
}

void Mapping::refresh() {
  for ( int i = 0; i < 3; ++i){ //asic
		for ( int j = 0; j < 64; ++j){ // pin
      for ( int k = 0; k < 9; ++k){ // pixel
        status[j][i][k]=0;
        colors[j][i][k]=16;
        alphas[j][i][k]=1;
      }
    }
	}
}
