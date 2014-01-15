#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED
#include <fstream>
namespace MC {

    class InputClass {
        public:
            int layerCount;
            double** layers;
            InputClass();
    };
}
#endif // STRUCTS_H_INCLUDED
