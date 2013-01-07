#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "Formulas.h"
#include "Structs.h"
#include "Microsim.h"
#include "WriteToCVS.h"
#include "ReadConfig.h"
// Main.cpp


using namespace std;


int main()
{
    srand(time(NULL));
    MC::InputStruct in = MC::InputStruct(2);
    //in.CalculateCosC();  //doesn't work
    in.wtolerance = 1e-60;
    in.count = 2;
    ofstream filestr("simlog.txt");
    cout << filestr.fail() << endl;

    for(int i = 0; i<=0; i++)
    {
        system("pause");
        MC::OutputStruct ret;
        MC::PhotonClass photon = MC::PhotonClass();
        MC::simulatePhoton(&in, &photon, &ret, &filestr);
       //MC::Output(photon, ret);
    }
    filestr.close();
    return 0;
}

