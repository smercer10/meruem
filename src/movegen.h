#pragma once

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

void gen_pseudo_legal_moves(const State* restrict state, MoveList* restrict move_list);
bool make_move(State* restrict state, Move move, int move_type);

void print_move(Move move);                                // For UCI
void print_move_list(const MoveList* restrict move_list);  // For debugging
