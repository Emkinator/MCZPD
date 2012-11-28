//Simulates a single photon packet
#include <iostream>
#include "structs.h"
#include "formulas.h"

namespace MC
{
    OutputStruct* simulatePhoton() // returns pointer to OutputStruct with all the neccesary info
    {
        std::cout << "Photon simulated.." << std::endl;
        OutputStruct output;
        output.a = 1;
        std::cout << output.a << " ";  //it works, but comment this line out and...
        return (&output);
    }
}

