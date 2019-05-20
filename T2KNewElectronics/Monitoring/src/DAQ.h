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
        int connector(int daqchannel){return(arc2detector[daq2arc[daqchannel]]);}

        // Other

    private :
        int detector2arc[n::bins];
        int arc2daq[n::bins];
        int daq2arc[n::bins];
        int arc2detector[n::bins];
};

#endif
