#ifndef MICROSIM_H_INCLUDED
#define MICROSIM_H_INCLUDED
#include "Structs.h"
#include <fstream>
namespace MC
{
    void simulatePhoton(InputClass * in, PhotonClass * photon, OutputClass * out, std::ofstream* debuglog); // returns pointer to OutputStruct with all the neccesary info
}


#endif // MICROSIM_H_INCLUDED
