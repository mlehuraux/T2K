#include "T2KConstants.h"
#include "DAQ.h"

#include <fstream>
#include <string>
#include <iostream>

using namespace std;

void DAQ::loadDAQ()
{
    int det, arc;

    ifstream det2arc0((loc::daq + "detector2arc.txt").c_str());
    while (!det2arc0.eof())
    {
        det2arc0 >> det >> arc;
        detector2arc[det]=arc;
        arc2detector[arc]=det;
        //detector2arc[2][det]=arc;
        //arc2detector[2][arc]=det;
    }
    det2arc0.close();
/*
    ifstream det2arc1((loc::daq + "detector2arc1.txt").c_str());
    while (!det2arc1.eof())
    {
        det2arc1 >> det >> arc;
        detector2arc[1][det]=arc;
        arc2detector[1][arc]=det;
        detector2arc[3][det]=arc;
        arc2detector[3][arc]=det;
    }
    det2arc1.close();
*/

    int daq;
    ifstream farc2daq((loc::daq + "arc2daq.txt").c_str());
    while (!farc2daq.eof())
    {
        farc2daq >> arc >> daq;
        arc2daq[arc]=daq;
        daq2arc[daq]=arc;
    }
    farc2daq.close();

}
