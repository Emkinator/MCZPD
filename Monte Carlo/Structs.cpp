#include "Structs.h"
#include "ReadConfig.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <cstring>
#include "formulas.h"

using namespace std;

vector<string> explode(const string& str, const char& ch);

InputClass::InputClass()
:
    stepcount(0),
    range((750 - 390) / 10),
    gridsize(256),
    zoom(8),
    chromophores(1),
    wtolerance(1e-5),
    layerCount(1),
    layers(NULL),
    threads(1),
    passes(1),
    timelimit(0xffffffff),
    chunk(1),
    absorbance(NULL),
    specular(0),
    absorbance_modifier(1),
    molarMass(NULL)
{
    ConfigClass ip("config.txt"); //layer parameters
    ip.GetValue(gridsize, "resolution", 0);
    ip.GetValue(zoom, "zoom", 0);
    ip.GetValue(chromophores, "chromophores", 0);
    ip.GetValue(wtolerance, "wtolerance", 0);

    ip.GetValue(layerCount, "count", 0);
    layers = new LayerClass[layerCount];

    ip.GetValue(threads, "threads", 0);
    ip.GetValue(passes, "passes", 0);
    passes = max(int(passes / threads), 1);
    chunk = min(max(1, int(passes / 100)), 6000);
    ip.GetValue(timelimit, "timelimit", 0);
    ip.GetValue(absorbance_modifier, "absorbance_modifier", 0);
    molarMass = new double[chromophores-1];
    for(int i = 0; i<chromophores-1; i++){
        ip.GetValue(molarMass[i],"molarMass",0,i);
    }

    ConfigClass light("light_source.txt");
    double total_light = 0;
    for(int i = 0; i < range; i++) {
        light.GetValue(light_bias[i], "light_source", 0, i);
        total_light += light_bias[i];
    }
    for(int i = 0; i < range; i++) {
        light_bias[i] /= total_light;
    }
    for(int i = 0; i < layerCount; i++) {
        int z;
        ip.GetValue(z, "z", i, -1);
        if(i > 0) {
            layers[i].z[0] = layers[i-1].z[1];
            layers[i].z[1] = layers[i-1].z[1] + z * 0.001; //conversion from micrometers to mm
        }
        else {
            layers[i].z[0] = 0;
            layers[i].z[1] = z *0.001;
        }
        ip.GetValue(layers[i].n, "n", i);
        ip.GetValue(layers[i].mus, "mus", i);
        ip.GetValue(layers[i].g, "g", i);

        layers[i].volume = new double[chromophores];
        for(int j = 0; j < chromophores; j++) {
            ip.GetValue(layers[i].volume[j], "volume", i, j);
        }
    }
    this->ReadAbsorbance();
    specular = SpecularReflect(1, layers[0].n);
}

void InputClass::ReadAbsorbance()
{
    ifstream file;
    file.open("mua.txt");
    string line;
    int i = 0;
    absorbance = new double*[range];
    for(int i = 0; i < range; i++) {
        absorbance[i] = new double[chromophores];
        for(int j = 0; j < chromophores; j++) {
            absorbance[i][j] = 0;
        }
    }
    while(file.good() && i < range) {
        getline(file, line);

        vector<string> result = explode(line, ',');
        for (size_t j = 0; j < result.size() && j < chromophores; j++) {
            absorbance[i][j] = atof(result[j].c_str()) / absorbance_modifier;
        }
        i++;
    }
    file.close();

    for(i = 0; i < layerCount; i++) {
        layers[i].mua = new double[range];
        for(int n = 0; n < range; n++) {
            int wl = 400 + n * 10;
            double base_absorbance = 0.0244 + 8.53 * exp(-(wl - 154) / 66.2);
            layers[i].mua[n] = this->CalculateAbsorbance(base_absorbance, i, n);
        }
    }
}

double InputClass::CalculateAbsorbance(double base_absorbance, int layer, int waveindex)
{

    double x = 0.0;
    double temp;
    for(int i = 0; i < chromophores; i++) {
        temp = (absorbance[waveindex][i] * 2.303 * layers[layer].volume[i] / molarMass[i]  ) * layers[layer].volume[i];
        for(int j = 0; j < i+1; j++) {
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

void InputClass::CalculateCosC(int count, ofstream* debuglog)
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

InputClass::~InputClass()
{
    delete[] layers;
    for(int i = 0; i < range; i++) {
        delete[] absorbance[i];
    }
    delete[] absorbance;
}

LayerClass::~LayerClass()
{
    delete[] volume;
    delete[] mua;
}

OutputClass::OutputClass(int size, int range, int layers)
:
    gridSize(size),
    count(0),
    photonDispersion(NULL),
    range(range)
{
    photonDispersion = new double***[size];
    for(int x = 0; x < size; x++) {
        photonDispersion[x] = new double**[size];
        for(int y = 0; y < size; y++) {
            photonDispersion[x][y] = new double*[range];
            for(int z = 0; z < range; z++) {
                photonDispersion[x][y][z] = new double[layers];
                for(int layer = 0; layer < layers; layer++) {
                    photonDispersion[x][y][z][layer] = 0.0;
                }
            }
        }
    }
}

OutputClass::~OutputClass()
{
    for(int x = 0; x < gridSize; x++) {
        for(int y = 0; y < gridSize; y++) {
            for(int z = 0; z < range; z++) {
                delete[] photonDispersion[x][y][z];
            }
            delete[] photonDispersion[x][y];
        }
        delete[] photonDispersion[x];
    }
    delete[] photonDispersion;
}

void OutputClass::PrintStatus(const char * title, int width)
{
    int len = strlen(title);
    int chars = (width - 2 - len) / 2;
    if(chars < 0) {
        cout << endl << "Bad title for status bar - too long (\"" << title << "\")" <<  endl;
        return;
    }
    cout << "|";
    if((width - len) & 1)
        cout << "-";
    for(int i = 0; i < chars; i++)
        cout << "-";
    cout << title;
    for(int i = 0; i < chars; i++)
        cout << "-";
    cout << "|";
}

vector<string> explode(const string& str, const char& ch) { //source: http://stackoverflow.com/questions/890164/how-can-i-split-a-string-by-a-delimiter-into-an-array
    string next;
    vector<string> result;

    for (string::const_iterator it = str.begin(); it != str.end(); it++) {
        if (*it == ch) {
            if (!next.empty()) {
                result.push_back(next);
                next.clear();
            }
        } else {
            next += *it;
        }
    }
    if (!next.empty())
         result.push_back(next);
    return result;
}


