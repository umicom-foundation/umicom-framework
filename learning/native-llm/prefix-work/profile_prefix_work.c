/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/prefix-work/profile_prefix_work.c
 * PURPOSE: Demonstrate actual reuse for a growing unpadded token prefix.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_prefix_reuse.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261023)
    };
    const size_t tokens[] = {109U, 114U, 120U, 36U, 124U, 108U};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingKvPrefixPerformance report = {0};
    UmiStatus status;
    (void)argv;
    if (argc != 1) {
        fputs("usage: umicom-native-llm-prefix-work\n", stderr);
        return 2;
    }
    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingKvPrefixPerformanceCompare(model,
            tokens, sizeof(tokens) / sizeof(tokens[0]), &report);
    UmiAiTrainingDecoderDestroy(model);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "prefix comparison refused: status=%d\n", (int)status);
        return 1;
    }
    printf("Umicom native C23 — growing-prefix K/V reuse\n"
           "Qualified matching prefix steps: %zu\n"
           "Recomputed prefix work: %" PRIu64 " token rows\n"
           "Incremental K/V work: %" PRIu64 " token rows\n"
           "Avoided token rows: %" PRIu64 "\n"
           "Context rebuilds: %" PRIu64 "\n"
           "Maximum absolute logit delta: %.8g\n"
           "One cache heap allocation: %zu bytes\n",
           report.matchingPrefixes, report.observedReferenceRows,
           report.observedIncrementalRows, report.estimate.computedRowsAvoided,
           report.observedWindowRebuilds,
           (double)report.maximumAbsoluteLogitDifference,
           report.estimate.singleCacheHeapBytes);
    if (report.cpuClockAvailable)
        printf("Repeated-prefix process CPU: %.6f s\n"
               "Incremental process CPU: %.6f s\n",
               report.referenceProcessCpuSeconds,
               report.incrementalProcessCpuSeconds);
    else
        puts("CPU clock unavailable; no speed claim is possible.");
    puts("This is NOT the legacy BOS-padded generation workload."
         "\nAt full context, rolling eviction still forces a rebuild."
         "\nRow counts are not elapsed time or a general speedup guarantee.");
    return 0;
}
