#include <fstream>
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

int GetStepCount(char* filename)
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

void ReadMap(double*** spectrum, double &max_intensity, int resolution)
{
    int i = 0;
    ifstream file;
    file.open("grid.csv");
    string line;
    getline(file, line);
    int max_line;

    while(file.good() && i < resolution*resolution) {
        getline(file, line);
        int x = i / resolution;
        int y = i % resolution;
        double this_intensity = 0;
        vector<string> result = explode(line, ',');

        for (size_t j = 0; j < result.size() && j < range; j++) {
            spectrum[x][y][j] = atof(result[j].c_str());

            this_intensity += spectrum[x][y][j];
        }

        if(this_intensity > max_intensity) {
            max_intensity = this_intensity;
        }
        i++;
    }

    file.close();
}
