//Simulates a single photon packet
#include <iostream>
#include "structs.h"
#include "formulas.h"

#define sign(num) ((num >> 31) | 1)
#define isglass(layer) (in->layers[layer].mus == 0.0 && in->layers[layer].mua == 0.0)

namespace MC
{
    void simulatePhoton(InputStruct * in, PhotonClass * photon, OutputStruct * out) // returns pointer to OutputStruct with all the neccesary info
    {
        std::cout << "Photon simulated.." << std::endl;
        if(MoveAndBound(in, photon)) { //move stuff and collison check
            CrossMaybe(sign(photon.uz), in, photon, out); //collison action
        } else {
            LoseWeight(in, photon); //todo
            Turn(in, photon); //todo
        }
        if(photon.alive && (photon->w < in.wtolerance))
            LiveOrDie(photon); //todo
    }
}

