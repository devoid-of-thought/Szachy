#include "Figury.h"
#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "SDL/include/SDL3/SDL.h"

#include <vector>
#include <iostream>
#include "Window.h"

//LTexture Implementation
Piece::Piece(int row, int col, bool isWhite, int type):
    //Initialize texture variables
    mTexture{nullptr},
    mWidth{0},
    mHeight{0},
    BoardPosition{row, col},
    White{isWhite},
    figure{type},
    hasMoved{0} {
}

Piece::~Piece() {
    //Clean up texture
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

void Piece::handleEvent(SDL_Event *e, Tile *Tiles[8][8], Piece *chessPieces[2][16]) {
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
                    ((selectedPiece->figure == 5 && figure == 999) ||
                     (selectedPiece->figure == 999 && figure == 5))) {
                    return;
                }
                selectedPiece = this;
                generatePossibleMoves(Tiles,chessPieces);
            }
        }
    }
}

void Piece::destroy() {
    //Clean up texture
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
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

void Piece::pawn(Tile *Tiles[8][8]) {
    int x = BoardPosition.first; // Column
    int y = BoardPosition.second; // Row

    possibleMoves.clear(); // Make sure it's empty before adding

    if (!White) {
        // Black pawn (moves down the board, y+1)
        if (y + 1 < 8 && !Tiles[y + 1][x]->hasPiece) {
            possibleMoves.push_back({x, y + 1});
        }

        if (y == 1 && !Tiles[2][x]->hasPiece && !Tiles[3][x]->hasPiece) {
            possibleMoves.push_back({x, y + 2});
        }

        // Captures
        if (x - 1 >= 0 && y + 1 < 8 && Tiles[y + 1][x - 1]->hasPiece && Tiles[y + 1][x - 1]->isWhite) {
            possibleMoves.push_back({x - 1, y + 1});
        }

        if (x + 1 < 8 && y + 1 < 8 && Tiles[y + 1][x + 1]->hasPiece && Tiles[y + 1][x + 1]->isWhite) {
            possibleMoves.push_back({x + 1, y + 1});
        }
    } else {
        // White pawn (moves up the board, y-1)
        if (y - 1 >= 0 && !Tiles[y - 1][x]->hasPiece) {
            possibleMoves.push_back({x, y - 1});
        }

        if (y == 6 && !Tiles[5][x]->hasPiece && !Tiles[4][x]->hasPiece) {
            possibleMoves.push_back({x, y - 2});
        }

        // Captures
        if (x - 1 >= 0 && y - 1 >= 0 && Tiles[y - 1][x - 1]->hasPiece && !Tiles[y - 1][x - 1]->isWhite) {
            possibleMoves.push_back({x - 1, y - 1});
        }

        if (x + 1 < 8 && y - 1 >= 0 && Tiles[y - 1][x + 1]->hasPiece && !Tiles[y - 1][x + 1]->isWhite) {
            possibleMoves.push_back({x + 1, y - 1});
        }
    }
}

void Piece::Rook(Tile *Tiles[8][8]) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    // Up (increasing row)
    for (int i = y + 1; i < 8; ++i) {
        if (Tiles[i][x]->hasPiece) {
            if (Tiles[i][x]->pieceOnTile->White != White) {
                possibleMoves.push_back({x, i});  // can capture enemy
            }
            break;  // either way, you stop scanning in this direction
        }
        possibleMoves.push_back({x, i});
    }

    // Down (decreasing row)
    for (int i = y - 1; i >= 0; --i) {
        if (Tiles[i][x]->hasPiece) {
            if (Tiles[i][x]->pieceOnTile->White != White) {
                possibleMoves.push_back({x, i});
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }

    // Right (increasing column)  ← fix: use Tiles[y][i]
    for (int i = x + 1; i < 8; ++i) {
        if (Tiles[y][i]->hasPiece) {
            if (Tiles[y][i]->pieceOnTile->White != White) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }

    // Left (decreasing column)  ← fix: use Tiles[y][i]
    for (int i = x - 1; i >= 0; --i) {
        if (Tiles[y][i]->hasPiece) {
            if (Tiles[y][i]->pieceOnTile->White != White) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }
    if (hasMoved==0) {
        int row = BoardPosition.second;

        // Kingside castling
        if (Tiles[row][4]->hasPiece) {
            Piece *king = Tiles[row][4]->pieceOnTile;
            if (king && king->figure == 999 && king->hasMoved==0) {
                if (!Tiles[row][5]->hasPiece && !Tiles[row][6]->hasPiece) {
                    possibleMoves.push_back({4, row}); // King's target square
                }
            }
        }

        // Queenside castling
        if (Tiles[row][4]->hasPiece) {
            Piece *king = Tiles[row][4]->pieceOnTile;
            if (king && king->figure == 999 && king->hasMoved==0) {
                if (!Tiles[row][1]->hasPiece && !Tiles[row][2]->hasPiece && !Tiles[row][3]->hasPiece) {
                    possibleMoves.push_back({4, row}); // King's target square
                }
            }
        }
    }
}

void Piece::Bishop(Tile *Tiles[8][8]) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    // Down-right
    for (int i = 1; i < 8; ++i) {
        if (y + i >= 8 || x + i >= 8) break;
        if (Tiles[y + i][x + i]->hasPiece && Tiles[y+i][x+i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x + i, y + i});
            break;
        }
        if (Tiles[y + i][x + i]->hasPiece && Tiles[y+i][x+i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x + i, y + i});
    }

    // Up-left
    for (int i = 1; i < 8; ++i) {
        if (y - i < 0 || x - i < 0) break;
        if (Tiles[y - i][x - i]->hasPiece && Tiles[y-i][x-i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x - i, y - i});
            break;
        }
        if (Tiles[y - i][x - i]->hasPiece && Tiles[y-i][x-i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x - i, y - i});
    }

    // Up-right
    for (int i = 1; i < 8; ++i) {
        if (y - i < 0 || x + i >= 8) break;
        if (Tiles[y - i][x + i]->hasPiece && Tiles[y-i][x+i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x + i, y - i});
            break;
        }
        if (Tiles[y - i][x + i]->hasPiece && Tiles[y-i][x+i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x + i, y - i});
    }

    // Down-left
    for (int i = 1; i < 8; ++i) {
        if (y + i >= 8 || x - i < 0) break;
        if (Tiles[y + i][x - i]->hasPiece && Tiles[y+i][x-i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x - i, y + i});
            break;
        }
        if (Tiles[y + i][x - i]->hasPiece && Tiles[y+i][x-i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x - i, y + i});
    }
}

void Piece::Knight(Tile *Tiles[8][8]) {
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
            if (!Tiles[newY][newX]->hasPiece || (Tiles[newY][newX]->hasPiece && Tiles[newY][newX]->isWhite != White)) {
                possibleMoves.push_back({newX, newY});
            }
        }
    }
}

void Piece::Queen(Tile *Tiles[8][8]) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;

    // Down-right
    for (int i = 1; i < 8; ++i) {
        if (y + i >= 8 || x + i >= 8) break;
        if (Tiles[y + i][x + i]->hasPiece && Tiles[y+i][x+i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x + i, y + i});
            break;
        }
        if (Tiles[y + i][x + i]->hasPiece && Tiles[y+i][x+i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x + i, y + i});
    }

    // Up-left
    for (int i = 1; i < 8; ++i) {
        if (y - i < 0 || x - i < 0) break;
        if (Tiles[y - i][x - i]->hasPiece && Tiles[y-i][x-i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x - i, y - i});
            break;
        }
        if (Tiles[y - i][x - i]->hasPiece && Tiles[y-i][x-i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x - i, y - i});
    }

    // Up-right
    for (int i = 1; i < 8; ++i) {
        if (y - i < 0 || x + i >= 8) break;
        if (Tiles[y - i][x + i]->hasPiece && Tiles[y-i][x+i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x + i, y - i});
            break;
        }
        if (Tiles[y - i][x + i]->hasPiece && Tiles[y-i][x+i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x + i, y - i});
    }

    // Down-left
    for (int i = 1; i < 8; ++i) {
        if (y + i >= 8 || x - i < 0) break;
        if (Tiles[y + i][x - i]->hasPiece && Tiles[y+i][x-i]->pieceOnTile->White!=White) {
            possibleMoves.push_back({x - i, y + i});
            break;
        }
        if (Tiles[y + i][x - i]->hasPiece && Tiles[y+i][x-i]->pieceOnTile->White==White) {
            break;
        }
        possibleMoves.push_back({x - i, y + i});
    }
    // Up (increasing row)
    for (int i = y + 1; i < 8; ++i) {
        if (Tiles[i][x]->hasPiece) {
            if (Tiles[i][x]->pieceOnTile->White != White) {
                possibleMoves.push_back({x, i});  // can capture enemy
            }
            break;  // either way, you stop scanning in this direction
        }
        possibleMoves.push_back({x, i});
    }

    // Down (decreasing row)
    for (int i = y - 1; i >= 0; --i) {
        if (Tiles[i][x]->hasPiece) {
            if (Tiles[i][x]->pieceOnTile->White != White) {
                possibleMoves.push_back({x, i});
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }

    // Right (increasing column)  ← fix: use Tiles[y][i]
    for (int i = x + 1; i < 8; ++i) {
        if (Tiles[y][i]->hasPiece) {
            if (Tiles[y][i]->pieceOnTile->White != White) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }

    // Left (decreasing column)  ← fix: use Tiles[y][i]
    for (int i = x - 1; i >= 0; --i) {
        if (Tiles[y][i]->hasPiece) {
            if (Tiles[y][i]->pieceOnTile->White != White) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }
}
bool Piece::isChecked(int color,
                     Tile* Tiles[8][8],
                     Piece* chessPieces[2][16],
                     std::pair<int,int> targetSquare)
{
    int enemyColor = (color == 0) ? 1 : 0;

    // First: check sliding pieces (rook/queen along ranks & files, bishop/queen along diagonals),
    // knights, pawns, etc., by calling generatePossibleMoves on those only.

    for (int i = 0; i < 16; ++i) {
        Piece* enemy = chessPieces[enemyColor][i];
        if (!enemy) continue;

        // If this enemy is the king, skip it here; we'll handle it later.
        if (enemy->figure == 999)
            continue;

        enemy->possibleMoves.clear();
        enemy->generatePossibleMoves(Tiles, chessPieces);

        // If any of its “possibleMoves” hits targetSquare, we are in check.
        for (auto& mv : enemy->possibleMoves) {
            if (mv == targetSquare)
                return true;
        }
    }

    // Now explicitly check for an adjacent enemy king:
    Piece* enemyKing = chessPieces[enemyColor][3]; // assuming index 3 is always the king
    if (enemyKing) {
        int kx = enemyKing->BoardPosition.first;
        int ky = enemyKing->BoardPosition.second;
        int tx = targetSquare.first;
        int ty = targetSquare.second;

        // Are they adjacent (but not the same square)?
        if (std::abs(kx - tx) <= 1 && std::abs(ky - ty) <= 1 &&
            !(kx == tx && ky == ty))
        {
            return true;
        }
    }

    return false;
}


void Piece::King(Tile *Tiles[8][8], Piece* chessPieces[2][16]) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    int kingMoves[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1},
        {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
    };
    for (int i = 0; i < 8; ++i) {
        int newX = x + kingMoves[i][0];
        int newY = y + kingMoves[i][1];
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) { // Ensure valid board coordinates
            Tile* targetTile = Tiles[newY][newX]; // Access the target tile
            if (!targetTile->hasPiece || (targetTile->hasPiece && targetTile->isWhite != White)) {
                // Check if the move does not place the king in check
                if (!isChecked(White ? 0 : 1, Tiles, chessPieces, {newX, newY})) {
                    possibleMoves.push_back({newX, newY});
                }
            }
        }
    }
    if (hasMoved==0) {
        int row = y;
        // Kingside castling
        if (White) {
            if (Tiles[7][7]->hasPiece) {
                Piece* rook = Tiles[row][7]->pieceOnTile;
                if (rook && rook->figure == 5 && 0==rook->hasMoved) {
                    if (!Tiles[7][5]->hasPiece && !Tiles[7][6]->hasPiece) {
                        possibleMoves.push_back({7, 7}); // King's target square
                    }
                }
            }
            if (Tiles[7][0]->hasPiece) {
                Piece* rook = Tiles[row][0]->pieceOnTile;
                if (rook && rook->figure == 5 && 0==rook->hasMoved) {
                    if (!Tiles[7][1]->hasPiece && !Tiles[7][2]->hasPiece && !Tiles[7][3]->hasPiece) {
                        possibleMoves.push_back({0, 7}); // King's target square
                    }
                }
            }
        } else {
            if (Tiles[0][7]->hasPiece) {
                Piece* rook = Tiles[0][7]->pieceOnTile;
                if (rook && rook->figure == 5 && 0==rook->hasMoved) {
                    if (!Tiles[0][5]->hasPiece && !Tiles[0][6]->hasPiece) {
                        possibleMoves.push_back({7, 0}); // King's target square
                    }
                }
            }
            if (Tiles[0][0]->hasPiece) {
                Piece* rook = Tiles[0][0]->pieceOnTile;
                if (rook && rook->figure == 5 && 0==rook->hasMoved) {
                    if (!Tiles[0][1]->hasPiece && !Tiles[0][2]->hasPiece && !Tiles[0][3]->hasPiece) {
                        possibleMoves.push_back({0, 0}); // King's target square
                    }
                }
            }
        }


    }
}

void Piece::resetPossibleMoves() {
    possibleMoves.clear();
}

void Piece::generatePossibleMoves(Tile *Tiles[8][8], Piece* chessPieces[2][16]) {
    resetPossibleMoves();

    int fig = figure;
    switch (fig) {
        case 1:
            pawn(Tiles);
            break;
        case 5:
            Rook(Tiles);
            break;
        case 3:
            Knight(Tiles);
            break;
        case 4:
            Bishop(Tiles);
            break;
        case 9:
            Queen(Tiles);
            break;
        case 999:
            King(Tiles,chessPieces);
            break;
    }
}
