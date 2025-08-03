#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uci.h>

#include "fen.h"
#include "globals.h"
#include "movegen.h"
#include "search.h"
#include "state.h"

Move parse_move(State *state, const char *move, int *consumed) {
    assert(state != nullptr);
    assert(move != nullptr);
    assert(move[0] >= 'a' && move[0] <= 'h');
    assert(move[1] >= '1' && move[1] <= '8');
    assert(move[2] >= 'a' && move[2] <= 'h');
    assert(move[3] >= '1' && move[3] <= '8');
    assert(consumed != nullptr);

    int source_sq = move[0] - 'a' + (move[1] - '1') * 8;
    int target_sq = move[2] - 'a' + (move[3] - '1') * 8;
    *consumed = 4;

    int promoted_piece = INVALID_PIECE;
    if (move[4] != '\0' && move[4] != ' ') {  // Need to check spaces in case there are multiple moves in the string
        ++(*consumed);
        switch (move[4]) {
            case 'q':
                promoted_piece = state->packed.side == WHITE ? WQ : BQ;
                break;
            case 'r':
                promoted_piece = state->packed.side == WHITE ? WR : BR;
                break;
            case 'b':
                promoted_piece = state->packed.side == WHITE ? WB : BB;
                break;
            case 'n':
                promoted_piece = state->packed.side == WHITE ? WN : BN;
                break;
            default:
                return (Move){.is_invalid = 1};
        }
    }

    MoveList move_list;
    generate_moves(state, &move_list);

    for (int i = 0; i < move_list.count; ++i) {
        Move m = move_list.moves[i];
        if (m.source_sq == source_sq && m.target_sq == target_sq && m.promoted_piece == promoted_piece) return m;
    }

    return (Move){.is_invalid = 1};
}

void parse_position(State *state, const char *command) {
    assert(state != nullptr);
    assert(command != nullptr);

    command += 9;  // Skip "position " command
    if (strncmp(command, "startpos", 8) == 0) {
        parse_fen(START_POSITION, state);
    } else {
        command += 4;  // Skip "fen " command
        parse_fen(command, state);
    }

    const char *moves = strstr(command, "moves");
    if (moves != nullptr) {
        moves += 6;  // Skip "moves " command
        while (*moves != '\0') {
            while (*moves == ' ') ++moves;  // Skip spaces
            if (*moves == '\0') break;      // Accounts for trailing spaces
            int consumed = 0;
            Move move = parse_move(state, moves, &consumed);
            if (move.is_invalid) break;
            make_move(state, move, ALL_MOVES);
            moves += consumed;
        }
    }
}

void parse_go(State *state, const char *command) {
    assert(state != nullptr);
    assert(command != nullptr);

    int depth = 0;

    command += 3;  // Skip "go "
    if (strncmp(command, "depth ", 6) == 0) {
        command += 6;  // Skip "depth "
        depth = atoi(command);
    }

    search_position(state, depth);
}

void start_uci_loop(State *state) {
    assert(state != nullptr);

    setbuf(stdout, NULL);  // Ensure stdout is unbuffered for immediate output

    char command[256];

    while (fgets(command, sizeof(command), stdin)) {
        command[strcspn(command, "\n")] = '\0';  // Remove trailing newline

        if (strcmp(command, "ucinewgame") == 0) {
            parse_position(state, "position startpos");
        } else if (strcmp(command, "uci") == 0) {
            puts("id name Meruem");
            puts("id author smercer10");
            puts("uciok");
        } else if (strcmp(command, "isready") == 0) {
            puts("readyok");
        } else if (strncmp(command, "position ", 9) == 0) {
            parse_position(state, command);
        } else if (strncmp(command, "go ", 3) == 0) {
            parse_go(state, command);
        } else if (strcmp(command, "quit") == 0) {
            break;
        }
    }
}
