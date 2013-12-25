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
    double x;
    double y;
    double z;
};
/*
void GradientLine(SDL_Surface* dst, int x_1, int y_1, int x_2, int y_2, int startColour, int endColour) //color ranges from 0 to 255, black to white
{
    int x1;
    int y1;
    int x2;
    int y2;
    int c1;
    int c2;

    int c3;

    if(x_1>x_2)
    {
        x1 = x_2;
        y1 = y_2;
        x2 = x_1;
        y2 = y_1;
        c1 = endColour;
        c2 = startColour;
    }
    else
    {
        x1 = x_1;
        y1 = y_1;
        x2 = x_2;
        y2 = y_2;
        c1 = startColour;
        c2 = endColour;
    }

    float dx = (x2-x1);
    float dy = (y2-y1);

    int steps = abs(dy);

    if(abs(dx)>abs(dy))
        steps = abs(dx);

    float x_inc = (dx/(float)steps);
    float y_inc = (dy/(float)steps);

    float c = (float)c1;
    float cStep;
    if(steps!=0)
        cStep = (c2-c1)/steps;

    float x = x1;
    float y = y1;

    pixelRGBA(dst,x,y,c1,c1,c1,255);
    for(int count = 1; count <= steps; count++)
    {
        x += x_inc;
        y += y_inc;
        c += cStep;
        c3 = round(c);
        pixelRGBA(dst,x,y,c3,c3,c3,255);
    }
}
*/
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

    MC::InputClass in = MC::InputClass(7);
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

            cords[i].x = x;   //change this to "..= y" for other viewpoint
            cords[i].z = z;
            cords[i].y = y;
        }
        file.close();
    }
    double scale = 10000;


    double x1, x2, y1, y2, c1, c2;
    int n = 1;
    int view = 1;
    bool recalcScale = false;
    // program main loop ----------------------------------------------
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;

            case SDL_KEYDOWN:
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    else if(event.key.keysym.sym == SDLK_SPACE)
                    {
                        if(n<c-1)
                            n += 1;
                        else n = c;
                    }
                    else if(event.key.keysym.sym == SDLK_BACKSPACE)
                    {
                        if(n>5)
                            n -= 5;
                        else n = 0;
                    }
                    else if(event.key.keysym.sym == SDLK_F1)
                    {
                        view = 1;
                        std::cout << "view x/z (front)" << std::endl;
                        recalcScale = true;
                    }
                    else if(event.key.keysym.sym == SDLK_F2)
                    {
                        view = 2;
                        std::cout << "view y/z (side)" << std::endl;
                        recalcScale = true;
                    }
                    else if(event.key.keysym.sym == SDLK_F3)
                    {
                        view = 3;
                        std::cout << "view x/y (topdown)" << std::endl;
                        recalcScale = true;
                    }

                    break;
                }
            } // end switch
        } // end of message processing
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        if(view!=3)
        {
            lineRGBA(screen, 0, 50, 1279, 50, 255,255,255,255); // air/tissue boundary
            for(int i = 0; i < in.count;i++)
                lineRGBA(screen, 0, 50+(in.layers[i].z[1]*scale), 1279, 50+(in.layers[i].z[1]*scale), 255,255,255,255);
        }
        else
            pixelRGBA(screen,640,360,255,100,0,0);


        for(int i = 0; i<n; i++)
        {
            if(view == 1)
            {
                x1 = (cords[i].x * scale) + 640;
                x2 = (cords[i+1].x * scale) + 640;
                y1 = (cords[i].z * scale) + 50;
                y2 = (cords[i+1].z * scale) + 50;
                c1 = (cords[i].y * (scale/300) * 255) + 127;
                c2 = (cords[i+1].y * (scale/300) * 255) + 127;
            }
            else if(view == 2)
            {
                x1 = (cords[i].y * scale) + 640;
                x2 = (cords[i+1].y * scale) + 640;
                y1 = (cords[i].z * scale) + 50;
                y2 = (cords[i+1].z * scale) + 50;
                c1 = (cords[i].x * (scale/100) * 255) + 127;
                c2 = (cords[i+1].x * (scale/100) * 255) + 127;
            }
            else if(view == 3)
            {
                x1 = (cords[i].x * scale) + 640;
                x2 = (cords[i+1].x * scale) + 640;
                y1 = (cords[i].y * scale) + 360;
                y2 = (cords[i+1].y * scale) + 360;
                c1 = (cords[i].z * (scale/100) * 255) + 127;
                c2 = (cords[i+1].z * (scale/100) * 255) + 127;
            }

            lineRGBA(screen, round(x1), round(y1), round(x2), round(y2), 255,255,255,255);
            //GradientLine(screen, round(x1), round(y1), round(x2), round(y2),c1,c2);
        }

        if(x2>1200 || x2<80 || y2>640 || y2<50)
            scale = scale * 0.9;

        if(recalcScale)
        {
            scale = 10000;
            for(int i = 0; i<n; i++)
            {
                 if(view == 1)
                {
                    x1 = (cords[i].x * scale) + 640;
                    x2 = (cords[i+1].x * scale) + 640;
                    y1 = (cords[i].z * scale) + 50;
                    y2 = (cords[i+1].z * scale) + 50;
                }
                else if(view == 2)
                {
                    x1 = (cords[i].y * scale) + 640;
                    x2 = (cords[i+1].y * scale) + 640;
                    y1 = (cords[i].z * scale) + 50;
                    y2 = (cords[i+1].z * scale) + 50;
                }
                else if(view == 3)
                {
                    x1 = (cords[i].x * scale) + 640;
                    x2 = (cords[i+1].x * scale) + 640;
                    y1 = (cords[i].y * scale) + 360;
                    y2 = (cords[i+1].y * scale) + 360;
                }
                if(x2>1200 || x2<80 || y2>640 || y2<50)
                    scale = scale * 0.9;
            }
            recalcScale = false;
        }



        SDL_Flip(screen);
    } // end main loop


    printf("Exited cleanly\n");
    return 0;
}


