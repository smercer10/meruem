#include "bitboard.h"

#include <assert.h>
#include <globals.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

bool is_set(uint64_t bb, int sq) {
    assert(sq >= 0 && sq < 64);
    return (bb & (UINT64_C(1) << sq)) != 0;
}

void set_bit(uint64_t* bb, int sq) {
    assert(sq >= 0 && sq < 64);
    *bb |= (UINT64_C(1) << sq);
}

void clear_bit(uint64_t* bb, int sq) {
    assert(sq >= 0 && sq < 64);
    *bb &= ~(UINT64_C(1) << sq);
}

void print_bitboard(uint64_t bb) {
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            if (f == 0) printf("%d | ", r + 1);
            const int sq = r * 8 + f;
            printf("%d ", is_set(bb, sq) ? 1 : 0);
        }
        puts("");
    }
    puts("  +----------------");
    puts("    A B C D E F G H\n");
    printf("Hex: 0x%016" PRIX64 "\n", bb);
}
