#include "bitboard.h"

#include <assert.h>
#include <globals.h>
#include <inttypes.h>
#include <stdbit.h>
#include <stdint.h>
#include <stdio.h>

bool is_bit_set(uint64_t bb, int sq) {
    assert(sq >= A1 && sq <= H8);
    return bb & (UINT64_C(1) << sq);
}

void set_bit(uint64_t* restrict bb, int sq) {
    assert(bb != nullptr && sq >= A1 && sq <= H8);
    *bb |= (UINT64_C(1) << sq);
}

void clear_bit(uint64_t* restrict bb, int sq) {
    assert(bb != nullptr && sq >= A1 && sq <= H8);
    *bb &= ~(UINT64_C(1) << sq);
}

int get_lsb(uint64_t bb) {
    assert(bb != 0);
    return (int)stdc_first_trailing_one(bb) - 1;  // -1 to convert to 0-indexed square
}

int pop_lsb(uint64_t* restrict bb) {
    assert(bb != nullptr);
    const int sq = get_lsb(*bb);
    clear_bit(bb, sq);
    return sq;
}

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
