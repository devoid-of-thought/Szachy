#include "Figury.h"
#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "SDL/include/SDL3/SDL.h"

#include <vector>
#include <iostream>
#include "Window.h"

//LTexture Implementation
Piece::Piece(int row, int col, bool isWhite,int type):
    //Initialize texture variables
    mTexture{ nullptr },
    mWidth{ 0 },
    mHeight{ 0 },
    BoardPosition{row,col},
    White {isWhite},
    figure{type}

{

}

Piece::~Piece()
{
    //Clean up texture
    destroy();
}
bool Piece::loadFromFile( std::string path )
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
void Piece::handleEvent(SDL_Event* e, Tile* Tiles[8][8]) {
    if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN && e->button.button == SDL_BUTTON_LEFT) {
        int mouseX = e->button.x;
        int mouseY = e->button.y;

        int tileSize = 64;
        int pieceX = BoardPosition.first * tileSize;
        int pieceY = BoardPosition.second * tileSize;

        // Check if the mouse click is within this piece's rectangle
        if (mouseX >= pieceX && mouseX < pieceX + tileSize &&
            mouseY >= pieceY && mouseY < pieceY + tileSize) {
            if (this->White) {
                selectedPiece = this;
                generatePossibleMoves(Tiles);
                }
            if (this->White == false && selectedPiece != nullptr) {
                destroy();
            }
        }
    }
}

void Piece::destroy()
{
    //Clean up texture
    SDL_DestroyTexture( mTexture );
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}
void Piece::render( float x, float y )
{
    //Set texture position
    SDL_FRect dstRect = { x, y, static_cast<float>( mWidth ), static_cast<float>( mHeight ) };

    //Render texture
    SDL_RenderTexture( gRenderer, mTexture, nullptr, &dstRect );
}
int Piece::getWidth()
{
    return mWidth;
}

int Piece::getHeight()
{
    return mHeight;
}
void Piece::pawn(Tile* Tiles[8][8]) {
    int x = BoardPosition.first;   // Column
    int y = BoardPosition.second;  // Row

    possibleMoves.clear(); // Make sure it's empty before adding

    if (!White) { // Black pawn (moves down the board, y+1)
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

    } else { // White pawn (moves up the board, y-1)
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

    // Debug output
    for (auto& move : possibleMoves) {
        std::cout << "Possible move: " << move.first << ", " << move.second << std::endl;
    }
}

void Piece::Rook(Tile* Tiles[8][8]) {
    int x = BoardPosition.first;  // column
    int y = BoardPosition.second; // row

    // Up (increasing row)
    for (int i = y + 1; i < 8; ++i) {
        if (Tiles[i][x]->hasPiece) {
            if (White != Tiles[i][x]->isWhite) {
                possibleMoves.push_back({x, i}); // Capture
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }

    // Down (decreasing row)
    for (int i = y - 1; i >= 0; --i) {
        if (Tiles[i][x]->hasPiece) {
            if (White != Tiles[i][x]->isWhite) {
                possibleMoves.push_back({x, i}); // Capture
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }

    // Right (increasing column)
    for (int i = x + 1; i < 8; ++i) {
        if (Tiles[y][i]->hasPiece) {
            if (White != Tiles[y][i]->isWhite) {
                possibleMoves.push_back({i, y}); // Capture
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }

    // Left (decreasing column)
    for (int i = x - 1; i >= 0; --i) {
        if (Tiles[y][i]->hasPiece) {
            if (White != Tiles[y][i]->isWhite) {
                possibleMoves.push_back({i, y}); // Capture
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }
}

void Piece::Bishop(Tile *Tiles[8][8]) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
        for (int i = 1; i < 8; ++i) {
            if (x + i < 8 && y + i < 8 && Tiles[x + i][y + i]->hasPiece) {
                if (White != Tiles[x + i][y + i]->isWhite) {
                    possibleMoves.push_back({x + i, y + i}); // Capture
                }
                break; // Stop regardless of friendly or enemy
            }
            possibleMoves.push_back({x + i, y + i});
        }
        for (int i = 1; i < 8; ++i) {
            if (x - i >= 0 && y - i >= 0 && Tiles[x - i][y - i]->hasPiece) {
                if (White != Tiles[x - i][y - i]->isWhite) {
                    possibleMoves.push_back({x - i, y - i}); // Capture
                }
                break;
            }
            possibleMoves.push_back({x - i, y - i});
        }
        for (int i = 1; i < 8; ++i) {
            if (x + i < 8 && y - i >= 0 && Tiles[x + i][y - i]->hasPiece) {
                if (White != Tiles[x + i][y - i]->isWhite) {
                    possibleMoves.push_back({x + i, y - i}); // Capture
                }
                break;
            }
            possibleMoves.push_back({x + i, y - i});
        }
        for (int i = 1; i < 8; ++i) {
            if (x - i >= 0 && y + i < 8 && Tiles[x - i][y + i]->hasPiece) {
                if (White != Tiles[x - i][y + i]->isWhite) {
                    possibleMoves.push_back({x - i, y + i}); // Capture
                }
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
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            if (!Tiles[newX][newY]->hasPiece || (Tiles[newX][newY]->hasPiece && Tiles[newX][newY]->isWhite != White)) {
                possibleMoves.push_back({newX, newY});
            }
        }
    }
}
void Piece::Queen(Tile *Tiles[8][8]) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;

    // Up
    for (int i = y + 1; i < 8; ++i) {
        if (Tiles[x][i]->hasPiece) {
            if (White != Tiles[x][i]->isWhite) {
                possibleMoves.push_back({x, i}); // Capture
            }
            break; // Stop regardless of friendly or enemy
        }
        possibleMoves.push_back({x, i});
    }

    // Down
    for (int i = y - 1; i >= 0; --i) {
        if (Tiles[x][i]->hasPiece) {
            if (White != Tiles[x][i]->isWhite) {
                possibleMoves.push_back({x, i});
            }
            break;
        }
        possibleMoves.push_back({x, i});
    }

    // Right
    for (int i = x + 1; i < 8; ++i) {
        if (Tiles[i][y]->hasPiece) {
            if (White != Tiles[i][y]->isWhite) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }

    // Left
    for (int i = x - 1; i >= 0; --i) {
        if (Tiles[i][y]->hasPiece) {
            if (White != Tiles[i][y]->isWhite) {
                possibleMoves.push_back({i, y});
            }
            break;
        }
        possibleMoves.push_back({i, y});
    }
    for (int i = 1; i < 8; ++i) {
        if (x + i < 8 && y + i < 8 && Tiles[x + i][y + i]->hasPiece) {
            if (White != Tiles[x + i][y + i]->isWhite) {
                possibleMoves.push_back({x + i, y + i}); // Capture
            }
            break; // Stop regardless of friendly or enemy
        }
        possibleMoves.push_back({x + i, y + i});
    }
    for (int i = 1; i < 8; ++i) {
        if (x - i >= 0 && y - i >= 0 && Tiles[x - i][y - i]->hasPiece) {
            if (White != Tiles[x - i][y - i]->isWhite) {
                possibleMoves.push_back({x - i, y - i}); // Capture
            }
            break;
        }
        possibleMoves.push_back({x - i, y - i});
    }
    for (int i = 1; i < 8; ++i) {
        if (x + i < 8 && y - i >= 0 && Tiles[x + i][y - i]->hasPiece) {
            if (White != Tiles[x + i][y - i]->isWhite) {
                possibleMoves.push_back({x + i, y - i}); // Capture
            }
            break;
        }
        possibleMoves.push_back({x + i, y - i});
    }
    for (int i = 1; i < 8; ++i) {
        if (x - i >= 0 && y + i < 8 && Tiles[x - i][y + i]->hasPiece) {
            if (White != Tiles[x - i][y + i]->isWhite) {
                possibleMoves.push_back({x - i, y + i}); // Capture
            }
            break;
        }
        possibleMoves.push_back({x - i, y + i});
    }

}
void Piece::King(Tile *Tiles[8][8]) {
    int x = BoardPosition.first;
    int y = BoardPosition.second;
    int kingMoves[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1},
        {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
    };
    for (int i = 0; i < 8; ++i) {
        int newX = x + kingMoves[i][0];
        int newY = y + kingMoves[i][1];
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            if (!Tiles[newX][newY]->hasPiece || (Tiles[newX][newY]->hasPiece && Tiles[newX][newY]->isWhite != White)) {
                possibleMoves.push_back({newX, newY});
            }
        }
    }
}
void Piece::resetPossibleMoves() {
    possibleMoves.clear();
}
void Piece::generatePossibleMoves(Tile *Tiles[8][8]) {
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
                King(Tiles);
            break;
        }
}




