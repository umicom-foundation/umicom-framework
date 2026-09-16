/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/coding_conventions/main.c
 *
 * PURPOSE:
 *   Readable names and coding conventions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum { MAX_SCORE=100U, PASS_SCORE=75U };
/* outPassed is unchanged on invalid input. No pointers are retained. */
static bool EvaluateScore(unsigned score, bool *outPassed)
{
    if (outPassed==NULL || score>MAX_SCORE) return false;
    *outPassed=score>=PASS_SCORE;
    return true;
}
int main(void)
{
    bool passed=false;
    if (!EvaluateScore(75U,&passed) || !passed) return EXIT_FAILURE;
    if (EvaluateScore(101U,&passed) || !passed) return EXIT_FAILURE;
    if (!EvaluateScore(74U,&passed) || passed) return EXIT_FAILURE;
    puts("named threshold=75; invalid input preserved caller state");
    puts("PASS: coding_conventions");
    return EXIT_SUCCESS;
}
