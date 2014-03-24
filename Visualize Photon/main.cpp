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
#include <utility>
#include <string>
#include "utility.h"
#include "io.h"
#include "SDL_gfxPrimitives.h"
#include "vmath.h"
#include "ReadConfigs.h"
#include "Structure.h"

#define range       36
#define dbug(var) << #var": " << (var)
#define out(var) (var) << " "
#define RGBtoColor(r,g,b) ((static_cast<int>(r) << 16) | (static_cast<int>(g) << 8) | (static_cast<int>(b)) | 0xff000000)
#define saturate(color, mul) (color + mul * (255 - color))
using namespace std;
//#define abs(x) ((x ^ (x >> 31)) - (x >> 31))

double getintensity2(double a, double exp)
{
    return pow(a, exp);//log2(0.05 + 0.95 * intensity/total_i) + 4.4));
}

void BuildMap(SDL_Surface* bitmap, double***** spectrum, double** intensity, int resolution, double max_i, int colormap[][3],
    int mode, int curve, int range_low, int range_high, int zoom, int res_zoom, int max_zoom, int layer)
{
    SDL_Rect pixel;
    int factor = bitmap->w / (resolution >> (res_zoom + zoom));
    int count = bitmap->w / factor * (1 << zoom);
    int start = (count - (count >> zoom)) / 2;
    int stop = start + bitmap->w / factor;
    for(int x = start; x < stop; x++) {
        for(int y = start; y < stop; y++) {
            float r = 255, g = 255, b = 255;
            double total_i = 0;
            if(mode < 2) {
                r = 0, g = 0, b = 0;
                for(int n = range_low; n <= range_high; n++) {
                    double temp = spectrum[layer][res_zoom][x][y][n] / intensity[x][y];
                    r += temp * colormap[n][0];
                    g += temp * colormap[n][1];
                    b += temp * colormap[n][2];
                }
            }
            if(mode > 0) {
                double temp = getintensity2(intensity[x][y] / max_i, 1.0 / curve);
                r *= temp;
                g *= temp;
                b *= temp;
            }

            int px = x - start;
            int py = y - start;
            if(factor == 1) {
                pixelRGBA(bitmap, px, py, r, g, b, 255);
            }
            else {
                pixel.x = px * factor;
                pixel.y = py * factor;
                pixel.w = pixel.h = factor;
                SDL_FillRect(bitmap, &pixel, (static_cast<Uint32>(r) << 24)
                    | (static_cast<Uint32>(g) << 16) | (static_cast<Uint32>(b) << 8) | 255);
            }
        }
    }
    SDL_Flip(bitmap);
}

int main (int argc, char** argv)
{
    freopen("CON", "wt", stdout);
    // initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    int bounds[] = {1200, 720, 100}; //100 is just dummy for middle Z point
    SDL_Surface* screen = SDL_SetVideoMode(bounds[0], bounds[1], 32, SDL_HWSURFACE|SDL_DOUBLEBUF); //|SDL_FULLSCREEN);
    if (!screen) {
        printf("Unable to set %ix%i video: %s\n", bounds[0], bounds[1], SDL_GetError());
        return 1;
    }

    ConfigClass config("../Monte Carlo/config.txt");
    InputClass in(&config);

    // read cords from file, assign to array "cords"
    int c = GetStepCount("simlog.txt");
    pcords *cords = new pcords[c];
    ReadCords(cords, c);

    int colormap[range][3];
    GenerateColorMap(colormap);
    int resolution = 0;
    int max_layers = 1;
    int layer = 0;
    double photoncount = 0;
    double specular = 0;
    double units = 0;
    GetData(resolution, max_layers, photoncount, specular, units);
    units = 1 / units;
    double photonmass = photoncount * (1.0 - specular);
    int res_levels = ceil(log2(resolution) + 1);
    int res_zoom = 0;
    int zoom = 0;

    double***** spectrum = new double****[max_layers];

    for(int l = 0; l < max_layers; l++) {
        spectrum[l] = new double ***[res_levels];
        for(int n = 0; n < res_levels; n++) {
            int res = resolution >> n;
            spectrum[l][n] = new double**[res];
            for(int x = 0; x < res; x++) {
                spectrum[l][n][x] = new double*[res];
                for(int y = 0; y < res; y++) {
                    spectrum[l][n][x][y] = new double[range];
                }
            }
        }
    }

    double** intensity = new double*[resolution];
    for(int x = 0; x < resolution; x++) {
        intensity[x] = new double[resolution];
    }

    double max_intensity = 0;
    ReadMap(spectrum, resolution, max_layers);
    SDL_ShowCursor(true);
    int curve = 10;
    Uint32 background = 0xffffffff;

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
    int skin_color[3] = {187, 135, 111};
    int n = 1;
    bool paused = false;

    int rectsize = resolution;
    while(rectsize * 2 < bounds[1] - 30) rectsize *= 2;

    SDL_Surface *bitmap = SDL_CreateRGBSurface(SDL_HWSURFACE, rectsize, rectsize, 32, 0xff000000, 0xff0000, 0xff00, 0xff);
    SDL_FillRect(screen, 0, background);
    SDL_Rect mapcords = {
        30,
        (bounds[1] - rectsize) / 2,
        rectsize,
        rectsize
    };
    SDL_Rect selectorcords = {
        mapcords.x - 1,
        bounds[1] - ((bounds[1] - (mapcords.y + mapcords.h + 30)) / 2) - 30,
        mapcords.w,
        30
    };
    SDL_Rect graphcords = {
        mapcords.x + mapcords.w,
        mapcords.y - 30,
        bounds[0] - (mapcords.x + mapcords.w + 30) - 90 + 90,
        mapcords.h - 1 + 60
    };
    int intensity_mode = 0; //0 - just colors, 1 - both, 2 - just intensity
    bool map_updated = false;
    bool graph_updated = false;
    bool selector_updated = false;
    bool selecting = false;
    bool flipped = false;
    int sx = -1, sy = -1;
    int range_low = 0;
    int range_high = range - 1;

    SDL_Event event;

    bool done = false;
    int mode = 1;
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
                                selector_updated = false;
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
                            case SDLK_LEFTBRACKET:
                                if(res_zoom > 0) res_zoom--;
                                map_updated = false;
                                flipped = false;
                                break;
                            case SDLK_RIGHTBRACKET:
                                if(res_zoom < res_levels - 1) res_zoom++;
                                if(res_zoom + zoom >= res_levels) zoom--;
                                map_updated = false;
                                flipped = false;
                                break;
                            case SDLK_MINUS:
                                if(zoom > 0) zoom--;
                                map_updated = false;
                                flipped = false;
                                break;
                            case SDLK_EQUALS:
                                if(zoom + res_zoom < res_levels - 1) zoom++;
                                map_updated = false;
                                flipped = false;
                                break;
                            case SDLK_PERIOD:
                                if(layer < max_layers - 1) layer++;
                                map_updated = false;
                                graph_updated = false;
                                flipped = false;
                                break;
                            case SDLK_COMMA:
                                if(layer > 0) layer--;
                                map_updated = false;
                                graph_updated = false;
                                flipped = false;
                                break;
                            case SDLK_m:
                                mode = 0;
                                SDL_FillRect(screen, 0, background);
                                graph_updated = true;
                                map_updated = true;
                                selector_updated = true;
                                selecting = false;
                                flipped = false;
                                continue;
                                break;
                            case SDLK_r:
                                ReadMap(spectrum, resolution, max_layers);
                                map_updated = false;
                                break;
                            case SDLK_f:
                                flipped = !flipped;
                                if(flipped) {
                                    layer = 0;
                                    zoom = 0;
                                    res_zoom = res_levels - 1;
                                }
                                graph_updated = false;
                                map_updated = false;
                                break;
                            case SDLK_e:
                                ExportIntensityGraph(spectrum, range_low, range_high, resolution, res_levels, layer);
                                break;
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    if(mode == 1) {
                        if(ClickedIn(event, mapcords)) {
                            sx = (event.button.x - mapcords.x) / (float)mapcords.w * resolution;
                            sy = (event.button.y - mapcords.y) / (float)mapcords.h * resolution;
                            graph_updated = false;
                        }
                        else if(ClickedIn(event, selectorcords)) {
                            int x = floor((event.button.x - selectorcords.x) / (float)selectorcords.w * (range - 1));
                            selecting = true;
                            range_low = range_high = x;
                            selector_updated = false;
                            map_updated = false;
                            graph_updated = false;
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    if(mode == 1) {
                        if(ClickedIn(event, selectorcords)) {
                            selecting = false;
                        }
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    if(mode == 1) {
                        if(selecting && event.motion.y > selectorcords.y
                           && event.motion.y < selectorcords.y + selectorcords.h
                        ) {
                            int x = floor((event.button.x - selectorcords.x) / (float)selectorcords.w * (range - 1));
                            if(x >= 0 && x < range) {
                                if(range_low > x)
                                    range_low = x;
                                if(range_high < x)
                                    range_high = x;
                                selector_updated = false;
                                map_updated = false;
                                graph_updated = false;
                            }
                        }
                    }
                    break;
                }
            } // end switch
        } // end of message processing

        if(mode == 0) {
            SDL_FillRect(screen, 0, background);

            scale = newscale;

            SDL_Rect filler = {
                0,
                47,
                bounds[0],
                3
            };
            SDL_FillRect(screen, &filler, 0xff000000); // air/tissue boundary
            for(int i = 0; i < in.layerCount; i++) {
                int z = 50 + in.layers[i][1] * scale;
                float modifier = 1.0 - float(i + 1) / in.layerCount;
                filler.y += filler.h;
                filler.h = z - filler.y;
                SDL_FillRect(screen, &filler, RGBtoColor(saturate(skin_color[0], modifier),
                        saturate(skin_color[1], modifier), saturate(skin_color[2], modifier)));
            }
            filler.y += filler.h;
            filler.h = bounds[1] - filler.y;
            SDL_FillRect(screen, &filler, RGBtoColor(skin_color[0], skin_color[1], skin_color[2]));

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
                int c1 = min(120, max(0, (int)yaxis.dotProduct(p1) + 128));
                int c2 = min(120, max(0, (int)yaxis.dotProduct(p2) + 128));

                if(x2 > (bounds[0] - 40) || x2 < 40) {
                    newscale = min(newscale, abs(scale * ((bounds[0] - 40) / x2)));
                }

                if(y2 > (bounds[1] - 40) || y2 < 40) {
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
            bool needs_flip = false;
            if(!selector_updated) {
                int sx = selectorcords.x;
                int sy = selectorcords.y;
                float step = rectsize / float(range - 1);
                for(int i = 0; i < range - 1; i++) {
                    float shade = (i >= range_low && i <= range_high) ? 1.0 : 0.5;
                    for(int y = 0; y < 30; y++) {
                        GradientLine(screen, floor(sx + i * step), floor(sy + y), ceil(sx + (i + 1) * step), ceil(sy + y),
                            colormap[i][0] * shade, colormap[i][1] * shade, colormap[i][2] * shade,
                            colormap[i + 1][0] * shade, colormap[i + 1][1] * shade, colormap[i + 1][2] * shade);
                    }
                }
                needs_flip = true;
                selector_updated = true;
            }
            if(!map_updated) {
                SDL_FillRect(screen, &mapcords, background);
                GetIntensity(spectrum, intensity, max_intensity, range_low, range_high,
                    resolution, res_zoom, res_levels, layer, max_layers);
                BuildMap(bitmap, spectrum, intensity, resolution, max_intensity, colormap, intensity_mode,
                    curve, range_low, range_high, zoom, res_zoom, res_levels, layer);
                SDL_BlitSurface(bitmap, NULL, screen, &mapcords);
                needs_flip = true;
                map_updated = true;
            }
            if(!graph_updated) {
                int x = graphcords.x + 60;
                int y = (graphcords.y + 30) + (graphcords.h - 60);
                int w = graphcords.w - 90;
                int h = graphcords.h - 60;

                if(sx != -1) {
                    SDL_FillRect(screen, &graphcords, background);

                    int count = min(range - 1, range_high - range_low + 1);
                    float step = (w - 1) / float(count);

                    int factor = bitmap->w / (resolution >> (res_zoom + zoom));
                    int size = bitmap->w / factor * (1 << zoom);
                    int start = (size - (size >> zoom)) / 2;
                    int cx = start + sx / factor;
                    int cy = start + sy / factor;
                    char str[16];

                    double max_i = 0;
                    double min_i = 0;
                    for(int n = range_low; n <= range_low + count; n++) {
                        if(spectrum[layer][res_zoom][cx][cy][n] > max_i)
                            max_i = spectrum[layer][res_zoom][cx][cy][n];
                        else if(spectrum[layer][res_zoom][cx][cy][n] < min_i)
                            min_i = spectrum[layer][res_zoom][cx][cy][n];
                    }
                    if(max_i == 0) {
                        max_i = 1e-60;
                    }

                    int max_exp = ceil(log10(max_i));
                    double nearest;
                    if(flipped) {
                        max_i = photonmass / range;
                    }
                    else {
                        min_i = 0;
                        nearest = pow(10.0, max_exp);
                        max_i = ceil(max_i / nearest * 10) * nearest / 10;
                    }

                    float ldy = spectrum[layer][res_zoom][cx][cy][range_low] / max_i;

                    if(flipped)
                        ldy = 1.0 - ldy;
                    float dy = 0;

                    int steps;
                    if(flipped) {
                        steps = 10;
                    }
                    else {
                        steps = round(max_i / nearest * 10);
                        while(steps * 2 < 10)
                            steps *= 2;
                    }

                    for(int n = 0; n <= steps; n++) {
                        double value;
                        if(flipped) {
                            value = 1.0 * n / steps * 1.0000000001;
                        }
                        else {
                            value = (max_i * n / steps + min_i) * 1.0000001;
                        }
                        hlineColor(screen, x, x + w, y - (h * (n / float(steps))), 0xff);
                        convert(str, value, false);
                        stringColor(screen, x - 120, y - (h * (n / float(steps))), str, 0xff);
                    }

                    for(int n = 0; n < count; n++) {
                        int id = n + range_low;

                        dy = spectrum[layer][res_zoom][cx][cy][id] / max_i;

                        if(flipped)
                            dy = 1.0 - dy;

                        aalineRGBA(screen, x + step * n, y - ldy * h, x + step * (n + 1), y - dy * h,
                            colormap[id][0], colormap[id][1], colormap[id][2], 255);
                        ldy = dy;

                        vlineColor(screen, x + step * n, y - 1, y + 2, 0xff);
                        if(count < 15 || (n & 1) == 0) {
                            convert(str, 400 + id * 10);
                            stringColor(screen, x + step * n, y + 5, str, 0xff);
                        }
                    }
                }
                rectangleColor(screen, x, y, x + w, y + 1, 0xff);
                rectangleColor(screen, x, y - h, x + 1, y, 0xff);
                needs_flip = true;
                graph_updated = true;
            }

            if(needs_flip)
                SDL_Flip(screen);
        }
    } // end main loop

    return 0;
}
