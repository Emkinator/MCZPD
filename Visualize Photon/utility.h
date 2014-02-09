#ifndef  UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

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


using namespace std;

string convert(double x, bool align_left = true);
bool ClickedIn(SDL_Event event, SDL_Rect bounds);
int Adjust(double color, double factor);
void GetColor(int f, int n, int colormap[][3]);
void GenerateColorMap(int colormap[][3]);
vector<string> explode(const string& str, const char& ch);
Uint32 getpixel32(SDL_Surface *surface, int x, int y);
void GradientLine(SDL_Surface* dst, float x1, float y1, float x2, float y2, int sc1, int sc2, int sc3, int ec1, int ec2, int ec3);


#endif // UTILITY_H_INCLUDED
