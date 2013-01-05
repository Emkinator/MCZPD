#ifndef FORMULAS_H_INCLUDED
#define FORMULAS_H_INCLUDED

#ifndef STRUCTS_H_INCLUDED
#include "structs.h"
#endif

namespace MC
{

    void StepSize(PhotonClass* Photon, InputStruct* In); //internal

    double SpinTheta(double g); //internal

    void Spin(double g, PhotonClass* Photon);

    bool MoveAndBound(InputStruct* in, PhotonClass* photon); //gets step size, does some checks, moves and returns if bounds

    double FresnelReflect(double n1, double n2, double ca1, double* uzt); //internal

    void CrossMaybe(InputStruct* in, PhotonClass* photon);

    void Roulette(InputStruct* in, PhotonClass* photon);

    double SpecularReflect(double n1, double n2);

}



#endif // FORMULAS_H_INCLUDED
