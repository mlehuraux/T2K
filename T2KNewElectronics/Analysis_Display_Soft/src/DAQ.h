#ifndef DAQ_h
#define DAQ_h

#include "T2KConstants.h"

class DAQ
{
    public :
        // Constructors
        void loadDAQ();

        // Getters
        int DAQchannel(int detector){return(arc2daq[detector2arc[detector]]);}

        // Other

    private :
        int detector2arc[n::bins];
        int arc2daq[n::bins];
};

#endif
