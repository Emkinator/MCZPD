#include "Structs.h"
#include "ReadConfig.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>

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
    stepcount = 0;
    wavelength = 390; //10 will be added on the first run
    range = (750 - 390) / 10;
    ConfigClass ip = ConfigClass("config.txt"); //layer parameters
    wtolerance = read("wtolerance", 0, -1);
    //std::cout << "Creating InputClass" << std::endl;
    layerCount = read("count", 0, -1);
    layers = new LayerClass[layerCount];

    chromophores = read("chromophores", 0, -1);
    passes = read("passes", 0, -1) / range;

    this->ReadAbsorbance();

    for(int i = 0; i < layerCount; i++) {
        double z = read("z", i, -1);
        if(i > 0) {
            layers[i].z[0] = layers[i-1].z[1];
            layers[i].z[1] = layers[i-1].z[1] + z * 0.001; //conversion from micrometers to mm
        }
        else {
            layers[i].z[0] = 0;
            layers[i].z[1] = z *0.001;
        }
        layers[i].n = read("n", i, -1);
        layers[i].mus = read("mus", i,-1);
        layers[i].g = read("g", i, -1);

        layers[i].volume = new double[chromophores];
        for(int j = 0; j < chromophores; j++) {
            layers[i].volume[j] = read("volume", i, j);
        }
        //std::cout << "Layer read" << std::endl;
    }
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
        std::size_t oldpos = -1;
        int n = 0;
        do {
            pos = line.find(",", pos);
            if(pos < 1)
                pos = line.size();
            absorbance[i][n] = atof(line.substr(oldpos + 1, (pos - 1) - (oldpos + 1)).c_str());
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
    if(waveindex >= range) waveindex = range;

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
    photonDispersion = new double**[size];
    for(int x = 0; x < size; x++) {
        photonDispersion[x] = new double*[size];
        for(int y = 0; y < size; y++) {
            photonDispersion[x][y] = new double[range];
            for(int z = 0; z < range; z++)
                photonDispersion[x][y][z] = 0.0;
        }
    }
}

void MC::OutputClass::PrintStatus(const char * title, int width)
{
    int len = strlen(title);
    int chars = (width - 2 - len) / 2;
    if(chars < 0) {
        std::cout << std::endl << "Bad title for status bar - too long (\"" << title << "\")" << std:: endl;
        return;
    }
    std::cout << "|";
    if((width - len) & 1)
        std::cout << "-";
    for(int i = 0; i < chars; i++)
        std::cout << "-";
    std::cout << title;
    for(int i = 0; i < chars; i++)
        std::cout << "-";
    std::cout << "|" << std::endl;
}


