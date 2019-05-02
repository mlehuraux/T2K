#include "T2KConstants.h"
#include "Pads.h"

void Pads::loadPadPlane(Mapping& map)
{
    for (int i = 0; i < geom::nPadx; i++)
    {
        for (int j=0; j<geom::nPady; j++)
        {
            pads[i][j] = Pixel(map, i, j);
        }
    }
}
