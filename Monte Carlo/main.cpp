#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include "Formulas.h"
#include "Structs.h"
#include "Microsim.h"
#include "WriteToCVS.h"
#include "ReadConfig.h"

#define random() ((rand() % 65536)/65536.0)
// Main.cpp


using namespace std;


int main()
{
    srand(time(NULL));
    MC::ConfigClass ip = MC::ConfigClass("config.txt");
    int count = atof(ip.GetValue(0,"count").c_str());;
    MC::InputClass in = MC::InputClass(count);
    in.count = count;
    in.wtolerance = 1e-60;

    ofstream filestr("simlog.txt");
    if(filestr.fail() == 1) {
        cout << "Logfile could not be created." << endl;
    }

    in.CalculateCosC(in.count, &filestr);

    in.passes = atof(ip.GetValue(0,"passes").c_str());;
    for(int i = 0; i<=in.passes; i++) {
        MC::OutputStruct ret;
        MC::PhotonClass photon = MC::PhotonClass();
        MC::simulatePhoton(&in, &photon, &ret, &filestr);
        //filestr << endl << "-------------------------" << endl << endl;
        //MC::Output(photon, ret);
    }

    filestr.close();
    return 0;
}

