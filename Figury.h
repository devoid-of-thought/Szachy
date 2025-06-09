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
    // Initializes texture variables
    Piece(int row = 0, int col = 0, bool isWhite = true, int type = 0,int indeks=0);

    // Cleans up texture variables
    ~Piece();

    // Loads texture from disk
    bool loadFromFile(std::string path);

    // Cleans up texture
    void destroy();
    void handleEvent(SDL_Event* e, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces);

    // Draws texture
    void render(float x, float y);
    Piece(const Piece &other);
    // Gets texture dimensions
    int getWidth();
    int getHeight();
    SDL_Texture* mTexture;
    void pawn(vector<vector<Tile>> &Tiles);
    void Rook(vector<vector<Tile>> &Tiles);
    void Knight(vector<vector<Tile>> &Tiles);
    void Bishop(vector<vector<Tile>> &Tiles);
    void Queen(vector<vector<Tile>> &Tiles);
    void King(vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces);
    bool isChecked(int color, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces, pair<int, int> position);

    void resetPossibleMoves();
    void generatePossibleMoves(vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces);

    // Texture dimensions
    int mWidth;
    int mHeight;
    bool White;
    int figure;
    int hasMoved;
    bool isCaptured;
    bool wasPromoted;
    int ind;
    vector<pair<int, int>> possibleMoves;
    pair<int, int> BoardPosition{ 0, 0 };
};

#endif // FIGURY_H