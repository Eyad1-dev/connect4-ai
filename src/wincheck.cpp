#include "wincheck.h"

bool checkWin(const Board& board, int player) {
    int rows = ROWS;  // use the constants from board.h
    int cols = COLS;

    // Horizontal
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols - 3; c++)
            if (board[r][c] == player && board[r][c+1] == player &&
                board[r][c+2] == player && board[r][c+3] == player)
                return true;

    // Vertical
    for (int c = 0; c < cols; c++)
        for (int r = 0; r < rows - 3; r++)
            if (board[r][c] == player && board[r+1][c] == player &&
                board[r+2][c] == player && board[r+3][c] == player)
                return true;

    // Diagonal down-right
    for (int r = 0; r < rows - 3; r++)
        for (int c = 0; c < cols - 3; c++)
            if (board[r][c] == player && board[r+1][c+1] == player &&
                board[r+2][c+2] == player && board[r+3][c+3] == player)
                return true;

    // Diagonal up-right
    for (int r = 3; r < rows; r++)
        for (int c = 0; c < cols - 3; c++)
            if (board[r][c] == player && board[r-1][c+1] == player &&
                board[r-2][c+2] == player && board[r-3][c+3] == player)
                return true;

    return false;
}
