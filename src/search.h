#pragma once

#include "state.h"

int eval_state(const State* restrict state);
void search_position(State* restrict state, int depth, int* nodes_searched);
