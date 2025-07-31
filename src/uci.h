#pragma once

#include "movegen.h"
#include "state.h"

Move parse_move(State *state, const char *move, int *consumed);
void parse_position(State *state, const char *command);  // Assumes command is well-formed
