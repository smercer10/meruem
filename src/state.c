#include "state.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "bitboard.h"
#include "globals.h"

// clang-format off
static const char* const squares[65] = {
    "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
    "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
    "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
    "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
    "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
    "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
    "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "-"
};
// clang-format on

static constexpr char ascii_pieces[12] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};

void print_state(const State* state) {
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
