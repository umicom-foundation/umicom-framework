/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/test_design/main.c
 *
 * PURPOSE:
 *   Tests that remain active in Release.
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

static bool ValidScore(unsigned score) { return score<=100U; }
int main(void)
{
    const unsigned values[]={0U,1U,99U,100U,101U,999U};
    const bool expected[]={true,true,true,true,false,false};
    size_t checked=0U;
    for (size_t index=0U; index<sizeof(values)/sizeof(values[0]); ++index) {
        if (ValidScore(values[index])!=expected[index]) {
            fprintf(stderr,"score %u gave the wrong result\n",values[index]);
            return EXIT_FAILURE;
        }
        ++checked;
    }
    printf("%zu explicit checks ran\n",checked);
    puts("PASS: test_design");
    return EXIT_SUCCESS;
}
