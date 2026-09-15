/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/scores/test_score.c
 *
 * PURPOSE:
 *   Test normal values and boundaries even in release builds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "score.h"
#include <stdio.h>
#include <stdlib.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        return EXIT_FAILURE; \
    } \
} while (0)

int main(void)
{
    CHECK(!LearningScoreIsValid(-1));
    CHECK(LearningScoreIsValid(0));
    CHECK(LearningScoreIsValid(100));
    CHECK(!LearningScoreIsValid(101));
    CHECK(!LearningScoreIsReady(-1));
    CHECK(!LearningScoreIsReady(0));
    CHECK(!LearningScoreIsReady(69));
    CHECK(LearningScoreIsReady(70));
    CHECK(LearningScoreIsReady(100));
    CHECK(!LearningScoreIsReady(101));
    (void)puts("Score checks passed.");
    return EXIT_SUCCESS;
}
