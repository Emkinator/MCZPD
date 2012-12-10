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
    cout << MC::exampleFormula(1,2) << endl;
    MC::PhotonClass testPhoton = MC::PhotonClass();
    cout << testPhoton.alive << endl;
    MC::ConfigClass config = MC::ConfigClass("config.txt");
    MC::InputStruct in = InputStruct();
    //some more params or func with them here
    for(int i = 0; i<in.count; i++)
    {
        MC::OutputStruct ret;
        MC::PhotonClass photon = MC::PhotonClass();
        while(photon.alive) {
            MC::simulatePhoton(&in, &photon, &ret);
        }
        MC::Output(photon, ret);
    }
    MC::WriteCVS(a,"test",1,100);
    return 0;
}

