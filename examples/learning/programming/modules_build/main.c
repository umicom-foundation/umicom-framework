/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/modules_build/main.c
 *
 * PURPOSE:
 *   Headers, linking and separate compilation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "score.h"

int main(void)
{
    unsigned result=999U;
    if (!NormaliseScore(80U,100U,&result) || result!=80U) return EXIT_FAILURE;
    if (NormaliseScore(2U,0U,&result) || result!=80U) return EXIT_FAILURE;
    if (!NormaliseScore(1U,4U,&result) || result!=25U) return EXIT_FAILURE;
    puts("separate header/source linked; 1 of 4 = 25 percent");
    puts("PASS: modules_build");
    return EXIT_SUCCESS;
}
