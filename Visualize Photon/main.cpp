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

pcords* ReadCords(char* filename)
{
    using namespace std;

    int c = GetStepCount(filename);
    string line;
    double x;
    double y;
    double z;
    size_t pos;
    size_t pos2;
    pcords cords[c];

    ifstream file(filename);

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
        cout << i << ": " << cords[i].xorig << " " << cords[i].yorig << endl;
    }
    file.close();
    return cords;
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
    SDL_Surface* screen = SDL_SetVideoMode(1280, 720, 16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        printf("Unable to set 1280x720 video: %s\n", SDL_GetError());
        return 1;
    }

    std::cout << "Steps: " << GetStepCount("simlog.txt") << std::endl;


    pcords* cords = ReadCords("simlog.txt");

    for(int i=0;i<268;i++)
    {
        std::cout << i << ": " << cords[i].xorig << "  " << cords[i].yorig << std::endl;
    }

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
                    break;
                }
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        // draw bitmap

        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(screen);
    } // end main loop

    // free loaded bitmap

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}


