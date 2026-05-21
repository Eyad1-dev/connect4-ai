#include "board.h"

Board createBoard() {
    return Board(ROWS, vector<int>(COLS, EMPTY));
}

static string cellSymbol(int cell) {
    if (cell == PLAYER1) return string(RED)    + "R" + RESET;
    if (cell == PLAYER2) return string(YELLOW) + "Y" + RESET;
    return string(GRAY) + "." + RESET;
}

void printHeader() {
    cout << "\n";
    cout << BLUE << BOLD
         << "  +----------------------------------+\n"
         << "  |" << RESET << WHITE << BOLD
         << "         CONNECT  FOUR              "
         << BLUE << BOLD << "|\n"
         << "  |" << RESET
         << "  " << DIM << "Player 1: " << RESET << RED << BOLD << "[R]"
         << RESET << DIM << "     Player 2: " << RESET << YELLOW << BOLD << "[Y]"
         << RESET << "      " << BLUE << BOLD << "|\n"
         << "  +----------------------------------+"
         << RESET << "\n";
}

void printBoard(const Board& board, int moveCol, int currentPlayer) {
    cout << "\n";

    if (currentPlayer == PLAYER1)
        cout << GREEN << BOLD << "  >> Player 1's turn  [R]\n" << RESET;
    else if (currentPlayer == PLAYER2)
        cout << GREEN << BOLD << "  >> Player 2's turn  [Y]\n" << RESET;
    cout << "\n";

    // Column numbers
    cout << "  ";
    for (int c = 0; c < COLS; c++) {
        if (c == moveCol)
            cout << CYAN << BOLD << "  " << c << "   " << RESET;
        else
            cout << WHITE << "  " << c << "   " << RESET;
    }
    cout << "\n";

    // Drop arrow
    cout << "  ";
    for (int c = 0; c < COLS; c++) {
        if (c == moveCol)
            cout << CYAN << BOLD << "  ^   " << RESET;
        else
            cout << "      ";
    }
    cout << "\n";

    // Top border
    cout << BLUE << BOLD << "  +";
    for (int c = 0; c < COLS; c++) {
        cout << "-----";
        if (c < COLS - 1) cout << "+";
    }
    cout << "+" << RESET << "\n";

    // Rows
    for (int r = 0; r < ROWS; r++) {
        cout << BLUE << BOLD << "  |" << RESET;
        for (int c = 0; c < COLS; c++) {
            cout << "  " << cellSymbol(board[r][c]) << "  ";
            cout << BLUE << BOLD << "|" << RESET;
        }
        cout << "\n";

        if (r < ROWS - 1) {
            cout << BLUE << BOLD << "  +";
            for (int c = 0; c < COLS; c++) {
                cout << "-----";
                if (c < COLS - 1) cout << "+";
            }
            cout << "+" << RESET << "\n";
        }
    }

    // Bottom border
    cout << BLUE << BOLD << "  +";
    for (int c = 0; c < COLS; c++) {
        cout << "-----";
        if (c < COLS - 1) cout << "+";
    }
    cout << "+" << RESET << "\n\n";
}

bool isValidMove(const Board& board, int col) {
    if (col < 0 || col >= COLS) return false;
    return board[0][col] == EMPTY;
}

vector<int> getValidMoves(const Board& board) {
    vector<int> moves;
    for (int c = 0; c < COLS; c++)
        if (isValidMove(board, c)) moves.push_back(c);
    return moves;
}

int dropDisc(Board& board, int col, int player) {
    for (int r = ROWS - 1; r >= 0; r--) {
        if (board[r][col] == EMPTY) {
            board[r][col] = player;
            return r;
        }
    }
    return -1;
}

void undoMove(Board& board, int col) {
    for (int r = 0; r < ROWS; r++) {
        if (board[r][col] != EMPTY) {
            board[r][col] = EMPTY;
            return;
        }
    }
}

bool isBoardFull(const Board& board) {
    return getValidMoves(board).empty();
}

Board copyBoard(const Board& board) {
    return board;
}