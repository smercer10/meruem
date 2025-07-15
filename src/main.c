#include <stdint.h>
#include <stdlib.h>

#include "constants.h"
#include "movegen.h"
#include "utils.h"

int main() {
    for (int sq = 0; sq < 64; ++sq) {
        print_bitboard(pawn_attacks[BLACK][sq]);
    }

    return EXIT_SUCCESS;
}
