#pragma once

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

typedef struct {
    Move moves[256];
    int count;
} MoveList;

enum MoveType { ALL_MOVES, JUST_CAPTURES };

static constexpr char promotion_pieces[] = {
    [WQ] = 'q', [WR] = 'r', [WB] = 'b', [WN] = 'n', [BQ] = 'q', [BR] = 'r', [BB] = 'b', [BN] = 'n'};

Move encode_move(int source_sq, int target_sq, int moved_piece, int promoted_piece, bool is_capture,
                 bool is_double_push, bool is_en_passant, bool is_castling);
int get_source_sq(Move move);
int get_target_sq(Move move);
int get_moved_piece(Move move);
int get_promoted_piece(Move move);
bool is_capture(Move move);
bool is_double_push(Move move);
bool is_en_passant(Move move);
bool is_castling(Move move);

void add_move(MoveList* move_list, Move move);
void print_move(Move move);                       // For UCI
void print_move_list(const MoveList* move_list);  // For debugging

void generate_moves(const State* state, MoveList* move_list);
bool make_move(State* state, Move move, int move_type);
