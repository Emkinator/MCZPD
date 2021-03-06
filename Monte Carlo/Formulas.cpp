#include "Formulas.h"
#include <cmath>
#include <stdlib.h> //for rand()
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#define COS0 (1.0-1.0E-12)
#define COS90 1.0E-6
#define PI 3.14159265359
#define sign(num) (-(num < 0) | 1)
#define random() (rand() / double(RAND_MAX))
#define clamp(minv,x,maxv) max(minv, min(maxv, x))

using namespace std;

void StepSize(PhotonClass* Photon, InputClass* In, ofstream* filestr, int wl) //Internal function
{
    short layer = Photon->layer;
	double mua = In->layers[layer].mua[wl];
	double mus = In->layers[layer].mus;
    In->stepcount++;
	if(Photon->sLeft < 0.0000000000001) {
	    double rnd;
	    do rnd = random();
	    while(rnd <= 0.0);
        Photon->s = -(log(rnd) / (mua+mus));
	}
	else{
	    Photon->s = Photon->sLeft/(mua+mus);
	    Photon->sLeft = 0.0;
	}

}

double SpinTheta(double g)  //Internal function
{
    double cost;

    if(g == 0.0)
        cost = 2*random() - 1;
    else{
        double tmp = (1-g*g) / (1-g+2*g*random());
        cost = (1+g*g - tmp*tmp) / (2*g);
    }
    return(cost);
}

void Spin(double g, PhotonClass* Photon, ofstream* filestr)
{
    double cosp, sinp;
    double cost, sint;

    double ux = Photon->ux;
    double uy = Photon->uy;
    double uz = Photon->uz;
    double psi;

    cost = SpinTheta(g);
    sint = sqrt(1.0-cost*cost);

    psi = 2.0*PI*random();
    cosp = cos(psi);
    if(psi<PI)
        sinp = sqrt(1.0-cosp*cosp);
    else
        sinp = -sqrt(1.0-cosp*cosp);
    if(sign(uz) * uz > COS90) {
        Photon->ux = sint*cosp;
        Photon->uy = sint*sinp;
        Photon->uz = cost*sign(uz);
    }
    else {
        double tmp = sqrt(1.0 - uz*uz);
        Photon->ux = sint * (ux*uz*cosp - uy*sinp) / tmp + ux*cost;
        Photon->uy = sint * (uy*uz*cosp + ux*sinp) / tmp + uy*cost;
        Photon->uz = -sint*cosp*tmp + uz*cost;
    }
}

bool MoveAndBound(InputClass* in, PhotonClass* photon, ofstream* filestr, int wl)
{//gets step size, does some checks, moves and returns if bounds
    bool ret = false;
    short layer = photon->layer;
	double uz = photon->uz;
	double mua = in->layers[layer].mua[wl];
	double mus = in->layers[layer].mus;

    StepSize(photon, in, filestr, wl);

    if(uz != 0.0) { //cross check
        double s = (in->layers[layer].z[int(uz > 0.0)] - photon->z) / uz; //step size till bound
        if(s < photon->s) { //if crosses, recalc some things
            photon->sLeft = (photon->s - s)*(mua+mus); //multiplied with mua+mus for cross-layer compability
            photon->s = s;
            ret = true;
        }
    }

	photon->x += photon->s * photon->ux; //actually move
	photon->y += photon->s * photon->uy;
	photon->z += photon->s * uz;
    photon->w *= mus/(mua+mus);

	return ret;
}

double FresnelReflect(double n1, double n2, double ca1, double* uzt) //Internal function
{
	double r;
	if(n1 == n2){ //bounds match
		*uzt = ca1;
		r = 0.0;
	}
	else if(ca1>COS0){ //horizontal
		*uzt = ca1;
		r = (n2-n1)/(n2+n1);
		r *= r;
	}
	else if(ca1<COS90){ //vertical
		*uzt = 0.0;
		r = 1.0;
	}
	else{
		double sa1, sa2, ca2; // sine of the incident and transmission angles & cos of transmisson angle
		sa1 = sqrt(1-ca1*ca1); //r^2 = x^2 + x^2
		sa2 = sa1*n1/n2;
		if(sa2>=1.0){ //double check for total internal reflection
			*uzt = 0.0;
			r = 1.0;
		}
		else{
			double cap, cam, sap, sam; // cosines and sines of the sum or difference of the two angles. p = plus, m = minus
			ca2 = sqrt(1-sa2*sa2);
			cap = ca1*ca2 - sa1*sa2; // c+ = cc - ss.
			cam = ca1*ca2 + sa1*sa2; // c- = cc + ss.
			sap = sa1*ca2 + ca1*sa2; // s+ = sc + cs.
			sam = sa1*ca2 - ca1*sa2; // s- = sc - cs.
			r = 0.5*sam*sam*(cam*cam+cap*cap)/(sap*sap*cam*cam); // rearranged for speed.
			*uzt = ca2;
		}
	}
	if(r > 1.0) cout << "Found r above 1";
	return(r);
}


void CrossMaybe(InputClass* in, PhotonClass* photon, OutputClass* out, ofstream* filestr,  mutex* lock, int wl)
{
    double uz = photon->uz; // z directional cosine.
    int dir = sign(uz);
    short layer = photon->layer;
    double n1 = in->layers[layer].n;
    double n2;
    if(layer == 0 && dir == -1)
        n2 = 1; //air
    else
        n2 = in->layers[layer+dir].n;
	double uzt; //cosine of transmission alpha. uz1>0
	double r = 0.0; //reflectance
	if (dir * uz <= in->layers[layer].cos_critical[int(dir>0)])//int(uz>0.0) makes array index 1 on positive and 0 on negative
        r = 1.0;
    else
        r = FresnelReflect(n1, n2, abs(uz), &uzt);

    if((layer == 0) && (dir == -1) && r < 1.0) { //reflect and die/drop mass
        //*filestr << photon->x << "," << photon->y << "," << (photon->w - (photon->w * r)) << endl;
        //lock_guard<mutex> lk(*lock);
        double tmp = photon->w * r;
        int tmp2 = out->gridSize / 2;
        int px = clamp(0, int(photon->x * in->zoom * tmp2) + tmp2, out->gridSize - 1);
        int py = clamp(0, int(photon->y * in->zoom * tmp2) + tmp2, out->gridSize - 1);
        out->photonDispersion[px][py][wl][photon->maxlayer] += photon->w - tmp;
        photon->w = tmp;
        photon->uz = -uz;
    }
    else if(random() > r) { //let trough at an angle
        if(layer > photon->maxlayer) photon->maxlayer = layer;
        photon->layer += dir; //layer id change
        photon->ux *= n1/n2;
        photon->uy *= n1/n2;
        photon->uz = dir * uzt; //retain original direction
        //*filestr << "Layer: " << photon->layer << endl; //logs layer change
    }
    else
        photon->uz = -uz; //reflect
}

void Roulette(InputClass* in, PhotonClass* photon, ofstream* filestr)
{
    if(photon->w < in->wtolerance){
        int tmp = rand() % 10;
        if(tmp == 0) {
            photon->w *= 10.0;
        }
        else {
            photon->alive = false;
        }
    }
}

double SpecularReflect(double n1, double n2)
{//Called once on the start of the simulation
	double temp = (n1-n2) / (n1+n2);

	return temp*temp;
}
