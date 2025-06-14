
    #ifndef PLANSZA_H
    #define PLANSZA_H
#include "SDL/include/SDL3/SDL.h"
#include <vector>
#include <iostream>
    using namespace std;
    class Piece;
    class Tile
    {
    public:
        Tile();
        Tile(const Tile &other);
        ~Tile();

        bool loadFromFile( std::string path );

        void destroy();
        void render( float x, float y );

        int getWidth();
        int getHeight();
        SDL_Texture* mTexture;

        int mWidth;
        int mHeight;
        bool hasPiece;
        bool isWhite;
        Piece* pieceOnTile;

        pair<int, int> BoardPosition;

    };
    #endif //PLANSZA_H
