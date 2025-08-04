#include "search.h"

#include <assert.h>
#include <stdio.h>

#include "state.h"

void search_position(const State* restrict state, int depth) {
    assert(state != nullptr);
    assert(depth >= 0);

    // Silence unused variable warnings for now
    (void)state;
    (void)depth;

    puts("bestmove d2d4");  // TODO: Replace with actual search logic
}
