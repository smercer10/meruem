#include "movegen.h"

#include <assert.h>
#include <stdbit.h>
#include <stdint.h>
#include <stdio.h>

#include "attacks.h"
#include "bitboard.h"
#include "globals.h"
#include "leapers.h"
#include "sliders.h"
#include "state.h"

Move encode_move(int source_sq, int target_sq, int moved_piece, int promoted_piece, bool is_capture,
                 bool is_double_push, bool is_en_passant, bool is_castling) {
    assert(source_sq >= 0 && source_sq < 64);
    assert(target_sq >= 0 && target_sq < 64);
    assert(moved_piece >= 0 && moved_piece < 12);
    assert(promoted_piece < 12);

    if (promoted_piece < 0) promoted_piece = 12;  // No promotion

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    Move move = {.source_sq = (uint32_t)source_sq,
                 .target_sq = (uint32_t)target_sq,
                 .moved_piece = (uint32_t)moved_piece,
                 .promoted_piece = (uint32_t)promoted_piece,
                 .is_capture = is_capture,
                 .is_double_push = is_double_push,
                 .is_en_passant = is_en_passant,
                 .is_castling = is_castling,
                 .unused = 0};
#pragma GCC diagnostic pop

    return move;
}

int get_source_sq(Move move) { return move.source_sq; }
int get_target_sq(Move move) { return move.target_sq; }
int get_moved_piece(Move move) { return move.moved_piece; }
int get_promoted_piece(Move move) { return move.promoted_piece; }
bool is_capture(Move move) { return move.is_capture; }
bool is_double_push(Move move) { return move.is_double_push; }
bool is_en_passant(Move move) { return move.is_en_passant; }
bool is_castling(Move move) { return move.is_castling; }

void add_move(MoveList* move_list, Move move) {
    assert(move_list != nullptr);
    assert(move_list->count < 256);
    move_list->moves[move_list->count++] = move;
}

void print_move(Move move) {
    printf("%s%s", squares[move.source_sq], squares[move.target_sq]);
    if (move.promoted_piece < 12) {
        printf("%c", promotion_pieces[move.promoted_piece]);
    }
}

void print_move_list(const MoveList* move_list) {
    assert(move_list != nullptr);
    for (int i = 0; i < move_list->count; ++i) {
        printf("%d: ", i + 1);
        print_move(move_list->moves[i]);
        printf(",%c,%c,%c,%c,%c\n", ascii_pieces[get_moved_piece(move_list->moves[i])],
               is_capture(move_list->moves[i]) ? 'y' : 'n', is_double_push(move_list->moves[i]) ? 'y' : 'n',
               is_en_passant(move_list->moves[i]) ? 'y' : 'n', is_castling(move_list->moves[i]) ? 'y' : 'n');
    }
    printf("\nTotal moves: %d\n", move_list->count);
}

// TODO: Refactor this to reduce code duplication and improve performance
void generate_moves(const State* state, MoveList* move_list) {
    assert(state != nullptr && move_list != nullptr);

    move_list->count = 0;

    uint64_t bb = 0;
    uint64_t attacks = 0;
    int source_sq = NA;
    int target_sq = NA;

    if (state->packed.side == WHITE) {
        // Pawn moves
        bb = state->pieces[WP];
        while (bb) {
            source_sq = pop_lsb(&bb);
            if (source_sq <= H7) {
                if (!is_set(state->occupancy[ALL], source_sq + 8)) {  // One forward
                    target_sq = source_sq + 8;
                    if (target_sq >= A8) {  // Promotion
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WQ, false, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WR, false, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WB, false, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WN, false, false, false, false));
                    } else {
                        add_move(move_list, encode_move(source_sq, target_sq, WP, -1, false, false, false, false));
                    }
                    if (source_sq <= H2 && !is_set(state->occupancy[ALL], source_sq + 16)) {  // Double push
                        target_sq = source_sq + 16;
                        add_move(move_list, encode_move(source_sq, target_sq, WP, -1, false, true, false, false));
                    }
                }
                // Normal captures
                attacks = pawn_attacks[WHITE][source_sq] & state->occupancy[BLACK];
                while (attacks) {
                    target_sq = pop_lsb(&attacks);
                    if (target_sq >= A8) {  // Promotion
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WQ, true, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WR, true, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WB, true, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, WP, WN, true, false, false, false));
                    } else {
                        add_move(move_list, encode_move(source_sq, target_sq, WP, -1, true, false, false, false));
                    }
                }
                // En passant
                const int ep_sq = state->packed.en_passant;
                if (ep_sq != NA) {
                    if (pawn_attacks[WHITE][source_sq] & (UINT64_C(1) << ep_sq)) {
                        target_sq = ep_sq;
                        add_move(move_list, encode_move(source_sq, target_sq, WP, -1, true, false, true, false));
                    }
                }
            }
        }
        // Castling moves
        // King-side
        if ((state->packed.castling & WKS)  // Implies king and rook are on their original squaresS
            && !is_set(state->occupancy[ALL], F1) && !is_set(state->occupancy[ALL], G1) &&
            !is_attacked(E1, BLACK, state) && !is_attacked(F1, BLACK, state)) {  // Check if G1 is under attack later
            add_move(move_list, encode_move(E1, G1, WK, -1, false, false, false, true));
        }
        // Queen-side
        if ((state->packed.castling & WQS) &&  // Implies king and rook are on their original squares
            !is_set(state->occupancy[ALL], D1) && !is_set(state->occupancy[ALL], C1) &&
            !is_set(state->occupancy[ALL], B1) && !is_attacked(E1, BLACK, state) &&
            !is_attacked(D1, BLACK, state)) {  // Check if C1 is under attack later
            add_move(move_list, encode_move(E1, C1, WK, -1, false, false, false, true));
        }
    } else {
        // Pawn moves
        bb = state->pieces[BP];
        while (bb) {
            source_sq = pop_lsb(&bb);
            if (source_sq >= A2) {
                if (!is_set(state->occupancy[ALL], source_sq - 8)) {  // One forward
                    target_sq = source_sq - 8;
                    if (target_sq <= H1) {  // Promotion
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BQ, false, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BR, false, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BB, false, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BN, false, false, false, false));
                    } else {
                        add_move(move_list, encode_move(source_sq, target_sq, BP, -1, false, false, false, false));
                    }
                    if (source_sq >= A7 && !is_set(state->occupancy[ALL], source_sq - 16)) {  // Double push
                        target_sq = source_sq - 16;
                        add_move(move_list, encode_move(source_sq, target_sq, BP, -1, false, true, false, false));
                    }
                }
                // Normal captures
                attacks = pawn_attacks[BLACK][source_sq] & state->occupancy[WHITE];
                while (attacks) {
                    target_sq = pop_lsb(&attacks);
                    if (target_sq <= H1) {  // Promotion
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BQ, true, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BR, true, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BB, true, false, false, false));
                        add_move(move_list, encode_move(source_sq, target_sq, BP, BN, true, false, false, false));
                    } else {
                        add_move(move_list, encode_move(source_sq, target_sq, BP, -1, true, false, false, false));
                    }
                }
                // En passant
                const int ep_sq = state->packed.en_passant;
                if (ep_sq != NA) {
                    if (pawn_attacks[BLACK][source_sq] & (UINT64_C(1) << ep_sq)) {
                        target_sq = ep_sq;
                        add_move(move_list, encode_move(source_sq, target_sq, BP, -1, true, false, true, false));
                    }
                }
            }
        }
        // Castling moves
        // King-side
        if ((state->packed.castling & BKS) &&  // Implies king and rook are on their original squares
            !is_set(state->occupancy[ALL], F8) && !is_set(state->occupancy[ALL], G8) &&
            !is_attacked(E8, WHITE, state) && !is_attacked(F8, WHITE, state)) {  // Check if G8 is under attack later
            add_move(move_list, encode_move(E8, G8, BK, -1, false, false, false, true));
        }
        // Queen-side
        if ((state->packed.castling & BQS) &&  // Implies king and rook are on their original squares
            !is_set(state->occupancy[ALL], D8) && !is_set(state->occupancy[ALL], C8) &&
            !is_set(state->occupancy[ALL], B8) && !is_attacked(E8, WHITE, state) &&
            !is_attacked(D8, WHITE, state)) {  // Check if C8 is under attack later
            add_move(move_list, encode_move(E8, C8, BK, -1, false, false, false, true));
        }
    }
    // Knight moves
    int piece = (state->packed.side == WHITE) ? WN : BN;
    bb = state->pieces[piece];
    while (bb) {
        source_sq = pop_lsb(&bb);
        attacks = knight_attacks[source_sq] & ~state->occupancy[state->packed.side];
        while (attacks) {
            target_sq = pop_lsb(&attacks);
            if (is_set(state->occupancy[!state->packed.side], target_sq)) {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, true, false, false, false));
            } else {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, false, false, false, false));
            }
        }
    }
    // Bishop moves
    piece = (state->packed.side == WHITE) ? WB : BB;
    bb = state->pieces[piece];
    while (bb) {
        source_sq = pop_lsb(&bb);
        attacks = get_bishop_attacks(source_sq, state->occupancy[ALL]) & ~state->occupancy[state->packed.side];
        while (attacks) {
            target_sq = pop_lsb(&attacks);
            if (is_set(state->occupancy[!state->packed.side], target_sq)) {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, true, false, false, false));
            } else {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, false, false, false, false));
            }
        }
    }
    // Rook moves
    piece = (state->packed.side == WHITE) ? WR : BR;
    bb = state->pieces[piece];
    while (bb) {
        source_sq = pop_lsb(&bb);
        attacks = get_rook_attacks(source_sq, state->occupancy[ALL]) & ~state->occupancy[state->packed.side];
        while (attacks) {
            target_sq = pop_lsb(&attacks);
            if (is_set(state->occupancy[!state->packed.side], target_sq)) {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, true, false, false, false));
            } else {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, false, false, false, false));
            }
        }
    }
    // Queen moves
    piece = (state->packed.side == WHITE) ? WQ : BQ;
    bb = state->pieces[piece];
    while (bb) {
        source_sq = pop_lsb(&bb);
        attacks = get_queen_attacks(source_sq, state->occupancy[ALL]) & ~state->occupancy[state->packed.side];
        while (attacks) {
            target_sq = pop_lsb(&attacks);
            if (is_set(state->occupancy[!state->packed.side], target_sq)) {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, true, false, false, false));
            } else {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, false, false, false, false));
            }
        }
    }
    // King moves (not castling)
    piece = (state->packed.side == WHITE) ? WK : BK;
    bb = state->pieces[piece];
    while (bb) {
        source_sq = pop_lsb(&bb);
        attacks = king_attacks[source_sq] & ~state->occupancy[state->packed.side];
        while (attacks) {
            target_sq = pop_lsb(&attacks);
            if (is_set(state->occupancy[!state->packed.side], target_sq)) {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, true, false, false, false));
            } else {
                add_move(move_list, encode_move(source_sq, target_sq, piece, -1, false, false, false, false));
            }
        }
    }
}

bool make_move(State* state, Move move, int move_type) {
    assert(state != nullptr);
    assert(move_type == ALL_MOVES || move_type == JUST_CAPTURES);

    if (move_type == JUST_CAPTURES) {
        if (move.is_capture) {
            make_move(state, move, ALL_MOVES);
        } else {
            return false;
        }
    } else {
        // State backup = *state;

        clear_bit(&state->pieces[move.moved_piece], move.source_sq);
        set_bit(&state->pieces[move.moved_piece], move.target_sq);

        if (move.is_capture) {
            int piece_idx_start = (state->packed.side == WHITE) ? 6 : 0;  // Opponent's pieces
            for (int i = piece_idx_start; i < piece_idx_start + 6; ++i) {
                if (is_set(state->pieces[i], move.target_sq)) {
                    clear_bit(&state->pieces[i], move.target_sq);
                    break;
                }
            }
        }

        if (move.promoted_piece != 12) {
            clear_bit(&state->pieces[move.moved_piece], move.target_sq);
            set_bit(&state->pieces[move.promoted_piece], move.target_sq);
        }

        if (move.is_en_passant) {
            if (state->packed.side == WHITE) {
                clear_bit(&state->pieces[BP], move.target_sq - 8);
            } else {
                clear_bit(&state->pieces[WP], move.target_sq + 8);
            }
        }

        // *state = backup;
    }
    return true;
}
