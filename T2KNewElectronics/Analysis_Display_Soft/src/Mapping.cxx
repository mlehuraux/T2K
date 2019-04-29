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

    ifstream A("Mapping/ChipA.txt");
    cout << "File A open" << endl;
    int count = 0;
    while(count <= 72)
    {
        int i, j, channel;
        A >> i >> j >> channel;
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
        cout << count << endl;
        cout << m_ichip[0][0][index] << endl;

        count += 1;
    }
    A.close();
/*
    ifstream B("Mapping/ChipB.txt");
    cout << "File B open" << endl;
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
    }
    B.close();

    ifstream C("Mapping/ChipC.txt");
    cout << "File C open" << endl;
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
    }
    C.close();

    ifstream D("Mapping/ChipD.txt");
    cout << "File D open" << endl;
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
    }
    D.close();

    */

}
