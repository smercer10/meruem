#include "perft.h"

#include <assert.h>

#include "movegen.h"

long perft(State* restrict state, int depth) {
    assert(state != nullptr);
    assert(depth >= 0);

    if (depth == 0) return 1;

    MoveList move_list;
    gen_pseudo_legal_moves(state, &move_list);

    long nodes = 0;
    for (int i = 0; i < move_list.count; ++i) {
        State backup = *state;
        if (make_move(state, move_list.moves[i], ALL_MOVES)) nodes += perft(state, depth - 1);
        *state = backup;
    }
    return nodes;
}
