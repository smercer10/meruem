#include <assert.h>
#include <uci.h>

#include "globals.h"
#include "movegen.h"
#include "state.h"

Move parse_move(State *state, const char *move) {
    assert(state != nullptr);
    assert(move != nullptr);
    assert(move[0] >= 'a' && move[0] <= 'h');
    assert(move[1] >= '1' && move[1] <= '8');
    assert(move[2] >= 'a' && move[2] <= 'h');
    assert(move[3] >= '1' && move[3] <= '8');

    int source_sq = move[0] - 'a' + (move[1] - '1') * 8;
    int target_sq = move[2] - 'a' + (move[3] - '1') * 8;
    int promoted_piece = INVALID_PIECE;

    if (move[4] != '\0') {
        switch (move[4]) {
            case 'q':
                promoted_piece = state->packed.side == WHITE ? WQ : BQ;
                break;
            case 'r':
                promoted_piece = state->packed.side == WHITE ? WR : BR;
                break;
            case 'b':
                promoted_piece = state->packed.side == WHITE ? WB : BB;
                break;
            case 'n':
                promoted_piece = state->packed.side == WHITE ? WN : BN;
                break;
            default:
                return (Move){.is_invalid = 1};
        }
    }

    MoveList move_list;
    generate_moves(state, &move_list);

    for (int i = 0; i < move_list.count; ++i) {
        Move m = move_list.moves[i];
        if (m.source_sq == source_sq && m.target_sq == target_sq && m.promoted_piece == promoted_piece) return m;
    }

    return (Move){.is_invalid = 1};
}
