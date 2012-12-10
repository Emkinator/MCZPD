#include "Formulas.h"

#define cos0 (1.0-1.0E-12)
#define cos90 1.0E-6
#define sign(num) ((num >> 31) | 1)
#define isglass(layer) (in->layers[layer].mus == 0.0 && in->layers[layer].mua == 0.0)

int MC::exampleFormula(int a, int b)
{
    return a+b;
}

bool MC::MoveAndBound(InputStruct * in, PhotonClass * photon) //gets step size, does some checks, moves and returns if bounds
{
    bool ret = false;
    int layer = photon->layer;
	double uz = photon->uz;
    if(isglass(layer)) { //glass step calculation, calculates till border
        if(uz == 0.0) { //horizontal in glass means it'll bever collide again
            photon.alive = false;
            return ret;
        }
        photon->s = (in->layers[layer].z[int(uz>0.0)] - photon->z)/uz; //int(uz>0.0) makes array index 1 on positive and 0 on negative
        ret = true; //in glass will always collide
    } else {
        double mut = in->layers[layer].mua + in->layers[layer].mus;
        if(photon->sLeft == 0.0) { //random step size
            double rand;
            do rand = random();
            while(rand==0.0)
            photon->s = -log(rand)/mut; //random step formula
        } else { //use what was left last time
            photon->s = photon->sLeft/mut;
            photon->sLeft = 0.0;
        }
        if(uz != 0.0) { //cross check
            double s = (in->layers[layer].z[int(uz>0.0)] - photon->z)/uz; //step size till bound
            if(s > photon->s) { //if crosses, recalc some things
                photon->sLeft = (photon->s - s)*mut; //multiplied with mut for cross-layer compability
                photon->s = s;
                ret = true;
            }
        }
    }
	photon->x += photon->s * photon->ux; //actually move
	photon->y += photon->s * photon->uy;
	photon->z += photon->s * uz;
	return ret;
}

void MC::FresnelReflect(double * r, double * uzt, double sa1, double n, double nnext) //magic func #1
{ //todo: try to reduce the elseifs
	if(n==nnext) { //bounds match
		*uzt = sa1;
		*r = 0.0;
	} else if(ca1>cos0) { //horizontal
		*uzt = sa1;
		*r = (nnext-n)/(nnext+n);
		*r *= *r;
	} else if(ca1<cos90) { //vertical
		*uzt = 0.0;
		*r = 1.0;
	} else {
		double sa1, sa2, ca2; // sine of the incident and transmission angles & cos of transmisson angle
		sa1 = sqrt(1-ca1*ca1); //simpler than looks, just pythagor from r^2 = x^2 + x^2
		sa2 = sa1*n/nnext;
		if(sa2>=1.0) { //double check for total internal reflection, todo: check for better solution
			*uzt = 0.0;
			*r = 1.0;
		} else {
			double cap, cam, sap, sam; // cosines and sines of the sum or difference of the two angles. p = plus, m = minus
			ca2 = sqrt(1-sa2*sa2);
			cap = ca1*ca2 - sa1*sa2; // c+ = cc - ss.
			cam = ca1*ca2 + sa1*sa2; // c- = cc + ss.
			sap = sa1*ca2 + ca1*sa2; // s+ = sc + cs.
			sam = sa1*ca2 - ca1*sa2; // s- = sc - cs.
			*r = 0.5*sam*sam*(cam*cam+cap*cap)/(sap*sap*cam*cam); // rearranged for speed.
			*uzt = ca2;
		}
	}
}

void MC::CrossMaybe(int dir, InputStruct * in, PhotonClass * photon) //went bit haxish to not double up such a big func,
//which eats both instruction cache and makes scroll wheel explode, dir is just 1 or -1 and that makes everything work
{
    double uz = photon->uz; // z directional cosine.
    int layer = photon->layer;
    double n = in->layers[layer].n;
	double nnext = in->layers[layer+dir].n;
	double uzt; // cosine of transmission alpha. uz1>0
	double r = 0.0; // reflectance
	if (dir * uz <= in->layers[layer+dir].cos_critical[int(dir>0)]) ////int(uz>0.0) makes array index 1 on positive and 0 on negative
        r = 1.0;
    else
        FresnelReflect(&r, &uzt, dir * uz, n, nnext);
    if(((layer == 1) || (layer == in.count)) && r<1.0) //reflect and die/drop mass
        LogPartialDying(r, in, photon, out); //todo
        photon->w *= r; //decrease weight
        photon->uz = -uz;
    else if(random() > r) { //let trough at an angle
        photon->layer += dir; //layer id change
        photon->ux *= n/nnext;
        photon->uy *= n/nnext;
        photon->uz = dir * uzt; //retain original direction
    } else
        photon->uz = -uz; //reflect
}
