#ifndef WRITETOCVS_H_INCLUDED
#define WRITETOCVS_H_INCLUDED
#include <fstream>
#include <string>
#include <vector>
#include "Structs.h"
//A .cvs file consists of elements seperated by commas
//A single row makes a single row in excel
//An endl makes a new row

using namespace std;

void WriteCSV(OutputClass* a, const char* fileName, int nx, int ny, int nz)
{
    ofstream file;
    string fullName(fileName);
    fullName += ".csv";
    file.open(fullName.c_str());
    file << nx << ", " << ny << endl;

    int last = 0;
    int progress = 0;

    for(int x = 0; x < nx; x++){
        for(int y = 0; y < ny; y++){
            for(int z = 0; z < nz; z++){
                file << a->photonDispersion[x][y][z];
                if(z < nz-1)
                    file << ", ";
            }
            file << endl;
        }
        //progress output
        progress += ny * nz;
        if(progress > nx * ny * nz * last / 80) {
            cout << "|";
            last++;
        }

    }

    cout << endl;
    file.close();
}

#endif // WRITETOCVS_H_INCLUDED
