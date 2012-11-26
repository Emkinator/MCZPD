//Simulates a single photon packet
#include <iostream>
#include "structs.h"
#include "formulas.h"
namespace MC
{
    OutputStruct* simulatePhoton() // returns pointer to OutputStruct with all the neccesary info
    {
        std::cout << "Photon simulated.." << std::endl;
        OutputStruct* Output;
        Output->a = 1;
        return Output;
    }
}
