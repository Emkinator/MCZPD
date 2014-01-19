#ifdef __APPLE__
    #include <SDL/SDL.h>
#else
    #include <SDL.h>
#endif
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include <utility>
#include "SDL_gfxPrimitives.h"

#define range       36
#define gamma       0.80

using namespace std;

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
