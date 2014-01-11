#include "Structs.h"
#include "ReadConfig.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

#define read(name, layer, index) atof(ip.GetValue(layer,name,index).c_str())

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


MC::InputClass::InputClass()
{
    wtolerance = 1e-60;
    wavelength = 400;
    range = (750 - 400) / 10;
    ConfigClass ip = ConfigClass("config.txt"); //layer parameters
    //std::cout << "Creating InputClass" << std::endl;
    layerCount = read("count", 0, 0);
    layers = new LayerClass[layerCount];

    chromophores = read("chromophores", 0, 0);
    passes = read("passes", 0, 0) / range;

    this->ReadAbsorbance();

    for(int i = 0; i < layerCount; i++) {
        double z = read("z", i, 0);
        if(i > 0) {
            layers[i].z[0] = layers[i-1].z[1];
            layers[i].z[1] = layers[i-1].z[1] + z * 0.001; //conversion from micrometers to mm
        }
        else {
            layers[i].z[0] = 0;
            layers[i].z[1] = z *0.001;
        }
        layers[i].n = read("n", i, 0);
        layers[i].mus = read("mus", i, 0);
        layers[i].g = read("g", i, 0);

        layers[i].volume = new double[chromophores];
        for(int j = 0; j < chromophores; j++) {
            layers[i].volume[j] = read("volume", i, j);
        }
        //std::cout << "Layer read" << std::endl;
    }

    this->ChangeWavelength(wavelength);
    //std::cout << "Done" << std::endl;
}

void MC::InputClass::ReadAbsorbance()
{
    std::ifstream file;
    file.open("mua.txt");
    std::string line;
    int i = 0;

    absorbance = new double*[range];
    for(int i = 0; i < range; i++) {
        absorbance[i] = new double[chromophores];
    }

    while(file.good() && i < range) {
        getline(file, line);
        std::size_t pos = 0;
        std::size_t oldpos = 0;
        int n = 0;
        do {
            pos = line.find(",", pos);
            if(pos < 1)
                pos = line.size();
            absorbance[i][n] = atof(line.substr(oldpos + 1, pos - oldpos - 1).c_str());
            n++;
            oldpos = pos;
        }
        while(pos < line.size() && n < chromophores);
        i++;
    }
    file.close();
}

void MC::InputClass::ChangeWavelength(int wl)
{
    wavelength = wl;
    base_absorbance = 0.0244 + 8.53 * exp(-(wl - 154) / 66.2);
    //change absorbance[i]
    waveindex = (wl - 400) / 10;
    if(waveindex > range) waveindex = range;

    for(int i = 0; i < layerCount; i++) {
        layers[i].mua = this->CalculateAbsorbance(i);
    }
}

double MC::InputClass::CalculateAbsorbance(int layer)
{
    double x = 0.0;
    double temp;
    for(int i = 0; i < chromophores; i++) {
        temp = absorbance[waveindex][i] * layers[layer].volume[i];
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

    for(short i = 1; i < count; i++) {
        n1 = layers[i].n;
		n2 = layers[i-1].n;
		if(n1 > n2) { //precision loss shouldnt matter cause that case would be discarded anyway
            layers[i].cos_critical[0] = sqrt(1.0 - n2*n2/(n1*n1));
            layers[i-1].cos_critical[1] = 0.0;
		}
		else {
		    layers[i].cos_critical[0] = 0.0;
            layers[i-1].cos_critical[1] = sqrt(1.0 - n1*n1/(n2*n2));
		}
    }
    layers[count - 1].cos_critical[1] = 1.0;
}
/*
MC::InputClass::~InputClass()
{
    delete[] layers;
    for(int i = 0; i < range; i++) {
        delete[] absorbance[i];
    }
    delete[] absorbance;
}

MC::LayerClass::~LayerClass()
{
    delete[] volume;
}
*/
MC::OutputClass::OutputClass(int size, int range)
{
    gridSize = size;
    photonDispersion.resize(size*size);
    for (int i = 0; i < size*size; ++i) {
        photonDispersion[i].resize(range);
        for (int j = 0; j<range; j++)
            photonDispersion[i][j] = 0;
    }
}

