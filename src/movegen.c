#include "movegen.h"

#include <assert.h>
#include <stdbit.h>
#include <stdint.h>
#include <stdio.h>

#include "bitboard.h"
#include "globals.h"
#include "leapers.h"
#include "state.h"

void generate_moves(const State* state) {
    assert(state != nullptr);

    uint64_t bb = 0;
    int source_sq = NA;
    int target_sq = NA;

    if (state->packed.side == WHITE) {
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
                uint64_t attacks = pawn_attacks[WHITE][source_sq] & state->occupancy[BLACK];
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
    } else {
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
                uint64_t attacks = pawn_attacks[BLACK][source_sq] & state->occupancy[WHITE];
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
    }
}
