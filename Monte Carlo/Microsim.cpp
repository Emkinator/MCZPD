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
    void simulatePhoton(InputStruct * in, PhotonClass * photon, OutputStruct * out)
    {
        int i = 0;
        double uzt;
        double spec = SpecularReflect(1,in->layers[0].n);
        std::cout << "Specular reflectance: " << spec << std::endl;
        photon->w -= spec;
        while(photon->alive)
        {
            std::cout << "Cycle started" << std::endl;
            if(MoveAndBound(in, photon))
                CrossMaybe(in, photon);
            Spin(in->layers[photon->layer].g, photon);
            Roulette(in, photon);
            i++;
            std::cout << "Cycle " << i << " done" << std::endl << std::endl;
        }
        std::cout << "Photon simulated.." << std::endl;

    }
}

