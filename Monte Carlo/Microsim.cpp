//Simulates a single photon packet
#include <fstream>
#include <thread>
#include <mutex>
#include "structs.h"
#include "formulas.h"
#include "microsim.h"
#define isglass(layer) (in->layers[layer].mus == 0.0 && in->layers[layer].mua == 0.0)
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

void simulatePhoton(InputClass * in, PhotonClass* photon, OutputClass * out, ofstream* debuglog, int wl, mutex* lock)
{
    int i = 0;
    photon->w -= in->specular;
    while(photon->alive) {
        //*debuglog << "Cycle started" << endl;
        //*debuglog << "  Cords:     " << photon->x << " " << photon->y << " " << photon->z << endl;
        //*debuglog << "  Dir cos:   " << photon->ux << " " << photon->uy << " " << photon->uz << endl;
        //*debuglog << "  Step size: " << photon->s << endl;;
        //*debuglog << "  Weight: " << photon->w << endl;

        //*debuglog << photon->x << "," << photon->y << "," << photon->z << "," << photon->w << endl;

        if(MoveAndBound(in, photon, debuglog, wl)) {
            CrossMaybe(in, photon, out, debuglog, lock, wl);
        }
        else {
            Spin(in->layers[photon->layer].g, photon, debuglog);
        }
        Roulette(in, photon, debuglog);
        i++;
        //*debuglog << "Photon in layer " << photon->layer << endl;
        //*debuglog << "Cycle " << i << " done" << endl << endl;
        if(photon->uz!=photon->uz)
            photon->alive = false;
    }
    //*debuglog << "Photon simulated.. " << photon << endl;

}

