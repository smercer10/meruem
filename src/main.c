#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fen.h"
#include "movegen.h"
#include "sliders.h"
#include "state.h"

#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

int main() {
    init_slider_attack_tables();

    State state = {0};
    parse_fen("r3k2r/pP1pqpb1/bn2pnp1/2pPN3/Pp2P3/2N2Q1p/1PPBBPpP/R3K2R b KQkq a3 0 1 ", &state);

    MoveList move_list = {.count = 0};
    generate_moves(&state, &move_list);

    for (int i = 0; i < move_list.count; ++i) {
        State backup = state;
        make_move(&state, move_list.moves[i], ALL_MOVES);
        print_move(move_list.moves[i]);
        puts("\n");
        print_state(&state);
        state = backup;
    }

    return EXIT_SUCCESS;
}
