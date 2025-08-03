#include <stdint.h>
#include <stdlib.h>

#include "sliders.h"
#include "state.h"
#include "uci.h"

static void init(void) { init_slider_attack_tables(); }

int main(void) {
    init();

    State state;

    start_uci_loop(&state);

    return EXIT_SUCCESS;
}
