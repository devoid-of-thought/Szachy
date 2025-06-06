
#ifndef FIGURY_H
#define FIGURY_H
#include "SDL/include/SDL3/SDL.h"
#include "Plansza.h"
#include <vector>
#include <iostream>
using namespace std;
class Piece
{
public:
    //Initializes texture variables
    Piece(int row=0, int col=0,bool isWhite = true,int type = 0);

    //Cleans up texture variables
    ~Piece();

    //Loads texture from disk
    bool loadFromFile( std::string path );

    //Cleans up texture
    void destroy();
    void handleEvent( SDL_Event* e,Tile* Tiles[8][8], Piece* chessPieces[2][16] );
    //Draws texture
    void render( float x, float y );

    //Gets texture dimensions
    int getWidth();
    int getHeight();
    SDL_Texture* mTexture;

    void pawn(Tile* Tiles[8][8]);
    void Rook(Tile* Tiles[8][8]);
    void Knight(Tile* Tiles[8][8]);
    void Bishop(Tile* Tiles[8][8]);
    void Queen(Tile* Tiles[8][8]);
    void King(Tile* Tiles[8][8],Piece* chessPieces[2][16]);
    bool isChecked(int color, Tile* Tiles[8][8], Piece* chessPieces[2][16],pair<int, int> position);

    void resetPossibleMoves();
    void generatePossibleMoves(Tile *Tiles[8][8], Piece* chessPieces[2][16]);
    //Texture dimensions
    int mWidth;
    int mHeight;
    bool White;
    int figure;
    int hasMoved;
    vector<pair<int, int>> possibleMoves;
    pair<int, int> BoardPosition{ 0, 0 };
};
#endif //FIGURY_H
