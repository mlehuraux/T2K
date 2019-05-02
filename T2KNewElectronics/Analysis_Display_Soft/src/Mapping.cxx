#include "T2KConstants.h"
#include "Mapping.h"

#include <fstream>
#include <string>
#include <iostream>

using namespace std;

void Mapping::loadMapping()
{
    // Load in the correspondance betwwen pad coordinates and pin connectors
            /*
                    --> Cards
                ---  ---  ---  ---
    C          | A || C || A || C |
    h          | A || C || A || C |
    i           ---  ---  ---  ---
    p          | B || D || B || D |
    s          | B || D || B || D |
                ---  ---  ---  ---



        */

    int i, j, channel;

    ifstream A((loc::mapping + "ChipA.txt").c_str());
    while (!A.eof())
    {
        A >> j >> i >> channel;
        //int index = channel - 3;
        //if (channel >=43) index -= 6;
        int index = channel;
        m_ichip[0][0][index] = i ;
        m_jchip[0][0][index] = j ;
        m_ichip[0][1][index] = i ;
        m_jchip[0][1][index] = j ;
        m_ichip[2][0][index] = i ;
        m_jchip[2][0][index] = j ;
        m_ichip[2][1][index] = i ;
        m_jchip[2][1][index] = j ;
        // Coord to channel
        m_channel[0][0][i][j] = channel ;
        m_channel[0][1][i][j] = channel ;
        m_channel[2][0][i][j] = channel ;
        m_channel[2][1][i][j] = channel ;


    }
    A.close();

    ifstream B((loc::mapping + "ChipB.txt").c_str());
    while(!B.eof())
    {
        B >> i >> j >> channel ;
        //int index = channel - 3;
        //if (channel >=43) index -= 6;
        int index = channel;
        m_ichip[0][2][index] = i ;
        m_jchip[0][2][index] = j ;
        m_ichip[0][3][index] = i ;
        m_jchip[0][3][index] = j ;
        m_ichip[2][2][index] = i ;
        m_jchip[2][2][index] = j ;
        m_ichip[2][3][index] = i ;
        m_jchip[2][3][index] = j ;
        // Coord to channel
        m_channel[0][2][i][j] = channel ;
        m_channel[0][3][i][j] = channel ;
        m_channel[2][2][i][j] = channel ;
        m_channel[2][3][i][j] = channel ;
    }
    B.close();

    ifstream C((loc::mapping + "ChipC.txt").c_str());
    while(!C.eof())
    {
        C >> i >> j >> channel ;
        //int index = channel - 3;
        //if (channel >=43) index -= 6;
        int index = channel;
        m_ichip[1][0][index] = i ;
        m_jchip[1][0][index] = j ;
        m_ichip[1][1][index] = i ;
        m_jchip[1][1][index] = j ;
        m_ichip[3][0][index] = i ;
        m_jchip[3][0][index] = j ;
        m_ichip[3][1][index] = i ;
        m_jchip[3][1][index] = j ;
        // Coord to channel
        m_channel[1][0][i][j] = channel ;
        m_channel[1][1][i][j] = channel ;
        m_channel[2][0][i][j] = channel ;
        m_channel[2][1][i][j] = channel ;
    }
    C.close();

    ifstream D((loc::mapping + "ChipD.txt").c_str());
    while(!D.eof())
    {
        D >> i >> j >> channel ;
        //int index = channel - 3;
        //if (channel >=43) index -= 6;
        int index = channel;
        m_ichip[1][2][index] = i ;
        m_jchip[1][2][index] = j ;
        m_ichip[1][3][index] = i ;
        m_jchip[1][3][index] = j ;
        m_ichip[3][2][index] = i ;
        m_jchip[3][2][index] = j ;
        m_ichip[3][3][index] = i ;
        m_jchip[3][3][index] = j ;
        // Coord to channel
        m_channel[1][2][i][j] = channel ;
        m_channel[1][3][i][j] = channel ;
        m_channel[3][2][i][j] = channel ;
        m_channel[3][3][i][j] = channel ;

    }
    D.close();


}
