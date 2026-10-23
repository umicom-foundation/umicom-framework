/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_benchmark_score.c
 * PURPOSE: Distinguish exact, positional and prefix agreement, including
 *          binary data and length differences without modifying inputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_benchmark_score.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/benchmark.h"
#include <stdio.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "check failed at line %d: %s\n", __LINE__, #test); return 1; \
} } while (0)

int main(void)
{
    const uint8_t expected[] = {'a', 0, 'c', 'd'};
    const uint8_t candidate[] = {'a', 0, 'x', 'd'};
    UmiAiTrainingBenchmarkCaseResult result = {0};
    UmiAiTrainingBenchmarkCaseResult sentinel;
    CHECK(UmiAiTrainingBenchmarkScoreBytes(expected, 4U, expected, 4U,
                                          &result) == UMI_STATUS_OK);
    CHECK(result.exactMatch == 1 && result.matchingPrefixBytes == 4U &&
          result.matchingPositionBytes == 4U);
    CHECK(UmiAiTrainingBenchmarkScoreBytes(expected, 4U, candidate, 4U,
                                          &result) == UMI_STATUS_OK);
    CHECK(result.exactMatch == 0 && result.matchingPrefixBytes == 2U &&
          result.matchingPositionBytes == 3U);
    CHECK(UmiAiTrainingBenchmarkScoreBytes(expected, 4U, expected, 2U,
                                          &result) == UMI_STATUS_OK);
    CHECK(result.exactMatch == 0 && result.matchingPrefixBytes == 2U &&
          result.generatedBytes == 2U && result.expectedBytes == 4U);
    CHECK(UmiAiTrainingBenchmarkScoreBytes(NULL, 0U, NULL, 0U,
                                          &result) == UMI_STATUS_OK);
    CHECK(result.exactMatch == 1 && result.matchingPrefixBytes == 0U);
    sentinel = result;
    CHECK(UmiAiTrainingBenchmarkScoreBytes(NULL, 2U, expected, 2U,
                                          &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&result, &sentinel, sizeof(result)) == 0);
    CHECK(UmiAiTrainingBenchmarkScoreBytes(expected, 4U, NULL, 1U,
                                          &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&result, &sentinel, sizeof(result)) == 0);
    CHECK(UmiAiTrainingBenchmarkScoreBytes(expected, 1U, expected, 1U,
                                          NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
