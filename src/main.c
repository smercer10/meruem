#include <stdint.h>
#include <stdlib.h>

#include "movegen.h"
#include "utils.h"

int main() {
    for (int sq = 0; sq < 64; ++sq) {
        print_bitboard(king_attacks[sq]);
    }

    return EXIT_SUCCESS;
}
