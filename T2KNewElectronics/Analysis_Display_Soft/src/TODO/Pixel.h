#ifndef Pixel_h
#define Pixel_h

#include "T2KConstants.h"
#include <iostream>

using namespace std;

class Pixel
{
    public :
        // Constructors
        Pixel();
        void Pixel(int card, int chip, int channel, int ichip, int jchip);

        // Setters
        void setPixelConnect(int card, int chip, int channel);

        // Getters
        int coordi();
        int coordj();
        float coordx();
        float coordy();

        /*
        // Display cell address content.
	    friend ostream& operator<<(ostream& os, const Pixel& P)
	    {
            os << "*** Pixel ***" << endl;
            printf("Connections : #Card : %i   #Chip : %i   #Channel : %i", P.m_card, P.m_chip, P.m_channel);
            os << "Coordinates : " << endl;
            printf("\tOn chip : (%i, %i)", P.m_ichip, P.m_jchip);
            printf("\tOn board (%i, %i)", P.m_i, P.m_j);
            printf("\tOn board in mm (%f, %f)", P.m_x*1000, P.m_y*1000);
		    return os;
        }
        */

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
