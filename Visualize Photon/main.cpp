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
#include "Structs.h"
#include "SDL_gfxPrimitives.h"

struct pcords {
    double xorig;
    double yorig;
    double x;
    double y;
};
int GetStepCount(char* filename)
{
    using namespace std;
    int count = 0;
    string line;
    ifstream file(filename);
    while(file.good())
    {
      getline (file,line);
      count += 1;
    }
    file.close();
    return count-1;
}


int main ( int argc, char** argv )
{
    freopen( "CON", "wt", stdout );
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(1280, 720, 16, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        printf("Unable to set 1280x720 video: %s\n", SDL_GetError());
        return 1;
    }

    MC::InputStruct in = MC::InputStruct(7);
    in.count = 7;

    // read cords from file, assign to array "cords"
    int c = GetStepCount("simlog.txt");
    pcords cords[c];
    {
        using namespace std;

        string line;
        double x;
        double y;
        double z;
        size_t pos;
        size_t pos2;


        ifstream file("simlog.txt");

        for(int i=0;i<c;i++)
        {
            getline (file,line);
            pos = line.find(",");
            x = atof(line.substr(0, pos).c_str());
            pos2 = line.find(",",pos+1);

            y = atof(line.substr(pos+1,pos2-pos-1).c_str());

            pos = line.find(",",pos+1);
            pos2 = line.find(",",pos+1);

            z = atof(line.substr(pos+1,pos2-pos).c_str());

            cords[i].xorig = x;   //change this to "..= y" for other viewpoint
            cords[i].yorig = z;
        }
        file.close();
    }
    double scale = 1000;


    for(int i=0;i<c;i++)
    {
        cords[i].x = (cords[i].xorig * scale) + 640;
        cords[i].y = (cords[i].yorig * scale) + 50;
    }

    int n = 1;
    // program main loop
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    else if(event.key.keysym.sym == SDLK_SPACE)
                        if(n<c-1)
                        {
                            n+=1;
                            std::cout << n << std::endl;
                        }

                    break;
                }
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen


        // draw stuff
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
        lineRGBA(screen, 0, 50, 1279, 50, 255,255,255,255); // air/tissue boundary
        for(int i = 0; i < in.count;i++)
        {
            lineRGBA(screen, 0, 50+(in.layers[i].z[1]*scale), 1279, 50+(in.layers[i].z[1]*scale), 255,255,255,255);
        }
        for(int i = 0; i<n; i++)
            lineRGBA(screen, round(cords[i].x), round(cords[i].y), round(cords[i+1].x), round(cords[i+1].y), 255,255,255,255);




        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(screen);
    } // end main loop

    // free loaded bitmap

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}


