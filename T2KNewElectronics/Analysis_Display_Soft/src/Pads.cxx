#include "T2KConstants.h"
#include "Pads.h"

void Pads::loadPadPlane(DAQ& daq, Mapping& map)
{
    for (int i = 0; i < geom::nPadx; i++)
    {
        for (int j=0; j<geom::nPady; j++)
        {
            pads[i][j] = Pixel(daq, map, i, j, 0);
        }
    }
}
