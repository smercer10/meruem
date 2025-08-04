#pragma once

#include <stdint.h>

static constexpr int NUM_PIECES = 12;
static constexpr int NUM_OCCUPANCIES = 3;

typedef struct {
    uint64_t pieces[NUM_PIECES];
    uint64_t occupancy[NUM_OCCUPANCIES];
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

static constexpr char ascii_pieces[NUM_PIECES] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};

void print_state(const State* state);
