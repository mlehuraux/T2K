// #include required here
#ifndef T2KConstants_h
#define T2KConstants_h

#include <cmath>
#include "math.h"
#include <string>

using namespace std;

// To access constant PI
#define _USE_MATH_DEFINES

// Numbers of #
namespace n
{

    static const int cards = 4;
    static const int chips = 4;
    static const int bins = 82; // +1 because connectors and arc starts at 1 not 0 so dim [81] to go until index 80 and useless 0
    static const int samples = 511;

}

// Geometry
namespace geom
{
    static const int nPadx = 36;
    static const int nPady = 32;
    static const float dx = 0.011; //[m]
    static const float dy = 0.010; //[m]
    static const int padOnchipx = geom::nPadx/n::cards ;
    static const int padOnchipy = geom::nPady/n::chips ;

    // To represent geometry on canvas
    static const int wx = 3600; // width of canvas in pix
    static const int wy = 3600; // height of canvas in pix
    static const float convx = 1.;//0.85*float(wx)/(nPadx*dx); // conversion factor
    static const float convy = convx; // conversion factor
}



// Files location
namespace loc
{
    static const string mapping = "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft/src/Mapping/";
    static const string daq = "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft/src/DAQ/";
    static const string outputs = "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft/outputs/";
    static const string rootfiles = "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/root/";
    static const string aqs = "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/root/";
    static const string txt = "/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/txt/";


}


#endif
