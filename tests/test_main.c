#include <time.h>

#include "attacks.h"
#include "unity.h"
#include "unity_internals.h"

void test_perft_startpos(void);
void test_perft_kiwipete(void);
void test_perft_pos3(void);
void test_perft_pos4(void);
void test_perft_pos5(void);
void test_perft_pos6(void);

void setUp(void) { init_slider_attack_tables(); }
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    RUN_TEST(test_perft_startpos);
    RUN_TEST(test_perft_kiwipete);
    RUN_TEST(test_perft_pos3);
    RUN_TEST(test_perft_pos4);
    RUN_TEST(test_perft_pos5);
    RUN_TEST(test_perft_pos6);

    timespec_get(&end, TIME_UTC);
    double elapsed_s = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\nTests completed in %.3f s\n", elapsed_s);

    return UNITY_END();
}
