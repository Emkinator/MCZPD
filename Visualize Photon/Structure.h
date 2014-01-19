#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED
#include <fstream>
#include "ReadConfigs.h"

class InputClass {
    public:
        int layerCount;
        double** layers;
        InputClass(ConfigClass* lp);
};

#endif // STRUCTS_H_INCLUDED
