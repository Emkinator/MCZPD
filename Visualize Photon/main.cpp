#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include "Structs.h"
#include "SDL_gfxPrimitives.h"
#include "vmath.h"

using namespace std;

struct pcords {
    double x;
    double y;
    double z;
};

//#define abs(x) ((x ^ (x >> 31)) - (x >> 31))
#define interpolate(start, finish, progress) (start + progress * (finish - start))

void GradientLine(SDL_Surface* dst, float x1, float y1, float x2, float y2, int sc1, int sc2, int sc3, int ec1, int ec2, int ec3) //color ranges from 0 to 255, black to white
{
    int c1, c2, c3;
    float dx = x2-x1;
    float dy = y2-y1;

    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    float x, y;
    float progress = 0;
    for(int i = 0; i <= steps; i++) {
        progress = i / (float)steps;
        x = interpolate(x1, x2, progress);
        y = interpolate(y1, y2, progress);
        c1 = interpolate(sc1, ec1, progress);
        c2 = interpolate(sc2, ec2, progress);
        c3 = interpolate(sc3, ec3, progress);
        pixelRGBA(dst,x,y,c1,c2,c3,255);
    }
}

int GetStepCount(char* filename)
{
    int count = 0;
    string line;
    ifstream file(filename);
    while(file.good()) {
      getline (file,line);
      count += 1;
    }
    file.close();
    return count - 1;
}



int main (int argc, char** argv)
{
    freopen("CON", "wt", stdout);
    // initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    int bounds[] = {1366, 768, 100}; //100 is just dummy for middle Z point
    SDL_Surface* screen = SDL_SetVideoMode(bounds[0], bounds[1], 16, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
    if (!screen) {
        printf("Unable to set %ix%i video: %s\n", bounds[0], bounds[1], SDL_GetError());
        return 1;
    }

    //MC::ConfigClass ip = MC::ConfigClass("config.txt");
    //int count = atof(ip.GetValue(0,"count").c_str());
    MC::InputClass in = MC::InputClass(7);

    // read cords from file, assign to array "cords"
    int c = GetStepCount("simlog.txt");
    pcords cords[c];
    {
        string line;
        size_t pos, pos2;
        ifstream file("simlog.txt");

        for(int i = 0; i < c; i++) {
            getline (file,line);
            pos = line.find(",");
            cords[i].x = atof(line.substr(0, pos).c_str());

            pos2 = line.find(",",pos+1);
            cords[i].y = atof(line.substr(pos+1,pos2-pos-1).c_str());

            pos = line.find(",",pos+1);
            pos2 = line.find(",",pos+1);
            cords[i].z = atof(line.substr(pos+1,pos2-pos).c_str());
        }
        file.close();
    }


    double scale = 10000;
    double newscale = scale;
    int lasttick = SDL_GetTicks();
    int tick = lasttick;
    float degrees_per_second = 90.0;
    Vector3f xaxis(1, 0, 0);
    Vector3f yaxis(0, 1, 0);
    Vector3f zaxis(0, 0, 1);
    Vector3f p1(0, 0, 0);
    Vector3f p2(0, 0, 0);
    int n = 1;
    int view = 1;
    bool recalcScale = false;
    bool paused = false;
    // program main loop ----------------------------------------------
    bool done = false;
    while (!done) {
        // message processing loop
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_KEYDOWN: {
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            done = true;
                            break;
                        case SDLK_SPACE:
                            n = min(n + 1, c - 1);
                            break;
                        case SDLK_RETURN:
                            n = min(n + 5, c - 1);
                            break;
                        case SDLK_BACKSPACE:
                            n = max(n - 5, 0);
                            break;
                        case SDLK_p:
                            paused = !paused;
                            break;
                        case SDLK_RIGHT:
                            if(!paused)
                                degrees_per_second += 60.0;
                            break;
                        case SDLK_LEFT:
                            if(!paused)
                                degrees_per_second -= 60.0;
                            break;
                    }
                    break;
                }
            } // end switch
        } // end of message processing

        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        if(recalcScale) {
            scale = 10000;
            recalcScale = false;
        }
        else {
            scale = newscale;
        }

        lineRGBA(screen, 0, 50, bounds[0], 50, 255,255,255,255); // air/tissue boundary
        for(int i = 0; i < in.layerCount;i++) {
            int z = 50 + in.layers[i].z[1]*scale;
            lineRGBA(screen, 0, z, bounds[0], z, 255,255,255,255);
        }

        for(int i = 0; i<n; i++) {
            p1.x = cords[i].x * scale;
            p2.x = cords[i+1].x * scale;
            p1.z = cords[i].z * scale;
            p2.z = cords[i+1].z * scale;
            p1.y = cords[i].y * (scale/300) * 255;
            p2.y = cords[i+1].y * (scale/300) * 255;

            float x1 = xaxis.dotProduct(p1) + bounds[0] / 2;
            float x2 = xaxis.dotProduct(p2) + bounds[0] / 2;
            float y1 = zaxis.dotProduct(p1) + 50;
            float y2 = zaxis.dotProduct(p2) + 50;
            int c1 = min(255, max(80, (int)yaxis.dotProduct(p1) + 188));
            int c2 = min(255, max(80, (int)yaxis.dotProduct(p2) + 188));

            if(x2 > (bounds[0] - 40) || x2 < 40) {
                newscale = min(newscale, scale * ((bounds[0] - 40) / x2));
            }
            if(y2 > (bounds[1] - 40) || y2 < 40){
                newscale = min(newscale, scale * ((bounds[1] - 40) / y2));
            }

            x1 = min(bounds[0], max(0, (int)x1));
            x2 = min(bounds[0], max(0, (int)x2));
            y1 = min(bounds[1], max(0, (int)y1));
            y2 = min(bounds[1], max(0, (int)y2));
            GradientLine(screen, x1, y1, x2, y2, c1, c1, c1, c2, c2, c2);
        }

        tick = SDL_GetTicks();
        float degrees = 0;
        if(paused) {
            //lasttick = tick;
            Uint8 *keystate = SDL_GetKeyState(NULL);
            if(keystate[SDLK_RIGHT])
                degrees = 60.0;
            else if(keystate[SDLK_LEFT])
                degrees = -60.0;
        }
        else {
            degrees = degrees_per_second;
        }
        degrees *= (tick - lasttick) / 1000.0;

        xaxis.rotate(0, 0, degrees);
        yaxis.rotate(0, 0, degrees);
        //zaxis.rotate(0, 0, degrees);
        lasttick = tick;
        SDL_Flip(screen);
    } // end main loop

    printf("Exited cleanly\n");
    return 0;
}
