/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_benchmark_suite_rejections.c
 * PURPOSE: Reject duplicate labels, invalid hex, oversized and malformed
 *          reviewed suites without publishing partial parser output.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/benchmark_suite.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)

int main(void)
{
    const char *bad[] = {
        "UMICOM-AI-BENCHMARK-1\ncase a 41 42\ncase a 43 44\n",
        "UMICOM-AI-BENCHMARK-1\ncase a 4 42\n",
        "UMICOM-AI-BENCHMARK-1\ncase a Z1 42\n",
        "UMICOM-AI-BENCHMARK-1\ncase a 41 -\n",
        "UMICOM-AI-BENCHMARK-1\ncase a 41 42 trailing\n",
        "UMICOM-AI-BENCHMARK-1\ncase ../secret 41 42\n",
        "UMICOM-AI-BENCHMARK-1\n",
        "UMICOM-AI-BENCHMARK-2\ncase a 41 42\n",
        "UMICOM-AI-BENCHMARK-1\ncase a 41 42\ncase b 43 44 extra\n"
    };
    UmiAiTrainingBenchmarkSuite *sentinel = (UmiAiTrainingBenchmarkSuite *)0x1;
    for (size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
        UmiAiTrainingBenchmarkSuite *out = sentinel;
        CHECK(UmiAiTrainingBenchmarkSuiteParse(bad[i], strlen(bad[i]), &out)
              != UMI_STATUS_OK);
        CHECK(out == sentinel);
    }
    char payload[] = "UMICOM-AI-BENCHMARK-1\ncase a 41 42\n";
    payload[26U] = '\0';
    CHECK(UmiAiTrainingBenchmarkSuiteParse(payload, sizeof(payload)-1U,
          &sentinel) != UMI_STATUS_OK);
    CHECK(sentinel == (UmiAiTrainingBenchmarkSuite *)0x1);
    CHECK(UmiAiTrainingBenchmarkSuiteParse("x", 0U, &sentinel)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel == (UmiAiTrainingBenchmarkSuite *)0x1);
    return 0;
}
