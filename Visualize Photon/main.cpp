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
#include <vector>
#include <sstream>
#include <utility>
#include "ReadConfig.h"
#include "Structs.h"
#include "SDL_gfxPrimitives.h"
#include "vmath.h"

#define resolution  256
#define range       36
#define gamma       0.80

using namespace std;

struct pcords {
    double x;
    double y;
    double z;
};

//#define abs(x) ((x ^ (x >> 31)) - (x >> 31))
//#define interpolate(start, finish, progress) (start + progress * (finish - start))

vector<string> explode(const string& str, const char& ch);
SDL_Surface *ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height);
void putpixel32(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel32(SDL_Surface *surface, int x, int y);


void GradientLine(SDL_Surface* dst, float x1, float y1, float x2, float y2, int sc1, int sc2, int sc3, int ec1, int ec2, int ec3) //color ranges from 0 to 255, black to white
{
    float dx = x2-x1;
    float dy = y2-y1;
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
    if(!steps) steps = 1;

    float x = x1;
    float y = y1;
    float c1 = sc1;
    float c2 = sc2;
    float c3 = sc3;

    float ax = dx / steps;
    float ay = dy / steps;
    float ac1 = (ec1 - sc1) / steps;
    float ac2 = (ec2 - sc2) / steps;
    float ac3 = (ec3 - sc3) / steps;

    for(int i = 0; i < steps; i++) {
        x += ax;
        y += ay;
        c1 += ac1;
        c2 += ac2;
        c3 += ac3;
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

void ReadCords(pcords* cords, int c)
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

void ReadMap(double*** spectrum, double &max_intensity)
{
    int i = 0;
    ifstream file;
    file.open("grid.csv");
    string line;
    getline(file, line);
    int max_line;

    while(file.good() && i < resolution*resolution) {
        getline(file, line);
        int n = 0;
        int x = i / resolution;
        int y = i % resolution;
        vector<string> result = explode(line, ',');

        for (size_t j = 0; j < result.size(); j++) {

            spectrum[x][y][n] = atof(result[j].c_str());

            if(spectrum[x][y][n] > max_intensity) {
                max_intensity = spectrum[x][y][n];
            }
            n++;
        }
        i++;
    }

    file.close();
}

double getintensity(double intensity, double max_intensity)
{
    if(max_intensity < 1e-60) return 0;
    return intensity / max_intensity;
}


void BuildMap(SDL_Surface* grid, double*** spectrum, int colormap[][3], double max_intensity)
{
    for(int x = 0; x < resolution; x++) {
        for(int y = 0; y < resolution; y++) {
            int r = 0, g = 0, b = 0;
            double totalintensity = 0;
            for(int n = 0; n < range; n++)
                totalintensity += spectrum[x][y][n];
            for(int n = 0; n < range; n++) {
                r += getintensity(spectrum[x][y][n], totalintensity) * colormap[n][0];
                g += getintensity(spectrum[x][y][n], totalintensity) * colormap[n][1];
                b += getintensity(spectrum[x][y][n], totalintensity) * colormap[n][2];
            }

            pixelRGBA(grid, x, y, r, g, b, 255);
        }
    }
    SDL_Flip(grid);
}

int Adjust(double color, double factor)
{
    if(color > 0)
        return rint(255 * pow(color * factor, gamma));
    return 0;
}

void GetColor(int f, int n, int colormap[][3]) //algorythm source: http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
{
    double red = 0;
    double green = 0;
    double blue = 0;
    double factor = 0;
    int step = range * 10 / 6;
    if(f < 440) {
        red = -(f - 440.0) / (440 - 380);
        green = 0;
        blue = 1;
    }
    else if(f < 490) {
        red = 0;
        green = (f - 440.0) / (490 - 440);
        blue = 1;
    }
    else if(f < 510) {
        red = 0;
        green = 1;
        blue = -(f - 510.0) / (510 - 490);
    }
    else if(f < 580) {
        red = (f - 510.0) / (580 - 510);
        green = 1;
        blue = 0;
    }
    else if(f < 645) {
        red = 1;
        green = -(f - 645.0) / (645 - 580);
        blue = 0;
    }
    else {
        red = 1;
        green = 0;
        blue = 0;
    }

    if(f < 420) {
        factor = 0.3 + 0.7 * (f - 380.0) / (420 - 380);
    }
    else if(f < 700) {
        factor = 1;
    }
    else {
        factor = 0.3 + 0.7 * (780.0 - f) / (780 - 700);
    }

    colormap[n][0] = Adjust(red, factor);
    colormap[n][1] = Adjust(green, factor);
    colormap[n][2] = Adjust(blue, factor);
}

void GenerateColorMap(int colormap[range][3])
{
    for(int n = 0; n < range; n++) {
        int f = 400 + n * 10;
        GetColor(f, n, colormap);
    }
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
    int bounds[] = {600, 600, 100}; //100 is just dummy for middle Z point
    SDL_Surface* screen = SDL_SetVideoMode(bounds[0], bounds[1], 32, SDL_HWSURFACE|SDL_DOUBLEBUF); //|SDL_FULLSCREEN);
    if (!screen) {
        printf("Unable to set %ix%i video: %s\n", bounds[0], bounds[1], SDL_GetError());
        return 1;
    }

    MC::InputClass in;

    // read cords from file, assign to array "cords"
    int c = GetStepCount((char*)"simlog.txt");
    pcords *cords = new pcords[c];
    ReadCords(cords, c);

    /*int** bitmap = unsigned new int**[resolution];
    for(int x = 0; x < resolution; x++) {
        bitmap[x] = unsigned new int*[resolution];
        for(int y = 0; y < resolution; y++) {
            bitmap[x][y] = unsigned new int[3];
            bitmap[x][y][0] = 0;
            bitmap[x][y][1] = 0;
            bitmap[x][y][2] = 0;
        }
    }*/
    int colormap[range][3];
    GenerateColorMap(colormap);

    SDL_Surface *bitmap = SDL_CreateRGBSurface(SDL_HWSURFACE, resolution, resolution, 32, 0xff000000, 0xff0000, 0xff00, 0xff);
    double max_intensity;
    double*** spectrum = new double**[resolution];
    for(int x = 0; x < resolution; x++) {
        spectrum[x] = new double*[resolution];
        for(int y = 0; y < resolution; y++) {
            spectrum[x][y] = new double[range];
        }
    }
    ReadMap(spectrum, max_intensity);
    BuildMap(bitmap, spectrum, colormap, max_intensity);

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
        (bounds[0] - rectsize) / 2,
        (bounds[1] - rectsize) / 2 + 30,
        rectsize,
        rectsize
    };
    bool updated = false;

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
                                updated = false;
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
                            case SDLK_m:
                                mode = 0;
                                updated = true;
                                continue;
                                break;
                        }
                    }
                    break;
                }
            } // end switch
        } // end of message processing

        if(mode == 0) {
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

            scale = newscale;

            lineRGBA(screen, 0, 50, bounds[0], 50, 255,255,255,255); // air/tissue boundary
            for(int i = 0; i < in.layerCount; i++) {
                int z = 50 + in.layers[i][1] * scale;
                lineRGBA(screen, 0, z, bounds[0], z, 255,255,255,255);
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
                int c1 = min(255, max(80, (int)yaxis.dotProduct(p1) + 188));
                int c2 = min(255, max(80, (int)yaxis.dotProduct(p2) + 188));

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
            if(!updated) {
                SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
                int step = rectsize / (range - 1);
                int sx = destination.x;
                int sy = destination.y - 30;
                for(int i = 0; i < range - 1; i++) {
                    for(int y = 0; y < 30; y++) {
                        GradientLine(screen, sx + i * step, sy + y, sx + (i + 1) * step, sy + y,
                            colormap[i][0], colormap[i][1], colormap[i][2],
                            colormap[i + 1][0], colormap[i + 1][1], colormap[i + 1][2]);
                    }
                }
                SDL_Surface* scaled = ScaleSurface(bitmap, rectsize, rectsize);
                SDL_BlitSurface(scaled, NULL, screen, &destination);
                SDL_Flip(screen);
                updated = true;
            }
        }
    } // end main loop

    return 0;
}

Uint32 getpixel32(SDL_Surface *surface, int x, int y)
{ //source: http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html#GUIDEVIDEOINTRO
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    return *(Uint32 *)p;
}

void putpixel32(SDL_Surface *surface, int x, int y, Uint32 pixel)
{ //source: http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html#GUIDEVIDEOINTRO
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32 *)p = pixel;
}

SDL_Surface *ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{ //source: http://www.sdltutorials.com/sdl-scale-surface
    SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
        Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);
    double _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w));
    double _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));

    for(Sint32 y = 0; y < Surface->h; y++)
        for(Sint32 x = 0; x < Surface->w; x++)
            for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
                for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
                    putpixel32(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x,
                        static_cast<Sint32>(_stretch_factor_y * y) + o_y, getpixel32(Surface, x, y));

    return _ret;
}

vector<string> explode(const string& str, const char& ch) { //source: http://stackoverflow.com/questions/890164/how-can-i-split-a-string-by-a-delimiter-into-an-array
    string next;
    vector<string> result;

    for (string::const_iterator it = str.begin(); it != str.end(); it++) {
        if (*it == ch) {
            if (!next.empty()) {
                result.push_back(next);
                next.clear();
            }
        } else {
            next += *it;
        }
    }
    if (!next.empty())
         result.push_back(next);
    return result;
}
