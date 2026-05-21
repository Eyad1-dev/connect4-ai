#include "game.h"
#include "ai.h"
#include <iostream>
using namespace std;

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif
    // Seed the Zobrist hash table once at startup.
    // Seeding per-move would corrupt the transposition table when two moves
    // occur within the same second (identical srand seed → identical keys).
    initZobrist();

    // --- Mode selection --------------------------------------------------
    cout << "\n";
    cout << BLUE << BOLD
         << "  ╔══════════════════════════════════╗\n"
         << "  ║       SELECT GAME MODE           ║\n"
         << "  ║                                  ║\n"
         << "  ║   1 →  Human  vs  AI             ║\n"
         << "  ║   2 →  AI     vs  AI             ║\n"
         << "  ║                                  ║\n"
         << "  ╚══════════════════════════════════╝"
         << RESET << "\n\n";

    cout << GRAY << "  Enter mode (1 or 2) : " << RESET << WHITE;
    int mode;
    cin >> mode;
    cout << RESET << "\n";

    if (mode != 1 && mode != 2) {
        cout << RED << "  Invalid mode. Defaulting to Human vs AI.\n" << RESET;
        mode = MODE_HUMAN_VS_AI;
    }

    // --- Difficulty selection ---------------------------------------------
    cout << BLUE << BOLD
         << "  ╔══════════════════════════════════╗\n"
         << "  ║       SELECT DIFFICULTY          ║\n"
         << "  ║                                  ║\n"
         << "  ║   1 →  Easy    (depth  3)        ║\n"
         << "  ║   2 →  Medium  (depth  6)        ║\n"
         << "  ║   3 →  Hard    (depth  9)        ║\n"
         << "  ║                                  ║\n"
         << "  ╚══════════════════════════════════╝"
         << RESET << "\n\n";

    cout << GRAY << "  Enter difficulty (1–3) : " << RESET << WHITE;
    int choice;
    cin >> choice;
    cout << RESET << "\n";

    int depth;
    switch (choice) {
        case 1:  depth = DEPTH_EASY;   break;
        case 2:  depth = DEPTH_MEDIUM; break;
        case 3:  depth = DEPTH_HARD;   break;
        default:
            cout << RED << "  Invalid choice. Defaulting to Medium.\n" << RESET;
            depth = DEPTH_MEDIUM;
    }

    startGame(mode, depth);
    return 0;
}