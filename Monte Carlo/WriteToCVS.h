#ifndef WRITETOCVS_H_INCLUDED
#define WRITETOCVS_H_INCLUDED
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Structs.h"
//A .cvs file consists of elements seperated by commas
//A single row makes a single row in excel
//An endl makes a new row

using namespace std;

void WriteCSV(OutputClass* a, InputClass* in, const char* fileName, int nx, int ny, int nz)
{
    ofstream file;
    string fullName(fileName);
    fullName += ".csv";
    file.open(fullName.c_str());
    file << nx << "," << in->stepcount << "," << a->count * in->range * in->threads
        << "," << in->zoom << "," << in->specular << endl;
    ostringstream buffer;
    int approx_count = 0;

    int last = 0;
    int progress = 0;

    for(int x = 0; x < nx; x++){
        for(int y = 0; y < ny; y++){
            buffer << a->photonDispersion[x][y][0];
            for(int z = 1; z < nz; z++){
                buffer <<  "," << a->photonDispersion[x][y][z];
            }
            buffer << endl;
            approx_count += 130;
        }

        if(approx_count > 1024 * 1024) { //1MB
            file << buffer.str();
            buffer.str("");
            buffer.clear();
            approx_count = 0;
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
