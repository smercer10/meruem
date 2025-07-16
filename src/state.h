#pragma once

#include <stdint.h>

typedef struct {
    uint64_t pieces[12];
    uint64_t occupancy[3];
    uint64_t zobrist_hash;
    union {
        uint32_t raw;
        struct {
            uint32_t side : 1;
            uint32_t en_passant : 7;
            uint32_t castling : 4;
            uint32_t halfmove : 8;
            uint32_t fullmove : 12;
        };
    } packed;
} State;

void print_state(const State* state);
