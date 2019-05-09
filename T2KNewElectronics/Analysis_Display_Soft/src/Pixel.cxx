#include "T2KConstants.h"
#include "Pixel.h"
#include "Mapping.h"
#include "DAQ.h"

Pixel::Pixel(){}

Pixel::Pixel(DAQ& daq, Mapping& map, int i, int j, int amp)
{
    m_i = i;
    m_j = j;
    m_card = floor(m_i/geom::padOnchipx);
    if (m_card%2==0){m_chip = n::chips-1-floor(m_j/geom::padOnchipy);}
    else if (m_card%2==1){m_chip = floor(m_j/geom::padOnchipy);}
    m_ichip = i%geom::padOnchipx;
    m_jchip = j%geom::padOnchipy;
    m_channel = daq.DAQchannel(map.connector(m_card, m_chip, m_ichip, m_jchip));
    m_connector = map.connector(m_card, m_chip, m_ichip, m_jchip);
    m_x = (m_i+0.5)*geom::dx - 0.5*geom::nPadx*geom::dx;
    m_y = (m_j+0.5)*geom::dy - 0.5*geom::nPady*geom::dy;
    m_amp = amp;
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
    m_amp = 0;
}

// Setters

void Pixel::setAmp(int amp)
{
    m_amp = amp;
}

// Getters
int Pixel::card()
{
    return(m_card);
}

int Pixel::chip()
{
    return(m_chip);
}

int Pixel::channel()
{
    return(m_channel);
}
int Pixel::connector()
{
    return(m_connector);
}

int Pixel::ampl()
{
    return(m_amp);
}

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
