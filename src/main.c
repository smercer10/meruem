#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
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

    Move move = encode_move(E2, E4, WP, WQ, false, true, false, false);
    printf("Source Square: %s\n", squares[get_source_sq(move)]);
    printf("Target Square: %s\n", squares[get_target_sq(move)]);
    printf("Moved Piece: %c\n", ascii_pieces[get_moved_piece(move)]);
    printf("Promoted Piece: %c\n", ascii_pieces[get_promoted_piece(move)]);
    printf("Is Capture: %s\n", is_capture(move) ? "Yes" : "No");
    printf("Is Double Push: %s\n", is_double_push(move) ? "Yes" : "No");
    printf("Is En Passant: %s\n", is_en_passant(move) ? "Yes" : "No");
    printf("Is Castling: %s\n", is_castling(move) ? "Yes" : "No");

    return EXIT_SUCCESS;
}
