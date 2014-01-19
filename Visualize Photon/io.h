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

int GetStepCount(char* filename);
void ReadCords(pcords* cords, int c);
void ReadMap(double*** spectrum, double &max_intensity, int resolution);

#endif // IO_H_INCLUDED
