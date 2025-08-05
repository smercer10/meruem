#include "movegen.h"

#include <assert.h>
#include <stdbit.h>
#include <stdint.h>
#include <stdio.h>

#include "attacks.h"
#include "bitboard.h"
#include "globals.h"
#include "state.h"

static inline Move encode_move(int source_sq, int target_sq, int moved_piece, int promoted_piece, bool is_capture,
                               bool is_double_push, bool is_en_passant, bool is_castling) {
    assert(source_sq >= A1 && source_sq <= H8);
    assert(target_sq >= A1 && target_sq <= H8);
    assert(moved_piece >= WP && moved_piece <= BK);
    assert(promoted_piece >= WN && promoted_piece <= INVALID_PIECE);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    return (Move){.source_sq = (uint32_t)source_sq,
                  .target_sq = (uint32_t)target_sq,
                  .moved_piece = (uint32_t)moved_piece,
                  .promoted_piece = (uint32_t)promoted_piece,
                  .is_capture = is_capture,
                  .is_double_push = is_double_push,
                  .is_en_passant = is_en_passant,
                  .is_castling = is_castling,
                  .is_invalid = false,
                  .unused = 0};
#pragma GCC diagnostic pop
}

static inline void add_move(MoveList* restrict move_list, Move move) {
    assert(move_list != nullptr);
    assert(move_list->count < MAX_MOVES);
    move_list->moves[move_list->count++] = move;
}

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

static inline void add_pawn_promotion_moves(MoveList* restrict move_list, int source_sq, int target_sq, int pawn_type,
                                            bool is_capture) {
    assert(move_list != nullptr);
    assert(source_sq >= A1 && source_sq <= H8);
    assert(target_sq >= A1 && target_sq <= H8);
    assert(pawn_type == WP || pawn_type == BP);

    // Pawn values can be directly used as the index offset
    add_move(move_list, encode_move(source_sq, target_sq, pawn_type, WQ + pawn_type, is_capture, false, false, false));
    add_move(move_list, encode_move(source_sq, target_sq, pawn_type, WR + pawn_type, is_capture, false, false, false));
    add_move(move_list, encode_move(source_sq, target_sq, pawn_type, WB + pawn_type, is_capture, false, false, false));
    add_move(move_list, encode_move(source_sq, target_sq, pawn_type, WN + pawn_type, is_capture, false, false, false));
}

// Assume square is valid
static inline bool sq_in_last_rank(int sq, int side) { return (side == WHITE) ? (sq >= A8) : (sq <= H1); }
static inline bool sq_in_last_two_ranks(int sq, int side) { return (side == WHITE) ? (sq >= A7) : (sq <= H2); }
static inline bool pawn_hasnt_moved(int sq, int side) { return sq_in_last_two_ranks(sq, !side); }

static inline void add_pawn_moves(const State* restrict state, MoveList* restrict move_list, int pawn_type) {
    assert(state != nullptr && move_list != nullptr);
    assert(pawn_type == WP || pawn_type == BP);

    const int forward_jump = (pawn_type == WP) ? 8 : -8;

    uint64_t pawn = state->pieces[pawn_type];
    while (pawn) {
        const int source_sq = pop_lsb(&pawn);
        if (!sq_in_last_rank(source_sq, state->packed.side)) {
            const int target_sq = source_sq + forward_jump;
            if (!is_bit_set(state->occupancy[BOTH_SIDES], target_sq)) {
                if (sq_in_last_rank(target_sq, state->packed.side)) {
                    add_pawn_promotion_moves(move_list, source_sq, target_sq, pawn_type, false);
                } else {
                    add_move(move_list,
                             encode_move(source_sq, target_sq, pawn_type, INVALID_PIECE, false, false, false, false));
                }
                if (pawn_hasnt_moved(source_sq, state->packed.side) &&
                    !is_bit_set(state->occupancy[BOTH_SIDES], target_sq + forward_jump)) {
                    add_move(move_list, encode_move(source_sq, target_sq + forward_jump, pawn_type, INVALID_PIECE,
                                                    false, true, false, false));
                }
            }
        }
        uint64_t attacks = pawn_attacks[state->packed.side][source_sq] & state->occupancy[!state->packed.side];
        while (attacks) {
            const int target_sq = pop_lsb(&attacks);
            if (sq_in_last_rank(target_sq, state->packed.side)) {
                add_pawn_promotion_moves(move_list, source_sq, target_sq, pawn_type, true);
            } else {
                add_move(move_list,
                         encode_move(source_sq, target_sq, pawn_type, INVALID_PIECE, true, false, false, false));
            }
        }
        if (state->packed.en_passant != INVALID_SQ) {
            if (pawn_attacks[state->packed.side][source_sq] & (UINT64_C(1) << state->packed.en_passant)) {
                add_move(move_list, encode_move(source_sq, state->packed.en_passant, pawn_type, INVALID_PIECE, true,
                                                false, true, false));
            }
        }
    }
}

static inline void add_castling_moves(const State* restrict state, MoveList* restrict move_list) {
    assert(state != nullptr && move_list != nullptr);

    if (state->packed.side == WHITE) {
        if ((state->packed.castling & WKS) && !is_bit_set(state->occupancy[BOTH_SIDES], F1) &&
            !is_bit_set(state->occupancy[BOTH_SIDES], G1) && !is_sq_attacked(state, E1, BLACK) &&
            !is_sq_attacked(state, F1, BLACK)) {
            add_move(move_list, encode_move(E1, G1, WK, INVALID_PIECE, false, false, false, true));
        }
        if ((state->packed.castling & WQS) && !is_bit_set(state->occupancy[BOTH_SIDES], D1) &&
            !is_bit_set(state->occupancy[BOTH_SIDES], C1) && !is_bit_set(state->occupancy[BOTH_SIDES], B1) &&
            !is_sq_attacked(state, E1, BLACK) && !is_sq_attacked(state, D1, BLACK)) {
            add_move(move_list, encode_move(E1, C1, WK, INVALID_PIECE, false, false, false, true));
        }
    } else {
        if ((state->packed.castling & BKS) && !is_bit_set(state->occupancy[BOTH_SIDES], F8) &&
            !is_bit_set(state->occupancy[BOTH_SIDES], G8) && !is_sq_attacked(state, E8, WHITE) &&
            !is_sq_attacked(state, F8, WHITE)) {
            add_move(move_list, encode_move(E8, G8, BK, INVALID_PIECE, false, false, false, true));
        }
        if ((state->packed.castling & BQS) && !is_bit_set(state->occupancy[BOTH_SIDES], D8) &&
            !is_bit_set(state->occupancy[BOTH_SIDES], C8) && !is_bit_set(state->occupancy[BOTH_SIDES], B8) &&
            !is_sq_attacked(state, E8, WHITE) && !is_sq_attacked(state, D8, WHITE)) {
            add_move(move_list, encode_move(E8, C8, BK, INVALID_PIECE, false, false, false, true));
        }
    }
}

static inline void add_generic_piece_moves(const State* restrict state, MoveList* restrict move_list,
                                           uint64_t (*attacks_func)(int, uint64_t), int piece_type) {
    assert(state != nullptr && move_list != nullptr && attacks_func != nullptr);
    assert(piece_type >= WN && piece_type <= BK);

    uint64_t piece = state->pieces[piece_type];
    while (piece) {
        const int source_sq = pop_lsb(&piece);
        uint64_t attacks =
            attacks_func(source_sq, state->occupancy[BOTH_SIDES]) & ~state->occupancy[state->packed.side];
        while (attacks) {
            const int target_sq = pop_lsb(&attacks);
            if (is_bit_set(state->occupancy[!state->packed.side], target_sq)) {
                add_move(move_list,
                         encode_move(source_sq, target_sq, piece_type, INVALID_PIECE, true, false, false, false));
            } else {
                add_move(move_list,
                         encode_move(source_sq, target_sq, piece_type, INVALID_PIECE, false, false, false, false));
            }
        }
    }
}

void gen_pseudo_legal_moves(const State* restrict state, MoveList* restrict move_list) {
    assert(state != nullptr && move_list != nullptr);

    move_list->count = 0;

    const int idx_offset = ((state->packed.side == WHITE) ? 0 : 6);
    add_pawn_moves(state, move_list, WP + idx_offset);
    add_castling_moves(state, move_list);
    add_generic_piece_moves(state, move_list, get_knight_attacks, WN + idx_offset);
    add_generic_piece_moves(state, move_list, get_bishop_attacks, WB + idx_offset);
    add_generic_piece_moves(state, move_list, get_rook_attacks, WR + idx_offset);
    add_generic_piece_moves(state, move_list, get_queen_attacks, WQ + idx_offset);
    add_generic_piece_moves(state, move_list, get_king_attacks, WK + idx_offset);
}

// clang-format off
static constexpr uint32_t castling_rights[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    7,  15, 15, 15, 3,  15, 15, 11
};
// clang-format on

bool make_move(State* restrict state, Move move, int move_type) {
    assert(state != nullptr);
    assert(move_type == ALL_MOVES || move_type == JUST_CAPTURES);

    if (move.is_invalid) return false;

    int idx_offset = 0;
    int rev_idx_offset = 6;
    int ep_direction = -8;
    if (state->packed.side == BLACK) {
        idx_offset = 6;
        rev_idx_offset = 0;
        ep_direction = 8;
    }

    if (move_type == JUST_CAPTURES) {
        if (move.is_capture) {
            make_move(state, move, ALL_MOVES);
        } else {
            return false;
        }
    } else {
        State backup = *state;

        clear_bit(&state->pieces[move.moved_piece], move.source_sq);
        set_bit(&state->pieces[move.moved_piece], move.target_sq);

        if (move.is_capture) {
            for (int i = rev_idx_offset; i < rev_idx_offset + 6; ++i) {
                if (is_bit_set(state->pieces[i], move.target_sq)) {
                    clear_bit(&state->pieces[i], move.target_sq);
                    break;
                }
            }
        }

        if (move.promoted_piece != INVALID_PIECE) {
            clear_bit(&state->pieces[move.moved_piece], move.target_sq);
            set_bit(&state->pieces[move.promoted_piece], move.target_sq);
        }

        if (move.is_en_passant) clear_bit(&state->pieces[WP + rev_idx_offset], move.target_sq + ep_direction);

#define FIRST_RANK_SQ(file) ((state->packed.side == WHITE) ? file##1 : file##8)

        if (move.is_castling) {
            if (move.target_sq == FIRST_RANK_SQ(G)) {
                clear_bit(&state->pieces[WR + idx_offset], FIRST_RANK_SQ(H));
                set_bit(&state->pieces[WR + idx_offset], FIRST_RANK_SQ(F));
            } else if (move.target_sq == FIRST_RANK_SQ(C)) {
                clear_bit(&state->pieces[WR + idx_offset], FIRST_RANK_SQ(A));
                set_bit(&state->pieces[WR + idx_offset], FIRST_RANK_SQ(D));
            }
        }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        state->packed.en_passant = INVALID_SQ;
        if (move.is_double_push) state->packed.en_passant = (uint32_t)(move.target_sq + ep_direction);

        state->packed.castling &= castling_rights[move.source_sq];
        state->packed.castling &= castling_rights[move.target_sq];
#pragma GCC diagnostic pop

        if (move.is_capture || move.moved_piece == WP || move.moved_piece == BP) {
            state->packed.halfmove = 0;
        } else {
            ++state->packed.halfmove;
        }
        if (state->packed.side == BLACK) ++state->packed.fullmove;

        state->occupancy[WHITE] = 0;
        state->occupancy[BLACK] = 0;
        state->occupancy[BOTH_SIDES] = 0;
        state->occupancy[WHITE] |= state->pieces[WP] | state->pieces[WN] | state->pieces[WB] | state->pieces[WR] |
                                   state->pieces[WQ] | state->pieces[WK];
        state->occupancy[BLACK] |= state->pieces[BP] | state->pieces[BN] | state->pieces[BB] | state->pieces[BR] |
                                   state->pieces[BQ] | state->pieces[BK];
        state->occupancy[BOTH_SIDES] = state->occupancy[WHITE] | state->occupancy[BLACK];

        if (is_sq_attacked(state, get_lsb(state->pieces[WK + idx_offset]), !state->packed.side)) {
            *state = backup;
            return false;
        }

        state->packed.side = !state->packed.side;
    }

    return true;
}

static constexpr char promotion_pieces[] = {
    [WQ] = 'q', [WR] = 'r', [WB] = 'b', [WN] = 'n', [BQ] = 'q', [BR] = 'r', [BB] = 'b', [BN] = 'n'};

void print_move(Move move) {
    printf("%s%s", squares[move.source_sq], squares[move.target_sq]);
    if (move.promoted_piece != INVALID_PIECE) printf("%c", promotion_pieces[move.promoted_piece]);
}

void print_move_list(const MoveList* restrict move_list) {
    assert(move_list != nullptr);

    for (int i = 0; i < move_list->count; ++i) {
        printf("%d: ", i + 1);
        print_move(move_list->moves[i]);
        printf(",%c,%c,%c,%c,%c\n", ascii_pieces[move_list->moves[i].moved_piece],
               move_list->moves[i].is_capture ? 'y' : 'n', move_list->moves[i].is_double_push ? 'y' : 'n',
               move_list->moves[i].is_en_passant ? 'y' : 'n', move_list->moves[i].is_castling ? 'y' : 'n');
    }
    printf("\nTotal moves: %d\n", move_list->count);
}
