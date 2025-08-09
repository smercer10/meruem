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

static constexpr int mvv_lva_scores[NUM_PIECES][NUM_PIECES] = {
    {105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605},
    {104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604},
    {103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603},
    {102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602},
    {101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601},
    {100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600},
    {105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605},
    {104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604},
    {103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603},
    {102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602},
    {101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601},
    {100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600}};

int nodes_searched = 0;
static Move killer_moves[MAX_DEPTH][2] = {0};
static int history_scores[NUM_PIECES][64] = {0};

static inline void update_killer_moves(int ply, Move move) {
    if (ply >= MAX_DEPTH || move.is_capture || killer_moves[ply][0].raw == move.raw) return;
    killer_moves[ply][1] = killer_moves[ply][0];
    killer_moves[ply][0] = move;
}

static inline void update_history_scores(Move move, int depth) {
    if (move.is_capture) return;
    history_scores[move.moved_piece][move.target_sq] += depth * depth;
}

static inline int get_piece_on_square(const State* restrict state, int sq) {
    assert(state != nullptr);
    assert(sq >= A1 && sq <= H8);

    for (int p = WP; p < NUM_PIECES; ++p) {
        if (state->pieces[p] & (1ULL << sq)) return p;
    }
    return INVALID_PIECE;
}

static inline int score_move(const State* restrict state, Move move, int ply) {
    assert(state != nullptr);

    if (move.is_capture) return mvv_lva_scores[move.moved_piece][get_piece_on_square(state, move.target_sq)] + 10000;
    if (ply < MAX_DEPTH) {
        if (killer_moves[ply][0].raw == move.raw) return 9000;
        if (killer_moves[ply][1].raw == move.raw) return 8000;
    }
    return history_scores[move.moved_piece][move.target_sq];
}

static inline void sort_moves(const State* restrict state, MoveList* restrict move_list, int ply) {
    assert(state != nullptr && move_list != nullptr);

    int scores[MAX_MOVES];
    for (int i = 0; i < move_list->count; ++i) scores[i] = score_move(state, move_list->moves[i], ply);

    for (int i = 1; i < move_list->count; ++i) {
        const Move key = move_list->moves[i];
        const int key_score = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j] < key_score) {
            move_list->moves[j + 1] = move_list->moves[j];
            scores[j + 1] = scores[j];
            --j;
        }
        move_list->moves[j + 1] = key;
        scores[j + 1] = key_score;
    }
}

static inline int quiescence(State* restrict state, int alpha, int beta, int ply) {
    assert(state != nullptr);

    ++nodes_searched;

    const int stand_pat = eval_state(state);

    if (stand_pat >= beta) return beta;
    if (stand_pat > alpha) alpha = stand_pat;
    if ((stand_pat + 1000) < alpha) return alpha;  // If even the best capture is worse than alpha, return alpha

    MoveList move_list;
    gen_pseudo_legal_moves(state, &move_list);

    sort_moves(state, &move_list, ply);

    for (int i = 0; i < move_list.count; ++i) {
        const State backup = *state;

        if (!make_move(state, move_list.moves[i], JUST_CAPTURES)) continue;
        const int score = -quiescence(state, -beta, -alpha, ply + 1);

        *state = backup;

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

static inline int in_check(const State* restrict state) {
    assert(state != nullptr);
    assert(state->packed.side == WHITE || state->packed.side == BLACK);
    return is_sq_attacked(state,
                          ((state->packed.side == WHITE) ? get_lsb(state->pieces[WK]) : get_lsb(state->pieces[BK])),
                          !state->packed.side);
}

static inline int negamax(State* restrict state, Move* restrict best_move, int depth, int alpha, int beta, int ply) {
    assert(state != nullptr && best_move != nullptr);
    assert(depth >= 0);

    ++nodes_searched;

    if (depth == 0) return quiescence(state, alpha, beta, ply);

    MoveList move_list;
    gen_pseudo_legal_moves(state, &move_list);

    sort_moves(state, &move_list, ply);

    int legal_moves = 0;
    for (int i = 0; i < move_list.count; ++i) {
        const State backup = *state;

        if (!make_move(state, move_list.moves[i], ALL_MOVES)) continue;
        ++legal_moves;
        const int score = -negamax(state, best_move, depth - 1, -beta, -alpha, ply + 1);

        *state = backup;

        if (score >= beta) {
            update_killer_moves(ply, move_list.moves[i]);
            update_history_scores(move_list.moves[i], depth);
            return beta;
        }
        if (score > alpha) {
            alpha = score;
            if (ply == 0) *best_move = move_list.moves[i];
        }
    }

    if (legal_moves == 0) return (in_check(state) ? (-999999 + ply) : 0);

    return alpha;
}

void search_position(State* restrict state, int depth) {
    assert(state != nullptr);
    assert(depth >= 0);

    nodes_searched = 0;

    Move best_move = {.is_invalid = true};
    negamax(state, &best_move, depth, -999999, 999999, 0);

    printf("bestmove ");
    print_move(best_move);
    puts("");
}
