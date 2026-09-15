/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/scores/main.c
 *
 * PURPOSE:
 *   Use an array, a loop and a named function to explain decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "score.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const int scores[] = {45, 69, 70, 100};
    const size_t scoreCount = sizeof(scores) / sizeof(scores[0]);
    for (size_t index = 0U; index < scoreCount; ++index) {
        const char *description = LearningScoreIsReady(scores[index])
            ? "ready" : "keep practising";
        if (printf("%d: %s\n", scores[index], description) < 0) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
