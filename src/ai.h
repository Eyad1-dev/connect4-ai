#pragma once
#include "board.h"

// Call once at program startup to seed the Zobrist hash table.
void initZobrist();

// Returns the best column for `player` to play given the current board.
// `depth` controls search depth: 3 = Easy, 6 = Medium, 9 = Hard.
int getBestMove(const Board& board, int player, int depth);