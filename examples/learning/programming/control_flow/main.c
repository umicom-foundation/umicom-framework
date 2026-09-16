/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/control_flow/main.c
 *
 * PURPOSE:
 *   Decisions, loops and early returns.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *ClassifyScore(unsigned score)
{
    if (score>100U) return "invalid";
    if (score>=75U) return "ready";
    return "practise";
}
int main(void)
{
    const unsigned scores[]={0U,74U,75U,100U,101U};
    const char *const expected[]={"practise","practise","ready","ready","invalid"};
    size_t ready=0U, invalid=0U;
    for (size_t index=0U; index<sizeof(scores)/sizeof(scores[0]); ++index) {
        const char *result=ClassifyScore(scores[index]);
        if (strcmp(result,expected[index])!=0) return EXIT_FAILURE;
        printf("%u -> %s\n",scores[index],result);
        if (scores[index]>100U) { ++invalid; continue; }
        if (scores[index]>=75U) ++ready;
    }
    if (ready!=2U || invalid!=1U) return EXIT_FAILURE;
    puts("PASS: control_flow");
    return EXIT_SUCCESS;
}
