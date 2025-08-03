#pragma once

#include <stdint.h>

bool is_bit_set(uint64_t bb, int sq);
void set_bit(uint64_t* restrict bb, int sq);
void clear_bit(uint64_t* restrict bb, int sq);
int get_lsb(uint64_t bb);
int pop_lsb(uint64_t* restrict bb);
void print_bitboard(uint64_t bb);
