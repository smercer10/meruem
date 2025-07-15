#pragma once

#include <stdint.h>

typedef struct {
    uint64_t pieces[6][2];  // [PIECE_TYPE][COLOR]
    uint64_t occupancy[3];  // [WHITE, BLACK, ALL]
    uint64_t zobrist_hash;
    uint32_t packed_state;  // Side (1), en passant (7), castling (4), halfmove (8), fullmove (12)
} State;

#define GET_SIDE(s) ((s)->packed_state & 1)
#define GET_EN_PASSANT(s) (((s)->packed_state >> 1) & 0x7F)
#define GET_CASTLING(s) (((s)->packed_state >> 8) & 0xF)
#define GET_HALFMOVE(s) (((s)->packed_state >> 12) & 0xFF)
#define GET_FULLMOVE(s) (((s)->packed_state >> 20) & 0xFFF)

#define SET_SIDE(s, val) ((s)->packed_state = ((s)->packed_state & ~1) | (val))
#define SET_EN_PASSANT(s, val) ((s)->packed_state = ((s)->packed_state & ~(0x7F << 1)) | ((val) << 1))
#define SET_CASTLING(s, val) ((s)->packed_state = ((s)->packed_state & ~(0xF << 8)) | ((val) << 8))
#define SET_HALFMOVE(s, val) ((s)->packed_state = ((s)->packed_state & ~(0xFF << 12)) | ((val) << 12))
#define SET_FULLMOVE(s, val) ((s)->packed_state = ((s)->packed_state & ~(0xFFF << 20)) | ((val) << 20))
