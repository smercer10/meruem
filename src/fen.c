#include "fen.h"

#include <assert.h>
#include <ctype.h>
#include <stdint.h>

#include "bitboard.h"
#include "globals.h"
#include "state.h"

static int char_to_piece(char c) {
    switch (c) {
        case 'P':
            return WP;
        case 'N':
            return WN;
        case 'B':
            return WB;
        case 'R':
            return WR;
        case 'Q':
            return WQ;
        case 'K':
            return WK;
        case 'p':
            return BP;
        case 'n':
            return BN;
        case 'b':
            return BB;
        case 'r':
            return BR;
        case 'q':
            return BQ;
        case 'k':
            return BK;
        default:
            return INVALID_PIECE;
    }
}

static int parse_sq(const char* restrict str) {
    assert(str != nullptr);
    if (str[0] < 'a' || str[0] > 'h' || str[1] < '1' || str[1] > '8') return INVALID_SQ;
    return ((str[1] - '1') * 8) + (str[0] - 'a');
}

void parse_fen(State* restrict state, const char* restrict fen) {
    assert(state != nullptr && fen != nullptr);

    // Reset bitboards
    for (int i = 0; i < 12; ++i) {
        state->pieces[i] = 0;
    }
    for (int i = 0; i < 3; ++i) {
        state->occupancy[i] = 0;
    }

    // Parse piece placement
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            const char c = *fen++;
            if (isdigit(c)) {
                f += c - '0' - 1;  // -1 because f will be incremented in the loop
            } else {
                const int piece = char_to_piece(c);
                if (piece != INVALID_PIECE) {
                    const int sq = (r * 8) + f;
                    set_bit(&state->pieces[piece], sq);
                    set_bit(&state->occupancy[ALL], sq);
                    set_bit(&state->occupancy[piece < BP ? WHITE : BLACK], sq);
                }
            }
        }
        if (r > 0) ++fen;  // Skip the rank separators (doesn't apply to the last rank)
    }

    while (*fen == ' ') ++fen;  // Skip space

    // Parse side to move
    state->packed.side = (*fen++ == 'w') ? WHITE : BLACK;

    while (*fen == ' ') ++fen;  // Skip space

    // Parse castling rights
    state->packed.castling = 0;
    if (*fen == '-') {
        ++fen;
    } else {
        while (*fen != ' ') {
            switch (*fen) {
                case 'K':
                    state->packed.castling |= WKS;
                    break;
                case 'Q':
                    state->packed.castling |= WQS;
                    break;
                case 'k':
                    state->packed.castling |= BKS;
                    break;
                case 'q':
                    state->packed.castling |= BQS;
                    break;
            }
            ++fen;
        }
    }

    while (*fen == ' ') ++fen;  // Skip space

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    // Parse en passant square
    if (*fen == '-') {
        state->packed.en_passant = INVALID_SQ;
        ++fen;
    } else {
        state->packed.en_passant = parse_sq(fen);
        fen += 2;
    }

    while (*fen == ' ') ++fen;  // Skip space

    // Parse halfmove clock
    state->packed.halfmove = 0;
    while (isdigit(*fen) && state->packed.halfmove < 255) {  // Make sure it fits in 8 bits
        state->packed.halfmove = (state->packed.halfmove * 10 + (*fen++ - '0'));
    }

    while (*fen == ' ') ++fen;  // Skip space

    // Parse fullmove number
    state->packed.fullmove = 0;
    while (isdigit(*fen) && state->packed.fullmove < 4095) {  // Make sure it fits in 12 bits
        state->packed.fullmove = (state->packed.fullmove * 10 + (*fen++ - '0'));
    }
    if (state->packed.fullmove < 1) state->packed.fullmove = 1;  // Ensure fullmove is at least 1
#pragma GCC diagnostic pop
}
