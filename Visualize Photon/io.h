#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED
#include <fstream>
#include <string>
#include <cmath>
#include <utility>
#include <sstream>
#include "utility.h"

using namespace std;

struct pcords {
    double x;
    double y;
    double z;
};

int GetStepCount(const char* filename);
void ReadCords(pcords* cords, int c);
void GetData(int& resolution, int& max_layers, double& photons, double& specular);
void GetIntensity(double***** spectrum, double** intensity, double& max_intensity,
    int range_low, int range_high, int resolution, int res_zoom, int layer, int max_layers);
void ReadMap(double***** spectrum, int resolution, int max_layers);

#endif // IO_H_INCLUDED
