#pragma once

#include "state.h"

// Assumes FEN is well-formed
void parse_fen(const char* fen, State* state);
