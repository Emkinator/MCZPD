#include <iostream>
#include "Formulas.h"
#include "Structs.h"
#include "Microsim.cpp"
// Main.cpp


using namespace std;

int main()
{
    cout << MC::exampleFormula(1,2) << endl;
    MC::PhotonClass testPhoton = MC::PhotonClass();
    cout << testPhoton.alive << endl;
    for(int i = 1; i<100; i++)
    {
        MC::simulatePhoton();
    }
    return 0;
}
