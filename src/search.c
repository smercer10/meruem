#include "search.h"

#include <assert.h>
#include <stdio.h>

#include "bitboard.h"
#include "globals.h"
#include "movegen.h"
#include "state.h"

static constexpr int material_scores[NUM_PIECES] = {100,  300,  350,  500,  1000,  10000,
                                                    -100, -300, -350, -500, -1000, -10000};

// clang-format off
static constexpr int pawn_positional_scores[64] = {
     0,  0,  0,   0,   0,  0,  0,  0,
     0,  0,  0, -10, -10,  0,  0,  0,
     0,  0,  0,   5,   5,  0,  0,  0,
     5,  5, 10,  20,  20, 10,  5,  5,
    10, 10, 10,  20,  20, 10, 10, 10,
    20, 20, 20,  30,  30, 20, 20, 20,
    30, 30, 30,  40,  40, 30, 30, 30, 
    90, 90, 90,  90,  90, 90, 90, 90
};
static constexpr int knight_positional_scores[64] = {
    -5, -10,  0,  0,  0,  0, -10, -5,
    -5,   0,  0,  0,  0,  0,   0, -5,
    -5,   5, 20, 10, 10, 20,   5, -5,
    -5,  10, 20, 30, 30, 20,  10, -5,
    -5,  10, 20, 30, 30, 20,  10, -5,
    -5,   5, 20, 20, 20, 20,   5, -5,
    -5,   0,  0, 10, 10,  0,   0, -5,
    -5,   0,  0,  0,  0,  0,   0, -5
};
static constexpr int bishop_positional_scores[64] = {
    0,  0, -10,  0,  0, -10,  0, 0,
    0, 30,   0,  0,  0,   0, 30, 0,
    0, 10,   0,  0,  0,   0, 10, 0,
    0,  0,  10, 20, 20,  10,  0, 0,
    0,  0,  10, 20, 20,  10,  0, 0,
    0,  0,   0, 10, 10,   0,  0, 0,
    0,  0,   0,  0,  0,   0,  0, 0,
    0,  0,   0,  0,  0,   0,  0, 0
};
static constexpr int rook_positional_scores[64] = {
    0,  0,  0, 20, 20,  0,  0,  0,
    0,  0, 10, 20, 20, 10,  0,  0,
    0,  0, 10, 20, 20, 10,  0,  0,
    0,  0, 10, 20, 20, 10,  0,  0,
    0,  0, 10, 20, 20, 10,  0,  0,
    0,  0, 10, 20, 20, 10,  0,  0,
   50, 50, 50, 50, 50, 50, 50, 50,
   50, 50, 50, 50, 50, 50, 50, 50
};
static constexpr int king_positional_scores[64] = {
    0, 0,  5,  0, -15,  0, 10, 0,
    0, 5,  5, -5,  -5,  0,  5, 0,
    0, 0,  5, 10,  10,  5,  0, 0,
    0, 5, 10, 20,  20, 10,  5, 0,
    0, 5, 10, 20,  20, 10,  5, 0,
    0, 5,  5, 10,  10,  5,  5, 0,
    0, 0,  5,  5,   5,  5,  0, 0,
    0, 0,  0,  0,   0,  0,  0, 0
};
// clang-format on

int eval_state(const State* restrict state) {
    assert(state != nullptr);

    int score = 0;
    for (int i = WP; i < NUM_PIECES; ++i) {
        uint64_t piece = state->pieces[i];
        while (piece) {
            score += material_scores[i];
            int sq = pop_lsb(&piece);
            if (i >= BP) sq ^= 56;  // Mirror positional scores
            switch (i) {
                case WP: score += pawn_positional_scores[sq]; break;
                case WN: score += knight_positional_scores[sq]; break;
                case WB: score += bishop_positional_scores[sq]; break;
                case WR: score += rook_positional_scores[sq]; break;
                case WK: score += king_positional_scores[sq]; break;
                case BP: score -= pawn_positional_scores[sq]; break;
                case BN: score -= knight_positional_scores[sq]; break;
                case BB: score -= bishop_positional_scores[sq]; break;
                case BR: score -= rook_positional_scores[sq]; break;
                case BK: score -= king_positional_scores[sq]; break;
            }
        }
    }
    return (state->packed.side == WHITE) ? score : -score;
}

static inline int negamax(State* restrict state, Move* restrict best_move, int depth, int max_depth, int alpha,
                          int beta) {
    assert(state != nullptr && best_move != nullptr);
    assert(depth >= 0 && max_depth >= 0);

    if (depth == 0) return eval_state(state);

    MoveList move_list;
    gen_pseudo_legal_moves(state, &move_list);
    int legal_moves = 0;

    for (int i = 0; i < move_list.count; ++i) {
        State backup = *state;

        if (!make_move(state, move_list.moves[i], ALL_MOVES)) continue;

        ++legal_moves;

        int score = -negamax(state, best_move, depth - 1, max_depth, -beta, -alpha);

        *state = backup;

        if (score >= beta) return beta;  // Beta cutoff
        if (score > alpha) {
            alpha = score;
            if (depth == max_depth) *best_move = move_list.moves[i];  // Update best move at root depth
        }
    }

    (void)legal_moves;  // TODO: Handle positions with no legal moves

    return alpha;  // Return the best score found
}

void search_position(State* restrict state, int depth) {
    assert(state != nullptr);
    assert(depth >= 0);

    Move best_move = {.is_invalid = true};
    negamax(state, &best_move, depth, depth, -999999, 999999);

    printf("bestmove ");
    print_move(best_move);
    puts("");
}
