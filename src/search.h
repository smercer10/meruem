#pragma once

#include "state.h"

static constexpr int MAX_DEPTH = 64;

extern int nodes_searched;  // Used for performance testing

int eval_state(const State* restrict state);
void search_position(State* restrict state, int depth);
