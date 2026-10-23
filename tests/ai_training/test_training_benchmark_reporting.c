/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_benchmark_reporting.c
 * PURPOSE: Verify bounded, non-executing metrics are honest about mismatches
 *          and do not claim correctness from a partial byte coincidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_benchmark_reporting.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/benchmark.h"
#include <stdio.h>
#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "check failed at line %d: %s\n", __LINE__, #test); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingBenchmarkCaseResult report;
    const uint8_t expected[] = "int64_t balanceMinor;";
    uint8_t mutated[sizeof(expected) - 1U];
    for (size_t i = 0U; i < sizeof(mutated); ++i)
        mutated[i] = expected[i];
    mutated[8U] = (uint8_t)'X';
    CHECK(UmiAiTrainingBenchmarkScoreBytes(expected, sizeof(mutated),
          mutated, sizeof(mutated), &report) == UMI_STATUS_OK);
    CHECK(report.exactMatch == 0);
    CHECK(report.matchingPositionBytes == sizeof(mutated) - 1U);
    CHECK(report.matchingPrefixBytes == 8U);
    /* The benchmark records textual agreement only; it does not attempt to
     * interpret, compile or execute the generated C23 fragment. */
    return 0;
}
