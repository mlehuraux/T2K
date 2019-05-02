#include "T2KConstants.h"
#include "Pixel.h"
#include "Mapping.h"

Pixel::Pixel(){}

Pixel::Pixel(Mapping& map, int i, int j)
{
    m_i = i;
    m_j = j;
    m_card = floor(m_i/geom::padOnchipx);
    m_chip = n::chips-1-floor(m_j/geom::padOnchipy);
    m_ichip = i%geom::padOnchipx;
    m_jchip = j%geom::padOnchipy;
    m_channel = map.channel(m_card, m_chip, m_ichip, m_jchip);
    m_x = (m_i+0.5)*geom::dx - 0.5*geom::nPadx*geom::dx;
    m_y = (m_j+0.5)*geom::dy - 0.5*geom::nPady*geom::dy;
    m_amp = 0.;
}

void Pixel::setPixel(int card, int chip, int channel, int ichip, int jchip)
{


    m_card = card;
    m_chip = chip;
    m_channel = channel;
    m_ichip = ichip;
    m_jchip = jchip;
    m_i = m_card*geom::padOnchipx + m_ichip;
    m_j = (n::chips-1-m_chip)*geom::padOnchipy + m_jchip;
    m_x = (m_i+0.5)*geom::dx - 0.5*geom::nPadx*geom::dx;
    m_y = (m_j+0.5)*geom::dy - 0.5*geom::nPady*geom::dy;
    m_amp = 0.;
}

// Getters
int Pixel::coordi()
{
    return(m_i);
}

int Pixel::coordj()
{
    return(m_j);
}

float Pixel::coordx()
{
    return(m_x);
}

float Pixel::coordy()
{
    return(m_y);
}
