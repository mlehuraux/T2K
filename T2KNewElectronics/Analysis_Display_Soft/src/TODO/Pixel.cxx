#include "T2KConstants.h"
#include "Pixel.h"

Pixel::Pixel(){}

void Pixel::Pixel(int card, int chip, int channel, int ichip, int jchip)
{
    m_card = card;
    m_chip = chip;
    m_channel = channel;
    m_ichip = ichip;
    m_jchip = jchip;
    m_i = m_card*(geom::nPadx/n::cards) + m_ichip;
    m_j = (n::chips-1-m_chip)*(geom::nPady/n::cards) + m_jchip;
    m_x = (m_i+0.5)*geom::dx;
    m_y = (m_j+0.5)*geom::dy;
    m_amp = 0.;
}

void Pixel::setPixelConnect(int card, int chip, int channel)
{
    m_card = card;
    m_chip = chip;
    m_channel = channel;

}

// Getters
int Pixel::coordi()
{
    return(m_i);
}

int Pixel::coordj()
{
    retunr(m_j);
}

float Pixel::coordx()
{
    return(m_x);
}

float Pixel::coordy()
{
    return(m_y);
}
