#include "movegen.h"

#include <assert.h>
#include <stdbit.h>
#include <stdint.h>
#include <stdio.h>

#include "attacks.h"
#include "bitboard.h"
#include "globals.h"
#include "leapers.h"
#include "state.h"

// TODO: Refactor this to reduce code duplication and improve performance
void generate_moves(const State* state) {
    assert(state != nullptr);

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
                        printf("%s%sQ\n", squares[source_sq], squares[target_sq]);
                        printf("%s%sR\n", squares[source_sq], squares[target_sq]);
                        printf("%s%sB\n", squares[source_sq], squares[target_sq]);
                        printf("%s%sN\n", squares[source_sq], squares[target_sq]);
                    } else {
                        printf("%s%s\n", squares[source_sq], squares[target_sq]);
                    }
                    if (source_sq <= H2 && !is_set(state->occupancy[ALL], source_sq + 16)) {  // Two forward
                        target_sq = source_sq + 16;
                        printf("%s%s\n", squares[source_sq], squares[target_sq]);
                    }
                }
                // Normal captures
                attacks = pawn_attacks[WHITE][source_sq] & state->occupancy[BLACK];
                while (attacks) {
                    target_sq = pop_lsb(&attacks);
                    if (target_sq >= A8) {  // Promotion
                        printf("%sx%sQ\n", squares[source_sq], squares[target_sq]);
                        printf("%sx%sR\n", squares[source_sq], squares[target_sq]);
                        printf("%sx%sB\n", squares[source_sq], squares[target_sq]);
                        printf("%sx%sN\n", squares[source_sq], squares[target_sq]);
                    } else {
                        printf("%sx%s\n", squares[source_sq], squares[target_sq]);
                    }
                }
                // En passant
                const int ep_sq = state->packed.en_passant;
                if (ep_sq != NA) {
                    if (pawn_attacks[WHITE][source_sq] & (UINT64_C(1) << ep_sq)) {
                        target_sq = ep_sq;
                        printf("%sx%s\n", squares[source_sq], squares[target_sq]);
                    }
                }
            }
        }
        // Castling moves
        // King-side
        if ((state->packed.castling & WKS)  // Implies king and rook are on their original squaresS
            && !is_set(state->occupancy[ALL], F1) && !is_set(state->occupancy[ALL], G1) &&
            !is_attacked(E1, BLACK, state) && !is_attacked(F1, BLACK, state)) {  // Check if G1 is under attack later
            puts("0-0");
        }
        // Queen-side
        if ((state->packed.castling & WQS) &&  // Implies king and rook are on their original squares
            !is_set(state->occupancy[ALL], D1) && !is_set(state->occupancy[ALL], C1) &&
            !is_set(state->occupancy[ALL], B1) && !is_attacked(E1, BLACK, state) &&
            !is_attacked(D1, BLACK, state)) {  // Check if C1 is under attack later
            puts("0-0-0");
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
                        printf("%s%sQ\n", squares[source_sq], squares[target_sq]);
                        printf("%s%sR\n", squares[source_sq], squares[target_sq]);
                        printf("%s%sB\n", squares[source_sq], squares[target_sq]);
                        printf("%s%sN\n", squares[source_sq], squares[target_sq]);
                    } else {
                        printf("%s%s\n", squares[source_sq], squares[target_sq]);
                    }
                    if (source_sq >= A7 && !is_set(state->occupancy[ALL], source_sq - 16)) {  // Two forward
                        target_sq = source_sq - 16;
                        printf("%s%s\n", squares[source_sq], squares[target_sq]);
                    }
                }
                // Normal captures
                attacks = pawn_attacks[BLACK][source_sq] & state->occupancy[WHITE];
                while (attacks) {
                    target_sq = pop_lsb(&attacks);
                    if (target_sq <= H1) {  // Promotion
                        printf("%sx%sQ\n", squares[source_sq], squares[target_sq]);
                        printf("%sx%sR\n", squares[source_sq], squares[target_sq]);
                        printf("%sx%sB\n", squares[source_sq], squares[target_sq]);
                        printf("%sx%sN\n", squares[source_sq], squares[target_sq]);
                    } else {
                        printf("%sx%s\n", squares[source_sq], squares[target_sq]);
                    }
                }
                // En passant
                const int ep_sq = state->packed.en_passant;
                if (ep_sq != NA) {
                    if (pawn_attacks[BLACK][source_sq] & (UINT64_C(1) << ep_sq)) {
                        target_sq = ep_sq;
                        printf("%sx%s\n", squares[source_sq], squares[target_sq]);
                    }
                }
            }
        }
        // Castling moves
        // King-side
        if ((state->packed.castling & BKS) &&  // Implies king and rook are on their original squares
            !is_set(state->occupancy[ALL], F8) && !is_set(state->occupancy[ALL], G8) &&
            !is_attacked(E8, WHITE, state) && !is_attacked(F8, WHITE, state)) {  // Check if G8 is under attack later
            puts("0-0");
        }
        // Queen-side
        if ((state->packed.castling & BQS) &&  // Implies king and rook are on their original squares
            !is_set(state->occupancy[ALL], D8) && !is_set(state->occupancy[ALL], C8) &&
            !is_set(state->occupancy[ALL], B8) && !is_attacked(E8, WHITE, state) &&
            !is_attacked(D8, WHITE, state)) {  // Check if C8 is under attack later
            puts("0-0-0");
        }
    }
    // Knight moves
    bb = (state->packed.side == WHITE) ? state->pieces[WN] : state->pieces[BN];
    while (bb) {
        source_sq = pop_lsb(&bb);
        attacks = knight_attacks[source_sq] & ~state->occupancy[state->packed.side];
        while (attacks) {
            target_sq = pop_lsb(&attacks);
            if (is_set(state->occupancy[!state->packed.side], target_sq)) {
                printf("%sx%s\n", squares[source_sq], squares[target_sq]);
            } else {
                printf("%s%s\n", squares[source_sq], squares[target_sq]);
            }
        }
    }
}
