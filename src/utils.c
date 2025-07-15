#include "utils.h"

#include <assert.h>
#include <constants.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

bool is_set(uint64_t bb, int sq) {
    assert(sq >= A1 && sq <= H8);
    return (bb & (UINT64_C(1) << sq)) != 0;
}

uint64_t set_bit(uint64_t bb, int sq) {
    assert(sq >= A1 && sq <= H8);
    return bb | (UINT64_C(1) << sq);
}

uint64_t clear_bit(uint64_t bb, int sq) {
    assert(sq >= A1 && sq <= H8);
    return bb & ~(UINT64_C(1) << sq);
}

void print_bitboard(uint64_t bb) {
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            if (f == 0) printf("%d | ", r + 1);
            auto sq = r * 8 + f;
            printf("%d ", is_set(bb, sq) ? 1 : 0);
        }
        puts("");
    }
    puts("  +----------------");
    puts("    A B C D E F G H");
    printf("\nHex: 0x%016" PRIX64 "\n\n", bb);
}
