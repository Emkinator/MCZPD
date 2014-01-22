#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <utility>
#include <sstream>
#include "utility.h"

#define range       36

using namespace std;

struct pcords {
    double x;
    double y;
    double z;
};

int GetStepCount(const char* filename)
{
    int count = 0;
    string line;
    ifstream file(filename);
    while(file.good()) {
      getline (file,line);
      count += 1;
    }
    file.close();
    return count - 1;
}

void ReadCords(pcords* cords, int c)
{
    string line;
    size_t pos, pos2;
    ifstream file("simlog.txt");

    for(int i = 0; i < c; i++) {
        getline (file,line);
        pos = line.find(",");
        cords[i].x = atof(line.substr(0, pos).c_str());

        pos2 = line.find(",",pos+1);
        cords[i].y = atof(line.substr(pos+1,pos2-pos-1).c_str());

        pos = line.find(",",pos+1);
        pos2 = line.find(",",pos+1);
        cords[i].z = atof(line.substr(pos+1,pos2-pos).c_str());
    }
    file.close();
}

int GetResolution()
{
    ifstream file;
    file.open("../Monte Carlo/grid.csv");
    string line;
    getline(file, line);
    int pos = line.find(",");
    if(pos > 0) {
        return atoi(line.substr(0, pos).c_str());
    }
    return 0;
}

void ReadMap(double**** spectrum, double *max_intensity, int resolution)
{

    ifstream file;
    file.open("../Monte Carlo/grid.csv");
    string line;
    getline(file, line); //ignoring 1st line due to it having some other data
    getline(file, line); //ignoring 2nd due to pile up of out of bounds values there
    for(int n = 0; n < range; n++)
        spectrum[0][0][0][n] = 0;

    int i = 1;
    while(file.good() && i < resolution*resolution) {
        getline(file, line);
        int x = i / resolution;
        int y = i % resolution;
        double this_intensity = 0;
        vector<string> result = explode(line, ',');

        for (size_t j = 0; j < result.size() && j < range; j++) {
            spectrum[0][x][y][j] = atof(result[j].c_str());

            this_intensity += spectrum[0][x][y][j];
        }

        if(this_intensity > max_intensity[0]) {
            max_intensity[0] = this_intensity;
        }
        i++;
    }

    int n = 1;
    for(int res = resolution / 2; res > 0; res /= 2) {
        double temp_max = 0.0;

        for(int x = 0; x < res; x++) {
            for(int y = 0; y < res; y++) {
                for(int s = 0; s < range; s++) {
                    double temp = (spectrum[n - 1][x * 2][y * 2][s] + spectrum[n - 1][x * 2 + 1][y * 2][s]
                        + spectrum[n - 1][x * 2][y * 2 + 1][s] + spectrum[n - 1][x * 2 + 1][y * 2 + 1][s]) / 4.0;
                    if(temp > temp_max)
                        temp_max = temp;
                    spectrum[n][x][y][s] = temp;
                }
            }
        }
        max_intensity[n] = temp_max;
        n++;
    }
    file.close();
}
