/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/profile_generation_work.c
 * PURPOSE: Measure complete native decoder and persistent KV-session work
 *          on exactly the same tokens, model and seeded policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_performance.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261023)
    };
    const UmiAiTrainingNucleusOptions options = {0.85F, 12U, 0.9F};
    const size_t prompt[] = {109U, 114U, 120U}; /* byte IDs: 'i', 'n', 't' */
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingGenerationPerformance result = {0};
    UmiStatus status;
    (void)argv;
    if (argc != 1) {
        fputs("usage: umicom-native-llm-generation-work\n", stderr);
        return 2;
    }
    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingGenerationPerformanceCompare(model,
            prompt, 3U, &options, 12U, UINT64_C(7801), &result);
    UmiAiTrainingDecoderDestroy(model);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "generation comparison refused (status=%d)\n", (int)status);
        return 1;
    }
    printf("Umicom native generation economics — single CPU observation\n"
           "Output parity: %zu/%zu tokens\n"
           "Decoder reference work: %" PRIu64 " token-rows\n"
           "KV cache observed work: %" PRIu64 " token-rows\n"
           "KV full-window rebuilds: %" PRIu64 "\n"
           "KV cache heap reservation: %zu bytes\n",
           result.matchingTokens, result.estimate.generatedTokens,
           result.estimate.referenceComputedRows, result.observedCachedRows,
           result.observedWindowRebuilds, result.estimate.cacheHeapBytes);
    if (result.cpuClockAvailable)
        printf("Reference process CPU: %.6f s\n"
               "Cached-session process CPU: %.6f s\n",
               result.referenceProcessCpuSeconds, result.cachedProcessCpuSeconds);
    else
        puts("Process CPU timing unavailable; do not infer a speed difference.");
    puts("Learned absolute positions require full recomputation after each rollover.");
    puts("CPU times are not wall-clock throughput or a reproducible speedup guarantee.");
    return 0;
}
