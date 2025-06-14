#include "Figury.h"
#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "SDL/include/SDL3/SDL.h"

#include <vector>
#include <iostream>
#include "Window.h"

//początkowe wartości figury
Piece::Piece(int row, int col, bool isWhite, int type,int indeks):
    mTexture{nullptr},
    mWidth{0},
    mHeight{0},
    BoardPosition{row, col},
    White{isWhite},
    figure{type},
    hasMoved{false},
    isCaptured{false},
    wasPromoted{false},
ind {indeks}

{
}
// konstruktor kopiujący
Piece::Piece(const Piece &other)
    : mWidth(other.mWidth),
      mHeight(other.mHeight),
      White(other.White),
      figure(other.figure),
      hasMoved(other.hasMoved),
      isCaptured(other.isCaptured),
      wasPromoted(other.wasPromoted),
      possibleMoves(other.possibleMoves),
      BoardPosition(other.BoardPosition),
    ind(other.ind)

      {
}
//destruktor
Piece::~Piece() {
    destroy();
}
//załadowanie tekstury z pliku
bool Piece::loadFromFile(std::string path) {
    destroy();

    //Load surface
    if (SDL_Surface *loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr) {
        SDL_Log("Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());
    } else {
        //Create texture from surface
        if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr) {
            SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Clean up loaded surface
        SDL_DestroySurface(loadedSurface);
    }

    //Return success if texture loaded
    return mTexture != nullptr;
}


void Piece::destroy() {
    // Clean up texture if it exists
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
    mWidth = 0;
    mHeight = 0;
}
//renderowanie figury
void Piece::render(float x, float y) {
    //Set texture position
    SDL_FRect dstRect = {x, y, static_cast<float>(mWidth), static_cast<float>(mHeight)};

    //Render texture
    SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
}
//zwrócenie szerokości i wysokości figury
int Piece::getWidth() {
    return mWidth;
}
int Piece::getHeight() {
    return mHeight;
}
//ruch pionka
void Piece::pawn(vector<vector<Tile>> &Tiles) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;

    if (!White) {
        if (y + 1 < 8 && !Tiles[y + 1][x].hasPiece) {
            possibleMoves.push_back({x, y + 1});
        }

        if (y == 1 && !Tiles[2][x].hasPiece && !Tiles[3][x].hasPiece) {
            possibleMoves.push_back({x, y + 2});
        }
        if (x - 1 >= 0 && y + 1 < 8 && Tiles[y + 1][x - 1].hasPiece && Tiles[y + 1][x - 1].isWhite) {
            possibleMoves.push_back({x - 1, y + 1});
        }

        if (x + 1 < 8 && y + 1 < 8 && Tiles[y + 1][x + 1].hasPiece && Tiles[y + 1][x + 1].isWhite) {
            possibleMoves.push_back({x + 1, y + 1});
        }
    } else {
        if (y - 1 >= 0 && !Tiles[y - 1][x].hasPiece) {
            possibleMoves.push_back({x, y - 1});
        }
        if (y == 6 && !Tiles[5][x].hasPiece && !Tiles[4][x].hasPiece) {
            possibleMoves.push_back({x, y - 2});
        }
        if (x - 1 >= 0 && y - 1 >= 0 && Tiles[y - 1][x - 1].hasPiece && !Tiles[y - 1][x - 1].isWhite) {
            possibleMoves.push_back({x - 1, y - 1});
        }
        if (x + 1 < 8 && y - 1 >= 0 && Tiles[y - 1][x + 1].hasPiece && !Tiles[y - 1][x + 1].isWhite) {
            possibleMoves.push_back({x + 1, y - 1});
        }
    }
}
//ruch wieży
void Piece::Rook(vector<vector<Tile>>& Tiles) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    for (int i = y - 1; i >= 0; --i) {
        if (Tiles[i][x].hasPiece) {
            if (Tiles[i][x].pieceOnTile->White != this->White) {
                possibleMoves.push_back({x, i});
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }
    for (int i = y + 1; i < 8; ++i) {
        if (Tiles[i][x].hasPiece) {
            if (Tiles[i][x].pieceOnTile->White != this->White) {
                possibleMoves.push_back({x, i});
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }
    for (int i = x + 1; i < 8; ++i) {
        if (Tiles[y][i].hasPiece) {
            if (Tiles[y][i].pieceOnTile->White != this->White) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }
    for (int i = x - 1; i >= 0; --i) {
        if (Tiles[y][i].hasPiece) {
            if (Tiles[y][i].pieceOnTile->White != this->White) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }
}
//ruch gońca
void Piece::Bishop(vector<vector<Tile>> &Tiles) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    for (int i = 1; i < 8; ++i) {
        if (y + i >= 8 || x + i >= 8) break;
        if (Tiles[y + i][x + i].hasPiece) {
            if (Tiles[y + i][x + i].pieceOnTile->White != White) {
                possibleMoves.push_back({x + i, y + i});
            }
            break;
        }
        possibleMoves.push_back({x + i, y + i});
    }
    for (int i = 1; i < 8; ++i) {
        if (y - i < 0 || x - i < 0) break;
        if (Tiles[y - i][x - i].hasPiece) {
            if (Tiles[y - i][x - i].pieceOnTile->White != White) {
                possibleMoves.push_back({x - i, y - i});
            }
            break;
        }
        possibleMoves.push_back({x - i, y - i});
    }
    for (int i = 1; i < 8; ++i) {
        if (y - i < 0 || x + i >= 8) break;
        if (Tiles[y - i][x + i].hasPiece) {
            if (Tiles[y - i][x + i].pieceOnTile->White != White) {
                possibleMoves.push_back({x + i, y - i});
            }
            break;
        }
        possibleMoves.push_back({x + i, y - i});
    }
    for (int i = 1; i < 8; ++i) {
        if (y + i >= 8 || x - i < 0) break;
        if (Tiles[y + i][x - i].hasPiece) {
            if (Tiles[y + i][x - i].pieceOnTile->White != White) {
                possibleMoves.push_back({x - i, y + i});
            }
            break;
        }
        possibleMoves.push_back({x - i, y + i});
    }
}
//ruch konia
void Piece::Knight(vector<vector<Tile>> &Tiles) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    int kon[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };
    for (int i = 0; i < 8; ++i) {
        int newX = x + kon[i][0];
        int newY = y + kon[i][1];
        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
            if (!Tiles[newY][newX].hasPiece || (Tiles[newY][newX].hasPiece && Tiles[newY][newX].pieceOnTile->White != White)) {
                possibleMoves.push_back({newX, newY});
            }
        }
    }
}
//ruch hetmana
void Piece::Queen(vector<vector<Tile>> &Tiles) {
    Rook(Tiles);
    Bishop(Tiles);
}
//ruch króla
void Piece::King(vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    int krol[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1},
        {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
    };

    for (int i = 0; i < 8; ++i) {
        int newX = x + krol[i][0];
        int newY = y + krol[i][1];
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            Tile &targetTile = Tiles[newY][newX];
            if (!targetTile.hasPiece || (targetTile.hasPiece && targetTile.pieceOnTile->White != White)) {
                possibleMoves.push_back({newX, newY});

            }
        }
    }
    //roszada
    if (hasMoved == 0) {
        int row = White ? 7 : 0;
        if (!Tiles[row][5].hasPiece && !Tiles[row][6].hasPiece) {
            Tile &rookTile = Tiles[row][7];
            if (rookTile.hasPiece && rookTile.pieceOnTile->figure == 500 && rookTile.pieceOnTile->hasMoved == 0) {
                possibleMoves.push_back({7, row});
            }
        }
        if (!Tiles[row][1].hasPiece && !Tiles[row][2].hasPiece && !Tiles[row][3].hasPiece) {
            Tile &rookTile = Tiles[row][0];
            if (rookTile.hasPiece && rookTile.pieceOnTile->figure == 500 && rookTile.pieceOnTile->hasMoved == 0) {
                possibleMoves.push_back({0, row});
            }
        }
    }
}
//generowanie możliwych ruchów figury
void Piece::generatePossibleMoves(vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    possibleMoves.clear();

    int fig = abs(figure);

    switch (fig) {
        case 100:
            pawn(Tiles);
            break;
        case 500:
            Rook(Tiles);
            break;
        case 350:
            Knight(Tiles);
            break;
        case 351:
            Bishop(Tiles);
            break;
        case 900:
            Queen(Tiles);
            break;
        case 999:
            King(Tiles,chessPieces);
            break;
    }
}
