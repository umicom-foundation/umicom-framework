/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_benchmark_suite_file.c
 * PURPOSE: Verify native read-only reviewed benchmark fixture ingestion and
 *          unchanged output when an explicitly requested file is absent.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/benchmark_suite.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)
#ifndef UMI_AI_TEST_BENCHMARK_SUITE_PATH
#error UMI_AI_TEST_BENCHMARK_SUITE_PATH must identify the source fixture
#endif
int main(void)
{
    UmiAiTrainingBenchmarkSuite *suite = NULL;
    UmiAiTrainingBenchmarkCase item;
    size_t count=0U;
    CHECK(UmiAiTrainingBenchmarkSuiteLoadFile(UMI_AI_TEST_BENCHMARK_SUITE_PATH,
          &suite) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingBenchmarkSuiteCount(suite, &count) == UMI_STATUS_OK);
    CHECK(count >= 2U);
    CHECK(UmiAiTrainingBenchmarkSuiteCase(suite, 0U, &item) == UMI_STATUS_OK);
    CHECK(item.expectedLength > 0U);
    CHECK(strlen(item.title) > 0U);
    UmiAiTrainingBenchmarkSuiteDestroy(suite);
    suite = (UmiAiTrainingBenchmarkSuite *)0x1;
    CHECK(UmiAiTrainingBenchmarkSuiteLoadFile("no-such-approved-benchmark-file-umicom",
          &suite) == UMI_STATUS_IO_ERROR);
    CHECK(suite == (UmiAiTrainingBenchmarkSuite *)0x1);
    return 0;
}
