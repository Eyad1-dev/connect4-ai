#pragma once
#include "board.h"
#include "wincheck.h"
#include "ai.h"
#include "game.h"
using namespace std;

void runAllTests();
void testCase1();   // Horizontal near-win   — must take the win at Col 3
void testCase2();   // Vertical block        — must block Col 3
void testCase3();   // Center preference     — must play Col 3 on empty board
void testCase4();   // Diagonal block        — must block diagonal at Col 5
void testCase5();   // Win over block        — must prefer winning over blocking
void testCase6();   // Avoid losing trap     — must not hand opponent the win