# Connect Four AI

A terminal-based Connect Four engine written in C++ featuring a strong AI opponent built on classical game-tree search algorithms. The AI combines **Minimax**, **Alpha-Beta Pruning**, **Iterative Deepening**, and a **Zobrist-hashed Transposition Table** to play at an adjustable difficulty level.

---

## Demo

```
  +----------------------------------+
  |         CONNECT  FOUR            |
  |  Player 1: [R]     Player 2: [Y] |
  +----------------------------------+

  >> Player 2's turn  [Y]

    0      1      2      3      4      5      6
    ^
  +-----+-----+-----+-----+-----+-----+-----+
  |  .  |  .  |  .  |  .  |  .  |  .  |  .  |
  +-----+-----+-----+-----+-----+-----+-----+
  |  .  |  .  |  .  |  .  |  .  |  .  |  .  |
  +-----+-----+-----+-----+-----+-----+-----+
  |  .  |  .  |  .  |  .  |  .  |  .  |  .  |
  +-----+-----+-----+-----+-----+-----+-----+
  |  .  |  .  |  .  |  R  |  .  |  .  |  .  |
  +-----+-----+-----+-----+-----+-----+-----+
  |  .  |  .  |  R  |  Y  |  .  |  .  |  .  |
  +-----+-----+-----+-----+-----+-----+-----+
  |  .  |  R  |  Y  |  Y  |  .  |  .  |  .  |
  +-----+-----+-----+-----+-----+-----+-----+

  [ID] depth 1  →  best col 3  |  score 22
  [ID] depth 2  →  best col 3  |  score 18
  [ID] depth 3  →  best col 3  |  score 34
  [ID] depth 4  →  best col 3  |  score 34
  [ID] depth 5  →  best col 3  |  score 52
  [ID] depth 6  →  best col 3  |  score 52
  [TT] Lookups: 8420  |  Hits: 3201  |  Hit rate: 38%
```

---

## Features

- **Human vs AI** and **AI vs AI** game modes
- **Three difficulty levels** — Easy (depth 3), Medium (depth 6), Hard (depth 9)
- **Iterative Deepening** — searches depth 1 → N, using each pass to sharpen move ordering for the next
- **Alpha-Beta Pruning** — eliminates branches that cannot affect the final decision, drastically reducing nodes evaluated
- **Transposition Table** — Zobrist-hashed cache of previously evaluated positions with EXACT / LOWERBOUND / UPPERBOUND flags to avoid redundant searches
- **Heuristic Evaluation** — scores all horizontal, vertical, and diagonal windows with center-column bias
- **Move Ordering** — center columns searched first to maximize pruning efficiency
- **Live AI stats** — iterative deepening trace and transposition table hit rate printed per move
- **6 automated test cases** covering near-wins, vertical blocks, diagonal threats, win-vs-block priority, and lookahead traps

---

## AI Architecture

The AI is built as a layered stack where each component feeds into the next:

```
getBestMove()
│
├── Iterative Deepening loop  (depth 1 → maxDepth)
│   └── minimax()
│       ├── Alpha-Beta Pruning      — cuts branches early
│       ├── Transposition Table     — skips already-evaluated positions
│       ├── Move Ordering           — best column first → more pruning
│       └── evaluate()              — scores terminal / depth-limit nodes
│           ├── scoreWindow()       — grades each 4-cell window
│           └── Center bias         — rewards center column occupancy
│
└── Zobrist Hashing             — O(1) incremental board fingerprinting
```

### Why Iterative Deepening?

A cold search at depth N evaluates moves in an arbitrary order. Iterative deepening runs depth 1, 2, … N−1 first, using the best move from each pass as the first move tried in the next. Since alpha-beta pruning is most effective when the best move is searched first, each successive pass prunes far more of the tree than a cold search would — making the total cost of all passes together cheaper than a single uninformed deep search.

### Transposition Table

The board state is fingerprinted using **Zobrist hashing** — each (cell, player) pair is assigned a random 64-bit key at startup, and the board hash is the XOR of all keys for occupied cells. XOR is its own inverse, so placing and removing a piece are the same operation. This gives O(1) incremental hash updates as minimax makes and undoes moves.

Each cache entry stores a score, the depth at which it was computed, and a bound flag:

| Flag | Meaning |
|------|---------|
| `EXACT` | True minimax score — can be returned immediately |
| `LOWERBOUND` | Beta cutoff occurred — actual score is ≥ this value |
| `UPPERBOUND` | Alpha cutoff occurred — actual score is ≤ this value |

Entries from shallower passes are kept across iterative deepening iterations — they remain valid bounds for deeper searches and further accelerate pruning.

---

## Project Structure

```
connect4-ai/
├── src/
│   ├── main.cpp        — entry point, mode & difficulty selection
│   ├── ai.cpp/h        — minimax, alpha-beta, iterative deepening, transposition table
│   ├── board.cpp/h     — board state, move application/undo, terminal rendering
│   ├── game.cpp/h      — game loop, human input, result display
│   ├── wincheck.cpp/h  — win detection (horizontal, vertical, diagonal)
│   └── test.cpp/h      — 6 automated AI correctness test cases
├── Makefile
├── .gitignore
└── LICENSE
```

---

## Getting Started

### Prerequisites

- g++ with C++17 support
- make (optional — you can also compile manually)

### Build

```bash
git clone https://github.com/Eyad1-dev/connect4-ai.git
cd connect4-ai
make
```

Or manually:

```bash
g++ -std=c++17 -Wall -Wextra -o connect4 src/main.cpp src/ai.cpp src/board.cpp src/game.cpp src/wincheck.cpp src/test.cpp
```

### Run

```bash
./connect4
```

On Windows:

```bash
connect4.exe
```

> **Note:** Run in a terminal that supports ANSI color codes. The VS Code integrated terminal and Windows Terminal both work correctly.

### Run Tests

The test suite verifies AI correctness across 6 scenarios. To run it, replace `main.cpp` with a small runner:

```cpp
#include "src/test.h"
int main() { runAllTests(); return 0; }
```

Then compile and run as above.

---

## Test Cases

| # | Name | Scenario | Expected |
|---|------|----------|----------|
| 1 | Near Win | AI has 3 in a row horizontally | Take the win at Col 3 |
| 2 | Defensive Block | Opponent has 3 stacked vertically | Block at Col 3 |
| 3 | Center Preference | Empty board | Play Col 3 (center) |
| 4 | Block Diagonal | Opponent threatens diagonal completion | Block at Col 5 |
| 5 | Win Over Block | Win and block both available | Prefer the win |
| 6 | Lookahead Block | Opponent threatens diagonal 2 moves ahead | Block at Col 4 |

---

## License

This project is licensed under the MIT License — see [LICENSE](LICENSE) for details.
