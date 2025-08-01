#include <stdint.h>
#include <stdlib.h>

#include "sliders.h"
#include "state.h"
#include "uci.h"

static void init(void) { init_slider_attack_tables(); }

int main(void) {
    init();

    State state;

    parse_position(&state,
                   "position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves a2a4 a6e2");
    print_state(&state);

    return EXIT_SUCCESS;
}
