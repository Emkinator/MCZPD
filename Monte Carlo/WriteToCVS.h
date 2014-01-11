#ifndef WRITETOCVS_H_INCLUDED
#define WRITETOCVS_H_INCLUDED
#include <fstream>
#include <string>
#include <vector>
#include "Structs.h"
//A .cvs file consists of elements seperated by commas
//A single row makes a single row in excel
//An endl makes a new row
namespace MC
{
    void WriteCSV(OutputClass* a, const char* fileName, int nrows, int ncols)
    {
        std::ofstream file;
        std::string fullName(fileName);
        fullName += ".csv";
        file.open(fullName.c_str());
        file << nrows << std::endl;
        for(int row = 0; row < nrows; row++){
            for(int col = 0; col < ncols; col++){
                file << a->photonDispersion[row][col];
                if(col < ncols-1)
                    file << ", ";
            }
            file << std::endl;
        }
        file.close();
    }
}

#endif // WRITETOCVS_H_INCLUDED
