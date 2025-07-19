#include <stdint.h>
#include <stdlib.h>

#include "globals.h"
#include "movegen.h"
#include "sliders.h"

#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

int main() {
    init_slider_attack_tables();

    Move move1 = encode_move(E2, E4, WP, WB, false, true, false, false);
    Move move2 = encode_move(B1, C3, WN, -1, false, false, false, false);
    MoveList move_list = {.count = 0};
    add_move(&move_list, move1);
    add_move(&move_list, move2);
    print_move_list(&move_list);

    return EXIT_SUCCESS;
}
