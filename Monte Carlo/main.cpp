#include <iostream>
#include "Formulas.h"
#include "Structs.h"
#include "Microsim.h"
#include "WriteToCVS.h"
// Main.cpp


using namespace std;


int main()
{
    cout << MC::exampleFormula(1,2) << endl;
    MC::PhotonClass testPhoton = MC::PhotonClass();
    cout << testPhoton.alive << endl;
    MC::OutputStruct* ret;
    int a[100];
    for(int i = 1; i<=100; i++)
    {
        ret = MC::simulatePhoton();
        a[i-1] = ret->a;
    }
    MC::WriteCVS(a,"test",10,10);
    return 0;
}

