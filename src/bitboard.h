#pragma once

#include <assert.h>
#include <stdbit.h>
#include <stdint.h>

#include "globals.h"

static inline bool is_bit_set(uint64_t bb, int sq) {
    assert(sq >= A1 && sq <= H8);
    return bb & (UINT64_C(1) << sq);
}

static inline void set_bit(uint64_t* restrict bb, int sq) {
    assert(bb != nullptr && sq >= A1 && sq <= H8);
    *bb |= (UINT64_C(1) << sq);
}

static inline void clear_bit(uint64_t* restrict bb, int sq) {
    assert(bb != nullptr && sq >= A1 && sq <= H8);
    *bb &= ~(UINT64_C(1) << sq);
}

static inline int get_lsb(uint64_t bb) {
    assert(bb != 0);
    return (int)stdc_first_trailing_one(bb) - 1;  // -1 to convert to 0-indexed square
}

static inline int pop_lsb(uint64_t* restrict bb) {
    assert(bb != nullptr);
    const int sq = get_lsb(*bb);
    clear_bit(bb, sq);
    return sq;
}

void print_bitboard(uint64_t bb);
