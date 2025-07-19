#include "state.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "bitboard.h"
#include "globals.h"

void print_state(const State* state) {
    assert(state != nullptr);

    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            if (f == 0) printf("%d | ", r + 1);

            const int sq = r * 8 + f;
            int piece = -1;
            for (int p = 0; p < 12; ++p) {
                if (is_set(state->pieces[p], sq)) {
                    piece = p;
                    break;
                }
            }
            printf("%c ", piece < 0 ? '.' : ascii_pieces[piece]);
        }
        puts("");
    }
    puts("  +----------------");
    puts("    A B C D E F G H\n");

    printf("Side to move:      %s\n", !state->packed.side ? "White" : "Black");
    printf("En passant square: %s\n", squares[state->packed.en_passant]);

    // clang-format off
    uint32_t castling = state->packed.castling;
    printf("Castling rights:   %c%c%c%c\n", 
        (castling & WKS) ? 'K' : '-', 
        (castling & WQS) ? 'Q' : '-',
        (castling & BKS) ? 'k' : '-', 
        (castling & BQS) ? 'q' : '-');
    // clang-format on

    printf("Halfmove clock:    %d\n", state->packed.halfmove);
    printf("Fullmove number:   %d\n", state->packed.fullmove);
    printf("Zobrist hash:      0x%016" PRIX64 "\n\n", state->zobrist_hash);
}
