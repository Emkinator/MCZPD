//Simulates a single photon packet
#include <fstream>
#include "structs.h"
#include "formulas.h"
#include "microsim.h"
#define isglass(layer) (in->layers[layer].mus == 0.0 && in->layers[layer].mua == 0.0)
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

namespace MC
{
    void simulatePhoton(InputStruct * in, PhotonClass * photon, OutputStruct * out, std::ofstream* debuglog)
    {
        int i = 0;
        double spec = SpecularReflect(1,in->layers[0].n);

        *debuglog << "Specular reflectance: " << spec << std::endl;
        photon->w -= spec;
        while(photon->alive)
        {


            *debuglog << "Cycle started" << std::endl;
            if(MoveAndBound(in, photon, debuglog))
                CrossMaybe(in, photon, debuglog);
            else
                Spin(in->layers[photon->layer].g, photon, debuglog);
            Roulette(in, photon, debuglog);
            i++;
            *debuglog << "Cycle " << i << " done" << std::endl << std::endl;
            if(photon->uz!=photon->uz)
            {
                photon->alive = false;
                *debuglog << "Not a number, photon killed." << std::endl;
            }
        }
        *debuglog << "Photon simulated.." << std::endl;

    }
}

