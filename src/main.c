#include <stdint.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

int main() {
    uint64_t b = 0x000000000000FFFF;

    b = set_bit(b, A8);
    b = set_bit(b, H8);
    b = set_bit(b, D4);
    b = clear_bit(b, A1);
    b = clear_bit(b, G1);

    print_bitboard(b);

    return EXIT_SUCCESS;
}
