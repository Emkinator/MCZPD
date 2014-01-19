#ifndef FORMULAS_H_INCLUDED
#define FORMULAS_H_INCLUDED

#ifndef STRUCTS_H_INCLUDED
#include "structs.h"
#include <fstream>
#endif
#include <thread>
#include <mutex>

using namespace std;

void StepSize(PhotonClass* Photon, InputClass* In, ofstream* filestr, int wl); //internal

double SpinTheta(double g); //internal

void Spin(double g, PhotonClass* Photon, ofstream* filestr); //calculates spin

bool MoveAndBound(InputClass* in, PhotonClass* photon, ofstream* filestr, int wl); //gets step size, does some checks, moves and returns if bounds

double FresnelReflect(double n1, double n2, double ca1, double* uzt); //internal

void CrossMaybe(InputClass* in, PhotonClass* photon, OutputClass* out, ofstream* filestr, mutex* lock, int wl); //checks if photon should cross layer boundary

void Roulette(InputClass* in, PhotonClass* photon, ofstream* filestr); //gives photon a 1/10 chance to not be completely absorbed

double SpecularReflect(double n1, double n2); //calculates ammount of reflection at photon entrance in tissue



#endif // FORMULAS_H_INCLUDED
