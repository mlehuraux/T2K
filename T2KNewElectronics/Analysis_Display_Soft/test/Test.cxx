#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "TCanvas.h"
#include "TPad.h"
#include "TF1.h"
#include "TBox.h"
#include "TPolyLine.h"
#include "TColor.h"
#include "TFrame.h"
#include "TAttLine.h"
#include "TStyle.h"

#include "../src/Pixel.h"
#include "../src/Mapping.h"
#include "../src/Pads.h"
#include "../src/DAQ.h"

using namespace std;

TPolyLine *padline(Pixel& P, int color=602)
{
    Float_t x[4] = {geom::convx*(P.coordx()-0.5*geom::dx), geom::convx*(P.coordx()-0.5*geom::dx), geom::convx*(P.coordx()+0.5*geom::dx), geom::convx*(P.coordx()+0.5*geom::dx)};
    Float_t y[4] = {geom::convy*(P.coordy()-0.5*geom::dy), geom::convy*(P.coordy()+0.5*geom::dy), geom::convy*(P.coordy()+0.5*geom::dy), geom::convy*(P.coordy()-0.5*geom::dy)};
    TPolyLine *pline = new TPolyLine(4,x,y);
    pline->SetFillColor(color);
    pline->SetLineColor(kGray);
    pline->SetLineWidth(2);
    return(pline);
}

int main(int argc, char **argv)
{
    Mapping T2K;
    T2K.loadMapping();
    cout << "...Mapping loaded succesfully." << endl;

    DAQ daq;
    daq.loadDAQ();
    cout << "... DAQ loaded successfully" << endl;

    Pads padPlane;
    padPlane.loadPadPlane(daq, T2K);
    cout << "...Pad plane loaded succesfully." << endl;

    cout << "#cards : " << n::cards << endl;
    cout << "#chips : " << n::chips << endl;
    cout << "#bins : " << n::bins << endl;
    cout << "#TimeSamples : " << n::samples << endl;

    cout << "*************** Card 0 Chip 0 ***************" << endl;
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 0, 3, T2K.ichip(0,0,3), T2K.jchip(0,0,3));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 0, 38, T2K.ichip(0,0,38), T2K.jchip(0,0,38));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 0, 43, T2K.ichip(0,0,43), T2K.jchip(0,0,43));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 0, 78, T2K.ichip(0,0,78), T2K.jchip(0,0,78));
    cout << "*************** Card 0 Chip 1 ***************" << endl;
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 1, 3, T2K.ichip(0,1,3), T2K.jchip(0,1,3));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 1, 38, T2K.ichip(0,1,38), T2K.jchip(0,1,38));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 1, 43, T2K.ichip(0,1,43), T2K.jchip(0,1,43));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 1, 78, T2K.ichip(0,1,78), T2K.jchip(0,1,78));
    cout << "*************** Card 0 Chip 2 ***************" << endl;
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 2, 3, T2K.ichip(0,2,3), T2K.jchip(0,2,3));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 2, 38, T2K.ichip(0,2,38), T2K.jchip(0,2,38));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 2, 43, T2K.ichip(0,2,43), T2K.jchip(0,2,43));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 2, 78, T2K.ichip(0,2,78), T2K.jchip(0,2,78));
    cout << "*************** Card 0 Chip 3 ***************" << endl;
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 3, 3, T2K.ichip(0,3,3), T2K.jchip(0,3,3));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 3, 38, T2K.ichip(0,3,38), T2K.jchip(0,3,38));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 3, 43, T2K.ichip(0,3,43), T2K.jchip(0,3,43));
    printf("Card : %i   Chip : %i   Channel : %i    -->    (i,j) = (%i,%i)\n", 0, 3, 78, T2K.ichip(0,3,78), T2K.jchip(0,3,78));

    cout << "*************** DAQ checks ***************" << endl;
    for (int i=0; i<n::bins; i++)
    {
        cout << "#connector : " << i << "   #DAQ channel : " << daq.DAQchannel(i) << "  back to connector : " << daq.connector(daq.DAQchannel(i)) << endl;
    }

    return 0;
}
