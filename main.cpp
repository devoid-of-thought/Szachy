#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "Plansza.h"
#include "SDL_ttf/include/SDL3_ttf/SDL_ttf.h"
#include "SDL/include/SDL3/SDL.h"
#include "Figury.h"
#include <unistd.h>
#include <map>
#include <iostream>
#include "Window.h"
#include <random>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

//Wysokość i szerokość okna
constexpr int kScreenWidth{512};
constexpr int kScreenHeight{512};

//Inicjalizacja okna SDl
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
TTF_Font* gFont{ nullptr };

//Inicjalizacja tekstur
SDL_Texture *whiteTileTexture = nullptr;
SDL_Texture *blackTileTexture = nullptr;
SDL_Texture *whiteHighlightTexture = nullptr;
SDL_Texture *blackHighlightTexture = nullptr;

//Inicjalizacja globalnych zmiennych
Piece *selectedPiece = nullptr;
Piece *temp = nullptr;
bool isWhiteTurn = true;

//Inicjalizacja planszy i figur
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

//Funkja inicjalizująca SDL i tworząca okno
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
SDL_Texture* createTextTexture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,int leng, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(),leng, color);
    if (!surface) {
        SDL_Log("Failed to create text surface: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
    }

    SDL_DestroySurface(surface); // We no longer need the surface
    return texture;
}
//Załadowanie tekstur

bool loadMedia(vector<vector<Tile> > &Tiles, vector<vector<Piece> > &chessPieces) {
    //Załadowanie tekstur planszy
    whiteTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_tile.png");
    blackTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_tile.png");
    whiteHighlightTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_Highlight.png");
    blackHighlightTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_Highlight.png");

    bool success{true};

    //wczytanie tekstur figur szachowych
    const string basePath = "/home/userbrigh/CLionProjects/Szachy/Tekstury/";
    map<int, std::string> textureFiles = {
        {500, "Chess_rlt60.png"}, {350, "Chess_nlt60.png"}, {351, "Chess_blt60.png"},
        {900, "Chess_qlt60.png"}, {999, "Chess_klt60.png"}, {100, "Chess_plt60.png"},
        {-500, "Chess_rdt60.png"}, {-350, "Chess_ndt60.png"}, {-351, "Chess_bdt60.png"},
        {-900, "Chess_qdt60.png"}, {-999, "Chess_kdt60.png"}, {-100, "Chess_pdt60.png"}
    };

    //przydzielenie tekstur do figur szachowych
    for (auto &row: chessPieces) {
        for (auto &piece: row) {
            if (textureFiles.find(piece.figure) != textureFiles.end()) {
                piece.loadFromFile(basePath + textureFiles[piece.figure]);
            }
        }
    }
    //Inicjalizacja planszy
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
//funkcja renderująca planszę i figury szachowe
void renderBoard(vector<vector<Tile> > &Tiles, vector<vector<Piece> > &chessPieces) {
    const int tileSize = Tiles[0][0].getHeight();
    //czyszczenie atrybutów kafelków
    for (int y = 7; y >= 0; --y) {
        for (int x = 7; x >= 0; --x) {
            if ((x + y) % 2) {
                Tiles[y][x].mTexture = blackTileTexture;
            } else {
                Tiles[y][x].mTexture = whiteTileTexture;
            }
            Tiles[y][x].hasPiece = false;
            Tiles[y][x].isWhite = false;
            Tiles[y][x].pieceOnTile = nullptr;
            //jeśli jest zaznaczona figura, to podświetlenie możliwych ruchów
            if (selectedPiece) {
                for (auto &move: selectedPiece->possibleMoves) {
                    if (move.first == y && move.second == x) {
                        Tiles[y][x].mTexture =((x + y) % 2)? blackHighlightTexture: whiteHighlightTexture;
                        break;
                    }
                }
            }
            //renderowanie kafelków
            Tiles[y][x].render(y * tileSize, x * tileSize);
        }
    }
    //renderowanie figur szachowych
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            Piece &p = chessPieces[i][j];
            if (!p.isCaptured && p.BoardPosition.first < 8 && p.BoardPosition.second < 8) {
                int tx = p.BoardPosition.first;
                int ty = p.BoardPosition.second;
                p.render(tx * tileSize, ty * tileSize);
                //przywrócenie wiadomości o figurzach na kafelkach
                Tiles[ty][tx].hasPiece = true;
                Tiles[ty][tx].pieceOnTile = &p;
                Tiles[ty][tx].isWhite = p.White;
            }
        }
    }
}
//Wyśiwietlenie planszy i figur szachowych w celu debugowania
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
//Degradacja figury w przypadku cofnięcia ruchu
void Unpromote(Piece *piece) {
    if (piece->figure == 900 && (piece->BoardPosition.second == 0 && piece->White)) {
        piece->figure = 100;
        piece->wasPromoted = false;
        piece->loadFromFile(
            "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png");
    } else if (piece->figure == -900 && (piece->BoardPosition.second == 7 && !piece->White)) {
        piece->figure = -100;
        piece->wasPromoted = false;
        piece->loadFromFile(
            "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png");
    }
}

//Promocja pionka w hetmana
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
//Aktualizacja stanu planszy w przypadku zmiany pozycji figur
void updateBoardStateFromPieces(vector<vector<Tile> > &tiles, vector<vector<Piece> > &chessPieces) {
    //czyszczenie tablicy
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            tiles[y][x].hasPiece = false;
            tiles[y][x].isWhite = false;
            tiles[y][x].pieceOnTile = nullptr;
        }
    }

    for (auto &color: chessPieces) {
        for (auto &p: color) {
            //jeżeli figura nie jest zbita, to zaktualizowanie kafelka, na którym się znajduje
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
//Funkcja wykonująca ruch figury na planszy
void Move(Piece *piece, pair<int, int> from, pair<int, int> to, vector<vector<Tile> > &Tiles) {
    piece->BoardPosition = to;
    updateBoardStateFromPieces(Tiles, Pieces);
}

//generacja wszystkich możliwych ruchów dla danego koloru
vector<Ruch> GenerateAllMoves(int color, vector<vector<Piece> > &chessPieces, vector<vector<Tile> > &Tiles) {
    vector<Ruch> Moves;
    // Dla każdej figury generowanie możliwych ruchów
    for (int i = 0; i < 16; ++i) {
        Piece &p = chessPieces[color][i];
        //jeżeli figura jest zbita, to pomijamy ją
        if (p.isCaptured) continue;
        //generujemy możliwe ruchy dla danej figury
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

//Sprawdzenie czy dany kafelek jest atakowany przez przeciwnika
bool isSquareAttacked(int x, int y, bool isEnemyWhite, vector<vector<Tile> > &tiles,vector<vector<Piece> > &chessPieces) {
    // wybranie koloru przeciwnika i wygenerowanie dla niego wszystkich możliwych ruchów
    int enemyColor = isEnemyWhite ? 0 : 1;
    vector<Ruch> attackerMoves = GenerateAllMoves(enemyColor, chessPieces, tiles);
    // sprawdzenie czy któryś z ruchów przeciwnika prowadzi do danego kafelka
    for (const Ruch &move: attackerMoves) {
        if (move.to.first == x && move.to.second == y) {
            return true;
        }
    }
    return false;
}

//Generowanie legalnych ruchów dla danego koloru
vector<Ruch> GenerateLegalMoves(int color, vector<vector<Piece> > &chessPieces, vector<vector<Tile> > &Tiles) {
    //wygenerowanie wszystkich możliwych ruchów dla danego koloru
    vector<Ruch> allMoves = GenerateAllMoves(color, chessPieces, Tiles);
    vector<Ruch> legalMoves;
    //sprawdzenie czy dany ruch nie prowadzi do szacha króla
    for (Ruch &testMove: allMoves) {
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
        //przywrócenie stanu planszy i figur
        testMove.pieceMoved->BoardPosition = originalPos;
        testMove.pieceMoved->hasMoved = originalHasMoved;
        if (capturedPiece) {
            capturedPiece->isCaptured = false;
            capturedPiece->hasMoved = capturedOriginalHasMoved;
        }
        updateBoardStateFromPieces(Tiles, chessPieces);

        //jeżeli ruch nie prowadzi do szacha króla, to dodajemy go do listy legalnych ruchów
        if (!isKingInCheck) {
            legalMoves.push_back(testMove);
        }
    }
    return legalMoves;
}
//Zbice figury przeciwnika
void Capture(Piece *piece, int i, pair<int, int> clickedPosition, vector<vector<Tile> > &Tiles,vector<vector<Piece> > &chessPieces) {
    // Sprawdzenie koloru przeciwnika
    int enemyColor = (piece->White) ? 1 : 0;
    //Aktualizacja kafelka zbitej figury
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].hasPiece = false;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].isWhite = false;
    Tiles[chessPieces[enemyColor][i].BoardPosition.second][chessPieces[enemyColor][i].
        BoardPosition.first].pieceOnTile = nullptr;
    //przesunięcie naszej figury na kafelek docelowy
    Move(piece, piece->BoardPosition, clickedPosition, Tiles);
    //zmienienie pozycji zbitej figury na 8,8 i usunięcie jej z planszy
    chessPieces[enemyColor][i].isCaptured = true;
    chessPieces[enemyColor][i].BoardPosition = {8, 8};
}

//ruch w ruchu ai
void move_black(Ruch move, vector<vector<Tile> > &Tiles, vector<vector<Piece>> &chessPieces) {
    Piece* pieceToMove = move.pieceMoved;
    pair<int, int> to = move.to;
    //sprawdzenie czy ai wykonał roszadę
    bool isCastling = (pieceToMove->figure == -999 && (7 == to.first || 0 == to.first)&&pieceToMove->hasMoved==false);
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
    //jeżeli ai wykonał zbicie figury przeciwnika, to zmieniamy jej status na zbita
    if (move.pieceCaptured) {
        move.pieceCaptured->isCaptured = true;
    }
    //przesunięcie figury na kafelek docelowy
    Move(pieceToMove, pieceToMove->BoardPosition, to, Tiles);
    pieceToMove->hasMoved = true;
    //sprawdzenie czy pionek, jest promowany
    Promotion(pieceToMove);
    //koniec tury
    isWhiteTurn = true;
}
//zwrócenie stanu planszy
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
int leaf = 0;
//algorytm minmax z alpha beta pruning
int negaMax(int color, int alpha, int beta, int depthLeft, vector<vector<Piece> > &chessPieces,vector<vector<Tile> > &tiles) {
    //jeżeli osiągnięto maksymalną głębokość lub ruch prowadzi do szach mata, to zwrócenie wartości planszy
    if (depthLeft == 0) {
        leaf++;
        return evaluate(chessPieces);
    }
    vector<Ruch> moves = GenerateLegalMoves(color, chessPieces, tiles);
    if (moves.empty()) {
        return -1000000;
    }
    //Dla każdego możliwego ruchu wykonanie ruchu, a następnie rekurencyjne wywołanie funkcji negaMax
    for (Ruch &m: moves) {
        Piece *captured = m.pieceCaptured;
        if (captured) captured->isCaptured = true;
        //wykonanie ruchu
        Move(m.pieceMoved, m.from, m.to, tiles);
        //sprawdzenie jaki potencjał ma ruch
        int score = -negaMax(1 - color, -beta, -alpha, depthLeft - 1, chessPieces, tiles);
        //cofnięcie ruchu
        Move(m.pieceMoved, m.to, m.from, tiles);
        if (captured) {
            captured->isCaptured = false;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].pieceOnTile = captured;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].hasPiece = true;
        }
    }
}

//Sztuczna inteligencja dla czarnykh figur
Ruch SI(int DEPTH, vector<vector<Tile> > &tiles, vector<vector<Piece> > &chessPieces) {
    //wygenerowanie wszystkich możliwych ruchów z obecnej pozycji
    vector<Ruch> possibleMoves = GenerateLegalMoves(1, chessPieces, tiles);
    //inicjalizacja najlepszego znalezionego ruchu i
    int bestScore = -9999999;
    vector<Ruch> bestMovesList;
    //sprawdzenie, każdego możliwego ruchu
    for (Ruch &m: possibleMoves) {
        Piece *captured = m.pieceCaptured;
        if (captured) captured->isCaptured = true;
        //wykonanie ruchu
        Move(m.pieceMoved, m.from, m.to, tiles);
        //sprawdzenie jaki potencjał ma ruch
        int score = -negaMax(0, -9999999, 9999999, DEPTH - 1, chessPieces, tiles);
        //cofnięcie ruchu
        Move(m.pieceMoved, m.to, m.from, tiles);
        if (captured) {
            captured->isCaptured = false;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].pieceOnTile = captured;
            tiles[captured->BoardPosition.second][captured->BoardPosition.first].hasPiece = true;
        }
        //jeżeli potencjał ruchu jest większy niż najlepszy znaleziony ruch, to aktualizacja najlepszego ruchu
        if (score > bestScore) {
            bestScore = score;
            bestMovesList.clear();
            bestMovesList.push_back(m);
        } else if (score == bestScore) {
            //jeżeli potencjał ruchu jest równy najlepszym znalezionym ruchom, to dodanie go do listy najlepszych ruchów
            bestMovesList.push_back(m);
        }
    }
    //Wybranie losowego ruchu z listy najlepszych ruchów
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dst(0, bestMovesList.size() - 1);
    Ruch chosenMove = bestMovesList[dst(gen)];
    //wykonanie wybranego ruchu
    move_black(chosenMove, tiles, chessPieces);
    return chosenMove;
}

//Sprawdzenie czy jest szach mat lub pat
bool isCheckmate(int color, vector<vector<Piece> > &chessPieces, vector<vector<Tile> > &Tiles) {
    Piece &myKing = chessPieces[color][4];
    bool opponentIsWhite = (color == 1);

    if (!isSquareAttacked(myKing.BoardPosition.first, myKing.BoardPosition.second, opponentIsWhite, Tiles,chessPieces)) {
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
void FEN(string &text,vector<vector<Tile>> &tiless, vector<vector<Piece>> &piecee) {
    map<char, int> fenToValue = {
        {'P', 100}, {'N', 350}, {'B', 351}, {'R', 500}, {'Q', 900}, {'K', 999},
        {'p', -100}, {'n', -350}, {'b', -351}, {'r', -500}, {'q', -900}, {'k', -999}
    };
    vector<string> rows;
    string boardPart;
    string row="";
    for (auto& pieceRow : piecee) {
        for (auto& piece : pieceRow) {
            piece.BoardPosition = {8, 8};
            piece.isCaptured = true;
            piece.hasMoved = false;
        }
    }
    for (auto& tileRow : tiless) {
        for (auto& tile : tileRow) {
            tile.pieceOnTile = nullptr;
            tile.hasPiece = false;
            tile.isWhite = false;
        }
    }
    for (char ch : text) {
        if (ch == '/') {
            rows.push_back(row);
            row.clear();
        } else if (isdigit(ch)) {
            row.append(ch - '0', '0');
        } else {
            row += ch;
        }
        if (ch== ' ') {
            rows.push_back(row);
            break;
        }
    }
    for(int i = 0; i<8;i++) {
     for(int j = 0; j<8;j++) {
         char p = rows[i][j];
         if (p==0) continue;
         int val = fenToValue[p];
         for (auto& pieceRow : piecee) {
             for (auto& piece : pieceRow) {
                 if (piece.figure == val && !piece.hasMoved) {
                        piece.BoardPosition = {j, i};
                        piece.isCaptured = false;
                        piece.hasMoved = true;
                        tiless[i][j].pieceOnTile = &piece;
                        tiless[i][j].hasPiece = true;
                        tiless[i][j].isWhite = (val > 0);
                        break;
                 }
             }
         }
     }
    }
}
void count_leaves(int depth,string fen) {
    FEN(fen, Board, Pieces);
    cout << "--- Liście na głębokości " << depth << " ---" << endl;
    leaf = 0;
    SI(depth,Board, Pieces);
    cout << "Głębokość: " << depth << " | ilość liści: " << leaf << endl;
    cout << "---------------------------------" << endl;
}
void game(int depth) {
    //inicjalizacja zmiennych końca gry
    bool quit{false};
    bool gameOver{false};
    //obsługa zdarzeń SDL
    SDL_Event e;
    SDL_zero(e);
    string fen = "r3r2k/8/8/4r3/8/8/8/R3K2R w KQkq - 0 1";
    FEN(fen, Board, Pieces);
    for (auto& pieceRow : Pieces) {
        for (auto& piece : pieceRow) {
            piece.hasMoved = false;
        }
    }

    while (!quit) {
        //dopóki gra trwa, sprawdzanie zdarzeń
        while (SDL_PollEvent(&e)) {
            //jeżeli zdarzenie to zdarzenie zamknięcia okna, to kończymy grę
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            //ruch białych figur
            if (!gameOver && isWhiteTurn) {
                //jeżeli kliknięcie myszką, to sprawdzenie wciśniętego kafelka
                if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
                    int tileSize = 64;
                    int clickedX = e.button.x / tileSize;
                    int clickedY = e.button.y / tileSize;
                    pair<int, int> clickedPos = {clickedX, clickedY};
                    //sprawdzenie czy kliknięty kafelek jest w zakresie planszy
                    if (clickedX >= 0 && clickedX < 8 && clickedY >= 0 && clickedY < 8) {
                        //jeżeli jest zaznaczona figura, to sprawdzenie czy kliknięty kafelek jest jednym z możliwych ruchów
                        if (selectedPiece) {
                            bool isValidMove = false;
                            for (const auto &move: selectedPiece->possibleMoves) {
                                if (clickedPos == move) {
                                    isValidMove = true;
                                    break;
                                }
                            }
                            //jeżeli kliknięty kafelek jest jednym z możliwych ruchów, to wykonanie ruchu
                            if (isValidMove) {
                                //zapisanie orginalnych pozycji figur
                                pair<int, int> originalPos = selectedPiece->BoardPosition;
                                Piece *targetPiece = Board[clickedY][clickedX].pieceOnTile;
                                //jeżeli wykonujemy roszadę, to wykonanie roszad
                                bool isCastlingAttempt = (selectedPiece->figure == 999 && targetPiece && targetPiece->figure == 500 && targetPiece->White == selectedPiece->White);
                                if (isCastlingAttempt) {
                                    if (targetPiece->BoardPosition.first == 7&& !isSquareAttacked(selectedPiece->BoardPosition.first,selectedPiece->BoardPosition.second,false,Board,Pieces)&&!isSquareAttacked(selectedPiece->BoardPosition.first+1,selectedPiece->BoardPosition.second,false,Board,Pieces)&&!isSquareAttacked(selectedPiece->BoardPosition.first+2,selectedPiece->BoardPosition.second,false,Board,Pieces)) {
                                        Move(selectedPiece, originalPos, {6, originalPos.second}, Board);
                                        Move(targetPiece, targetPiece->BoardPosition, {5, originalPos.second},
                                             Board);
                                        selectedPiece->hasMoved = true;
                                        targetPiece->hasMoved = true;
                                        isWhiteTurn = false;
                                        selectedPiece = nullptr;

                                    } else if (targetPiece->BoardPosition.first == 0&& !isSquareAttacked(selectedPiece->BoardPosition.first,selectedPiece->BoardPosition.second,false,Board,Pieces)&&!isSquareAttacked(selectedPiece->BoardPosition.first-1,selectedPiece->BoardPosition.second,false,Board,Pieces)&&!isSquareAttacked(selectedPiece->BoardPosition.first-2,selectedPiece->BoardPosition.second,false,Board,Pieces)) {
                                        Move(selectedPiece, originalPos, {2, originalPos.second}, Board);
                                        Move(targetPiece, targetPiece->BoardPosition, {3, originalPos.second},
                                             Board);
                                        selectedPiece->hasMoved = true;
                                        targetPiece->hasMoved = true;
                                        isWhiteTurn = false;
                                        selectedPiece = nullptr;
                                    }

                                } else {
                                    //wykonanie ruchu i sprawdzenie czy nie zostawiamy króla w szachu
                                    if (targetPiece) { targetPiece->isCaptured = true; }
                                    Move(selectedPiece, originalPos, clickedPos, Board);
                                    selectedPiece->hasMoved = true;
                                    Piece &whiteKing = Pieces[0][4];
                                    bool isKingInCheck = isSquareAttacked(
                                        whiteKing.BoardPosition.first, whiteKing.BoardPosition.second, false, Board,
                                        Pieces);
                                    //jeżeli król w szachu to cofnięcie ruchu i wyświetlenie komunikatu
                                    if (isKingInCheck) {
                                        cout << "RUCH NIEPOPRAWNY: KRÓL W SZACHU." << endl;
                                        selectedPiece->BoardPosition = originalPos;
                                        if (targetPiece) { targetPiece->isCaptured = false; }
                                        updateBoardStateFromPieces(Board, Pieces);
                                        selectedPiece = nullptr;
                                    } else {
                                        //jeżeli ruch jest poprawny, to sprawdzenie czy pionek jest promowany
                                        Promotion(selectedPiece);
                                        isWhiteTurn = false;
                                        selectedPiece = nullptr;
                                    }
                                }
                            } else {
                                //odznaczeni figury, jeżeli kliknięty kafelek nie jest jednym z możliwych ruchów
                                selectedPiece = nullptr;
                            }
                        } else {
                            //jeżeli nie ma zaznaczonej figury, to sprawdzenie czy kliknięty kafelek zawiera białą figurę
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
            //ruch czarnych figur
            if (!isWhiteTurn) {
                //sprawdzenie czy czarne figury mają ruch
                if (isCheckmate(1, Pieces, Board)) {
                    cout << "CHECKMATE! White wins!" << endl;
                    gameOver = true;
                } else if (isStalemate(1, Pieces, Board)) {
                    cout << "STALEMATE! It's a draw." << endl;
                    gameOver = true;
                }
                //wykonanie ruchu przez algorytm sztucznej inteligencji
                if (!gameOver) {
                    SI(depth,Board, Pieces);
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
        //renderowanie planszy i figur
        SDL_RenderClear(gRenderer);
        SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
        renderBoard(Board, Pieces);
        SDL_RenderPresent(gRenderer);
        if (gameOver) {
            SDL_Delay(9999);
            SDL_Quit();
            return;
        }
    }
}
void test(int depth,int runs_per_test ) {
    map<string, string> fens_to_test = {
        {"Starting", "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"},
        {"Developed", "rn2kb1r/pp3ppp/2p1pn2/q3Nb2/2BP2P1/2N5/PPP2P1P/R1BQK2R b KQkq - 0 8"},
        {"Middlegame", "1r3rk1/1pp2ppp/p7/2PPQ3/2n1P3/6P1/5PBP/3R2K1 b - - 0 26"},
        {"Endgame", "b4r2/p3R1PP/1p5k/2p4P/1P1p2n1/8/P7/6K1 b - - 0 48"}
    };
    ofstream results_file("/home/userbrigh/CLionProjects/Szachy/szachy_test_depth_" + to_string(depth) + "_3.csv");
    ofstream avg_file("/home/userbrigh/CLionProjects/Szachy/szachy_avg_depth_" + to_string(depth) + "_3.csv");
    if (!results_file.is_open()) {
        cerr << "Error: Could not open results file for writing." << endl;
        return;
    }
    results_file << "Position,Depth,Run,ExecutionTime_ms\n";
    for (const auto& fen_pair : fens_to_test) {
        string position_name = fen_pair.first;
        string fen_string = fen_pair.second;

        cout << "\n--- Testing Position: " << position_name << " ---" << endl;
        cout << "  Testing Depth: " << depth << " (x" << runs_per_test << " runs)... " << flush;

            vector<long long> run_times;
            long long total_duration_ms = 0;

            for (int run = 1; run <= runs_per_test; ++run) {
                FEN(fen_string, Board, Pieces);
                stringstream ss(fen_string);
                string part;
                ss >> part;
                ss >> part;
                isWhiteTurn = (part == "w");
                auto start_time = chrono::high_resolution_clock::now();
                Ruch ruch = SI(depth, Board, Pieces);
                auto end_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
                run_times.push_back(duration);
                total_duration_ms += duration;
                results_file << position_name << "," << depth << "," << run << "," << duration<<"," << ruch.pieceMoved->figure<<","<<ruch.from.first<<","<<ruch.from.second<<","<<ruch.to.first<<","<<ruch.to.second<<","<<ruch.value<<"\n";
                cout << "Run " << run << ": " << duration << " ms" << endl;
                }
            double average_time = static_cast<double>(total_duration_ms) / runs_per_test;
            cout << "Done. Average Time: " << average_time << " ms" << endl;
            avg_file<< position_name << "," << depth << ","<< average_time << endl;
        }



    results_file.close();
    cout << "\nBenchmark complete. Results have been saved" << endl;
    cout << "Press Enter to continue..." << endl;
}
int runMenu(SDL_Renderer* renderer) {
    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf could not initialize! TTF_Error: %s\n", SDL_GetError());
        return 0;
    }
    TTF_Font* font = TTF_OpenFont("/home/userbrigh/CLionProjects/Szachy/Tekstury/Roboto-VariableFont_wdth,wght.ttf", 32);
    if (!font) {
        SDL_Log("Failed to load font! TTF_Error: %s\n", SDL_GetError());
        return 0;
    }
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color buttonColor = { 25, 25, 25, 255 };
    int currentDepth = 3;
    bool menuRunning = true;
    SDL_Event e;
    SDL_Texture* titleTexture = createTextTexture(renderer, font, "Wybierz głębokość: ", 0, textColor);
    SDL_Texture* recTexture = createTextTexture(renderer, font, "Rekomendowana 3 ", 0, textColor);
    SDL_Texture* startTexture = createTextTexture(renderer, font, "Start", 0, textColor);
    SDL_Texture* plusTexture = createTextTexture(renderer, font, "+", 0, textColor);
    SDL_Texture* minusTexture = createTextTexture(renderer, font, "-", 0, textColor);
    SDL_Texture* depthValueTexture = createTextTexture(renderer, font, to_string(currentDepth), 0, textColor);
    SDL_Texture* testTexture = createTextTexture(renderer, font, "Testy złożoności", 0, textColor);
    SDL_FRect titleRect = { (float)kScreenWidth / 2 - 150, 50.0f, 300.0f, 50.0f };
    SDL_FRect recRect = { (float)kScreenWidth / 2 - 50, 150.0f, 100.0f, 25.0f };
    SDL_FRect minusRect = { (float)kScreenWidth / 2 - 100, 225.0f, 30.0f, 30.0f };
    SDL_FRect depthRect = { (float)kScreenWidth / 2 - 25, 200.0f, 50.0f, 70.0f };
    SDL_FRect plusRect = { (float)kScreenWidth / 2 + 50, 225.0f, 30.0f, 30.0f };
    SDL_FRect startRect = { (float)kScreenWidth / 2 - 50, 350.0f, 100.0f, 40.0f };
    SDL_FRect testRect = { (float)kScreenWidth / 2 + 40, 450.0f, 100.0f, 40.0f };
    while (menuRunning) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                currentDepth = 0;
                menuRunning = false;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                SDL_FPoint mousePoint = { e.button.x, e.button.y };

               if (SDL_PointInRectFloat(&mousePoint, &minusRect) && currentDepth > 1) {
                    currentDepth--;
                   currentDepth--;
                    SDL_DestroyTexture(depthValueTexture);
                    depthValueTexture = createTextTexture(renderer, font, to_string(currentDepth), 0, textColor);
                } else if (SDL_PointInRectFloat(&mousePoint, &plusRect) && currentDepth < 7) {
                    currentDepth++;
                    currentDepth++;
                    SDL_DestroyTexture(depthValueTexture);
                    depthValueTexture = createTextTexture(renderer, font, to_string(currentDepth), 0, textColor);
                } else if (SDL_PointInRectFloat(&mousePoint, &startRect)) {
                    menuRunning = false;
                }
                else if (SDL_PointInRectFloat(&mousePoint, &testRect)) {
                    currentDepth = -1;
                    menuRunning = false;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &minusRect);
        SDL_RenderFillRect(renderer, &plusRect);
        SDL_RenderFillRect(renderer, &startRect);
        SDL_RenderFillRect(renderer, &testRect);
        SDL_RenderTexture(renderer, recTexture, nullptr, &recRect);
        SDL_RenderTexture(renderer, testTexture, nullptr, &testRect);
        SDL_RenderTexture(renderer, titleTexture, nullptr, &titleRect);
        SDL_RenderTexture(renderer, minusTexture, nullptr, &minusRect);
        SDL_RenderTexture(renderer, plusTexture, nullptr, &plusRect);
        SDL_RenderTexture(renderer, startTexture, nullptr, &startRect);
        SDL_RenderTexture(renderer, depthValueTexture, nullptr, &depthRect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(plusTexture);
    SDL_DestroyTexture(minusTexture);
    SDL_DestroyTexture(depthValueTexture);
    TTF_CloseFont(font);
    font = nullptr;
    return currentDepth;
}void close() {
    SDL_DestroyTexture(whiteTileTexture);
    SDL_DestroyTexture(blackTileTexture);
    SDL_DestroyTexture(whiteHighlightTexture);
    SDL_DestroyTexture(blackHighlightTexture);
    whiteTileTexture = nullptr;
    blackTileTexture = nullptr;
    whiteHighlightTexture = nullptr;
    blackHighlightTexture = nullptr;
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = nullptr;
    gWindow = nullptr;
    TTF_Quit();
    SDL_Quit();
}
int main(int argc, char *args[]) {
    if (!init()) {
        SDL_Log("Failed to initialize SDL.\n");
        return 1;
    }
    int menuChoice = runMenu(gRenderer);
    if (menuChoice > 0) {
        int chosenDepth = menuChoice;
        SDL_HideWindow(gWindow);
        if (!loadMedia(Board, Pieces)) {
            SDL_Log("Failed to load media!\n");
        } else {
            SDL_ShowWindow(gWindow);
            game(chosenDepth);
        }
    } else if (menuChoice == -1) {
        SDL_HideWindow(gWindow);
        cout << "TESTY" << endl;
        test(1, 10);
        test(3,10);
        test(5,5);
    }
    close();
    return 0;
}