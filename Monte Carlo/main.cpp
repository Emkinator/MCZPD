#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <thread>
#include "ReadConfig.h"
#include "Formulas.h"
#include "Structs.h"
#include "Microsim.h"
#include "WriteToCVS.h"

// Main.cpp

using namespace std;

void Thread(int thread_id, clock_t start, InputClass* in, OutputClass* ret, ofstream* filestr, mutex* lock)
{
    int passes = in->passes;
    int chunk = in->chunk;
    int total = passes;
    int progress = 0;
    while(passes > 0) {
        for(int i = 0; i < in->range; i++) {
            for(int n = 0; n < chunk; n++) {
                PhotonClass photon;
                simulatePhoton(in, &photon, ret, filestr, i, lock);
            }
        }
        passes -= chunk;
        chunk = min(passes, chunk);

        float timeprogress = (float(clock() - start) / CLOCKS_PER_SEC) / in->timelimit;
        float done = max(float(total - passes)/total, timeprogress);
        while(done * 80 > progress && progress < 80) {
            progress++;
            if((progress + thread_id) % in->threads == 0) cout << "|";
        }
        if(timeprogress >= 1.0)
            break;
    }
    {
        //lock_guard<mutex> lk(*lock);
        ret->count += (total - passes);
    }
}

int main()
{
    srand(time(NULL));
    clock_t start = clock();

    InputClass in;
    OutputClass ret(in.gridsize, in.range);

    ofstream filestr("simlog.txt");
    if(filestr.fail() == 1) {
        cout << "Logfile could not be created." << endl;
    }

    in.CalculateCosC(in.layerCount, &filestr);

    ret.PrintStatus("Simulating", 80);
    mutex lock;
    //thread threads[in.threads];
    for(int i; i < in.threads; i++) {
        //threads[i] = thread(Thread, i, start, &in, &ret, &filestr, &lock);
        Thread(i, start, &in, &ret, &filestr, &lock);
    }

    for(int i; i < in.threads; i++) {
        //threads[i].join();
    }

    cout << "Average step count:" << in.stepcount / (ret.count *in.range* in.threads) <<
        "   " << ret.count * in.range * in.threads << " photons simulated." << endl;

    ret.PrintStatus("Outputting", 80);
    WriteCSV(&ret, "grid", ret.gridSize, ret.gridSize, in.range);
    filestr.close();
    return 0;
}

