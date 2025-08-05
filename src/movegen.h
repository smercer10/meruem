#pragma once

#include <assert.h>

#include "attacks.h"
#include "globals.h"
#include "state.h"

typedef union {
    uint32_t raw;
    struct {
        uint32_t source_sq : 6;
        uint32_t target_sq : 6;
        uint32_t moved_piece : 4;
        uint32_t promoted_piece : 4;
        uint32_t is_capture : 1;
        uint32_t is_double_push : 1;
        uint32_t is_en_passant : 1;
        uint32_t is_castling : 1;
        uint32_t is_invalid : 1;
        uint32_t unused : 11;
    };
} Move;

static constexpr int MAX_MOVES = 256;
typedef struct {
    Move moves[MAX_MOVES];
    int count;
} MoveList;

enum MoveType { ALL_MOVES, JUST_CAPTURES };

static inline bool is_sq_attacked(const State* restrict state, int sq, int attacking_side) {
    assert(state != nullptr);
    assert(sq >= A1 && sq <= H8);
    assert(attacking_side == WHITE || attacking_side == BLACK);

    const int idx_offset = ((attacking_side == WHITE) ? 0 : 6);

    if (knight_attacks[sq] & state->pieces[WN + idx_offset]) return true;
    if (king_attacks[sq] & state->pieces[WK + idx_offset]) return true;
    if (pawn_attacks[!attacking_side][sq] & state->pieces[WP + idx_offset]) return true;

    if (get_bishop_attacks(sq, state->occupancy[BOTH_SIDES]) &
        (state->pieces[WB + idx_offset] | state->pieces[WQ + idx_offset]))
        return true;

    if (get_rook_attacks(sq, state->occupancy[BOTH_SIDES]) &
        (state->pieces[WR + idx_offset] | state->pieces[WQ + idx_offset]))
        return true;

    return false;
}

void gen_pseudo_legal_moves(const State* restrict state, MoveList* restrict move_list);
bool make_move(State* restrict state, Move move, int move_type);

void print_move(Move move);                                // For UCI
void print_move_list(const MoveList* restrict move_list);  // For debugging
