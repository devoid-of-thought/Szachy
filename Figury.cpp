#include "Figury.h"
#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "SDL/include/SDL3/SDL.h"

#include <vector>
#include <iostream>
#include "Window.h"


//LTexture Implementation
Piece::Piece(int row, int col, bool isWhite, int type,int indeks):
    //Initialize texture variables
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

Piece::~Piece() {
    //C
    destroy();
}

bool Piece::loadFromFile(std::string path) {
    //Clean up texture if it already exists
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

void Piece::handleEvent(SDL_Event *e, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN && e->button.button == SDL_BUTTON_LEFT) {
        int mouseX = e->button.x;
        int mouseY = e->button.y;

        int tileSize = 64;
        int pieceX = BoardPosition.first * tileSize;
        int pieceY = BoardPosition.second * tileSize;

        // Check if the mouse click is within this piece's rectangle
        if (mouseX >= pieceX && mouseX < pieceX + tileSize &&
            mouseY >= pieceY && mouseY < pieceY + tileSize) {
            if (White) {
                if (selectedPiece != nullptr &&
                    ((selectedPiece->figure == 500 && figure == 999) ||
                     (selectedPiece->figure == 999 && figure == 500))) {
                    return;
                }
                selectedPiece = this;
                generatePossibleMoves(Tiles,chessPieces);
            }
        }
    }
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
void Piece::render(float x, float y) {
    //Set texture position
    SDL_FRect dstRect = {x, y, static_cast<float>(mWidth), static_cast<float>(mHeight)};

    //Render texture
    SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
}

int Piece::getWidth() {
    return mWidth;
}

int Piece::getHeight() {
    return mHeight;
}

void Piece::pawn(vector<vector<Tile>> &Tiles) {
    int x = BoardPosition.first; // Column
    int y = BoardPosition.second; // Row

    if (!White) {
        // Black pawn (moves down the board, y+1)
        if (y + 1 < 8 && !Tiles[y + 1][x].hasPiece) {
            possibleMoves.push_back({x, y + 1});
        }

        if (y == 1 && !Tiles[2][x].hasPiece && !Tiles[3][x].hasPiece) {
            possibleMoves.push_back({x, y + 2});
        }

        // Captures
        if (x - 1 >= 0 && y + 1 < 8 && Tiles[y + 1][x - 1].hasPiece && Tiles[y + 1][x - 1].isWhite) {
            possibleMoves.push_back({x - 1, y + 1});
        }

        if (x + 1 < 8 && y + 1 < 8 && Tiles[y + 1][x + 1].hasPiece && Tiles[y + 1][x + 1].isWhite) {
            possibleMoves.push_back({x + 1, y + 1});
        }
    } else {
        // White pawn (moves up the board, y-1)
        if (y - 1 >= 0 && !Tiles[y - 1][x].hasPiece) {
            possibleMoves.push_back({x, y - 1});
        }

        if (y == 6 && !Tiles[5][x].hasPiece && !Tiles[4][x].hasPiece) {
            possibleMoves.push_back({x, y - 2});
        }

        // Captures
        if (x - 1 >= 0 && y - 1 >= 0 && Tiles[y - 1][x - 1].hasPiece && !Tiles[y - 1][x - 1].isWhite) {
            possibleMoves.push_back({x - 1, y - 1});
        }

        if (x + 1 < 8 && y - 1 >= 0 && Tiles[y - 1][x + 1].hasPiece && !Tiles[y - 1][x + 1].isWhite) {
            possibleMoves.push_back({x + 1, y - 1});
        }
    }
}

// This is the corrected Rook move generation function.
void Piece::Rook(vector<vector<Tile>>& Tiles) {
    // Get the current position for easier access.
    // BoardPosition.first is the column (x)
    // BoardPosition.second is the row (y)
    int x = BoardPosition.first;
    int y = BoardPosition.second;

    // Clear any moves from a previous turn before generating new ones.
    // (This is a good practice to add if it's not already in your generatePossibleMoves function)
    // possibleMoves.clear();

    // --- Scan UP (decreasing y, since row 0 is at the top) ---
    for (int i = y - 1; i >= 0; --i) {
        if (Tiles[i][x].hasPiece) {
            // If the piece is an opponent, we can capture it.
            if (Tiles[i][x].pieceOnTile->White != this->White) {
                possibleMoves.push_back({x, i});
            }
            // Stop scanning in this direction, as the path is blocked.
            break;
        }
        // The square is empty, so it's a valid move.
        possibleMoves.push_back({x, i});
    }

    // --- Scan DOWN (increasing y) ---
    for (int i = y + 1; i < 8; ++i) {
        if (Tiles[i][x].hasPiece) {
            if (Tiles[i][x].pieceOnTile->White != this->White) {
                possibleMoves.push_back({x, i});
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }

    // --- Scan RIGHT (increasing x) ---
    for (int i = x + 1; i < 8; ++i) {
        if (Tiles[y][i].hasPiece) {
            if (Tiles[y][i].pieceOnTile->White != this->White) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }

    // --- Scan LEFT (decreasing x) ---  // THIS BLOCK IS THE CORRECTED ONE
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
void Piece::Bishop(vector<vector<Tile>> &Tiles) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;

    // Down-right
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

    // Up-left
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

    // Up-right
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

    // Down-left
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
void Piece::Knight(vector<vector<Tile>> &Tiles) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    int knightMoves[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };
    for (int i = 0; i < 8; ++i) {
        int newX = x + knightMoves[i][0];
        int newY = y + knightMoves[i][1];
        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
            if (!Tiles[newY][newX].hasPiece || (Tiles[newY][newX].hasPiece && Tiles[newY][newX].pieceOnTile->White != White)) {
                possibleMoves.push_back({newX, newY});
            }
        }
    }
}

void Piece::Queen(vector<vector<Tile>> &Tiles) {
    // A queen combines the moves of a rook and a bishop
    Rook(Tiles);
    Bishop(Tiles);
}

void Piece::King(vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    // Define all possible king moves
    int kingMoves[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1},
        {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
    };

    // Check all adjacent squares
    for (int i = 0; i < 8; ++i) {
        int newX = x + kingMoves[i][0];
        int newY = y + kingMoves[i][1];
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            Tile &targetTile = Tiles[newY][newX];
            if (!targetTile.hasPiece || (targetTile.hasPiece && targetTile.pieceOnTile->White != White)) {
                possibleMoves.push_back({newX, newY});

            }
        }
    }

    // Castling logic
    if (hasMoved == 0) {
        int row = White ? 7 : 0; // Row for white or black king
        // Kingside castling
        if (!Tiles[row][5].hasPiece && !Tiles[row][6].hasPiece) {
            Tile &rookTile = Tiles[row][7];
            if (rookTile.hasPiece && rookTile.pieceOnTile->figure == 500 && rookTile.pieceOnTile->hasMoved == 0) {
                possibleMoves.push_back({7, row});
            }
        }
        // Queenside castling
        if (!Tiles[row][1].hasPiece && !Tiles[row][2].hasPiece && !Tiles[row][3].hasPiece) {
            Tile &rookTile = Tiles[row][0];
            if (rookTile.hasPiece && rookTile.pieceOnTile->figure == 500 && rookTile.pieceOnTile->hasMoved == 0) {
                possibleMoves.push_back({0, row});
            }
        }
    }
}
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
