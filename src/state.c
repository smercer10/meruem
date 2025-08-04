#include "state.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "bitboard.h"
#include "globals.h"

void print_state(const State* restrict state) {
    assert(state != nullptr);

    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            if (f == 0) printf("%d | ", r + 1);
            const int sq = (r * 8) + f;
            int piece = INVALID_PIECE;
            for (int p = WP; p < NUM_PIECES; ++p) {
                if (is_bit_set(state->pieces[p], sq)) {
                    piece = p;
                    break;
                }
            }
            printf("%c ", piece == INVALID_PIECE ? '.' : ascii_pieces[piece]);
        }
        puts("");
    }
    puts("  +----------------");
    puts("    A B C D E F G H\n");

    printf("Side to move:      %s\n", (state->packed.side == WHITE) ? "White" : "Black");
    printf("En passant square: %s\n", squares[state->packed.en_passant]);

    // clang-format off
    printf("Castling rights:   %c%c%c%c\n", 
        (state->packed.castling & WKS) ? 'K' : '-', 
        (state->packed.castling & WQS) ? 'Q' : '-',
        (state->packed.castling & BKS) ? 'k' : '-', 
        (state->packed.castling & BQS) ? 'q' : '-');
    // clang-format on

    printf("Halfmove clock:    %d\n", state->packed.halfmove);
    printf("Fullmove number:   %d\n", state->packed.fullmove);
    printf("Zobrist hash:      0x%016" PRIX64 "\n\n", state->zobrist_hash);
}
