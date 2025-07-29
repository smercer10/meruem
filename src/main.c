#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fen.h"
#include "perft.h"
#include "sliders.h"
#include "state.h"

#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9"

int main() {
    init_slider_attack_tables();

    State state = {0};
    parse_fen(tricky_position, &state);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    long nodes = perft(&state, 5);

    timespec_get(&end, TIME_UTC);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;

    printf("Nodes: %ld\n", nodes);
    printf("Elapsed time: %ld ms\n", elapsed_ms);

    return EXIT_SUCCESS;
}
