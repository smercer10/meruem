#include "attacks.h"

#include <assert.h>
#include <stdint.h>

#include "globals.h"
#include "leapers.h"
#include "sliders.h"
#include "state.h"

bool is_attacked(int sq, int attacking_side, const State* state) {
    assert(sq >= 0 && sq < 64);
    assert(attacking_side == WHITE || attacking_side == BLACK);
    assert(state != nullptr);

    if ((attacking_side == WHITE) && (pawn_attacks[BLACK][sq] & state->pieces[WP])) return true;
    if ((attacking_side == BLACK) && (pawn_attacks[WHITE][sq] & state->pieces[BP])) return true;

    const int idx_offset = (attacking_side == WHITE ? 0 : 6);

    if (knight_attacks[sq] & state->pieces[WN + idx_offset]) return true;
    if (king_attacks[sq] & state->pieces[WK + idx_offset]) return true;

    const uint64_t diagonal_attacks = get_bishop_attacks(sq, state->occupancy[ALL]);
    if (diagonal_attacks & (state->pieces[WB + idx_offset] | state->pieces[WQ + idx_offset])) return true;

    const uint64_t orthogonal_attacks = get_rook_attacks(sq, state->occupancy[ALL]);
    if (orthogonal_attacks & (state->pieces[WR + idx_offset] | state->pieces[WQ + idx_offset])) return true;

    return false;
}
