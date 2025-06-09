#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "Plansza.h"
#include "SDL/include/SDL3/SDL.h"

#include "Figury.h"
#include <unistd.h>
#include <map>

#include <iostream>
#include <algorithm>
#include "Window.h"
#include <random>

using namespace std;


constexpr int kScreenWidth{512};
constexpr int kScreenHeight{512};

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

SDL_Texture *whiteTileTexture = nullptr;
SDL_Texture *blackTileTexture = nullptr;
SDL_Texture *whiteHighlightTexture = nullptr;
SDL_Texture *blackHighlightTexture = nullptr;

Piece *selectedPiece = nullptr;
Piece *temp = nullptr;
int lastCaptured;
vector<pair<pair<int, int>, pair<int, int> > > lastMovement;
bool isWhiteTurn = true;
vector<pair<int, int> > pamiec;
vector<vector<Tile>> Board(8, vector<Tile>(8));


vector<vector<Piece>> Pieces = {
    {
        Piece(0, 7, true, 500, 0), Piece(1, 7, true, 350, 1), Piece(2, 7, true, 351, 2), Piece(3, 7, true, 900, 3),
        Piece(4, 7, true, 999, 4), Piece(5, 7, true, 351, 5), Piece(6, 7, true, 350, 6), Piece(7, 7, true, 500, 7),
        Piece(0, 6, true, 100, 8), Piece(1, 6, true, 100, 9), Piece(2, 6, true, 100, 10), Piece(3, 6, true, 100, 11),
        Piece(4, 6, true, 100, 12), Piece(5, 6, true, 100, 13), Piece(6, 6, true, 100, 14), Piece(7, 6, true, 100, 15)
    },
    {
        Piece(0, 0, false, -500, 0), Piece(1, 0, false, -350, 1), Piece(2, 0, false, -351, 2), Piece(3, 0, false, -900, 3),
        Piece(4, 0, false, -999, 4), Piece(5, 0, false, -351, 5), Piece(6, 0, false, -350, 6), Piece(7, 0, false, -500, 7),
        Piece(0, 1, false, -100, 8), Piece(1, 1, false, -100, 9), Piece(2, 1, false, -100, 10), Piece(3, 1, false, -100, 11),
        Piece(4, 1, false, -100, 12), Piece(5, 1, false, -100, 13), Piece(6, 1, false, -100, 14), Piece(7, 1, false, -100, 15)
    }
};
vector<Piece *> capturedPieces[2];

bool init() {
    //Initialization flag
    bool success{true};

    //Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        success = false;
    } else {
        //Create window with renderer
        if (!SDL_CreateWindowAndRenderer("Szachy", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer)) {
            SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
            success = false;
        }
    }


    return success;
}


bool loadMedia(vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    whiteTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_tile.png");
    blackTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_tile.png");
    whiteHighlightTexture = IMG_LoadTexture(
        gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_Highlight.png");
    blackHighlightTexture = IMG_LoadTexture(
        gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_Highlight.png");

    // File loading flag
    bool success{true};

    const string basePath = "/home/userbrigh/CLionProjects/Szachy/Tekstury/";
    map<int, std::string> textureFiles = {
        {500, "Chess_rlt60.png"}, {350, "Chess_nlt60.png"}, {351, "Chess_blt60.png"},
        {900, "Chess_qlt60.png"}, {999, "Chess_klt60.png"}, {100, "Chess_plt60.png"},
        {-500, "Chess_rdt60.png"}, {-350, "Chess_ndt60.png"}, {-351, "Chess_bdt60.png"},
        {-900, "Chess_qdt60.png"}, {-999, "Chess_kdt60.png"}, {-100, "Chess_pdt60.png"}
    };

    for (auto &row : chessPieces) {
        for (auto &piece : row) {
            if (textureFiles.find(piece.figure) != textureFiles.end()) {
                piece.loadFromFile(basePath + textureFiles[piece.figure]);
            }
        }
    }

    const int tileSize = 64;
    for (int x = 0; x < 8; x++) { // Fixed loop bounds to 8x8
        for (int y = 0; y < 8; y++) {
            Tiles[x][y].BoardPosition = {x, y};
            Tiles[x][y].mWidth = tileSize;
            Tiles[x][y].mHeight = tileSize;
            if ((x + y) % 2 != 0) {
                Tiles[x][y].mTexture= whiteTileTexture;
            } else {
                Tiles[x][y].mTexture = blackTileTexture;
            }
        }
    }
    return success;
}
template<typename T>
void close(T &PngTexture) {
    //Clean up texture
    PngTexture.destroy();

    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    SDL_Quit();
}

void renderBoard(vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    whiteTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_tile.png");
    blackTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_tile.png");
    whiteHighlightTexture = IMG_LoadTexture(
        gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_Highlight.png");
    blackHighlightTexture = IMG_LoadTexture(
        gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_Highlight.png");

    const int tileSize = Tiles[0][0].getHeight();
    for (int y = 7; y >= 0; --y) {
        for (int x = 7; x >= 0; --x) {
            // 1) reset tile graphics
            if ((x + y) % 2) {
                Tiles[y][x].mTexture = blackTileTexture;
            } else {
                Tiles[y][x].mTexture = whiteTileTexture;
            }
            Tiles[y][x].highlightBorder = false;

            // 2) *clear* any stale occupancy
            Tiles[y][x].hasPiece = false;
            Tiles[y][x].isWhite = false;
            Tiles[y][x].pieceOnTile = nullptr;

            // 3) then do your highlight pass...
            if (selectedPiece) {
                for (auto &move: selectedPiece->possibleMoves) {
                    if (move.first == y && move.second == x) {
                        Tiles[y][x].mTexture =
                                ((x + y) % 2)
                                    ? blackHighlightTexture
                                    : whiteHighlightTexture;
                        break;
                    }
                }
            }

            // 4) finally draw the tile itself
            Tiles[y][x].render(y * tileSize, x * tileSize);
        }
    }

    // 5) now stamp your *live* pieces back onto the board
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            Piece& p = chessPieces[i][j];
            if (!p.isCaptured && p.BoardPosition.first < 8 && p.BoardPosition.second < 8) {
                int tx = p.BoardPosition.first;
                int ty = p.BoardPosition.second;
                p.render(tx * tileSize, ty * tileSize);
                Tiles[ty][tx].hasPiece = true;
                Tiles[ty][tx].pieceOnTile = &p;
                Tiles[ty][tx].isWhite = p.White;
            }
        }
    }
}
void Debug(vector<vector<Tile>> &Tiles) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (Tiles[i][j].hasPiece) {
                cout << Tiles[i][j].pieceOnTile->figure << " ";
            } else {
                cout << "0" << "   ";
            }
        }
        cout << endl;
    }
    cout << "------------------------" << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (Tiles[i][j].hasPiece) {
                cout << Tiles[i][j].hasPiece<< " ";
            } else {
                cout << "0" << " ";
            }
        }
        cout << endl;
    }
    cout << "------------------------" << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (Tiles[i][j].hasPiece) {
                cout << Tiles[i][j].isWhite << " ";
            } else {
                cout << "0" << " ";
            }
        }
        cout << endl;
    }
    cout << "------------------------" << endl;

}


bool Castle(pair<int, int> clickedPosition,vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    Piece *piece = Tiles[clickedPosition.second][clickedPosition.first].pieceOnTile;
    if (piece == nullptr) {
        return false;
    }
    if (piece->figure == 500 && piece->BoardPosition.first == 0) {
        int color = (piece->White) ? 0 : 1;
        Tiles[piece->BoardPosition.second][2].pieceOnTile = &chessPieces[color][3];
        Tiles[piece->BoardPosition.second][2].hasPiece = true;
        Tiles[piece->BoardPosition.second][4].hasPiece = false;
        Tiles[piece->BoardPosition.second][4].pieceOnTile = nullptr;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = false;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = nullptr;
        chessPieces[color][3].BoardPosition.first = 2;
        chessPieces[color][3].BoardPosition.second = (piece->White) ? 7 : 0;
        lastMovement.emplace_back(piece->BoardPosition, piece->BoardPosition);
        piece->BoardPosition.first = 3;
        piece->BoardPosition.second = (piece->White) ? 7 : 0;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = true;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = piece;
        return true;
    } else if (piece->figure == 500 && piece->BoardPosition.first == 7) {
        int color = (piece->White) ? 0 : 1;
        Tiles[piece->BoardPosition.second][6].pieceOnTile = &chessPieces[color][3];
        Tiles[piece->BoardPosition.second][6].hasPiece = true;
        Tiles[piece->BoardPosition.second][4].hasPiece = false;
        Tiles[piece->BoardPosition.second][4].pieceOnTile = nullptr;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = false;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = nullptr;
        chessPieces[color][3].BoardPosition.first = 6;
        chessPieces[color][3].BoardPosition.second = (piece->White) ? 7 : 0;
        lastMovement.emplace_back(piece->BoardPosition, piece->BoardPosition);

        piece->BoardPosition.first = 5;
        piece->BoardPosition.second = (piece->White) ? 7 : 0;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = true;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = piece;
        return true;
    } else if (piece->figure == 999 && clickedPosition.first == 0) {
        int color = (piece->White) ? 0 : 1;
        Tiles[piece->BoardPosition.second][3].pieceOnTile = &chessPieces[color][0];
        Tiles[piece->BoardPosition.second][3].hasPiece = true;
        Tiles[piece->BoardPosition.second][4].hasPiece = false;
        Tiles[piece->BoardPosition.second][4].pieceOnTile = nullptr;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = false;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = nullptr;
        chessPieces[color][0].BoardPosition.first = 3;
        chessPieces[color][0].BoardPosition.second = (piece->White) ? 7 : 0;
        lastMovement.emplace_back(piece->BoardPosition, piece->BoardPosition);

        piece->BoardPosition.first = 2;
        piece->BoardPosition.second = (piece->White) ? 7 : 0;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = true;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = piece;
        return true;
    } else if (piece->figure == 999 && clickedPosition.first == 7) {
        int color = (piece->White) ? 0 : 1;
        Tiles[piece->BoardPosition.second][5].pieceOnTile = &chessPieces[color][7];
        Tiles[piece->BoardPosition.second][5].hasPiece = true;
        Tiles[piece->BoardPosition.second][4].hasPiece = false;
        Tiles[piece->BoardPosition.second][4].pieceOnTile = nullptr;
        chessPieces[color][7].BoardPosition.first = 5;
        chessPieces[color][7].BoardPosition.second = (piece->White) ? 7 : 0;
        lastMovement.emplace_back(piece->BoardPosition, piece->BoardPosition);
        piece->BoardPosition.first = 6;
        piece->BoardPosition.second = (piece->White) ? 7 : 0;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = true;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = piece;
        return true;
    }
    return false;
}

bool inCheck(int color,vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    int enemyColor = (color == 0) ? 1 : 0;
    std::pair<int, int> kingPos = chessPieces[color][3].BoardPosition;
    for (int i = 0; i < 16; ++i) {
        Piece& enemy = chessPieces[enemyColor][i];
        if (enemy.isCaptured)continue;
            enemy.possibleMoves.clear();
            enemy.generatePossibleMoves(Tiles, chessPieces);

            for (auto &move: enemy.possibleMoves) {
                if (move == kingPos) {
                    return true;
                }
            }
        }
    return false;
}

void Move(Piece *piece, pair<int, int> from, pair<int, int> to,vector<vector<Tile>> &Tiles) {
    Tiles[from.second][from.first].hasPiece =
            false;
    Tiles[to.second][to.first].isWhite = false;

    Tiles[from.second][from.first].pieceOnTile =
            nullptr;
    piece->BoardPosition.first = to.first;
    piece->BoardPosition.second = to.second;
    Tiles[to.second][to.first].hasPiece = true;
    Tiles[to.second][to.first].isWhite = piece->White;
    Tiles[to.second][to.first].pieceOnTile =
            piece;
}

void Capture(Piece *piece, int i, pair<int, int> clickedPosition,vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    int enemyColor = (piece->White) ? 1 : 0;
    chessPieces[enemyColor][i].isCaptured = true;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].hasPiece = false;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
    BoardPosition.first].isWhite = false;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].pieceOnTile = nullptr;
    Move(piece, piece->BoardPosition, clickedPosition,Tiles);
    chessPieces[enemyColor][i].BoardPosition = {8, 8};
}

void Unpromote(Piece *piece) {
    if (piece->figure == 100 && (piece->BoardPosition.second == 0 && piece->White)) {
        piece->figure = 900;
        piece->wasPromoted = false;
        piece->loadFromFile(
            "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png");
    } else if (piece->figure == -100 && (piece->BoardPosition.second == 7 && !piece->White)) {
        piece->figure = -900;
        piece->wasPromoted = false;
        piece->loadFromFile(
            "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png");
    }
}

void Promotion(Piece *piece) {
    if (piece->figure == 100 && (piece->BoardPosition.second == 0 && piece->White)) {
        piece->figure = 900;
        piece->wasPromoted = true;
        piece->loadFromFile(
            "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_qlt60.png");
    } else if (piece->figure == -100 && (piece->BoardPosition.second == 7 && !piece->White)) {
        piece->figure = -900;
        piece->wasPromoted = true;
        piece->loadFromFile(
            "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_qdt60.png");
    }
}

void UndoMove(Piece *piece, pair<int, int> last, Piece *captured, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {

    // Clear the current position of the piece
    pair<int, int> lastCapturedPos = piece->BoardPosition;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = false;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].isWhite = false;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = nullptr;

    // Restore the piece's previous position
    piece->BoardPosition = last;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = true;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = piece;
        Tiles[piece->BoardPosition.second][piece->BoardPosition.first].isWhite = piece->White;


    // Restore the captured piece, if any
    if (captured != nullptr && lastCaptured >= 0) {
        int enemyColor = piece->White ? 1 : 0; // Color of the captured side
        chessPieces[enemyColor][lastCaptured] = *captured; // Restore the captured piece in the array
        captured->isCaptured = false;
        auto [cx, cy] = lastCapturedPos;
        Tiles[cy][cx].hasPiece = true;
        Tiles[cy][cx].isWhite = captured->White; // Restore the color of the captured piece
        Tiles[cy][cx].pieceOnTile = captured;

        // Clear the lastCaptured variable for safety
        lastCaptured = -1;
    }

    // Handle promotion reversal
    if (piece->wasPromoted) {
        Unpromote(piece);
    }

    // Remove the last move from the move history
    lastMovement.pop_back();
}

Piece *pseudo_move_piece(Piece *piece, pair<int, int> to, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    Tile &endTile = Tiles[to.second][to.first];
    if (piece != nullptr && (to != piece->BoardPosition)) {
        for (const auto &move : piece->possibleMoves) {
            if (to == move) {
                if (endTile.hasPiece && endTile.isWhite == piece->White) {
                    cout << "Cannot capture own piece!" << endl;
                    return nullptr;
                } else if (endTile.hasPiece && endTile.isWhite != piece->White) {
                    // Capture the piece
                    int enemyColor = piece->White ? 1 : 0;
                    for (int i = 0; i < 16; ++i) {
                        Piece &enemyPiece = chessPieces[enemyColor][i];
                        if (!enemyPiece.isCaptured && enemyPiece.BoardPosition == to && enemyPiece.figure != 999) {
                            lastMovement.emplace_back(piece->BoardPosition, enemyPiece.BoardPosition);
                            lastCaptured = i;
                            Move(piece, piece->BoardPosition, to, Tiles);
                            temp = piece;
                            return &enemyPiece; // Return the captured piece
                        }
                    }
                } else {
                    cout << "[DEBUG] Moving piece from " << piece->BoardPosition.first << "," << piece->BoardPosition.second
                         << " to " << to.first << "," << to.second << endl;
                    lastMovement.emplace_back(piece->BoardPosition, to);
                    Move(piece, piece->BoardPosition, to, Tiles);
                    Promotion(piece);
                    temp = piece;
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}
Piece *move_black(Piece *piece, pair<int, int> to, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    Tile &endTile = Tiles[to.second][to.first];

    if (endTile.hasPiece && endTile.isWhite == piece->White) {
        if (piece->figure == 999 || piece->figure == 500) {
            Castle(to, Tiles, chessPieces);
            isWhiteTurn = true;
        } else {
            cout << "Cannot capture own piece!" << endl;
            return nullptr;
        }
    } else if (endTile.hasPiece && endTile.isWhite != piece->White) {
        // Capture the piece
        int enemyColor = piece->White ? 1 : 0;
        for (int i = 0; i < 16; ++i) {
            Piece &enemyPiece = chessPieces[enemyColor][i];
            if (!enemyPiece.isCaptured && enemyPiece.BoardPosition == to && enemyPiece.figure != 999) {
                lastMovement.emplace_back(piece->BoardPosition, enemyPiece.BoardPosition);
                Capture(piece, i, to, Tiles, chessPieces);
                Promotion(piece);
                isWhiteTurn = true;
                temp = piece;
                return &enemyPiece; // Return the captured piece
            }
        }
    } else {
        lastMovement.emplace_back(piece->BoardPosition, to);
        Move(piece, piece->BoardPosition, to, Tiles);
        Promotion(piece);
        isWhiteTurn = true;
        temp = piece;
        return nullptr;
    }
    return nullptr;
}

Piece *move_white(SDL_Event e, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
        int tileSize = 64;
        int clickedX = e.button.x / tileSize;
        int clickedY = e.button.y / tileSize;

        // Ensure clicked position is within bounds
        if (clickedX < 0 || clickedX >= 8 || clickedY < 0 || clickedY >= 8) {
            return nullptr;
        }

        pair<int, int> clickedPosition = {clickedX, clickedY};
        Tile &endTile = Tiles[clickedY][clickedX];

        if (selectedPiece != nullptr) {
            cout << "[DEBUG] Trying to move " << selectedPiece->White << " " << selectedPiece->figure << " from "
                 << selectedPiece->BoardPosition.first << "," << selectedPiece->BoardPosition.second
                 << " to " << clickedPosition.first << "," << clickedPosition.second
                 << " | endTile.hasPiece=" << endTile.hasPiece
                 << " endTile.isWhite=" << endTile.isWhite
                 << endl;
        }

        temp = selectedPiece;

        if (selectedPiece != nullptr && clickedPosition != selectedPiece->BoardPosition) {
            for (const auto &move : selectedPiece->possibleMoves) {
                if (clickedPosition == move) {
                    if (endTile.hasPiece && endTile.isWhite == selectedPiece->White) {
                        if (selectedPiece->figure == 999 || selectedPiece->figure == 500) {
                            Castle(clickedPosition, Tiles, chessPieces);
                            selectedPiece = nullptr;
                            isWhiteTurn = false;
                        } else {
                            cout << "Cannot capture own piece!" << endl;
                            return nullptr;
                        }
                    } else if (endTile.hasPiece && endTile.isWhite != selectedPiece->White) {
                        // Capture the piece
                        int enemyColor = selectedPiece->White ? 1 : 0;
                        for (int i = 0; i < 16; ++i) {
                            Piece &enemyPiece = chessPieces[enemyColor][i];
                            if (!enemyPiece.isCaptured && enemyPiece.BoardPosition == clickedPosition &&
                                enemyPiece.figure != 999) {
                                lastMovement.emplace_back(selectedPiece->BoardPosition, enemyPiece.BoardPosition);
                                Capture(selectedPiece, i, clickedPosition, Tiles, chessPieces);
                                Promotion(selectedPiece);
                                temp = selectedPiece;
                                selectedPiece = nullptr;
                                isWhiteTurn = false;
                                if (inCheck(0, Tiles, chessPieces)) {
                                    UndoMove(temp, lastMovement.back().first, &enemyPiece, Tiles, chessPieces);
                                    isWhiteTurn = true;
                                }
                                return &enemyPiece; // Return the captured piece
                            }
                        }
                    } else {
                        lastMovement.emplace_back(selectedPiece->BoardPosition, clickedPosition);
                        Move(selectedPiece, selectedPiece->BoardPosition, clickedPosition, Tiles);
                        Promotion(selectedPiece);
                        isWhiteTurn = false;
                        temp = selectedPiece;
                        selectedPiece = nullptr;
                        Debug(Tiles);
                        if (inCheck(0, Tiles, chessPieces)) {
                            UndoMove(temp, lastMovement.back().first, nullptr, Tiles, chessPieces);
                            isWhiteTurn = true;
                        }
                        return nullptr;
                    }
                }
            }
        }
    }
    Debug(Tiles);
    return nullptr;
}


struct Ruch {
    Piece *piece;
    pair<int, int> from;
    pair<int, int> to;
    int value;
    Ruch(Piece *p, pair<int, int> f, pair<int, int> t, int v)
    : piece(p), from(f), to(t), value(v) {}
};

int evaluate(vector<vector<Piece>> &chessPieces) {
    int boardValue = 0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (!chessPieces[i][j].isCaptured) {
                boardValue += chessPieces[i][j].figure;
            }
        }
    }
    return boardValue;
}

vector<Ruch> GenerateAllMoves(int color,vector<vector<Piece>> &chessPieces,vector<vector<Tile>> &Tiles) {
    vector<Ruch> Moves;
    for (int i = 0; i < 16; ++i) {
        Piece& p = chessPieces[color][i];
        if (p.isCaptured) continue;
        p.generatePossibleMoves(Tiles, chessPieces);
        vector <pair<int, int>> &possibleMoves = p.possibleMoves;
        for (int i = 0; i < possibleMoves.size(); ++i) {
            pair<int,int> move = possibleMoves[i];
            pair<int, int> from = p.BoardPosition;
            Piece *captured = move_black(&p, move,Tiles,chessPieces);
            bool wasInCheck = inCheck(color,Tiles,chessPieces);
            UndoMove(&p, from, captured,Tiles,chessPieces);
            if (!wasInCheck) {
                Moves.emplace_back(&p, p.BoardPosition, move, 0);
            }
        }
    }
    return Moves;
}
void reconcilePointers(vector<vector<Piece>>& newPieces,
                      vector<vector<Tile>>& newTiles)
{
    for (auto& colorPieces : newPieces) {
        for (Piece& p : colorPieces) {
            if (!p.isCaptured) {
                auto [x, y] = p.BoardPosition;
                newTiles[y][x].pieceOnTile = &p;
            }
        }
    }
}
vector<vector<Tile>> deepCopyTiles( vector<vector<Tile>> &originalTiles) {
    vector<vector<Tile>> copiedTiles;
    copiedTiles.reserve(originalTiles.size());
    for (const auto &row : originalTiles) {
        vector<Tile> copiedRow;
        copiedRow.reserve(row.size());
        for (const auto &tile : row) {
            copiedRow.push_back(tile); // Assumes Tile has a proper copy constructor

        }
        copiedTiles.push_back(copiedRow);
    }
    return copiedTiles;
}
vector<vector<Piece>> deepCopyPieces(vector<vector<Piece>> &originalPieces) {
    vector<vector<Piece>> copiedPieces;
    copiedPieces.reserve(originalPieces.size());
    for (const auto &row : originalPieces) {
        vector<Piece> copiedRow;
        copiedRow.reserve(row.size());
        for (const auto &piece : row) {
            copiedRow.push_back(piece); // Assumes Piece has a proper copy constructor
        }
        copiedPieces.push_back(copiedRow);
    }
    return copiedPieces;
}
vector<Ruch> BESTMOVES;
int INF = 10000000;
int negaMax(int color,
            int alpha,
            int beta,
            int depthLeft,
            vector<vector<Piece>> &chessPieces,
             vector<vector<Tile>>  &tiles) {
    if (depthLeft == 0) {
        return evaluate(chessPieces);
    }
    cout << "[Debug]  "<<  "negaMax called with color: " << color
         << ", alpha: " << alpha << ", beta: " << beta
         << ", depthLeft: " << depthLeft << endl;
    Debug(tiles);
    vector<Ruch> moves = GenerateAllMoves(color, chessPieces, tiles);
    if (inCheck(color,tiles,chessPieces)) {
    return color == 1 ? -INF : INF; // If in check, return extreme values
    }

    for (int i = 0; i<moves.size();i++) {
        Ruch &m = moves[i];
        auto newPieces = deepCopyPieces(chessPieces);
        auto newTiles  = deepCopyTiles(tiles);
        for (int y = 0; y < newPieces.size(); ++y) {
            for (int x = 0; x < newPieces[y].size(); ++x) {
                Piece& piece = newPieces[y][x];
                auto [col, row] = piece.BoardPosition;
                if (col >= 0 && row >= 0 && row < newTiles.size() && col < newTiles[row].size()) {
                    newTiles[row][col].pieceOnTile = &piece;
                }
            }
        }
        Piece* pCopy = nullptr;
        for (auto& row : newPieces) {
            for (auto& p : row) {
                if (p.BoardPosition == m.piece->BoardPosition && p.figure == m.piece->figure && !p.isCaptured) {
                    pCopy = &p;
                    break;
                }
            }
        }
        // Apply move
        Piece* czy = move_black(pCopy, m.to, newTiles, newPieces);
        // Recursive negamax
        int score = -negaMax(1 - color,
                             -beta,
                             -alpha,
                             depthLeft - 1,
                             newPieces,
                             newTiles);
        UndoMove(pCopy, m.from, czy, newTiles, newPieces);

        if (score > alpha) {
            alpha = score;
        }
        if (alpha >= beta) {
            break;  // Beta cutoff
        }
    }

    return alpha;
}

// Search Interface: chooses and plays a move
void SI(vector<vector<Tile>> &tiles, vector<vector<Piece>> &chessPieces) {
    const int MAX_DEPTH = 1;
    int rootColor = /* current player color */ 1;

    vector<Ruch> possibleMoves = GenerateAllMoves(rootColor, chessPieces, tiles);
    if (possibleMoves.empty()) {
        cout << "No valid moves found!" << endl;
        return;
    }

    int alpha = -INF, beta = INF;
    int bestScore = -INF;
    vector<Ruch> bestMoves;
    auto basePieces = deepCopyPieces(chessPieces);

    auto baseTiles  = deepCopyTiles(tiles);
    for (int y = 0; y < basePieces.size(); ++y) {
        for (int x = 0; x < basePieces[y].size(); ++x) {
            Piece& piece = basePieces[y][x];
            auto [col, row] = piece.BoardPosition;
            if (col >= 0 && row >= 0 && row < baseTiles.size() && col < baseTiles[row].size()) {
                baseTiles[row][col].pieceOnTile = &piece;
            }
        }
    }
    for (const Ruch &m : possibleMoves) {
        auto newPieces = deepCopyPieces(chessPieces);

        auto newTiles  = deepCopyTiles(tiles);        for (int y = 0; y < newPieces.size(); ++y) {
            for (int x = 0; x < newPieces[y].size(); ++x) {
                Piece& piece = newPieces[y][x];
                auto [col, row] = piece.BoardPosition;
                if (col >= 0 && row >= 0 && row < newTiles.size() && col < newTiles[row].size()) {
                    newTiles[row][col].pieceOnTile = &piece;
                }
            }
        }
        Piece* pCopy = nullptr;
        for (auto& row : newPieces) {
            for (auto& p : row) {
                if (p.BoardPosition == m.piece->BoardPosition && p.figure == m.piece->figure && !p.isCaptured) {
                    pCopy = &p;
                    break;
                }
            }
        }
        Piece* czy = move_black(pCopy, m.to, newTiles, newPieces);
        int score = -negaMax(1 - rootColor,
                             -beta,
                             -alpha,
                             MAX_DEPTH - 1,
                             newPieces,
                             newTiles);
        UndoMove(pCopy, m.from, czy, newTiles, newPieces);

        if (score > bestScore) {
            bestScore = score;
            bestMoves.clear();
            bestMoves.push_back(m);
        } else if (score == bestScore) {
            bestMoves.push_back(m);
        }

        if (score > alpha) {
            alpha = score;
        }
    }
    for (auto & move : bestMoves) {
        cout << "Best move: " << move.piece->figure << " from "
             << move.from.first << "," << move.from.second
             << " to " << move.to.first << "," << move.to.second
             << " with score: " << move.value << endl;
    }
    // Pick a random move among best
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, bestMoves.size() - 1);
    Ruch chosen = bestMoves[dis(gen)];

    // Apply chosen move to actual board
    Piece *pOrig = chosen.piece;
    move_black(pOrig, chosen.to, baseTiles, basePieces);
    chessPieces= deepCopyPieces(basePieces);

    tiles = deepCopyTiles(baseTiles);
    for (int y = 0; y < chessPieces.size(); ++y) {
        for (int x = 0; x < chessPieces[y].size(); ++x) {
            Piece& piece = chessPieces[y][x];
            auto [col, row] = piece.BoardPosition;
            if (col >= 0 && row >= 0 && row < baseTiles.size() && col < baseTiles[row].size()) {
                tiles[row][col].pieceOnTile = &piece;
            }
        }
    }

    isWhiteTurn = true;;
}

bool Checkmate(int color, vector<vector<Tile>> &Tiles, vector<vector<Piece>> &chessPieces) {
    if (inCheck(color, Tiles, chessPieces)) {
        for (int i = 0; i < 16; ++i) {
            Piece &piece = chessPieces[color][i];
            if (!piece.isCaptured) {
                piece.generatePossibleMoves(Tiles, chessPieces);
                if (!piece.possibleMoves.empty()) {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}
int main(int argc, char *args[]) {
    // Final exit code
    int exitCode{0};

    // Initialize
    if (!init()) {
        SDL_Log("Unable to initialize program!\n");
        exitCode = 1;
    } else {

        // Load media
        if (!loadMedia(Board, Pieces)) {
            SDL_Log("Unable to load media!\n");
            exitCode = 2;
        } else {
            bool quit{false};

            // The event data
            SDL_Event e;
            SDL_zero(e);

            // The main loop
            while (!quit) {
                // Get event data
                while (SDL_PollEvent(&e)) {
                    // If event is quit type
                    if (e.type == SDL_EVENT_QUIT) {
                        // End the main loop
                        quit = true;
                    }

                    // Handle button events
                    for (auto &row : Pieces)
                        for (auto &piece : row)
                            piece.handleEvent(&e, Board, Pieces);

                    // Fill the background white
                    if (isWhiteTurn) {
                        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
                            Piece *captured = move_white(e, Board, Pieces);

                            if (Checkmate(1, Board, Pieces)) {
                                cout << "BIAŁY WYGRAŁ" << endl;
                            }
                        }
                    }
                }

                if (!isWhiteTurn) {
                    SI(Board, Pieces);
                }

                SDL_RenderClear(gRenderer);
                SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
                renderBoard(Board, Pieces);
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    return exitCode;
}