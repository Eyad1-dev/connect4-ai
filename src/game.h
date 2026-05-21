#pragma once
#include "board.h"
#include "wincheck.h"
#include "ai.h"
using namespace std;

const int MODE_HUMAN_VS_AI = 1;
const int MODE_AI_VS_AI    = 2;

// Search depths mapped to difficulty levels.
const int DEPTH_EASY   = 3;
const int DEPTH_MEDIUM = 6;
const int DEPTH_HARD   = 9;

void startGame(int mode, int depth);
void printResult(int winner);
int  getHumanMove(const Board& board);