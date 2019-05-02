#ifndef Pixel_h
#define Pixel_h

#include "T2KConstants.h"
#include "Mapping.h"
#include <iostream>

using namespace std;

class Pixel
{
    public :
        // Constructors
        Pixel();
        Pixel(Mapping& map, int i, int j);

        // Setters different
        void setPixel(int card, int chip, int channel, int ichip, int jchip);

        // Getters
        int coordi();
        int coordj();
        float coordx();
        float coordy();

        // Display cell address content.
	    friend ostream& operator<<(ostream& os, const Pixel& P)
	    {
            os << "Pixel" << endl;
            os << "{" << endl;
            printf("\tConnections : #Card : %i   #Chip : %i   #Channel : %i\n", P.m_card, P.m_chip, P.m_channel);
            os << "\tCoordinates : " << endl;
            printf("\t\tOn chip : (%i, %i)\n", P.m_ichip, P.m_jchip);
            printf("\t\tOn board (%i, %i)\n", P.m_i, P.m_j);
            printf("\t\tOn board in mm (%.1f, %.1f)\n", P.m_x*1000, P.m_y*1000);
            os << "}" << endl;
		    return os;
        }

        // Other

    private :
        int m_card;
        int m_chip;
        int m_channel;
        int m_i;
        int m_j;
        int m_ichip;
        int m_jchip;
        float m_x;
        float m_y;
        float m_amp;
};

#endif
