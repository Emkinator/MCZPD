#ifndef MICROSIM_H_INCLUDED
#define MICROSIM_H_INCLUDED
#include "Structs.h"
#include <fstream>
#include <mutex>
#include <thread>


using namespace std;

void simulatePhoton(InputClass * in, PhotonClass * photon, OutputClass * out, ofstream* debuglog, int wl, mutex* lock); // returns pointer to OutputStruct with all the neccesary info



#endif // MICROSIM_H_INCLUDED
