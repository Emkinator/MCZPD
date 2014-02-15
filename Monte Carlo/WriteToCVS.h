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

void WriteCSV(OutputClass* out, InputClass* in, const char* fileName, int nx, int ny, int nz)
{
    ofstream file;
    string fullName(fileName);
    fullName += ".csv";
    file.open(fullName.c_str());
    file << nx << "," << in->stepcount << "," << out->count * in->range * in->threads
        << "," << in->zoom << "," << in->specular << "," << in->layerCount << endl;
    ostringstream buffer;
    int approx_count = 0;

    double last = 0;
    double progress = 0;

    for(int layer = in->layerCount - 1; layer >= 0; layer--) {
        for(int x = 0; x < nx; x++) {
            for(int y = 0; y < ny; y++) {
                int z = 0;
                int last = 0;
                double value;
                do {
                    value = out->photonDispersion[x][y][z][layer];
                    if(value > 1e-60) {
                        while(last < z) {
                            buffer <<  ',';
                            last++;
                        }
                        buffer << value;
                        approx_count += 16;
                    }
                    z++;
                } while(z < nz);

                buffer << endl;
            }

            if(approx_count > 1024 * 1024) { //1MB
                file << buffer.str();
                buffer.str("");
                buffer.clear();
                approx_count = 0;
            }
            //progress output
            progress = double(x + nx * (in->layerCount - layer)) / (nx * in->layerCount) * 80;
            if(progress >= last + 1) {
                cout << "|";
                last = progress;
            }
        }
    }
    cout << endl;
    file << buffer.str();
    file.close();
}

#endif // WRITETOCVS_H_INCLUDED
