#include "Structure.h"
#include "ReadConfigs.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

InputClass::InputClass(ConfigClass* lp)
{
    layerCount = atof(lp->GetValue(0,"count", -1).c_str());
    layers = new double*[layerCount];

    for(int i = 0; i < layerCount; i++) {
        layers[i] = new double[2];
        double z = atof(lp->GetValue(i,"z", -1).c_str());
        if(i > 0) {
            layers[i][0] = layers[i-1][1];
            layers[i][1] = layers[i-1][1] + z * 0.001; //conversion from mm to micrometers
        }
        else {
            layers[i][0] = 0;
            layers[i][1] = z * 0.001;
        }
    }
}

