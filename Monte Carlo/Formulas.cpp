#include "Formulas.h"
#include <cmath>
#include <stdlib.h> //for rand()
#include <iostream>
#define COS0 (1.0-1.0E-12)
#define COS90 1.0E-6
#define PI 3.14159265359
#define sign(num) (num < 0)
#define random() ((rand() % 1000)/1000.0)

int debugtmp = 0;

void MC::StepSize(PhotonClass* Photon, InputStruct* In) //Internal function
{
    std::cout << "Step size ";

    short layer = Photon->layer;
	double mua = In->layers[layer].mua;
	double mus = In->layers[layer].mus;

	if(Photon->sLeft < 0.0000000000001) //can't compare double with zero, though not sure if this is correct
	{
	    double rnd;
	    do rnd = random();
	    while(rnd == 0.0);
	    Photon->s = -(log(random()) / (mua+mus));
	    std::cout << Photon->s << std::endl;
	}
	else
	{
	    Photon->s = Photon->sLeft/(mua+mus);
	    Photon->sLeft = 0.0;
	}

}

double MC::SpinTheta(double g)  //Internal function
{
    double cost;

    if(g == 0.0)
        cost = 2*random() - 1;
    else
    {
        double tmp = (1-g*g) / (1-g+2*g*random());
        cost = (1+g*g - tmp*tmp) / (2*g);
    }
    return(cost);
}

void MC::Spin(double g, PhotonClass* Photon)
{
    double cosp, sinp;
    double cost, sint;

    double ux = Photon->ux;
    double uy = Photon->uy;
    double uz = Photon->uz;
    std::cout << "Dir cos: " << ux << " " << uy << " " << uz << std::endl;
    double psi;

    cost = SpinTheta(g);
    sint = sqrt(1.0-cost*cost);

    psi = 2.0*PI*random();
    cosp = cos(psi);
    if(psi<PI)
        sinp = sqrt(1.0-cosp*cosp);
    else
        sinp = -sqrt(1.0-cosp*cosp);

    if(abs(uz) > COS0)
    {
        Photon->ux = sint*cosp;
        Photon->uy = sint*sinp;
        Photon->uz = cost*sign(uz);
    }
    else
    {
        double tmp = sqrt(1.0 - uz*uz);
        Photon->ux = sint * (ux*uz*cosp - uy*sinp) / tmp + ux*cost;
        Photon->uy = sint * (uy*uz*cosp + ux*sinp) / tmp + uy*cost;
        Photon->uz = -sint*cosp*tmp + uz*cost;
    }
}

bool MC::MoveAndBound(InputStruct* in, PhotonClass* photon)   // !! bounding doesn't work correctly, often the photon packet seems to move outside of bounds.
{//gets step size, does some checks, moves and returns if bounds
    bool ret = false;
    short layer = photon->layer;
	double uz = photon->uz;
	double mua = in->layers[layer].mua;
	double mus = in->layers[layer].mus;

    MC::StepSize(photon, in);

    if(uz != 0.0) { //cross check
        double s = (in->layers[layer].z[int(uz>0.0)] - photon->z)/uz; //step size till bound
        if(s < photon->s) { //if crosses, recalc some things
            photon->sLeft = (photon->s - s)*(mua+mus); //multiplied with mua+mus for cross-layer compability
            photon->s = s;
            ret = true;
        }
    }

	photon->x += photon->s * photon->ux; //actually move
	photon->y += photon->s * photon->uy;
	photon->z += photon->s * uz;
    photon->w *= mua/(mua+mus);

	return ret;
}

double MC::FresnelReflect(double n1, double n2, double ca1, double* uzt) //Internal function
{ //todo: try to reduce the elseifs   EMK: Premature optimization is the root of all evil
	double r;

	if(n1 == n2)
	{ //bounds match
		*uzt = ca1;
		r = 0.0;
	}
	else if(ca1>COS0)
	{ //horizontal
		*uzt = ca1;
		r = (n2-n1)/(n2+n1);
		r *= r;
	}
	else if(ca1<COS90)
	{ //vertical
		*uzt = 0.0;
		r = 1.0;
	}
	else
	{
		double sa1, sa2, ca2; // sine of the incident and transmission angles & cos of transmisson angle
		sa1 = sqrt(1-ca1*ca1); //simpler than looks, just pythagoras from r^2 = x^2 + x^2
		sa2 = sa1*n1/n2;
		if(sa2>=1.0)
		{ //double check for total internal reflection, todo: check for better solution
			*uzt = 0.0;
			r = 1.0;
		}
		else
		{
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
	return(r);
}


void MC::CrossMaybe(InputStruct* in, PhotonClass* photon) //went bit haxish to not double up such a big func,
//which eats both instruction cache and makes scroll wheel explode, dir is just 1 or -1 and that makes everything work
{
    std::cout << "Cross maybe" << std::endl;
    if(debugtmp == 0)
        debugtmp = 2;

    system("pause");

    double uz = photon->uz; // z directional cosine.
    int dir;
    if(sign(uz))
        dir = 1;
    else
        dir = -1;

    short layer = photon->layer;
    double n1 = in->layers[layer].n;
	double n2 = in->layers[layer+dir].n;
	double uzt; // cosine of transmission alpha. uz1>0
	double r = 0.0; // reflectance

	if (dir * uz <= in->layers[layer+dir].cos_critical[int(dir>0)]) ////int(uz>0.0) makes array index 1 on positive and 0 on negative
        r = 1.0;
    else
        r = FresnelReflect(n1, n2, uz, &uzt);

    if(((layer == 1) || (layer == in->count)) && r<1.0)
    {//reflect and die/drop mass
        //LogPartialDying(r, in, photon, out); //todo
        photon->w *= r; //decrease weight
        photon->uz = -uz;
    }
    else if(random() > r) { //let trough at an angle
        photon->layer += dir; //layer id change
        photon->ux *= n1/n2;
        photon->uy *= n1/n2;
        photon->uz = dir * uzt; //retain original direction
    } else
        photon->uz = -uz; //reflect
}

void MC::Roulette(InputStruct* in, PhotonClass* photon)
{
    std::cout << "Weight: " << photon->w << std::endl;
    std::cout << "Cords: " << photon->x << " " << photon->y << " " << photon->z << std::endl;
    if(photon->w < in->wtolerance)
    {
        int tmp = rand() % 10;
        if(tmp == 0)
        {
            photon->w *= 10.0;
        }
        else
        {
            photon->alive = false;
        }
    }

    if(debugtmp>0)   //debugging stuff, pauses multiple times when CrossMaybe runs
    {
        system("pause");
        debugtmp--;
    }
}

double MC::SpecularReflect(double n1, double n2) //doesn't work, since n1 would always be 0. The other formula that's in the wong_thesis has 3 n variables, not sure how to use it.
{//Called once on the start of the simulation
	double r1;
	double temp;

	temp = n1-n2 / n1+n2;
	r1 = temp*temp;

	return (0.1); //temporary, should output r1
}
