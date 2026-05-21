#include "test.h"

// -----------------------------------------------------------------------------
//  Helpers
// -----------------------------------------------------------------------------

static void printTestHeader(int num, const string& name) {
    cout << "\n";
    cout << CYAN << BOLD
         << "  ╔══════════════════════════════════╗\n"
         << "  ║  TEST CASE " << num << ": " << name;
    int pad = 22 - (int)name.size();
    for (int i = 0; i < pad; i++) cout << " ";
    cout << "║\n"
         << "  ╚══════════════════════════════════╝"
         << RESET << "\n";
}

static void printTestResult(bool passed, int aiCol, int expectedCol) {
    cout << "\n";
    if (passed) {
        cout << GREEN << BOLD
             << "  ✔  PASSED — AI chose column " << aiCol << " (expected " << expectedCol << ")\n"
             << RESET;
    } else {
        cout << RED << BOLD
             << "  ✘  FAILED — AI chose column " << aiCol << " (expected " << expectedCol << ")\n"
             << RESET;
    }
    cout << "\n";
    cout << GRAY << "  " << string(36, '-') << RESET << "\n";
}

// -----------------------------------------------------------------------------
//  Test Cases
// -----------------------------------------------------------------------------

// AI has 3 in a row horizontally — must take the win at Col 3.
void testCase1() {
    printTestHeader(1, "Near Win");
    cout << GRAY << "\n  Scenario: Player 1 (Red) has 3 in a row\n"
         << "  at Row 5, Cols 0-1-2. AI must pick Col 3.\n" << RESET;

    Board board = createBoard();
    board[5][0] = PLAYER1;
    board[5][1] = PLAYER1;
    board[5][2] = PLAYER1;
    board[5][4] = PLAYER2;
    board[4][4] = PLAYER2;

    printBoard(board, -1, PLAYER1);

    int aiCol    = getBestMove(board, PLAYER1, DEPTH_MEDIUM);
    int expected = 3;
    bool passed  = (aiCol == expected);

    dropDisc(board, aiCol, PLAYER1);
    cout << WHITE << "  Board after AI move:\n" << RESET;
    printBoard(board, aiCol, 0);
    printTestResult(passed, aiCol, expected);
}

// Opponent has 3 stacked vertically — AI must block before they complete 4.
void testCase2() {
    printTestHeader(2, "Defensive Block");
    cout << GRAY << "\n  Scenario: Player 1 (Red) has 3 stacked\n"
         << "  vertically in Col 3. AI (Yellow) must block.\n" << RESET;

    Board board = createBoard();
    board[5][3] = PLAYER1;
    board[4][3] = PLAYER1;
    board[3][3] = PLAYER1;
    board[5][2] = PLAYER2;
    board[5][4] = PLAYER2;

    printBoard(board, -1, PLAYER2);

    int aiCol    = getBestMove(board, PLAYER2, DEPTH_MEDIUM);
    int expected = 3;
    bool passed  = (aiCol == expected);

    dropDisc(board, aiCol, PLAYER2);
    cout << WHITE << "  Board after AI move:\n" << RESET;
    printBoard(board, aiCol, 0);
    printTestResult(passed, aiCol, expected);
}

// On an empty board the center column is statistically strongest,
// as it participates in the most possible winning lines.
void testCase3() {
    printTestHeader(3, "Center Preference");
    cout << GRAY << "\n  Scenario: Empty board. AI plays first.\n"
         << "  Should choose Col 3 (center column).\n" << RESET;

    Board board = createBoard();
    printBoard(board, -1, PLAYER1);

    int aiCol    = getBestMove(board, PLAYER1, DEPTH_MEDIUM);
    int expected = 3;
    bool passed  = (aiCol == expected);

    dropDisc(board, aiCol, PLAYER1);
    cout << WHITE << "  Board after AI move:\n" << RESET;
    printBoard(board, aiCol, 0);
    printTestResult(passed, aiCol, expected);
}

// Opponent has 3 pieces on an ascending diagonal (row 5 col 2 → row 3 col 4).
// The winning square is (row 2, col 5) — AI must drop into Col 5 to block.
void testCase4() {
    printTestHeader(4, "Block Diagonal");
    cout << GRAY << "\n  Scenario: Player 1 (Red) has 3 on a diagonal.\n"
         << "  AI (Yellow) must block at Col 5.\n" << RESET;

    Board board = createBoard();

    // Red diagonal: (5,2) → (4,3) → (3,4), threatening (2,5)
    board[5][2] = PLAYER1;

    board[5][3] = PLAYER2;              // support column for (4,3)
    board[4][3] = PLAYER1;

    board[5][4] = PLAYER2;              // support column for (3,4)
    board[4][4] = PLAYER2;
    board[3][4] = PLAYER1;

    board[5][5] = PLAYER2;              // support column for (2,5)
    board[4][5] = PLAYER2;
    board[3][5] = PLAYER2;
    // (2,5) is the next empty cell in Col 5 — AI must land here

    printBoard(board, -1, PLAYER2);

    int aiCol    = getBestMove(board, PLAYER2, DEPTH_MEDIUM);
    int expected = 5;
    bool passed  = (aiCol == expected);

    dropDisc(board, aiCol, PLAYER2);
    cout << WHITE << "  Board after AI move:\n" << RESET;
    printBoard(board, aiCol, 0);
    printTestResult(passed, aiCol, expected);
}

// AI can win immediately at Col 4, but opponent also threatens at Col 0.
// AI must take the win rather than play defensively.
void testCase5() {
    printTestHeader(5, "Win Over Block");
    cout << GRAY << "\n  Scenario: AI (Red) can win at Col 4, but\n"
         << "  opponent also threatens. Must win first.\n" << RESET;

    Board board = createBoard();

    // Red has 3 in a row at (5,1),(5,2),(5,3) — Col 4 wins immediately
    board[5][1] = PLAYER1;
    board[5][2] = PLAYER1;
    board[5][3] = PLAYER1;

    // Yellow has 3 stacked at Col 0, threatening to complete vertically
    board[5][0] = PLAYER2;
    board[4][0] = PLAYER2;
    board[3][0] = PLAYER2;

    printBoard(board, -1, PLAYER1);

    int aiCol    = getBestMove(board, PLAYER1, DEPTH_MEDIUM);
    int expected = 4;
    bool passed  = (aiCol == expected);

    dropDisc(board, aiCol, PLAYER1);
    cout << WHITE << "  Board after AI move:\n" << RESET;
    printBoard(board, aiCol, 0);
    printTestResult(passed, aiCol, expected);
}

// Opponent has 3 on an ascending diagonal threatening (2,4).
// AI must look ahead and block at Col 4 rather than play elsewhere.
void testCase6() {
    printTestHeader(6, "Lookahead Block");
    cout << GRAY << "\n  Scenario: Opponent threatens a diagonal win\n"
         << "  at Col 4. AI must see it and block.\n" << RESET;

    Board board = createBoard();

    // Yellow diagonal: (5,1) → (4,2) → (3,3), threatening (2,4)
    board[5][1] = PLAYER2;

    board[5][2] = PLAYER1;              // support
    board[4][2] = PLAYER2;

    board[5][3] = PLAYER1;              // support
    board[4][3] = PLAYER1;              // support
    board[3][3] = PLAYER2;

    board[5][4] = PLAYER1;              // support
    board[4][4] = PLAYER1;              // support
    board[3][4] = PLAYER1;              // support
    // (2,4) is the winning square — AI must block Col 4

    printBoard(board, -1, PLAYER1);

    int aiCol    = getBestMove(board, PLAYER1, DEPTH_MEDIUM);
    int expected = 4;
    bool passed  = (aiCol == expected);

    dropDisc(board, aiCol, PLAYER1);
    cout << WHITE << "  Board after AI move:\n" << RESET;
    printBoard(board, aiCol, 0);
    printTestResult(passed, aiCol, expected);
}

// -----------------------------------------------------------------------------
//  Test Runner
// -----------------------------------------------------------------------------

void runAllTests() {
    // Ensure the Zobrist table is seeded — safe to call even if main() already did it.
    initZobrist();

    cout << "\n";
    cout << YELLOW << BOLD
         << "  ╔══════════════════════════════════╗\n"
         << "  ║      RUNNING ALL TEST CASES      ║\n"
         << "  ╚══════════════════════════════════╝"
         << RESET << "\n";

    testCase1();
    testCase2();
    testCase3();
    testCase4();
    testCase5();
    testCase6();

    cout << YELLOW << BOLD
         << "\n  ╔══════════════════════════════════╗\n"
         << "  ║        ALL TESTS COMPLETE        ║\n"
         << "  ╚══════════════════════════════════╝"
         << RESET << "\n\n";
}