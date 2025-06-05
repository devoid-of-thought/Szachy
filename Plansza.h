    #include "SDL_image/include/SDL3_image/SDL_image.h"
    #include "SDL/include/SDL3/SDL.h"

    #include <vector>
    #include <iostream>
    #ifndef PLANSZA_H
    #define PLANSZA_H
    using namespace std;
    class Tile
    {
    public:
        //Initializes texture variables
        Tile();

        //Cleans up texture variables
        ~Tile();


        //Loads texture from disk
        bool loadFromFile( std::string path );

        //Cleans up texture
        void destroy();
        void render( float x, float y );

        //Gets texture dimensions
        int getWidth();
        int getHeight();
        SDL_Texture* mTexture;

        //Texture dimensions
        int mWidth;
        int mHeight;
        bool hasPiece;
        bool isWhite;
        bool highlightBorder;

        pair<int, int> BoardPosition;
    };
    #endif //PLANSZA_H
