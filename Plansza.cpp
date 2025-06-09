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
    highlightBorder{ false },
    pieceOnTile{ nullptr }{

}
Tile::Tile(const Tile &other)
    : mTexture(nullptr), // don't copy texture pointer directly
      mWidth(other.mWidth),
      mHeight(other.mHeight),
      hasPiece(other.hasPiece),
      isWhite(other.isWhite),
      highlightBorder(other.highlightBorder),
      BoardPosition(other.BoardPosition),
      pieceOnTile(nullptr) // fix later after deep-copy of pieces
{
    // texture should be reloaded later if needed
}
Tile::~Tile()
{
    //Clean up texture
    destroy();
}

bool Tile::loadFromFile( std::string path )
{
    //Clean up texture if it already exists
    destroy();

    //Load surface
    if( SDL_Surface* loadedSurface = IMG_Load( path.c_str() ); loadedSurface == nullptr )
    {
        SDL_Log( "Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
        //Create texture from surface
        if( mTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface ); mTexture == nullptr )
        {
            SDL_Log( "Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Clean up loaded surface
        SDL_DestroySurface( loadedSurface );
    }

    //Return success if texture loaded
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
