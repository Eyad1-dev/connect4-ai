#pragma once
#include <vector>
#include <iostream>
#include <string>
using namespace std;

#define RESET      "\033[0m"
#define BOLD       "\033[1m"
#define DIM        "\033[2m"
#define RED        "\033[91m"
#define YELLOW     "\033[93m"
#define BLUE       "\033[94m"
#define CYAN       "\033[96m"
#define GREEN      "\033[92m"
#define WHITE      "\033[97m"
#define GRAY       "\033[90m"

const int ROWS    = 6;
const int COLS    = 7;
const int EMPTY   = 0;
const int PLAYER1 = 1;
const int PLAYER2 = 2;

using Board = vector<vector<int>>;

Board       createBoard   ();
void        printBoard    (const Board& board, int moveCol = -1, int currentPlayer = 0);
void        printHeader   ();
bool        isValidMove   (const Board& board, int col);
vector<int> getValidMoves (const Board& board);
int         dropDisc      (Board& board, int col, int player);
void        undoMove      (Board& board, int col);
bool        isBoardFull   (const Board& board);
Board       copyBoard     (const Board& board);