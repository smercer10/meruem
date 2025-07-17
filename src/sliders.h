#pragma once

#include <assert.h>
#include <stdbit.h>
#include <stdint.h>

void init_slider_attack_tables(void);
uint64_t get_rook_attacks(int sq, uint64_t occ);
uint64_t get_bishop_attacks(int sq, uint64_t occ);
uint64_t get_queen_attacks(int sq, uint64_t occ);
