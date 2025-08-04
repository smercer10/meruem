#pragma once

#include "movegen.h"
#include "state.h"

Move parse_move(const State *restrict state, const char *restrict move, int *restrict consumed);

// Assumes command is well-formed
void parse_position(State *restrict state, const char *restrict command);
void parse_go(State *restrict state, const char *restrict command);
void start_uci_loop(State *restrict state);
