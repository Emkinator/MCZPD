#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <fstream>
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
    MC::InputStruct in = MC::InputStruct(3);
    in.wtolerance = 1e-60;
    in.count = 3;
    ofstream filestr("simlog.txt");
    cout << filestr.fail() << endl;
    in.CalculateCosC(in.count, &filestr);


    for(int i = 0; i<=10; i++)
    {
        system("Pause");
        filestr << "Photon packet nr. " << i << endl << endl;
        MC::OutputStruct ret;
        MC::PhotonClass photon = MC::PhotonClass();
        MC::simulatePhoton(&in, &photon, &ret, &filestr);
        filestr << endl << "--------------------------------------" << endl << endl;
       //MC::Output(photon, ret);
    }
    filestr.close();
    return 0;
}

