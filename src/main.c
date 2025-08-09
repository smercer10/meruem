#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "attacks.h"
#include "fen.h"
#include "search.h"
#include "state.h"

static void init(void) { init_slider_attack_tables(); }

int main(void) {
    init();
    State state;
    // start_uci_loop(&state);

    parse_fen(&state, KILLER_POSITION);
    search_position(&state, 4);
    printf("Nodes searched: %d\n", nodes_searched);

    return EXIT_SUCCESS;
}
