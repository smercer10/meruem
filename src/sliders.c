#include "sliders.h"

#include <assert.h>
#include <stdint.h>

uint64_t gen_rook_attacks(int start_sq, uint64_t occ) {
    assert(start_sq >= 0 && start_sq < 64);

    uint64_t attacks = 0;

    // North
    for (int sq = start_sq + 8; sq < 64; sq += 8) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }
    // South
    for (int sq = start_sq - 8; sq >= 0; sq -= 8) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }
    // East
    for (int sq = start_sq + 1; (sq < 64) && ((sq & 7) != 0); ++sq) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }
    // West
    for (int sq = start_sq - 1; (sq >= 0) && ((sq & 7) != 7); --sq) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }

    return attacks;
}

uint64_t gen_bishop_attacks(int start_sq, uint64_t occ) {
    assert(start_sq >= 0 && start_sq < 64);

    uint64_t attacks = 0;

    // Northeast
    for (int sq = start_sq + 9; (sq < 64) && ((sq & 7) != 0); sq += 9) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }
    // Northwest
    for (int sq = start_sq + 7; (sq < 64) && ((sq & 7) != 7); sq += 7) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }
    // Southeast
    for (int sq = start_sq - 7; (sq >= 0) && ((sq & 7) != 0); sq -= 7) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }
    // Southwest
    for (int sq = start_sq - 9; (sq >= 0) && ((sq & 7) != 7); sq -= 9) {
        attacks |= (UINT64_C(1) << sq);
        if (occ & (UINT64_C(1) << sq)) break;
    }

    return attacks;
}

uint64_t idx_to_occ(int idx, int bits, uint64_t mask) {
    assert(bits >= 0 && bits <= 64);
    assert(idx >= 0 && idx < (1 << bits));
    assert(stdc_count_ones(mask) == (uint64_t)bits);

    uint64_t occ = 0;
    for (int i = 0; i < bits; ++i) {
        const unsigned int j = stdc_trailing_zeros(mask);
        mask &= mask - 1;
        if (idx & (1 << i)) occ |= (UINT64_C(1) << j);
    }
    return occ;
}

uint64_t rook_attacks[64][4096];
uint64_t bishop_attacks[64][512];

#define INIT_SLIDER_TABLE(piece)                                     \
    for (int sq = 0; sq < 64; ++sq) {                                \
        const int shift = piece##_shifts[sq];                        \
        const int bits = 64 - shift;                                 \
        const uint64_t mask = piece##_masks[sq];                     \
        const uint64_t magic = piece##_magics[sq];                   \
        for (int idx = 0; idx < (1 << bits); ++idx) {                \
            const uint64_t occ = idx_to_occ(idx, bits, mask);        \
            const uint64_t attacks = gen_##piece##_attacks(sq, occ); \
            const size_t magic_idx = (occ * magic) >> shift;         \
            piece##_attacks[sq][magic_idx] = attacks;                \
        }                                                            \
    }

void init_slider_attack_tables(void) {
    INIT_SLIDER_TABLE(rook);
    INIT_SLIDER_TABLE(bishop);
}

uint64_t get_rook_attacks(int sq, uint64_t occ) {
    assert(sq >= 0 && sq < 64);
    occ &= rook_masks[sq];
    occ *= rook_magics[sq];
    occ >>= rook_shifts[sq];
    return rook_attacks[sq][occ];
}

uint64_t get_bishop_attacks(int sq, uint64_t occ) {
    assert(sq >= 0 && sq < 64);
    occ &= bishop_masks[sq];
    occ *= bishop_magics[sq];
    occ >>= bishop_shifts[sq];
    return bishop_attacks[sq][occ];
}

uint64_t get_queen_attacks(int sq, uint64_t occ) {
    return get_rook_attacks(sq, occ) | get_bishop_attacks(sq, occ);
}
