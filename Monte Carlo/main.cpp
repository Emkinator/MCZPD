#include <iostream>
#include "Formulas.h"
#include "Structs.h"
#include "Microsim.h"
#include "WriteToCVS.h"
#include "ReadConfig.h"
// Main.cpp


using namespace std;


int main()
{
    MC::ConfigClass config = MC::ConfigClass("config.txt");
    MC::InputStruct in = MC::InputStruct();
    //some more params or func with them here
    for(int i = 0; i<=1000; i++)
    {
        MC::OutputStruct ret;
        MC::PhotonClass photon = MC::PhotonClass();
        MC::simulatePhoton(&in, &photon, &ret);
       // MC::Output(photon, ret);
    }
    return 0;
}

