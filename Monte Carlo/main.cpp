#include <iostream>
#include "Formulas.h"
#include "Structs.h"
#include "Microsim.h"
// Main.cpp


using namespace std;


int main()
{
    cout << MC::exampleFormula(1,2) << endl;
    MC::PhotonClass testPhoton = MC::PhotonClass();
    cout << testPhoton.alive << endl;
    MC::OutputStruct* ret = 0;
    for(int i = 1; i<100; i++)
    {
        ret = MC::simulatePhoton();
    }
    return 0;
}

