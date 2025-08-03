#pragma once

#include "movegen.h"
#include "state.h"

Move parse_move(State *state, const char *move, int *consumed);

// Assumes command is well-formed
void parse_position(State *state, const char *command);
void parse_go(State *state, const char *command);
void start_uci_loop(State *state);
