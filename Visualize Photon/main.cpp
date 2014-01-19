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
#include <iostream>
#include <cmath>
#include <ctime>
#include <utility>
#include "utility.h"
#include "io.h"
#include "SDL_gfxPrimitives.h"
#include "vmath.h"
#include "ReadConfigs.h"
#include "Structure.h"

#define range       36

using namespace std;

//#define abs(x) ((x ^ (x >> 31)) - (x >> 31))

double getintensity(double a, double b, int mode)
{
    if(b < 1e-60) return 0;
    switch(mode) {
        case 0:
            return a / b;
        case 1:
            return pow(a, b);//log2(0.05 + 0.95 * intensity/total_i) + 4.4));
    }
}

void BuildMap(SDL_Surface* grid, double*** spectrum, int colormap[][3], double max_i, int curve, int mode)
{
    for(int x = 0; x < grid->w; x++) {
        for(int y = 0; y < grid->h; y++) {
            float r = 255, g = 255, b = 255;
            double total_i = 0;
            for(int n = 0; n < range; n++)
                total_i += spectrum[x][y][n];
            if(mode < 2) {
                r = 0, g = 0, b = 0;
                for(int n = 0; n < range; n++) {
                    double temp = getintensity(spectrum[x][y][n], total_i, 0);
                    r += temp * colormap[n][0];
                    g += temp * colormap[n][1];
                    b += temp * colormap[n][2];
                }
            }
            if(mode > 0) {
                double temp = getintensity(total_i / max_i, 1.0 / curve, 1);
                r *= temp;
                g *= temp;
                b *= temp;
            }

            pixelRGBA(grid, x, y, r, g, b, 255);
        }
    }
    SDL_Flip(grid);
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
    int bounds[] = {1200, 700, 100}; //100 is just dummy for middle Z point
    SDL_Surface* screen = SDL_SetVideoMode(bounds[0], bounds[1], 32, SDL_HWSURFACE|SDL_DOUBLEBUF); //|SDL_FULLSCREEN);
    if (!screen) {
        printf("Unable to set %ix%i video: %s\n", bounds[0], bounds[1], SDL_GetError());
        return 1;
    }

    ConfigClass config((char*)"config.txt");
    InputClass in(&config);

    // read cords from file, assign to array "cords"
    int c = GetStepCount((char*)"simlog.txt");
    pcords *cords = new pcords[c];
    ReadCords(cords, c);

    int colormap[range][3];
    GenerateColorMap(colormap);

    int resolution = atof(config.GetValue(0,"resolution", -1).c_str());
    SDL_Surface *bitmap = SDL_CreateRGBSurface(SDL_HWSURFACE, resolution, resolution, 32, 0xff000000, 0xff0000, 0xff00, 0xff);
    double*** spectrum = new double**[resolution];
    for(int x = 0; x < resolution; x++) {
        spectrum[x] = new double*[resolution];
        for(int y = 0; y < resolution; y++) {
            spectrum[x][y] = new double[range];
        }
    }
    double max_intensity = 0;
    ReadMap(spectrum, max_intensity, resolution);
    SDL_ShowCursor(true);
    int curve = 10;

    Uint32 background = SDL_MapRGB(screen->format, 255, 255, 255);

    double scale = 10000;
    double newscale = scale;
    int lasttick = 0;
    int tick = 0;
    float degrees_per_second = 90.0;
    Vector3f xaxis(1, 0, 0);
    Vector3f yaxis(0, 1, 0);
    Vector3f zaxis(0, 0, 1);
    Vector3f p1(0, 0, 0);
    Vector3f p2(0, 0, 0);
    int n = 1;
    bool paused = false;

    int rectsize = resolution;
    while(rectsize * 2 < bounds[1] - 30) rectsize *= 2;

    SDL_Rect destination = {
        30,
        (bounds[1] - rectsize) / 2 + 30,
        rectsize,
        rectsize
    };
    SDL_Rect graphcords = {
        destination.x + destination.w + 30,
        destination.y,
        bounds[0] - (destination.x + destination.w + 30) - 30,
        destination.h
    };
    int intensity_mode = 0; //0 - just colors, 1 - both, 2 - just intensity
    bool map_updated = false;
    bool graph_updated = false;
    int sx = -1, sy = -1;

    SDL_Event event;

    bool done = false;
    int mode = 0;
    while (!done) {
        // message processing loop
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_KEYDOWN: {
                    if(mode == 0) {
                        switch(event.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                done = true;
                                break;
                            case SDLK_m:
                                mode = 1;
                                graph_updated = false;
                                map_updated = false;
                                SDL_FillRect(screen, 0, background);
                                continue;
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
                    }
                    else {
                        switch(event.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                done = true;
                                break;
                            case SDLK_i:
                                intensity_mode = (intensity_mode + 1) % 3;
                                map_updated = false;
                                break;
                            case SDLK_KP_MULTIPLY:
                                curve = min(curve + 1, 20);
                                map_updated = false;
                                break;
                            case SDLK_KP_DIVIDE:
                                curve = max(curve - 1, 1);
                                map_updated = false;
                                break;
                            case SDLK_m:
                                mode = 0;
                                SDL_FillRect(screen, 0, background);
                                graph_updated = true;
                                map_updated = true;
                                continue;
                                break;
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    if(mode == 1) {
                        if(event.button.x >= destination.x && event.button.x < destination.x + destination.w &&
                            event.button.y >= destination.y && event.button.y < destination.y + destination.h &&
                            event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_PRESSED
                        ) {
                            sx = int((event.button.x - destination.x) / (float)destination.w * resolution);
                            sy = int((event.button.y - destination.y) / (float)destination.h * resolution);
                            graph_updated = false;
                        }
                    }
                    break;
                }
            } // end switch
        } // end of message processing

        if(mode == 0) {
            SDL_FillRect(screen, 0, background);

            scale = newscale;

            lineRGBA(screen, 0, 50, bounds[0], 50, 0,0,0,255); // air/tissue boundary
            for(int i = 0; i < in.layerCount; i++) {
                int z = 50 + in.layers[i][1] * scale;
                lineRGBA(screen, 0, z, bounds[0], z, 0,0,0,255);
            }

            for(int i = 0; i<n; i++) {
                p1.x = cords[i].x * scale;
                p2.x = cords[i+1].x * scale;
                p1.z = cords[i].z * scale;
                p2.z = cords[i+1].z * scale;
                p1.y = cords[i].y * (scale/500) * 255;
                p2.y = cords[i+1].y * (scale/500) * 255;

                float x1 = xaxis.dotProduct(p1) + bounds[0] / 2;
                float x2 = xaxis.dotProduct(p2) + bounds[0] / 2;
                float y1 = zaxis.dotProduct(p1) + 50;
                float y2 = zaxis.dotProduct(p2) + 50;
                int c1 = min(180, max(0, (int)yaxis.dotProduct(p1) + 188));
                int c2 = min(180, max(0, (int)yaxis.dotProduct(p2) + 188));

                if(x2 > (bounds[0] - 40) || x2 < 40) {
                    newscale = min(newscale, abs(scale * ((bounds[0] - 40) / x2)));
                }

                if(y2 > (bounds[1] - 40) || y2 < 40){
                    newscale = min(newscale, abs(scale * ((bounds[1] - 40) / y2)));
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
        }
        else {
            if(!map_updated) {
                SDL_FillRect(screen, &destination, background);

                float step = rectsize / float(range - 1);
                int sx = destination.x - 1;
                int sy = destination.y - 30;
                for(int i = 0; i < range - 1; i++) {
                    for(int y = 0; y < 30; y++) {
                        GradientLine(screen, floor(sx + i * step), floor(sy + y), ceil(sx + (i + 1) * step), ceil(sy + y),
                            colormap[i][0], colormap[i][1], colormap[i][2],
                            colormap[i + 1][0], colormap[i + 1][1], colormap[i + 1][2]);
                    }
                }

                BuildMap(bitmap, spectrum, colormap, max_intensity, curve, intensity_mode);
                SDL_Surface* scaled = ScaleSurface(bitmap, rectsize, rectsize);
                SDL_BlitSurface(scaled, NULL, screen, &destination);
                SDL_Flip(screen);
                map_updated = true;
            }
            if(!graph_updated) {
                int x = graphcords.x;
                int y = graphcords.y + graphcords.h;
                int w = graphcords.w;
                int h = graphcords.h;

                if(sx != -1) {
                    SDL_FillRect(screen, &graphcords, background);

                    double total_i = 0;
                    for(int n = 0; n < range; n++)
                        if(spectrum[sx][sy][n] > total_i)
                            total_i = spectrum[sx][sy][n];

                    float ldy = getintensity(spectrum[sx][sy][0], total_i, 0) * h;
                    float dy = 0;
                    float step = w / (float)range;

                    for(int n = 0; n < range - 1; n++) {
                        float dy = getintensity(spectrum[sx][sy][n + 1], total_i, 0) * h;

                        GradientLine(screen, rint(x + step * n), rint(y - ldy), rint(x + step * (n + 1)), rint(y - dy),
                            colormap[n][0], colormap[n][1], colormap[n][2],
                            colormap[n + 1][0], colormap[n + 1][1], colormap[n + 1][2]);
                        ldy = dy;
                    }
                }

                GradientLine(screen, x, y + 2, x, y - h, 0, 0, 0, 0, 0, 0);
                GradientLine(screen, x - 1, y + 2, x - 1, y - h, 0, 0, 0, 0, 0, 0);
                GradientLine(screen, x, y, x + w, y, 0, 0, 0, 0, 0, 0);
                GradientLine(screen, x, y + 1, x + w, y + 1, 0, 0, 0, 0, 0, 0);
                SDL_Flip(screen);
                graph_updated = true;
            }
        }
    } // end main loop

    return 0;
}
