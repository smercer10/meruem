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
    parse_fen(&state, "rnbqkbnr/ppp1pppp/8/3p4/3P4/2N5/PPP1PPPP/R1BQKBNR w KQkq - 0 1");
    print_state(&state);

    printf("Score: %d\n", eval_state(&state));

    // start_uci_loop(&state);

    return EXIT_SUCCESS;
}
