#include "Mapping.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char **argv)
{
    Mapping T2K;
    T2K.loadMapping();
    cout << "Mapping loaded succesfully." << endl;
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

    return 0;
}
