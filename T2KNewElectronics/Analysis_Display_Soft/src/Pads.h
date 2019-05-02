#ifndef Pads_h
#define Pads_h

#include "T2KConstants.h"
#include <iostream>
#include "Pixel.h"
#include "Mapping.h"

using namespace std;

class Pads
{
    public :
        // Constructors
        void loadPadPlane(Mapping& map);

        //Getters
        Pixel& pad(int i, int j){return pads[i][j];}

        // Other

    private :
        Pixel pads[geom::nPadx][geom::nPady];
};

#endif
