#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include "Formulas.h"
#include "ReadConfig.h"
#include "Structs.h"
#include "Microsim.h"
#include "WriteToCVS.h"

#define random() ((rand() % 65536)/65536.0)
// Main.cpp


using namespace std;


int main()
{
    srand(time(NULL));

    MC::InputClass in;
    MC::OutputClass ret(256, in.range);

    ofstream filestr("simlog.txt");
    if(filestr.fail() == 1) {
        cout << "Logfile could not be created." << endl;
    }

    in.CalculateCosC(in.layerCount, &filestr);

    for(int i = 0; i < in.range; i++) {
        for(int n = 0; n < in.passes; n++) {
            MC::PhotonClass photon = MC::PhotonClass();
            MC::simulatePhoton(&in, &photon, &ret, &filestr);
            //filestr << endl << "-------------------------" << endl << endl;
            //MC::Output(photon, ret);
        }
        in.ChangeWavelength(in.wavelength + 10);
    }

    MC::WriteCSV(&ret, "grid", ret.gridSize * ret.gridSize, in.range);
    filestr.close();
    return 0;
}

