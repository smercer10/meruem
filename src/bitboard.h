#pragma once

#include <stdint.h>

bool is_set(uint64_t bb, int sq);
uint64_t set_bit(uint64_t bb, int sq);
uint64_t clear_bit(uint64_t bb, int sq);
void print_bitboard(uint64_t bb);
