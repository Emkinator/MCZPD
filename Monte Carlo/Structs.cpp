#include "Structs.h"

MC::PhotonClass::PhotonClass()
{
    using namespace MC;
    x = 0;
    y = 0;
    z = 0;
    ux = 0;
    uy = 0;
    uz = 0;
    w = 1.0;
    alive = 1;
    layer = 1;
    s = 0;
    sLeft = 0;
}

MC::LayerClass::LayerClass(double nz0, double nz1, double nn, double nmua, double nmus, double ng)
{
    using namespace MC;
    z[0] = nz0;
    z[1] = nz1;
    n = nn;
    mua = nmua;
    mus = nmus;
    g = ng;
}
