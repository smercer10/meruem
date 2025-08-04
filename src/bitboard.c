#include "bitboard.h"

#include <assert.h>
#include <globals.h>
#include <inttypes.h>
#include <stdbit.h>
#include <stdint.h>
#include <stdio.h>

void print_bitboard(uint64_t bb) {
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            if (f == 0) printf("%d | ", r + 1);
            const int sq = (r * 8) + f;
            printf("%d ", is_bit_set(bb, sq) ? 1 : 0);
        }
        puts("");
    }
    puts("  +----------------");
    puts("    A B C D E F G H\n");
    printf("Hex: 0x%016" PRIX64 "\n\n", bb);
}
