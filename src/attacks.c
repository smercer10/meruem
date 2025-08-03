#include "attacks.h"

#include <assert.h>
#include <stdint.h>

#include "globals.h"
#include "leapers.h"
#include "sliders.h"
#include "state.h"

bool is_sq_attacked(const State* restrict state, int sq, int attacking_side) {
    assert(state != nullptr);
    assert(sq >= A1 && sq <= H8);
    assert(attacking_side == WHITE || attacking_side == BLACK);

    const int idx_offset = (attacking_side == WHITE ? 0 : 6);

    if (knight_attacks[sq] & state->pieces[WN + idx_offset]) return true;
    if (king_attacks[sq] & state->pieces[WK + idx_offset]) return true;
    if (pawn_attacks[!attacking_side][sq] & state->pieces[WP + idx_offset]) return true;

    if (get_bishop_attacks(sq, state->occupancy[ALL]) &
        (state->pieces[WB + idx_offset] | state->pieces[WQ + idx_offset]))
        return true;

    if (get_rook_attacks(sq, state->occupancy[ALL]) & (state->pieces[WR + idx_offset] | state->pieces[WQ + idx_offset]))
        return true;

    return false;
}
