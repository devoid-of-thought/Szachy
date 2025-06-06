#include "SDL_image/include/SDL3_image/SDL_image.h"
#include "Plansza.h"
#include "SDL/include/SDL3/SDL.h"

#include "Figury.h"

#include <iostream>
#include <algorithm>
#include "Window.h"

using namespace std;


constexpr int kScreenWidth{ 512 };
constexpr int kScreenHeight{ 512 };

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

SDL_Texture* whiteTileTexture = nullptr;
SDL_Texture* blackTileTexture = nullptr;
SDL_Texture* whiteHighlightTexture = nullptr;
SDL_Texture* blackHighlightTexture = nullptr;

Piece* selectedPiece = nullptr;
Piece* temp = nullptr;
vector<pair<int,int>> pamiec;
bool isWhiteTurn = true;
Tile* Tiles[8][8];


Piece WhiteRook1(0, 7, true, 5);
Piece WhiteKnight1(1, 7, true, 3);
Piece WhiteBishop1(2, 7, true, 4);
Piece WhiteQueen(3, 7, true, 9);
Piece WhiteKing(4, 7, true, 999);
Piece WhiteBishop2(5, 7, true, 4);
Piece WhiteKnight2(6, 7, true, 3);
Piece WhiteRook2(7, 7, true, 5);

Piece WhitePawn1(0, 6, true, 1);
Piece WhitePawn2(1, 6, true, 1);
Piece WhitePawn3(2, 6, true, 1);
Piece WhitePawn4(3, 6, true, 1);
Piece WhitePawn5(4, 6, true, 1);
Piece WhitePawn6(5, 6, true, 1);
Piece WhitePawn7(6, 6, true, 1);
Piece WhitePawn8(7, 6, true, 1);

Piece BlackRook1(0, 0, false, 5);
Piece BlackKnight1(1, 0, false, 3);
Piece BlackBishop1(2, 0, false, 4);
Piece BlackQueen(3, 0, false, 9);
Piece BlackKing(4, 0, false, 999);
Piece BlackBishop2(5, 0, false, 4);
Piece BlackKnight2(6, 0, false, 3);
Piece BlackRook2(7, 0, false, 5);

Piece BlackPawn1(0, 1, false, 1);
Piece BlackPawn2(1, 1, false, 1);
Piece BlackPawn3(2, 1, false, 1);
Piece BlackPawn4(3, 1, false, 1);
Piece BlackPawn5(4, 1, false, 1);
Piece BlackPawn6(5, 1, false, 1);
Piece BlackPawn7(6, 1, false, 1);
Piece BlackPawn8(7, 1, false, 1);


Piece* chessPieces[2][16] = {
    { &WhiteRook1, &WhiteBishop1,&WhiteKnight1,&WhiteKing, &WhiteQueen, &WhiteBishop2, &WhiteKnight2,&WhiteRook2,
      &WhitePawn1, &WhitePawn2, &WhitePawn3, &WhitePawn4, &WhitePawn5,
      &WhitePawn6, &WhitePawn7, &WhitePawn8 },
{ &BlackRook1, &BlackBishop1, &BlackKnight1, &BlackKing, &BlackQueen, &BlackBishop2, &BlackKnight2, &BlackRook2,
&BlackPawn1, &BlackPawn2, &BlackPawn3, &BlackPawn4, &BlackPawn5,
&BlackPawn6, &BlackPawn7, &BlackPawn8 },
};
bool init()
{
    //Initialization flag
    bool success{ true };

    //Initialize SDL
    if( !SDL_Init( SDL_INIT_VIDEO ) )
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window with renderer
        if( !SDL_CreateWindowAndRenderer( "Szachy", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer ) )
        {
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        }

    }


    return success;
}



void renderBoard() {
    const int tileSize = 64;
    for (int y = 7; y>=0; --y) {
        for (int x = 7; x>=0; --x) {
            // Reset highlight
            if ((x + y) % 2 != 0) {
                Tiles[y][x]->mTexture = blackTileTexture;
            } else {
                Tiles[y][x]->mTexture = whiteTileTexture;
            }
            Tiles[y][x]->highlightBorder = false;
            // Highlight if move is legal for selected piece
            if (selectedPiece) {

                    for (auto& move : selectedPiece->possibleMoves) {
                        pamiec.push_back(move);
                        if (move.first == y && move.second == x) {
                            if ((x + y) % 2 != 0) {
                                Tiles[y][x]->mTexture = blackHighlightTexture;
                            } else {
                                Tiles[y][x]->mTexture = whiteHighlightTexture;
                            }
                            break;
                        }

                    }
                }

                Tiles[y][x]->render(y * tileSize, x * tileSize);
        }
        temp = selectedPiece;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 16; j++) {
                if (chessPieces[i][j] != nullptr) {
                    chessPieces[i][j]->render(chessPieces[i][j]->BoardPosition.first* tileSize, chessPieces[i][j]->BoardPosition.second * tileSize);
                    Tiles[chessPieces[i][j]->BoardPosition.second][chessPieces[i][j]->BoardPosition.first]->hasPiece = true;
                    Tiles[chessPieces[i][j]->BoardPosition.second][chessPieces[i][j]->BoardPosition.first]->pieceOnTile = chessPieces[i][j];
                    if (chessPieces[i][j]->White) {
                        Tiles[chessPieces[i][j]->BoardPosition.second][chessPieces[i][j]->BoardPosition.first]->isWhite = true;
                    } else {
                        Tiles[chessPieces[i][j]->BoardPosition.second][chessPieces[i][j]->BoardPosition.first]->isWhite = false;
                    }
                }
            }
        }
    }
}


bool loadMedia()
{
     whiteTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_tile.png");
    blackTileTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_tile.png");
    whiteHighlightTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/White_Highlight.png");
    blackHighlightTexture = IMG_LoadTexture(gRenderer, "/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_Highlight.png");

    //File loading flag
    bool success{ true };

    //Load splash image
    BlackBishop1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_bdt60.png" );
    BlackBishop2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_bdt60.png" );
    BlackKing.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_kdt60.png" );
    BlackKnight1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_ndt60.png" );
    BlackKnight2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_ndt60.png" );
    BlackPawn1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackPawn2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackPawn3.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackPawn4.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackPawn5.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackPawn6.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackPawn7.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackPawn8.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_pdt60.png" );
    BlackQueen.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_qdt60.png" );
    BlackRook1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_rdt60.png" );
    BlackRook2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_rdt60.png" );
    WhiteBishop1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_blt60.png" );
    WhiteBishop2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_blt60.png" );
    WhiteKing.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_klt60.png" );
    WhiteKnight1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_nlt60.png" );
    WhiteKnight2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_nlt60.png" );
    WhitePawn1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhitePawn2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhitePawn3.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhitePawn4.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhitePawn5.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhitePawn6.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhitePawn7.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhitePawn8.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_plt60.png" );
    WhiteQueen.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_qlt60.png" );
    WhiteRook1.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_rlt60.png" );
    WhiteRook2.loadFromFile( "/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_rlt60.png" );
    const int tileSize = 64;
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++) {
            Tiles[x][y] = new Tile();
            Tiles[x][y]->BoardPosition = {x, y};
            Tiles[x][y]->mWidth = tileSize;
            Tiles[x][y]->mHeight = tileSize;
            if ((x + y) % 2 != 0) {
                Tiles[x][y]->loadFromFile("/home/userbrigh/CLionProjects/Szachy/Tekstury/Black_tile.png");
            } else {
                Tiles[x][y]->loadFromFile("/home/userbrigh/CLionProjects/Szachy/Tekstury/White_tile.png");
            }
        }
    }
    return success;
}

template <typename T>
void close(T& PngTexture)
{
    //Clean up texture
    PngTexture.destroy();

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    gRenderer = nullptr;
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;

    SDL_Quit();
}

    void move(SDL_Event e) {
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
            int tileSize = 64;
            int clickedX = e.button.x / tileSize;
            int clickedY = e.button.y / tileSize;
            pair<int,int> clickedPosition = {clickedX, clickedY};
            if(selectedPiece!=nullptr&&( clickedX != selectedPiece->BoardPosition.first || clickedY != selectedPiece->BoardPosition.second))
            {
                for (auto& move : selectedPiece->possibleMoves) {
                    if (clickedPosition == move) {
                        if ((Tiles[clickedY][clickedX]->hasPiece && Tiles[clickedY][clickedX]->isWhite == selectedPiece->White&&(selectedPiece->figure==999||selectedPiece->figure==5))) {
                            if (selectedPiece->figure == 5 && selectedPiece->BoardPosition.first== 0) {
                                int color = (selectedPiece->White) ? 0 : 1;
                                Tiles[selectedPiece->BoardPosition.second][2]->pieceOnTile = chessPieces[color][3];
                                chessPieces[color][3]->BoardPosition.first = 2;
                                chessPieces[color][3]->BoardPosition.second=clickedY;
                                selectedPiece->BoardPosition.first = 3;
                                selectedPiece->BoardPosition.second = clickedY;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = true;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = selectedPiece;
                            }
                            else if (selectedPiece->figure == 5 && selectedPiece->BoardPosition.first== 7) {
                                int color = (selectedPiece->White) ? 0 : 1;
                                Tiles[selectedPiece->BoardPosition.second][6]->pieceOnTile = chessPieces[color][3];
                                chessPieces[color][3]->BoardPosition.first = 6;
                                chessPieces[color][3]->BoardPosition.second=clickedY;
                                selectedPiece->BoardPosition.first = 5;
                                selectedPiece->BoardPosition.second = clickedY;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = true;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = selectedPiece;
                            } else if (selectedPiece->figure==999&& clickedX==0) {
                                int color = (selectedPiece->White) ? 0 : 1;
                                Tiles[selectedPiece->BoardPosition.second][3]->pieceOnTile = chessPieces[color][3];
                                chessPieces[color][0]->BoardPosition.first = 3;
                                chessPieces[color][0]->BoardPosition.second=clickedY;
                                selectedPiece->BoardPosition.first = 2;
                                selectedPiece->BoardPosition.second = clickedY;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = true;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = selectedPiece;
                            }
                            else {
                                int color = (selectedPiece->White) ? 0 : 1;
                                Tiles[selectedPiece->BoardPosition.second][5]->pieceOnTile = chessPieces[color][3];
                                chessPieces[color][7]->BoardPosition.first =5;
                                chessPieces[color][7]->BoardPosition.second=clickedY;
                                selectedPiece->BoardPosition.first = 6;
                                selectedPiece->BoardPosition.second = clickedY;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = true;
                                Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = selectedPiece;
                            }
                        } else

                        if (Tiles[clickedY][clickedX]->hasPiece && Tiles[clickedY][clickedX]->isWhite == selectedPiece->White) {
                            cout << "Cannot capture own piece!" << endl;
                            return;
                        }
                        else if (Tiles[clickedY][clickedX]->hasPiece && Tiles[clickedY][clickedX]->isWhite != selectedPiece->White) {
                            // Capture the piece
                            int enemyColor = (selectedPiece->White) ? 1 : 0;
                            for (int i = 0; i < 16; ++i) {
                                if (chessPieces[enemyColor][i] && chessPieces[enemyColor][i]->BoardPosition == clickedPosition && chessPieces[enemyColor][i]->figure!=999) {
                                    chessPieces[enemyColor][i]->destroy();
                                    Tiles[chessPieces[enemyColor][i]->BoardPosition.second][chessPieces[enemyColor][i]->BoardPosition.first]->hasPiece = false;
                                    Tiles[chessPieces[enemyColor][i]->BoardPosition.second][chessPieces[enemyColor][i]->BoardPosition.first]->pieceOnTile = nullptr;
                                    chessPieces[enemyColor][i] = nullptr;

                                    Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = false;
                                    Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = nullptr;
                                    selectedPiece->BoardPosition.first = clickedX;
                                    selectedPiece->BoardPosition.second = clickedY;
                                    Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = true;
                                    Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = selectedPiece;
                                    if(selectedPiece->figure == 1 && selectedPiece->BoardPosition.second == 0) {
                                        selectedPiece->figure =9;
                                        selectedPiece->loadFromFile("/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_qlt60.png");
                                    }
                                    selectedPiece = nullptr;
                                    break;
                                }
                            }
                        } else{
                            Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = false;
                            Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = nullptr;
                            selectedPiece->BoardPosition.first = clickedX;
                            selectedPiece->BoardPosition.second = clickedY;
                            Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->hasPiece = true;
                            Tiles[selectedPiece->BoardPosition.second][selectedPiece->BoardPosition.first]->pieceOnTile = selectedPiece;
                            if(selectedPiece->figure == 1 && selectedPiece->BoardPosition.second == 0) {
                                selectedPiece->figure =9;
                                selectedPiece->loadFromFile("/home/userbrigh/CLionProjects/Szachy/Tekstury/Chess_qlt60.png");
                            }
                            selectedPiece = nullptr;
                        }
                    }


                }


            }
        }
    }
bool isChecked(int color) {
    int enemyColor = (color == 0) ? 1 : 0;
    std::pair<int, int> kingPos = chessPieces[color][3]->BoardPosition;
    for (int i = 0; i < 16; ++i) {
        Piece* enemy = chessPieces[enemyColor][i];
        if (enemy) {
            enemy->possibleMoves.clear();
            enemy->generatePossibleMoves(Tiles);

            for (auto& move : enemy->possibleMoves) {
                if (move == kingPos) {
                    return true;
                }
            }
        }
    }
    return false;

}





int main( int argc, char* args[] ) {
    //Final exit code
    int exitCode{ 0 };

    //Initialize
    if( !init() )
    {
        SDL_Log( "Unable to initialize program!\n" );
        exitCode = 1;
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            SDL_Log( "Unable to load media!\n" );
            exitCode = 2;
        }

        else
        {
            bool quit{ false };

            //The event data
            SDL_Event e;
            SDL_zero( e );

            //The main loop

            while( quit == false )
            {
                //Get event data
                while( SDL_PollEvent( &e ) ) {
                    //If event is quit type
                    if( e.type == SDL_EVENT_QUIT )
                    {
                        //End the main loop
                        quit = true;
                    }

                    //Handle button events
                    for (auto& row : chessPieces)
                        for (auto& piece : row)
                            if (piece) piece->handleEvent(&e,Tiles);

                    //Fill the background white

                    SDL_RenderClear( gRenderer );

                    SDL_SetRenderDrawColor( gRenderer, 0x0, 0x0, 0x0, 0xFF );
                    renderBoard();
                    if (isWhiteTurn){
                        if (isChecked(0) && selectedPiece->figure!=999) {
                           cout << "WYBIERZ INNĄ FIGURĘ" << endl;
                            selectedPiece = nullptr;
                        } else {
                            move(e);
                        }
                    }else{
                        move(e);
                    }
                    SDL_RenderPresent( gRenderer );
                }
            }
        }


        return exitCode;
    }
}