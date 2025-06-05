#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "SDL/include/SDL3/SDL.h"
#include "Plansza.h"

#include <iostream>
#include "Window.h"

//LTexture Implementation
Tile::Tile():
    //Initialize texture variables
    mTexture{ nullptr },
    mWidth{ 0 },
    mHeight{ 0 },
    BoardPosition{0,0},
    hasPiece{ false },
    isWhite {false},
    highlightBorder{ false }
{

}

Tile::~Tile()
{
    //Clean up texture
    destroy();
}
bool Tile::load(SDL_Texture* Texture )
{
    //Clean up texture if it already exists
    destroy();
    mTexture = Texture;
    mWidth = Texture->w;
    mHeight = Texture->h;

    return mTexture != nullptr;
}

void Tile::destroy()
{
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;

    mWidth  = 0;
    mHeight = 0;
}

void Tile::render( float x, float y )
{
    //Set texture position
    SDL_FRect dstRect = { x, y, static_cast<float>( mWidth ), static_cast<float>( mHeight ) };

    //Render texture
    SDL_RenderTexture( gRenderer, mTexture, nullptr, &dstRect );

}
int Tile::getWidth()
{
    return mWidth;
}

int Tile::getHeight()
{
    return mHeight;
}
