#include <stdint.h>
#include <stdlib.h>

#include "bitboard.h"
#include "globals.h"
#include "state.h"

int main() {
    State state = {0};

    state.pieces[0] = 0x000000000000FF00;
    state.pieces[6] = 0x00FF000000000000;
    set_bit(&state.pieces[WN], B1);
    clear_bit(&state.pieces[0], A2);

    state.packed.side = WHITE;
    state.packed.en_passant = NA;
    state.packed.castling = WKS | WQS | BQS;
    state.packed.halfmove = 0;
    state.packed.fullmove = 1;
    state.zobrist_hash = 0x123456789ABCDEF0;

    print_state(&state);

    return EXIT_SUCCESS;
}
