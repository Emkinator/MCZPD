#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include "ReadConfig.h"

using namespace std;

namespace std {
    class mutex {}; //dummy until i sort the compilers for multithreading
    class PhotonClass
    {
        private:

        public:
            double x, y, z;     //position
            double ux, uy, uz;  //directional cosines
            double w;           //weight
            bool alive;         //wether the photon packet is active
            short layer;        //which layer the photon packet is in
            double s;           //current step size
            double sLeft;       //step size left
            PhotonClass():
                x(0.0),
                y(0.0),
                z(0.0),
                ux(0.0),
                uy(0.0),
                uz(0.0),
                w (1.0),
                alive(1),
                layer(0),
                s(0.0),
                sLeft(0.0)
            {}
    };

    class LayerClass
    {
        public:
            double z[2];      //coordiantes of the layer
            double n;           //refractive index
            double* mua;         //absorption coefficient
            double mus;         //scattering coefficient
            double g;           //anisotropy
            double cos_critical[2]; //cosines of the critical angles, calculated later
            double* volume;
            ~LayerClass();
    };

    class InputClass
    {
        public:
            int threads;
            int gridsize;
            int range;
            long long int passes;
            int timelimit;
            int chunk;
            int layerCount;
            int chromophores;
            long long int stepcount;
            double specular;
            double wtolerance;
            double zoom;
            double** absorbance;
            LayerClass* layers;
            double CalculateAbsorbance(double base_absorbance, int layer, int waveindex);
            void CalculateCosC(int count, ofstream* debuglog);
            void ReadAbsorbance();
            ~InputClass();
            InputClass();
    };

    class OutputClass
    {
        private:

        public:
            int gridSize;
            long long int count;
            double*** photonDispersion;
            void PrintStatus(const char * title, int width);
            OutputClass(int size, int range);
            ~OutputClass();
    };


}
#endif // STRUCTS_H_INCLUDED
