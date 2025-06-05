//
// Created by userbrigh on 5/28/25.
//
#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "SDL/include/SDL3/SDL.h"

#include <vector>
#include <iostream>
#include "Plansza.h"
#include "Figury.h"

#ifndef WINDOW_H
#define WINDOW_H

extern SDL_Window* gWindow;
extern Piece* selectedPiece;
extern SDL_Renderer* gRenderer;

extern SDL_Surface* gScreenSurface;



#endif //WINDOW_H
