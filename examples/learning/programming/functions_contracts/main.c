/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/functions_contracts/main.c
 *
 * PURPOSE:
 *   Functions with explicit contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Scores must be 0..100; on rejection, *outMean is unchanged. */
static bool MeanScore(const unsigned *scores, size_t count, double *outMean)
{
    if (scores==NULL || outMean==NULL || count==0U || count>1000U) return false;
    uint32_t total=0U;
    for (size_t index=0U; index<count; ++index) {
        if (scores[index]>100U) return false;
        total+=scores[index]; /* At most 100,000, within uint32_t range. */
    }
    *outMean=(double)total/(double)count; return true;
}
int main(void)
{
    const unsigned scores[]={70U,80U,90U}, invalid[]={50U,101U};
    double mean=-1.0;
    if (!MeanScore(scores,3U,&mean) || mean!=80.0) return EXIT_FAILURE;
    if (MeanScore(invalid,2U,&mean) || mean!=80.0 || MeanScore(scores,0U,&mean))
        return EXIT_FAILURE;
    puts("mean=80.0; rejected inputs preserved the output");
    puts("PASS: functions_contracts");
    return EXIT_SUCCESS;
}
