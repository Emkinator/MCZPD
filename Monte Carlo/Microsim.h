#ifndef MICROSIM_H_INCLUDED
#define MICROSIM_H_INCLUDED
#include "Structs.h"
namespace MC
{
    void simulatePhoton(InputStruct * in, PhotonClass * photon, OutputStruct * out); // returns pointer to OutputStruct with all the neccesary info
}


#endif // MICROSIM_H_INCLUDED
