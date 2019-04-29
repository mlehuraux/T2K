#ifndef Mapping_h
#define Mapping_h

#include "T2KConstants.h"

class Mapping
{
    public :
        // Constructors
        void loadMapping();

        // Getters
        int ichip(int card, int chip, int bin){return m_ichip[card][chip][bin];}
        int jchip(int card, int chip, int bin){return m_jchip[card][chip][bin];}

        // Other

    private :
        int m_ichip[n::cards][n::chips][n::bins];
        int m_jchip[n::cards][n::chips][n::bins];
};

#endif
