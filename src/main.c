#include <stdint.h>
#include <stdlib.h>

#include "bitboard.h"
#include "globals.h"
#include "sliders.h"

int main() {
    init_slider_attack_tables();

    uint64_t occ = 0;
    occ = set_bit(occ, E4);
    occ = set_bit(occ, F7);
    occ = set_bit(occ, D4);
    occ = set_bit(occ, G5);
    occ = set_bit(occ, A8);

    print_bitboard(get_queen_attacks(D5, occ));

    return EXIT_SUCCESS;
}
