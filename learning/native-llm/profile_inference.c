/*-----------------------------------------------------------------------------
 * Umicom Framework / Native LLM Learning Laboratory
 * File: learning/native-llm/profile_inference.c
 *
 * PURPOSE:
 *   Inspect complete decoder versus cache workloads on a fixed, documented
 *   Umicom C23 teaching example. This thin tool only uses Framework APIs.
 *   An optional reviewed checkpoint substitutes its existing model weights.
 *
 * ARCHITECTURE:
 *   No local tensor mathematics, attention, stopwatch backends, or sampling
 *   implementations. Neither source code nor model weights are changed.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/inference_profile.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Deliberately use a realistic miniature source declaration instead of
 * unrelated sample prose. It is input DATA, never compiled or executed. */
static const char exampleSource[] =
    "typedef struct UmicomAccount { unsigned accountId; } UmicomAccount;\n"
    "int UmicomAccountOpen(const UmicomAccount *account) {\n"
    "    return account != 0 && account->accountId > 0;\n"
    "}\n";

int main(int argc, char **argv)
{
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261021)
    };
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingInferenceProfile profile;
    UmiStatus status;
    uint32_t tokenBuffer[sizeof(exampleSource)];
    size_t stream[sizeof(exampleSource)];
    size_t tokenCount = 0U;
    size_t prefixLength;
    if (argc > 2) {
        fprintf(stderr, "usage: umicom-native-llm-profile [reviewed-checkpoint]\n");
        return 2;
    }
    if (argc == 2) {
        status = UmiAiTrainingDecoderCheckpointLoadFile(argv[1], &decoder);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "checkpoint rejected: status=%d\n", (int)status);
            return 1;
        }
        uint64_t steps = 0U;
        status = UmiAiTrainingDecoderInspect(decoder, &config, &steps);
        if (status != UMI_STATUS_OK) goto failed;
        printf("Loaded reviewed checkpoint with %" PRIu64 " optimiser steps\n",
               steps);
    } else {
        status = UmiAiTrainingDecoderCreate(&config, &decoder);
        if (status != UMI_STATUS_OK) goto failed;
        puts("Using a seeded, UNTRAINED native decoder for numerical profiling.");
    }
    if (config.vocabularySize != UMI_AI_BYTE_VOCABULARY) {
        fprintf(stderr, "This laboratory requires the Framework byte vocabulary.\n");
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto failed;
    }
    status = UmiAiByteEncode((const uint8_t *)exampleSource,
                             sizeof(exampleSource) - 1U,
                             tokenBuffer, sizeof(tokenBuffer) / sizeof(tokenBuffer[0]),
                             &tokenCount);
    if (status != UMI_STATUS_OK) goto failed;
    if (tokenCount > UMI_AI_INFERENCE_PROFILE_MAX_TOKENS) {
        fprintf(stderr, "learning fixture exceeds the profiler's bounded stream\n");
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto failed;
    }
    for (size_t i = 0U; i < tokenCount; ++i)
        stream[i] = (size_t)tokenBuffer[i];
    prefixLength = config.contextLength > 2U ? 2U : 1U;
    status = UmiAiTrainingInferenceProfileStream(decoder, stream, tokenCount,
                                                  prefixLength, &profile);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Tokens: %zu; prefill: %zu; context: %u; queries: %zu\n",
           profile.streamTokens, profile.prefillTokens,
           (unsigned)config.contextLength, profile.referenceQueries);
    printf("Reference rows: %" PRIu64 "; cache rows: %" PRIu64
           "; window rebuilds: %" PRIu64 "\n",
           profile.referenceComputedRows, profile.cacheComputedRows,
           profile.cacheWindowRebuilds);
    printf("Maximum absolute logit deviation: %.9f\n",
           profile.maximumLogitDeviation);
    printf("Logical model parameters: %zu elements (%zu bytes)\n",
           profile.footprint.parameterElements,
           profile.footprint.logicalParameterBytes);
    printf("Maximum context K/V payload: %zu bytes; cache object: %zu bytes\n",
           profile.footprint.keyValuePayloadBytes,
           profile.footprint.cacheAllocationBytes);
    printf("Full-context output logits: %zu bytes per reference query\n",
           profile.footprint.fullContextLogitsBytes);
    if (profile.cpuClockAvailable) {
        printf("Observed process CPU seconds: reference %.6f; cache %.6f\n",
               profile.referenceProcessCpuSeconds,
               profile.cachedProcessCpuSeconds);
    } else {
        puts("Process CPU clock unavailable on this platform.");
    }
    puts("These are observations on one tiny model, not a certified speedup."
         " Full windows rebuild because learned absolute positions shift.");
    UmiAiTrainingDecoderDestroy(decoder);
    return 0;
failed:
    fprintf(stderr, "inference profile failed: status=%d\n", (int)status);
    UmiAiTrainingDecoderDestroy(decoder);
    return 1;
}
