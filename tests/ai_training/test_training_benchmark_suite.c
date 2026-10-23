/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_benchmark_suite.c
 * PURPOSE: Verify bounded text-case parsing, binary bytes and borrowed
 *          pointer lifetime without executing any source-code completion.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_benchmark_suite.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/benchmark_suite.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)

int main(void)
{
    const char content[] =
        "UMICOM-AI-BENCHMARK-1\r\n"
        "# Approved fictional source fragments.\r\n"
        "case account 410042 430044\r\n"
        "case empty - 00\r\n";
    UmiAiTrainingBenchmarkSuite *suite = NULL;
    UmiAiTrainingBenchmarkCase one = {0}, two = {0};
    size_t count = 100U;
    CHECK(UmiAiTrainingBenchmarkSuiteParse(content, sizeof(content)-1U,
          &suite) == UMI_STATUS_OK);
    CHECK(suite != NULL);
    CHECK(UmiAiTrainingBenchmarkSuiteCount(suite, &count) == UMI_STATUS_OK);
    CHECK(count == 2U);
    CHECK(UmiAiTrainingBenchmarkSuiteCase(suite, 0U, &one) == UMI_STATUS_OK);
    CHECK(strcmp(one.title, "account") == 0);
    CHECK(one.promptLength == 3U && one.expectedLength == 3U);
    CHECK(one.promptBytes[0] == 'A' && one.promptBytes[1] == 0U &&
          one.promptBytes[2] == 'B');
    CHECK(one.expectedBytes[0] == 'C' && one.expectedBytes[1] == 0U &&
          one.expectedBytes[2] == 'D');
    CHECK(UmiAiTrainingBenchmarkSuiteCase(suite, 1U, &two) == UMI_STATUS_OK);
    CHECK(two.promptLength == 0U && two.promptBytes == NULL &&
          two.expectedLength == 1U && two.expectedBytes[0] == 0U);
    CHECK(UmiAiTrainingBenchmarkSuiteCase(suite, 2U, &two)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(two.expectedLength == 1U); /* publication failure preserves output */
    UmiAiTrainingBenchmarkSuiteDestroy(suite);
    return 0;
}
