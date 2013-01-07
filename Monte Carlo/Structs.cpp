#include "Structs.h"
#include "ReadConfig.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

MC::PhotonClass::PhotonClass()
{
    using namespace MC;
    x = 0;
    y = 0;
    z = 0;
    ux = 0;
    uy = 0;
    uz = 1;
    w = 1.0;
    alive = 1;
    layer = 0;
    s = 0;
    sLeft = 0;
}


MC::InputStruct::InputStruct(int count)
{
    std::cout << "Creating InputStruct" << std::endl;
    layers = new LayerClass[count];
    ConfigClass lp = ConfigClass("config.txt"); //layer parameters
    for(int i = 0; i<count; i++)
    {
        layers[i].z[0] = atof(lp.GetValue(i,"z1").c_str());
        layers[i].z[1] = atof(lp.GetValue(i,"z2").c_str());
        layers[i].n = atof(lp.GetValue(i,"n").c_str());
        layers[i].mua = atof(lp.GetValue(i,"mua").c_str());
        layers[i].mus = atof(lp.GetValue(i,"mus").c_str());
        layers[i].g = atof(lp.GetValue(i,"g").c_str());

        std::cout << atof(lp.GetValue(i,"mua").c_str()) << std::endl;
        std::cout << "Layer read" << std::endl;
    }//label functionality in readconfig required for this, so that it can be replaced by lp.GetValue(layer,"z1").. etc. and automated
    std::cout << "Done" << std::endl;
}

void MC::InputStruct::CalculateCosC(int count, std::ofstream* debuglog) //doesn't work
{
	double n1, n2;
    layers[0].cos_critical[0] = 0.0;
    *debuglog << "cos_Crit " << (layers[0].cos_critical[0]) << " ";

    for(short i=1; i<=count; i++)
    {
        n1 = layers[i].n;
		n2 = layers[i-1].n;
		if(n1>n2) { //precision loss shouldnt matter cause that case would be discarded anyway
            layers[i].cos_critical[0] = sqrt(1.0 - n2*n2/(n1*n1));
            layers[i-1].cos_critical[1] = 0.0;
		}
		else {
		    layers[i].cos_critical[0] = 0.0;
            layers[i-1].cos_critical[1] = sqrt(1.0 - n1*n1/(n2*n2));
		}
		*debuglog << (layers[i-1].cos_critical[1]) << " " << (layers[i].cos_critical[0]) << " ";
    }
    layers[count].cos_critical[1] = 0.0;
    *debuglog << (layers[count].cos_critical[1]) << std::endl << std::endl;

    /*for(short i=1; i<=count; i++)
	{
		n1 = layers[i].n;
		n2 = layers[i-1].n;
		//std::cout << n1 << "  " << n2;
		layers[i].cos_critical[0] = n1>n2 ?
		sqrt(1.0 - n2*n2/(n1*n1)) : 0.0;

		n2 = layers[i+1].n;
		layers[i].cos_critical[1] = n1>n2 ?
		sqrt(1.0 - n2*n2/(n1*n1)) : 0.0;
	}*/
}
