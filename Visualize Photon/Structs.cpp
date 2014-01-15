#include "Structs.h"
#include "ReadConfig.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>


MC::InputClass::InputClass()
{
    ConfigClass lp = ConfigClass("config.txt");
    layerCount = atof(ip.GetValue(0,"count").c_str());
    layers = new double*[layerCount];

    for(int i = 0; i < layerCount; i++) {
        layers[i] = new double[2];
        double z = atof(lp.GetValue(i,"z").c_str());
        if(i > 0) {
            layers[i][0] = layers[i-1][1];
            layers[i][1] = layers[i-1][1] + z * 0.001; //conversion from micrometers to mm
        }
        else {
            layers[i][0] = 0;
            layers[i][1] = z *0.001;
        }
    }
}
