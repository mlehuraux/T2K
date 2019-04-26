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
    static const int bins = 72; // To check, that's for connectors, you might need the DAQ channel instead
    static const int samples = 511;

}

// Geometry
namespace geom
{
    static const int nPadx = 36;
    static const int nPady = 32;
    static const double dx = 0.011; //[m]
    static const double dy = 0.010; //[m]
}


#endif
