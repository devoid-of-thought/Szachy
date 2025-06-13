#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "Plansza.h"
#include "SDL/include/SDL3/SDL.h"
#include "Figury.h"
#include <unistd.h>
#include <map>
#include <iostream>
#include "Window.h"
#include <random>

using namespace std;

//Wysokość i szerokość okna
constexpr int kScreenWidth{512};
constexpr int kScreenHeight{512};

//Inicjalizacja zmiennych SDL
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

SDL_Texture *whiteTileTexture = nullptr;
SDL_Texture *blackTileTexture = nullptr;
SDL_Texture *whiteHighlightTexture = nullptr;
SDL_Texture *blackHighlightTexture = nullptr;

Piece *selectedPiece = nullptr;
Piece *temp = nullptr;
bool isWhiteTurn = true;
vector<vector<Tile> > Board(8, vector<Tile>(8));


vector<vector<Piece> > Pieces = {
    {
        Piece(0, 7, true, 500, 0), Piece(1, 7, true, 350, 1), Piece(2, 7, true, 351, 2), Piece(3, 7, true, 900, 3),
        Piece(4, 7, true, 999, 4), Piece(5, 7, true, 351, 5), Piece(6, 7, true, 350, 6), Piece(7, 7, true, 500, 7),
        Piece(0, 6, true, 100, 8), Piece(1, 6, true, 100, 9), Piece(2, 6, true, 100, 10), Piece(3, 6, true, 100, 11),
        Piece(4, 6, true, 100, 12), Piece(5, 6, true, 100, 13), Piece(6, 6, true, 100, 14), Piece(7, 6, true, 100, 15)
    },
    {
        Piece(0, 0, false, -500, 0), Piece(1, 0, false, -350, 1), Piece(2, 0, false, -351, 2),
        Piece(3, 0, false, -900, 3),
        Piece(4, 0, false, -999, 4), Piece(5, 0, false, -351, 5), Piece(6, 0, false, -350, 6),
        Piece(7, 0, false, -500, 7),
        Piece(0, 1, false, -100, 8), Piece(1, 1, false, -100, 9), Piece(2, 1, false, -100, 10),
        Piece(3, 1, false, -100, 11),
        Piece(4, 1, false, -100, 12), Piece(5, 1, false, -100, 13), Piece(6, 1, false, -100, 14),
        Piece(7, 1, false, -100, 15)
    }
};

bool init() {
    bool success{true};

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        success = false;
    } else {
        if (!SDL_CreateWindowAndRenderer("Szachy", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer)) {
            SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
            success = false;
        }
    }


    return success;
}


bool loadMedia(vector<vector<Tile> > &Tiles, vector<vector<Piece> > &chessPieces) {
    whiteTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_tile.png");
    blackTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_tile.png");
    whiteHighlightTexture = IMG_LoadTexture(
        gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_Highlight.png");
    blackHighlightTexture = IMG_LoadTexture(
        gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_Highlight.png");

    bool success{true};

    const string basePath = "/home/userbrigh/CLionProjects/Szachy/Tekstury/";
    map<int, std::string> textureFiles = {
        {500, "Chess_rlt60.png"}, {350, "Chess_nlt60.png"}, {351, "Chess_blt60.png"},
        {900, "Chess_qlt60.png"}, {999, "Chess_klt60.png"}, {100, "Chess_plt60.png"},
        {-500, "Chess_rdt60.png"}, {-350, "Chess_ndt60.png"}, {-351, "Chess_bdt60.png"},
        {-900, "Chess_qdt60.png"}, {-999, "Chess_kdt60.png"}, {-100, "Chess_pdt60.png"}
    };

    for (auto &row: chessPieces) {
        for (auto &piece: row) {
            if (textureFiles.find(piece.figure) != textureFiles.end()) {
                piece.loadFromFile(basePath + textureFiles[piece.figure]);
            }
        }
    }

    const int tileSize = 64;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Tiles[x][y].BoardPosition = {x, y};
            Tiles[x][y].mWidth = tileSize;
            Tiles[x][y].mHeight = tileSize;
            if ((x + y) % 2 != 0) {
                Tiles[x][y].mTexture = whiteTileTexture;
            } else {
                Tiles[x][y].mTexture = blackTileTexture;
            }
        }
    }
    return success;
}

template<typename T>
void close(T &PngTexture) {
    PngTexture.destroy();

    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    SDL_Quit();
}

void renderBoard(vector<vector<Tile> > &Tiles, vector<vector<Piece> > &chessPieces) {
    const int tileSize = Tiles[0][0].getHeight();
    for (int y = 7; y >= 0; --y) {
        for (int x = 7; x >= 0; --x) {
            if ((x + y) % 2) {
                Tiles[y][x].mTexture = blackTileTexture;
            } else {
                Tiles[y][x].mTexture = whiteTileTexture;
            }
            Tiles[y][x].highlightBorder = false;

            Tiles[y][x].hasPiece = false;
            Tiles[y][x].isWhite = false;
            Tiles[y][x].pieceOnTile = nullptr;

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

            Tiles[y][x].render(y * tileSize, x * tileSize);
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            Piece &p = chessPieces[i][j];
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

void Debug(vector<vector<Tile> > &Tiles) {
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
                cout << Tiles[i][j].hasPiece << " ";
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

void updateBoardStateFromPieces(vector<vector<Tile> > &tiles, vector<vector<Piece> > &chessPieces) {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            tiles[y][x].hasPiece = false;
            tiles[y][x].pieceOnTile = nullptr;
        }
    }

    for (auto &colorSet: chessPieces) {
        for (auto &p: colorSet) {
            if (!p.isCaptured) {
                int x = p.BoardPosition.first;
                int y = p.BoardPosition.second;
                if (x >= 0 && x < 8 && y >= 0 && y < 8) {
                    tiles[y][x].hasPiece = true;
                    tiles[y][x].isWhite = p.White;
                    tiles[y][x].pieceOnTile = &p;
                }
            }
        }
    }
}

void Move(Piece *piece, pair<int, int> from, pair<int, int> to, vector<vector<Tile> > &Tiles) {
    piece->BoardPosition = to;
    updateBoardStateFromPieces(Tiles, Pieces);
}


vector<Ruch> GenerateAllMoves(int color, vector<vector<Piece> > &chessPieces, vector<vector<Tile> > &Tiles) {
    vector<Ruch> Moves;
    for (int i = 0; i < 16; ++i) {
        Piece &p = chessPieces[color][i];
        if (p.isCaptured) continue;
        p.generatePossibleMoves(Tiles, chessPieces);
        vector<pair<int, int> > &possible = p.possibleMoves;
        for (int i = 0; i < possible.size(); ++i) {
            Ruch move = {&p, nullptr, p.BoardPosition, possible[i], false, 0};
            move.pieceCaptured = Tiles[possible[i].second][possible[i].first].pieceOnTile;
            Moves.emplace_back(move);
        }
    }
    return Moves;
}


bool isSquareAttacked(int x, int y, bool isEnemyWhite, vector<vector<Tile> > &tiles,
                      vector<vector<Piece> > &chessPieces) {
    int enemyColor = isEnemyWhite ? 0 : 1;

    vector<Ruch> attackerMoves = GenerateAllMoves(enemyColor, chessPieces, tiles);

    for (const Ruch &move: attackerMoves) {
        if (move.to.first == x && move.to.second == y) {
            return true;
        }
    }

    return false;
}


vector<Ruch> GenerateLegalMoves(int color, vector<vector<Piece> > &chessPieces, vector<vector<Tile> > &Tiles) {
    vector<Ruch> allPseudoMoves = GenerateAllMoves(color, chessPieces, Tiles);
    vector<Ruch> legalMoves;

    for (Ruch &testMove: allPseudoMoves) {
        pair<int, int> originalPos = testMove.pieceMoved->BoardPosition;
        Piece* capturedPiece = testMove.pieceCaptured;
        bool originalHasMoved = testMove.pieceMoved->hasMoved;
        bool capturedOriginalHasMoved = capturedPiece ? capturedPiece->hasMoved : false;


        if (capturedPiece) { capturedPiece->isCaptured = true; }
        testMove.pieceMoved->BoardPosition = testMove.to;
        testMove.pieceMoved->hasMoved = true;
        updateBoardStateFromPieces(Tiles, chessPieces);


        Piece &myKing = chessPieces[color][4];
        bool isOpponentWhite = (color == 1);
        bool isKingInCheck = isSquareAttacked(myKing.BoardPosition.first, myKing.BoardPosition.second, isOpponentWhite, Tiles, chessPieces);


        testMove.pieceMoved->BoardPosition = originalPos;
        testMove.pieceMoved->hasMoved = originalHasMoved;
        if (capturedPiece) {
            capturedPiece->isCaptured = false;
            capturedPiece->hasMoved = capturedOriginalHasMoved;
        }
        updateBoardStateFromPieces(Tiles, chessPieces);


        if (!isKingInCheck) {
            legalMoves.push_back(testMove);
        }
    }
    return legalMoves;
}

void Capture(Piece *piece, int i, pair<int, int> clickedPosition, vector<vector<Tile> > &Tiles,
             vector<vector<Piece> > &chessPieces) {
    int enemyColor = (piece->White) ? 1 : 0;
    chessPieces[enemyColor][i].isCaptured = true;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].hasPiece = false;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].isWhite = false;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].pieceOnTile = nullptr;
    Move(piece, piece->BoardPosition, clickedPosition, Tiles);
    chessPieces[enemyColor][i].BoardPosition = {8, 8};
}


void move_black(Ruch move, vector<vector<Tile> > &Tiles, vector<vector<Piece> > &chessPieces) {
    Piece* pieceToMove = move.pieceMoved;
    pair<int, int> to = move.to;

    bool isCastling = (pieceToMove->figure == -999 && abs(to.first - pieceToMove->BoardPosition.first) == 2);
    if (isCastling) {
        if (to.first == 6) {
            Piece* rook = Tiles[0][7].pieceOnTile;
            if (rook) {
                Move(rook, rook->BoardPosition, {5, 0}, Tiles);
                rook->hasMoved = true;
            }
        } else if (to.first == 2) {
            Piece* rook = Tiles[0][0].pieceOnTile;
            if (rook) {
                Move(rook, rook->BoardPosition, {3, 0}, Tiles);
                rook->hasMoved = true;
            }
        }
    }

    if (move.pieceCaptured) {
        move.pieceCaptured->isCaptured = true;
    }

    Move(pieceToMove, pieceToMove->BoardPosition, to, Tiles);
    pieceToMove->hasMoved = true;

    Promotion(pieceToMove);

    isWhiteTurn = true;
}

int evaluate(vector<vector<Piece> > &chessPieces) {
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

std::pair<vector<vector<Piece> >, vector<vector<Tile> > >
createBoardStateCopy(const vector<vector<Piece> > &originalPieces, const vector<vector<Tile> > &originalTiles) {
    vector<vector<Piece> > newPieces = originalPieces;
    vector<vector<Tile> > newTiles = originalTiles;

    for (auto &colorSet: newPieces) {
        for (auto &piece: colorSet) {
            if (!piece.isCaptured) {
                int x = piece.BoardPosition.first;
                int y = piece.BoardPosition.second;
                if (x >= 0 && x < 8 && y >= 0 && y < 8) {
                    newTiles[y][x].pieceOnTile = &piece;
                }
            }
        }
    }
    return {newPieces, newTiles};
}

void UndoMove(Ruch ruch, vector<vector<Tile> > &Tiles, vector<vector<Piece> > &chessPieces) {
    Piece *piece = ruch.pieceMoved;
    pair<int, int> lastCapturedPos = piece->BoardPosition;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = false;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].isWhite = false;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = nullptr;

    piece->BoardPosition = ruch.from;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].hasPiece = true;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].pieceOnTile = piece;
    Tiles[piece->BoardPosition.second][piece->BoardPosition.first].isWhite = piece->White;

    Piece *captured = ruch.pieceCaptured;
    if (captured != nullptr) {
        captured->isCaptured = false;
        auto [cx, cy] = ruch.to;
        Tiles[cy][cx].hasPiece = true;
        Tiles[cy][cx].isWhite = captured->White;
        Tiles[cy][cx].pieceOnTile = captured;
    }

    if (ruch.wasPromoted) {
        Unpromote(piece);
    }
}

vector<Ruch> BESTMOVES;
int INF = 10000000;

int negaMax(int color, int alpha, int beta, int depthLeft, vector<vector<Piece> > &chessPieces,
            vector<vector<Tile> > &tiles) {
    if (depthLeft == 0) {
        return evaluate(chessPieces);
    }

    vector<Ruch> moves = GenerateLegalMoves(color, chessPieces, tiles);
    if (moves.empty()) {
        return -1000000;
    }

    for (Ruch &m: moves) {
        Piece *captured = m.pieceCaptured;
        if (captured) captured->isCaptured = true;
        Move(m.pieceMoved, m.from, m.to, tiles);

        int score = -negaMax(1 - color, -beta, -alpha, depthLeft - 1, chessPieces, tiles);

        Move(m.pieceMoved, m.to, m.from, tiles);
        if (captured) {
            captured->isCaptured = false;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].pieceOnTile = captured;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].hasPiece = true;
        }

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}


void SI(vector<vector<Tile> > &tiles, vector<vector<Piece> > &chessPieces) {
    const int MAX_DEPTH = 3;
    int rootColor = 1;

    vector<Ruch> possibleMoves = GenerateLegalMoves(rootColor, chessPieces, tiles);

    int bestScore = -9999999;
    vector<Ruch> bestMovesList;

    for (Ruch &m: possibleMoves) {
        Piece *captured = m.pieceCaptured;
        if (captured) captured->isCaptured = true;
        Move(m.pieceMoved, m.from, m.to, tiles);

        int score = -negaMax(0, -9999999, 9999999, MAX_DEPTH - 1, chessPieces, tiles);

        Move(m.pieceMoved, m.to, m.from, tiles);
        if (captured) {
            captured->isCaptured = false;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].pieceOnTile = captured;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].hasPiece = true;
        }

        if (score > bestScore) {
            bestScore = score;
            bestMovesList.clear();
            bestMovesList.push_back(m);
        } else if (score == bestScore) {
            bestMovesList.push_back(m);
        }
    }

    Ruch chosenMove = bestMovesList[0];
    if (!bestMovesList.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dst(0, bestMovesList.size() - 1);

        chosenMove = bestMovesList[dst(gen)];
    }

    cout << "AI found " << bestMovesList.size() << " equally good move(s) with a score of: " << bestScore << endl;
    cout << "Chosen move: " << chosenMove.pieceMoved->figure << " from "
            << chosenMove.from.first << "," << chosenMove.from.second << " to "
            << chosenMove.to.first << "," << chosenMove.to.second << endl;
    move_black(chosenMove, tiles, chessPieces);

}



bool isCheckmate(int color, vector<vector<Piece> > &chessPieces, vector<vector<Tile> > &Tiles) {
    Piece &myKing = chessPieces[color][4];
    bool opponentIsWhite = (color == 1);

    if (!isSquareAttacked(myKing.BoardPosition.first, myKing.BoardPosition.second, opponentIsWhite, Tiles,
                          chessPieces)) {
        return false;
    }

    vector<Ruch> legalMoves = GenerateLegalMoves(color, chessPieces, Tiles);
    return legalMoves.empty();
}

bool isStalemate(int color, vector<vector<Piece> > &chessPieces, vector<vector<Tile> > &Tiles) {
    Piece &myKing = chessPieces[color][4];
    bool opponentIsWhite = (color == 1);

    if (isSquareAttacked(myKing.BoardPosition.first, myKing.BoardPosition.second, opponentIsWhite, Tiles,
                         chessPieces)) {
        return false;
    }

    vector<Ruch> legalMoves = GenerateLegalMoves(color, chessPieces, Tiles);
    return legalMoves.empty();
}



int main(int argc, char *args[]) {
    if (!init()) return 1;
    if (!loadMedia(Board, Pieces)) return 2;

    bool quit{false};
    bool gameOver{false};

    SDL_Event e;
    SDL_zero(e);

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }

            if (!gameOver && isWhiteTurn) {
                if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
                    int tileSize = 64;
                    int clickedX = e.button.x / tileSize;
                    int clickedY = e.button.y / tileSize;

                    if (clickedX >= 0 && clickedX < 8 && clickedY >= 0 && clickedY < 8) {
                        pair<int, int> clickedPos = {clickedX, clickedY};

                        if (selectedPiece) {
                            bool isValidMove = false;
                            for (const auto &move: selectedPiece->possibleMoves) {
                                if (clickedPos == move) {
                                    isValidMove = true;
                                    break;
                                }
                            }

                            if (isValidMove) {
                                pair<int, int> originalPos = selectedPiece->BoardPosition;
                                Piece *targetPiece = Board[clickedY][clickedX].pieceOnTile;

                                bool isCastlingAttempt = (
                                    selectedPiece->figure == 999 && targetPiece && targetPiece->figure == 500 &&
                                    targetPiece->White == selectedPiece->White);

                                if (isCastlingAttempt) {
                                    if (targetPiece->BoardPosition.first == 7) {
                                        Move(selectedPiece, originalPos, {6, originalPos.second}, Board);
                                        Move(targetPiece, targetPiece->BoardPosition, {5, originalPos.second},
                                             Board);
                                        selectedPiece->hasMoved = true;
                                        targetPiece->hasMoved = true;
                                    } else if (targetPiece->BoardPosition.first == 0) {
                                        Move(selectedPiece, originalPos, {2, originalPos.second}, Board);
                                        Move(targetPiece, targetPiece->BoardPosition, {3, originalPos.second},
                                             Board);
                                        selectedPiece->hasMoved = true;
                                        targetPiece->hasMoved = true;
                                    }

                                    isWhiteTurn = false;
                                    selectedPiece = nullptr;
                                } else {
                                    if (targetPiece) { targetPiece->isCaptured = true; }
                                    Move(selectedPiece, originalPos, clickedPos, Board);

                                    selectedPiece->hasMoved = true;

                                    Piece &whiteKing = Pieces[0][4];
                                    bool isKingInCheck = isSquareAttacked(
                                        whiteKing.BoardPosition.first, whiteKing.BoardPosition.second, false, Board,
                                        Pieces);

                                    if (isKingInCheck) {
                                        cout << "Illegal move: You cannot leave your king in check." << endl;

                                        selectedPiece->BoardPosition = originalPos;

                                        if (targetPiece) { targetPiece->isCaptured = false; }
                                        updateBoardStateFromPieces(Board, Pieces);
                                        selectedPiece = nullptr;
                                    } else {
                                        Promotion(selectedPiece);
                                        isWhiteTurn = false;
                                        selectedPiece = nullptr;
                                    }
                                }
                            } else {
                                selectedPiece = nullptr;
                            }

                        } else {
                            Piece *clickedPiece = Board[clickedY][clickedX].pieceOnTile;
                            if (clickedPiece && clickedPiece->White) {
                                selectedPiece = clickedPiece;
                                vector<Ruch> whiteLegalMoves = GenerateLegalMoves(0, Pieces, Board);
                                selectedPiece->possibleMoves.clear();
                                for (const auto &legalMove: whiteLegalMoves) {
                                    if (legalMove.pieceMoved == selectedPiece) {
                                        selectedPiece->possibleMoves.push_back(legalMove.to);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!gameOver) {
            if (!isWhiteTurn) {
                if (isCheckmate(1, Pieces, Board)) {
                    cout << "CHECKMATE! White wins!" << endl;
                    gameOver = true;
                } else if (isStalemate(1, Pieces, Board)) {
                    cout << "STALEMATE! It's a draw." << endl;
                    gameOver = true;
                }

                if (!gameOver) {
                    SI(Board, Pieces);
                    if (isCheckmate(0, Pieces, Board)) {
                        cout << "CHECKMATE! Black wins!" << endl;
                        gameOver = true;
                    } else if (isStalemate(0, Pieces, Board)) {
                        cout << "STALEMATE! It's a draw." << endl;
                        gameOver = true;
                    }
                }
            }
        }
        SDL_RenderClear(gRenderer);
        SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
        renderBoard(Board, Pieces);
        SDL_RenderPresent(gRenderer);
        if (gameOver) {
            SDL_Delay(9999);
            SDL_Quit();
            return 0;
        }
    }
}