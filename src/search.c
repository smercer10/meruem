#include "search.h"

#include <assert.h>
#include <stdio.h>

#include "state.h"

void search_position(State *state, int depth) {
    assert(state != nullptr);
    assert(depth >= 0);

    (void)depth;  // Silence unused variable warning for now

    puts("bestmove d2d4");  // TODO: Replace with actual search logic
}
