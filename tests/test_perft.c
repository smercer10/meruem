// Positions and expected perft results are taken from https://www.chessprogramming.org/Perft_Results

#include <unity.h>

#include "perft.h"
#include "state.h"
#include "uci.h"

void test_perft_startpos(void) {
    State state;
    parse_position(&state, "position startpos");
    TEST_ASSERT_EQUAL_INT64(1, perft(&state, 0));
    TEST_ASSERT_EQUAL_INT64(20, perft(&state, 1));
    TEST_ASSERT_EQUAL_INT64(400, perft(&state, 2));
    TEST_ASSERT_EQUAL_INT64(8902, perft(&state, 3));
    TEST_ASSERT_EQUAL_INT64(197281, perft(&state, 4));
    TEST_ASSERT_EQUAL_INT64(4865609, perft(&state, 5));
    TEST_ASSERT_EQUAL_INT64(119060324, perft(&state, 6));
}

void test_perft_kiwipete(void) {
    State state;
    // Purposely exlcude halfmove clock and fullmove number to test FEN parser robustness
    parse_position(&state, "position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    TEST_ASSERT_EQUAL_INT64(48, perft(&state, 1));
    TEST_ASSERT_EQUAL_INT64(2039, perft(&state, 2));
    TEST_ASSERT_EQUAL_INT64(97862, perft(&state, 3));
    TEST_ASSERT_EQUAL_INT64(4085603, perft(&state, 4));
    TEST_ASSERT_EQUAL_INT64(193690690, perft(&state, 5));
}

void test_perft_pos3(void) {
    State state;
    // Enter position 3 from a couple moves for extra coverage of UCI parsing
    parse_position(&state, "position fen 7r/2p5/3p4/KP6/5p1k/8/4P1P1/1R6 w - - 0 1 moves b1b4 h8h5");
    TEST_ASSERT_EQUAL_INT64(14, perft(&state, 1));
    TEST_ASSERT_EQUAL_INT64(191, perft(&state, 2));
    TEST_ASSERT_EQUAL_INT64(2812, perft(&state, 3));
    TEST_ASSERT_EQUAL_INT64(43238, perft(&state, 4));
    TEST_ASSERT_EQUAL_INT64(674624, perft(&state, 5));
    TEST_ASSERT_EQUAL_INT64(11030083, perft(&state, 6));
    TEST_ASSERT_EQUAL_INT64(178633661, perft(&state, 7));
}

void test_perft_pos4(void) {
    State state;
    // Original
    parse_position(&state, "position fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    TEST_ASSERT_EQUAL_INT64(6, perft(&state, 1));
    TEST_ASSERT_EQUAL_INT64(264, perft(&state, 2));
    TEST_ASSERT_EQUAL_INT64(9467, perft(&state, 3));
    TEST_ASSERT_EQUAL_INT64(422333, perft(&state, 4));
    TEST_ASSERT_EQUAL_INT64(15833292, perft(&state, 5));
    TEST_ASSERT_EQUAL_INT64(706045033, perft(&state, 6));
    // Mirrored
    parse_position(&state, "position fen r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    TEST_ASSERT_EQUAL_INT64(6, perft(&state, 1));
    TEST_ASSERT_EQUAL_INT64(264, perft(&state, 2));
    TEST_ASSERT_EQUAL_INT64(9467, perft(&state, 3));
    TEST_ASSERT_EQUAL_INT64(422333, perft(&state, 4));
    TEST_ASSERT_EQUAL_INT64(15833292, perft(&state, 5));
    TEST_ASSERT_EQUAL_INT64(706045033, perft(&state, 6));
}

void test_perft_pos5(void) {
    State state;
    parse_position(&state, "position fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    TEST_ASSERT_EQUAL_INT64(44, perft(&state, 1));
    TEST_ASSERT_EQUAL_INT64(1486, perft(&state, 2));
    TEST_ASSERT_EQUAL_INT64(62379, perft(&state, 3));
    TEST_ASSERT_EQUAL_INT64(2103487, perft(&state, 4));
    TEST_ASSERT_EQUAL_INT64(89941194, perft(&state, 5));
}

void test_perft_pos6(void) {
    State state;
    parse_position(&state, "position fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    TEST_ASSERT_EQUAL_INT64(1, perft(&state, 0));
    TEST_ASSERT_EQUAL_INT64(46, perft(&state, 1));
    TEST_ASSERT_EQUAL_INT64(2079, perft(&state, 2));
    TEST_ASSERT_EQUAL_INT64(89890, perft(&state, 3));
    TEST_ASSERT_EQUAL_INT64(3894594, perft(&state, 4));
    TEST_ASSERT_EQUAL_INT64(164075551, perft(&state, 5));
}
