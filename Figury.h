#ifndef FIGURY_H
#define FIGURY_H
#include "SDL/include/SDL3/SDL.h"
#include "Plansza.h"
#include <vector>
#include <iostream>
using namespace std;
struct Ruch {
    mutable Piece* pieceMoved;
    mutable Piece* pieceCaptured;
    pair<int, int> from;
    pair<int, int> to;
    bool wasPromoted;
    int value;
    Ruch(Piece* moved, Piece* captured, pair<int, int> start, pair<int, int> end, bool promoted,int val)
          : pieceMoved(moved), pieceCaptured(captured), from(start), to(end), wasPromoted(promoted),value(val) {}
};
class Piece
{
public:
    Piece(int row = 0, int col = 0, bool isWhite = true, int type = 0,int indeks=0);

    ~Piece();

    bool loadFromFile(std::string path);

    void destroy();
    void handleEvent(SDL_Event* e, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces);

    void render(float x, float y);
    Piece(const Piece &other);
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

    int mWidth;
    int mHeight;
    bool White;
    int figure;
    bool hasMoved;
    bool isCaptured;
    bool wasPromoted;
    int ind;
    vector<pair<int, int>> possibleMoves;
    pair<int, int> BoardPosition{ 0, 0 };
};

#endif // FIGURY_H