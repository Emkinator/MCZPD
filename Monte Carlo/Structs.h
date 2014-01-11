#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED
#include <fstream>
#include <vector>
namespace MC
{
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

            PhotonClass();
    };

    class LayerClass
    {
        private:

        public:
            double z[2];      //coordiantes of the layer
            double n;           //refractive index
            double mua;         //absorption coefficient
            double mus;         //scattering coefficient
            double g;           //anisotropy
            double cos_critical[2]; //cosines of the critical angles, calculated later
            double* volume;
            //LayerClass();

    };

    class InputClass
    {
        private:

        public:
            int layerCount;
            int passes;
            int chromophores;
            int wavelength; //in nm
            int waveindex;
            double wtolerance;
            int range;
            double** absorbance;
            double base_absorbance;
            LayerClass* layers;
            InputClass();
            //~InputClass();
            double CalculateAbsorbance(int layer);
            void CalculateCosC(int count, std::ofstream* debuglog);
            void ChangeWavelength(int wl);
            void ReadAbsorbance();

    };

    class OutputClass
    {
        private:

        public:
            int gridSize;
            double*** photonDispersion;
            OutputClass(int size, int range);
            void PrintStatus(const char * title, int width);
    };


}
#endif // STRUCTS_H_INCLUDED
