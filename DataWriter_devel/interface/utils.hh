#ifndef utils__hh_
#define utils__hh_

#include <string>
class TChain;

void rootStyle(void);
TChain* addInputFiles( std::string filelist, std::string treename );
std::string firstFileInList( std::string filelist );

#endif

