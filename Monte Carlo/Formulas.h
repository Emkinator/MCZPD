#ifndef FORMULAS_H_INCLUDED
#define FORMULAS_H_INCLUDED

#ifndef STRUCTS_H_INCLUDED
#include "structs.h"
#endif

namespace MC
{
    int exampleFormula(int a, int b);


    bool MoveAndBound(InputStruct * in, PhotonClass * photon); //gets step size, does some checks, moves and returns if bounds

    void FresnelReflect(double * r, double * uzt, double sa1, double n, double nnext); //magic func #1

    void CrossMaybe(int dir, InputStruct * in, PhotonClass * photon); //went bit haxish to not double up such a big func,
//which eats both instruction cache and makes scroll wheel explode, dir is just 1 or -1 and that makes everything work

}


#endif // FORMULAS_H_INCLUDED
