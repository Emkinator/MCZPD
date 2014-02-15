#ifdef __APPLE__
    #include <SDL/SDL.h>
#else
    #include <SDL.h>
#endif
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>
#include <utility>
#include "SDL_gfxPrimitives.h"

#define range       36
#define gamma       0.80

using namespace std;

void convert(char * result, double x, bool align_left = true)
{ //its hard to find easy stock converters that could change the significant digit count
    const char * digits = "0123456789";
    const int len = 16;
    char * at = result + len - 2;

    int power = log10(x);
    if(x != x) {//NaN
        *at-- = 'N';
        *at-- = 'a';
        *at-- = 'N';
    }
    else if(power > 255 || power < -256) {
        *at-- = '0';
    }
    else {
        if(power > 3 || power < 0) {
            if(power < 0)
                power--;

            int temp = (power < 0) ? -power : power;
            do {
                *at-- = digits[temp % 10];
                temp /= 10;
            } while(temp);

            if(power < 0) {
                *at-- = '-';
                power++;
            }
            *at-- = 'e';

            x /= pow(10, power);
        }
        int decimal = int(x * 10) % 10;
        if(decimal) {
            *at-- = digits[decimal];
            *at-- = '.';
        }

        int temp = (x < 0) ? -x : x;
        do {
            *at-- = digits[temp % 10];
            temp /= 10;
        } while(temp);

        if(x < 0)
            *at-- = '-';
    }

    if(align_left) {
        int n = 0;
        at++;
        while(at < result + len - 1) {
            result[n++] = *at++;
        }
        for(; n < len - 1; n++) {
            result[n] = ' ';
        }
    }
    else {
        while(at >= result)
            *at-- = ' ';
    }
    result[len - 1] = 0;
}

bool ClickedIn(SDL_Event event, SDL_Rect bounds)
{
    return (event.button.x >= bounds.x && event.button.x < bounds.x + bounds.w &&
            event.button.y >= bounds.y && event.button.y < bounds.y +bounds.h &&
            event.button.button == SDL_BUTTON_LEFT);
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

void GradientLine(SDL_Surface* dst, float x1, float y1, float x2, float y2, int sc1, int sc2, int sc3, int ec1, int ec2, int ec3) //color ranges from 0 to 255, black to white
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    double dc1 = ec1 - sc1;
    double dc2 = ec2 - sc2;
    double dc3 = ec3 - sc3;
    int steps = max(abs(dx), abs(dy));
    if(!steps) steps = 1;

    double x = x1;
    double y = y1;
    double c1 = sc1;
    double c2 = sc2;
    double c3 = sc3;

    dx /= steps;
    dy /= steps;
    dc1 /= steps;
    dc2 /= steps;
    dc3 /= steps;

    for(int i = 0; i < steps; i++) {
        c1 += dc1;
        c2 += dc2;
        c3 += dc3;
        x += dx;
        y += dy;
        pixelRGBA(dst, x, y, c1, c2, c3, 255);
    }
}

Uint32 getpixel32(SDL_Surface *surface, int x, int y)
{
    Uint8 * p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    return *(Uint32 *)p;
}

vector<string> explode(const string& str, const char& ch) { //source: http://stackoverflow.com/questions/890164/how-can-i-split-a-string-by-a-delimiter-into-an-array
    string next;
    vector<string> result;

    for (string::const_iterator it = str.begin(); it != str.end(); it++) {
        if (*it == ch) {
            result.push_back(next);
            if (!next.empty()) {
                next.clear();
            }
        } else {
            next += *it;
        }
    }
    result.push_back(next);
    return result;
}
