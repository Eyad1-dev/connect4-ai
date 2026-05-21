#include "game.h"

void printResult(int winner) {
    cout << "\n";
    if (winner == PLAYER1) {
        cout << RED << BOLD
             << "  +==================================+\n"
             << "  |     *** Player 1 (R) Wins! ***   |\n"
             << "  +==================================+"
             << RESET << "\n\n";
    } else if (winner == PLAYER2) {
        cout << YELLOW << BOLD
             << "  +==================================+\n"
             << "  |    *** Player 2 (Y) Wins!  ***   |\n"
             << "  +==================================+"
             << RESET << "\n\n";
    } else {
        cout << CYAN << BOLD
             << "  +==================================+\n"
             << "  |         ** It's a Draw! **       |\n"
             << "  +==================================+"
             << RESET << "\n\n";
    }
}

int getHumanMove(const Board& board) {
    int col;
    while (true) {
        cout << GRAY << "  Enter column (0–6) : " << RESET << WHITE;
        cin >> col;
        cout << RESET;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << RED << "  ✘  Invalid input. Enter a number 0–6.\n" << RESET;
            continue;
        }

        if (!isValidMove(board, col)) {
            cout << RED << "  ✘  Column " << col << " is full or out of range. Try again.\n" << RESET;
            continue;
        }

        return col;
    }
}

void startGame(int mode, int depth) {
    Board board = createBoard();
    int currentPlayer = PLAYER1;
    int lastCol = -1;
    int winner  = -1;   // -1 = in progress, 0 = draw

    printHeader();

    while (true) {
        printBoard(board, lastCol, currentPlayer);

        int col = -1;

        if (mode == MODE_HUMAN_VS_AI) {
            if (currentPlayer == PLAYER1) {
                col = getHumanMove(board);
            } else {
                cout << CYAN << "  AI is thinking...\n" << RESET;
                col = getBestMove(board, currentPlayer, depth);
                cout << GREEN << "  AI chose column " << col << "\n" << RESET;
            }
        } else {
            // Both sides use the same depth in AI vs AI mode.
            cout << CYAN << "  AI ("
                 << (currentPlayer == PLAYER1 ? "Red" : "Yellow")
                 << ") is thinking...\n" << RESET;
            col = getBestMove(board, currentPlayer, depth);
            cout << GREEN << "  AI chose column " << col << "\n" << RESET;
        }

        dropDisc(board, col, currentPlayer);
        lastCol = col;

        if (checkWin(board, currentPlayer)) {
            printBoard(board, lastCol, 0);
            winner = currentPlayer;
            printResult(winner);
            break;
        }

        if (isBoardFull(board)) {
            printBoard(board, lastCol, 0);
            winner = 0;
            printResult(winner);
            break;
        }

        currentPlayer = (currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;
    }
}