#include "T2KConstants.h"

class Pixel
{
    public :
        // Constructors
        Pixel();
        Pixel(int card, int chip, int channel, int ichip, int jchip);

        // Setters
        void setPixelConnect(int card, int chip, int channel);

        // Getters
        int coordi();
        int coordj();
        float coordx();
        float coordy();
        int iFromConnect();
        int jFromConnect();

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
