//Simulates a single photon packet
#include <iostream>
#include "structs.h"
#include "formulas.h"
#include "microsim.h"
#define isglass(layer) (in->layers[layer].mus == 0.0 && in->layers[layer].mua == 0.0)
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

namespace MC
{
    void simulatePhoton(InputStruct * in, PhotonClass * photon, OutputStruct * out) // returns pointer to OutputStruct with all the neccesary info
    {
        std::cout << "Photon simulated.." << std::endl;
        if(MoveAndBound(in, photon)) { //move stuff and collison check
            CrossMaybe(sign(photon->uz), in, photon); //collison action
        } else {
            //LoseWeight(in, photon); //todo
            //Turn(in, photon); //todo
        }
        if(photon->alive && (photon->w < in->wtolerance)) {
            //LiveOrDie(photon); //todo
        }
    }
}

