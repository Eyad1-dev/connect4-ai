#include "ai.h"
#include "wincheck.h"
#include <climits>
#include <algorithm>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <cstdint>
using namespace std;

// -----------------------------------------------------------------------------
//  Heuristic Evaluation
// -----------------------------------------------------------------------------

// Scores a 4-cell window from one player's perspective.
// Mixed windows (both players present) are worthless — return 0.
static int scoreWindow(const vector<int>& window, int player) {
    int opponent = (player == PLAYER1) ? PLAYER2 : PLAYER1;
    int playerCount = 0, opponentCount = 0, emptyCount = 0;

    for (int cell : window) {
        if      (cell == player)   playerCount++;
        else if (cell == opponent) opponentCount++;
        else                       emptyCount++;
    }

    if (playerCount > 0 && opponentCount > 0) return 0;

    if (playerCount == 4)                      return 1000;
    if (playerCount == 3 && emptyCount == 1)   return   50;
    if (playerCount == 2 && emptyCount == 2)   return   10;
    if (opponentCount == 3 && emptyCount == 1) return  -80;
    if (opponentCount == 2 && emptyCount == 2) return  -10;

    return 0;
}

// Scores the full board by aggregating all horizontal, vertical,
// and diagonal windows. Center column occupancy is weighted higher
// since it participates in the most possible winning lines.
static int evaluate(const Board& board, int player) {
    int score = 0;

    // Center column bias
    for (int r = 0; r < ROWS; r++)
        if (board[r][COLS / 2] == player)
            score += 6;

    // Horizontal
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c <= COLS - 4; c++) {
            vector<int> window = { board[r][c], board[r][c+1],
                                   board[r][c+2], board[r][c+3] };
            score += scoreWindow(window, player);
        }

    // Vertical
    for (int c = 0; c < COLS; c++)
        for (int r = 0; r <= ROWS - 4; r++) {
            vector<int> window = { board[r][c],   board[r+1][c],
                                   board[r+2][c], board[r+3][c] };
            score += scoreWindow(window, player);
        }

    // Diagonal down-right
    for (int r = 0; r <= ROWS - 4; r++)
        for (int c = 0; c <= COLS - 4; c++) {
            vector<int> window = { board[r][c],     board[r+1][c+1],
                                   board[r+2][c+2], board[r+3][c+3] };
            score += scoreWindow(window, player);
        }

    // Diagonal up-right
    for (int r = 3; r < ROWS; r++)
        for (int c = 0; c <= COLS - 4; c++) {
            vector<int> window = { board[r][c],     board[r-1][c+1],
                                   board[r-2][c+2], board[r-3][c+3] };
            score += scoreWindow(window, player);
        }

    return score;
}

// -----------------------------------------------------------------------------
//  Transposition Table
// -----------------------------------------------------------------------------

// EXACT      — true minimax score; can be used directly
// LOWERBOUND — beta cutoff occurred; actual score is >= this value
// UPPERBOUND — alpha cutoff occurred; actual score is <= this value
enum TTFlag { EXACT, LOWERBOUND, UPPERBOUND };

struct TTEntry {
    int    score;
    int    depth;
    TTFlag flag;
};

static unordered_map<uint64_t, TTEntry> ttTable;
static int ttHits    = 0;
static int ttLookups = 0;

// -----------------------------------------------------------------------------
//  Zobrist Hashing
// -----------------------------------------------------------------------------

// Each (cell, player) pair gets a unique random 64-bit key.
// The board hash is the XOR of all keys for occupied cells.
// XOR is its own inverse, so placing and removing a piece are identical operations.
static uint64_t zobristTable[ROWS][COLS][2];  // [row][col][player-1]
static uint64_t currentHash = 0;

void initZobrist() {
    // rand() yields ~15 bits; combine three calls for a full 64-bit value.
    auto rand64 = []() -> uint64_t {
        return ((uint64_t)rand() << 32) ^ ((uint64_t)rand() << 16) ^ (uint64_t)rand();
    };
    srand((unsigned)time(nullptr));

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            for (int p = 0; p < 2; p++)
                zobristTable[r][c][p] = rand64();

    currentHash = 0;
}

// Toggle a piece in the running hash. Call on place AND on undo.
static void zobristFlip(int row, int col, int player) {
    currentHash ^= zobristTable[row][col][player - 1];
}

// -----------------------------------------------------------------------------
//  Minimax with Alpha-Beta Pruning
// -----------------------------------------------------------------------------

static int minimax(Board& board, int depth, int alpha, int beta,
                   bool isMaximizing, int aiPlayer, int humanPlayer)
{
    // Transposition table lookup — skip search if we've seen this position
    // at an equal or greater depth.
    ttLookups++;
    auto it = ttTable.find(currentHash);
    if (it != ttTable.end() && it->second.depth >= depth) {
        TTEntry& e = it->second;
        if (e.flag == EXACT) { ttHits++; return e.score; }
        if (e.flag == LOWERBOUND) alpha = max(alpha, e.score);
        if (e.flag == UPPERBOUND) beta  = min(beta,  e.score);
        if (alpha >= beta) { ttHits++; return e.score; }
    }

    // Terminal checks
    if (checkWin(board, aiPlayer))    return  10000 + depth;
    if (checkWin(board, humanPlayer)) return -10000 - depth;
    if (isBoardFull(board))           return 0;
    if (depth == 0)                   return evaluate(board, aiPlayer);

    // Move ordering: prefer center columns to improve pruning efficiency.
    vector<int> moves = getValidMoves(board);
    sort(moves.begin(), moves.end(), [](int a, int b){
        return abs(a - 3) < abs(b - 3);
    });

    // Snapshot alpha/beta before the loop — needed to determine the TT flag.
    int originalAlpha = alpha;
    int originalBeta  = beta;
    int result;

    if (isMaximizing) {
        int best = INT_MIN;
        for (int col : moves) {
            int row = dropDisc(board, col, aiPlayer);
            zobristFlip(row, col, aiPlayer);

            int score = minimax(board, depth-1, alpha, beta, false, aiPlayer, humanPlayer);

            undoMove(board, col);
            zobristFlip(row, col, aiPlayer);

            if (score > best) best = score;
            if (best > alpha) alpha = best;
            if (beta <= alpha) break;
        }
        result = best;
    } else {
        int best = INT_MAX;
        for (int col : moves) {
            int row = dropDisc(board, col, humanPlayer);
            zobristFlip(row, col, humanPlayer);

            int score = minimax(board, depth-1, alpha, beta, true, aiPlayer, humanPlayer);

            undoMove(board, col);
            zobristFlip(row, col, humanPlayer);

            if (score < best) best = score;
            if (best < beta)  beta  = best;
            if (beta <= alpha) break;
        }
        result = best;
    }

    // Store result with the appropriate bound flag.
    TTFlag flag;
    if      (result <= originalAlpha) flag = UPPERBOUND;
    else if (result >= originalBeta)  flag = LOWERBOUND;
    else                              flag = EXACT;

    auto existing = ttTable.find(currentHash);
    if (existing == ttTable.end() || existing->second.depth <= depth)
        ttTable[currentHash] = { result, depth, flag };

    return result;
}

// -----------------------------------------------------------------------------
//  Public Entry Point  —  Iterative Deepening
// -----------------------------------------------------------------------------

// Searches depth 1 → maxDepth, updating the best move after each pass.
//
// Why not jump straight to maxDepth?
//   Each shallow pass produces a move ordering for the next pass.
//   Alpha-beta pruning is most effective when the best move is searched first,
//   so a depth-5 search seeded by depth-4 results prunes far more than a
//   cold depth-5 search. The extra shallow passes cost very little in practice.
//
// The TT is kept across passes so scores computed at depth d are reused
// as bounds at depth d+1, further accelerating the search.
int getBestMove(const Board& board, int player, int maxDepth) {
    int opponent = (player == PLAYER1) ? PLAYER2 : PLAYER1;
    int bestCol  = getValidMoves(board)[0];

    // Clear TT once before the full iterative search, not between passes.
    // Entries from shallower passes remain valid bounds for deeper ones.
    ttTable.clear();
    ttHits    = 0;
    ttLookups = 0;

    // Move order starts center-biased; each pass refines it by score.
    vector<int> moves = getValidMoves(board);
    sort(moves.begin(), moves.end(), [](int a, int b){
        return abs(a - 3) < abs(b - 3);
    });

    for (int depth = 1; depth <= maxDepth; depth++) {
        int bestScore        = INT_MIN;
        int bestColThisDepth = moves[0];

        for (int col : moves) {
            Board copy = copyBoard(board);

            // Apply the top-level move to both the board copy and the running
            // hash, then restore the hash after minimax returns.
            int row = dropDisc(copy, col, player);
            zobristFlip(row, col, player);

            int score = minimax(copy, depth, INT_MIN, INT_MAX, false, player, opponent);

            zobristFlip(row, col, player);

            if (score > bestScore) {
                bestScore        = score;
                bestColThisDepth = col;
            }
        }

        bestCol = bestColThisDepth;

        // Re-sort so the best column from this pass is searched first next time.
        // This is the key insight that makes iterative deepening efficient.
        sort(moves.begin(), moves.end(), [&](int a, int b) {
            return (a == bestCol) > (b == bestCol);
        });

        cout << GRAY
             << "  [ID] depth " << depth
             << "  →  best col " << bestCol
             << "  |  score "    << bestScore
             << RESET << "\n";
    }

    cout << GRAY
         << "  [TT] Lookups: " << ttLookups
         << "  |  Hits: "      << ttHits
         << "  |  Hit rate: "
         << (ttLookups > 0 ? (100 * ttHits / ttLookups) : 0) << "%"
         << RESET << "\n";

    return bestCol;
}