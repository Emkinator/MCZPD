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

void GetData(int& resolution, int& max_layers, double& photons, double& specular, double& scale)
{
    ifstream file;
    file.open("../Monte Carlo/grid.csv");
    string line;
    getline(file, line);
    vector<string> result = explode(line, ',');
    if(result.size() > 0) {
        resolution = atoi(result[0].c_str());
    }
    if(result.size() > 2) {
        photons = atof(result[2].c_str());
    }
    if(result.size() > 3) {
        scale = atof(result[2].c_str());
    }
    if(result.size() > 4) {
        specular = atof(result[4].c_str());
    }
    if(result.size() > 5) {
        max_layers = atoi(result[5].c_str());
    }
    file.close();
}

void ExportIntensityGraph(double***** spectrum, int range_low, int range_high, int resolution, int res_levels, int layer, double units)
{
    double avg_distance = 0;
    double* graph = new double[resolution / 2];
    for(int n = 0; n < resolution / 2; n++) {
        graph[n] = 0;
    }

    for(int x = 0; x < resolution; x++) {
        for(int y = 0; y < resolution; y++) {
            double sum = 0;
            for(int s = range_low; s <= range_high; s++) {
                sum += spectrum[layer][0][x][y][s];
            }
            if(sum != 0) {
                int mid = resolution / 2;
                double dist = sqrt((mid - x)*(mid - x) + (mid - y)*(mid - y));
                avg_distance += dist * sum;

                int whole = dist; //auto-rounding
                double fraction = dist - whole;
                if(whole + 1 < resolution / 2) {
                    graph[whole] += sum * (1 - fraction);
                    graph[whole + 1] += sum * fraction;
                }
            }
        }
    }

    double total_mass = 0;
    for(int s = range_low; s <= range_high; s++) {
        total_mass += spectrum[layer][res_levels - 1][0][0][s];
    }
    avg_distance /= total_mass * resolution;
    avg_distance *= units;
    cout << avg_distance << endl;

    ofstream file;
    file.open("intensity.csv");
    //file << graph[0];
    for(int n = 0; n < resolution / 2; n++) {
        file << double(n) / resolution * units << "," << graph[n] << endl;
    }
    //file << endl;
    file.close();
    delete[] graph;
}

void GetIntensity(double***** spectrum, double** intensity, double& max_intensity, int range_low, int range_high, int resolution, int res_zoom, int res_levels, int layer, int max_layers)
{
    int res = resolution >> res_zoom;
    double temp_max = 0;


    for(int x = 0; x < res; x++) {
        for(int y = 0; y < res; y++) {
            double sum = 0;
            for(int s = range_low; s <= range_high; s++) {
                sum += spectrum[layer][res_zoom][x][y][s];
            }
            intensity[x][y] = sum;
            if(sum > temp_max)
                temp_max = sum;
        }
    }
    max_intensity = temp_max;
}

void ReadMap(double***** spectrum, int resolution, int max_layers)
{

    ifstream file;
    file.open("../Monte Carlo/grid.csv");
    string line;
    do {
        getline(file, line);
    } while(line.size() < 1 || (line.c_str())[0] == '/');
    //ignoring 1st line due to it having some other data + comment lines

    int i = 0;
    int l = max_layers - 1;
    while(file.good()) {
        getline(file, line);
        int x = i / resolution;
        int y = i % resolution;
        vector<string> result = explode(line, ',');

        for (size_t j = 0; j < range; j++) {
            double temp = 0;
            if(j < result.size() && result[j] != "" && result[j] != "0") {
                temp = atof(result[j].c_str());
            }
            if(l < max_layers - 1) {
               temp += spectrum[l + 1][0][x][y][j];
            }
            spectrum[l][0][x][y][j] = temp;
        }
        i++;
        if(i == resolution*resolution) {
            l--;
            i = 0;
            if(l < 0) break;
        }
    }

    for(int l = 0; l < max_layers; l++) {
        for(int n = 0; n < range; n++)
            spectrum[l][0][0][0][n] = 0; //ignoring out-of-bounds pileup in corner
    }


    for(int l = 0; l < max_layers; l++) {
        int n = 1;
        for(int res = resolution / 2; res > 0; res /= 2) {
            for(int x = 0; x < res; x++) {
                for(int y = 0; y < res; y++) {
                    for(int s = 0; s < range; s++) {
                        spectrum[l][n][x][y][s] =  (spectrum[l][n - 1][x * 2][y * 2][s] + spectrum[l][n - 1][x * 2 + 1][y * 2][s]
                            + spectrum[l][n - 1][x * 2][y * 2 + 1][s] + spectrum[l][n - 1][x * 2 + 1][y * 2 + 1][s]);
                    }
                }
            }
            n++;
        }
    }
    file.close();
}
