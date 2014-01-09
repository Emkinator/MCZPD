#include "Structs.h"
#include "ReadConfig.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

MC::PhotonClass::PhotonClass()
{
    using namespace MC;
    x = 0;
    y = 0;
    z = 0;
    ux = 0;
    uy = 0;
    uz = 1;
    w = 1.0;
    alive = 1;
    layer = 0;
    s = 0;
    sLeft = 0;
}


MC::InputClass::InputClass(int count)
{
    //std::cout << "Creating InputClass" << std::endl;
    layerCount = count;
    wtolerance = 1e-60;
    layers = new LayerClass[layerCount];
    ConfigClass lp = ConfigClass("config.txt"); //layer parameters

    chromophores = atof(lp.GetValue(0,"chromophores").c_str());
    absorbance = new double[chromophores];
    for(int i = 0; i < chromophores; i++) {
        absorbance[i] = atof(lp.GetValue(0,"absorbance", i).c_str());
    }

    wavelength = atof(lp.GetValue(0,"wavelength").c_str());
    base_absorbance = 0.0244 + 8.53 * exp(-(wavelength - 154) / 66.2);

    for(int i = 0; i < layerCount; i++) {
        double z = atof(lp.GetValue(i,"z").c_str());
        if(i > 0) {
            layers[i].z[0] = layers[i-1].z[1];
            layers[i].z[1] = layers[i-1].z[1] + z * 0.001; //conversion from micrometers to mm
        }
        else {
            layers[i].z[0] = 0;
            layers[i].z[1] = z *0.001;
        }
        layers[i].n = atof(lp.GetValue(i,"n").c_str());
        layers[i].mus = atof(lp.GetValue(i,"mus").c_str());
        layers[i].g = atof(lp.GetValue(i,"g").c_str());

        layers[i].volume = new double[chromophores];
        for(int j = 0; j < chromophores; j++) {
            layers[i].volume[j] = atof(lp.GetValue(i,"volume", j).c_str());
        }
        layers[i].mua = this->CalculateAbsorbance(i);
        //std::cout << "Layer read" << std::endl;
    }//label functionality in readconfig required for this, so that it can be replaced by lp.GetValue(layer,"z1").. etc. and automated
    //std::cout << "Done" << std::endl;
}

double MC::InputClass::CalculateAbsorbance(int layer)
{
    double x = 0.0;
    double temp;
    for(int i = 0; i < chromophores; i++) {
        temp = absorbance[i] * layers[layer].volume[i];
        for(int j = 0; j < i; j++) {
            temp *= (1.0 - layers[layer].volume[j]);
        }
        x += temp;
    }

    temp = base_absorbance;
    for(int i = 0; i < chromophores; i++) {
        temp *= (1.0 - layers[layer].volume[i]);
    }
    x += temp;

    return x;
}

void MC::InputClass::CalculateCosC(int count, std::ofstream* debuglog)
{
	double n1, n2;
    layers[0].cos_critical[0] = 0.0;

    for(short i=1; i<count; i++) {
        n1 = layers[i].n;
		n2 = layers[i-1].n;
		if(n1>n2) { //precision loss shouldnt matter cause that case would be discarded anyway
            layers[i].cos_critical[0] = sqrt(1.0 - n2*n2/(n1*n1));
            layers[i-1].cos_critical[1] = 0.0;
		}
		else {
		    layers[i].cos_critical[0] = 0.0;
            layers[i-1].cos_critical[1] = sqrt(1.0 - n1*n1/(n2*n2));
		}
    }
    layers[count - 1].cos_critical[1] = 1.0;

    /*for(short i=1; i<=count; i++)
	{
		n1 = layers[i].n;
		n2 = layers[i-1].n;
		//std::cout << n1 << "  " << n2;
		layers[i].cos_critical[0] = n1>n2 ?
		sqrt(1.0 - n2*n2/(n1*n1)) : 0.0;

		n2 = layers[i+1].n;
		layers[i].cos_critical[1] = n1>n2 ?
		sqrt(1.0 - n2*n2/(n1*n1)) : 0.0;
	}*/
}

MC::OutputClass::OutputClass(int size)
{
    gridSize = size;
    photonDispersion.resize(size);
    for (int i = 0; i < size; ++i)
        photonDispersion[i].resize(size);

    for (int x = 0; x < size; x++)
        for (int y = 0; y<size; y++)
            photonDispersion[x][y] = 0;
}
