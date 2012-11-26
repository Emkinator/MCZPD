#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED
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
            double z0, z1;      //coordiantes of the layer
            double n;           //refractive index
            double mua;         //absorption coefficient
            double mus;         //scattering coefficient
            double g;           //anisotropy

            LayerClass(double z0, double z1, double n, double mua, double mus, double g);
    };

    struct OutputStruct
    {
        int a;
    };
}
#endif // STRUCTS_H_INCLUDED
