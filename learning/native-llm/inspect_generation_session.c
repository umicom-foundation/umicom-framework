/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM learning laboratory
 * File: learning/native-llm/inspect_generation_session.c
 * PURPOSE: Demonstrate cancellable, one-token-at-a-time generation using
 *          the same Framework sampler, weight snapshot and inference cache.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_session.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int Demonstrate(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261023)
    };
    const UmiAiTrainingNucleusOptions policy = {0.85F, 12U, 0.90F};
    const uint8_t promptBytes[] = {(uint8_t)'i', (uint8_t)'n', (uint8_t)'t'};
    uint32_t encoded[sizeof promptBytes] = {0};
    size_t prompt[sizeof promptBytes] = {0};
    size_t produced = 0U, referenceCount = 0U;
    size_t reference[12] = {0};
    size_t obtained[12] = {0};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingGenerationSession *session = NULL;
    UmiAiTrainingGenerationCacheStats statistics = {0};
    UmiStatus status = UMI_STATUS_OK;
    int outcome = 1;

    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiByteEncode(promptBytes, sizeof promptBytes, encoded,
        sizeof encoded / sizeof encoded[0], &produced);
    if (status != UMI_STATUS_OK || produced != sizeof promptBytes) goto cleanup;
    for (size_t i = 0U; i < produced; ++i) prompt[i] = (size_t)encoded[i];
    status = UmiAiTrainingDecoderGenerateCachedNucleus(model,
        prompt, produced, &policy, 12U, UINT64_C(555),
        reference, 12U, &referenceCount, NULL);
    if (status != UMI_STATUS_OK || referenceCount != 12U) goto cleanup;
    status = UmiAiTrainingGenerationSessionCreate(model, prompt, produced,
        &policy, UINT64_C(555), &session);
    if (status != UMI_STATUS_OK) goto cleanup;

    puts("Umicom native LLM pull-stream example (hexadecimal byte tokens)");
    puts("The caller can pause between pulls or destroy the session to cancel.");
    for (size_t chunk = 0U; chunk < 3U; ++chunk) {
        const size_t limit = chunk == 0U ? 3U : chunk == 1U ? 7U : 12U;
        const size_t begin = chunk == 0U ? 0U : chunk == 1U ? 3U : 7U;
        printf("Chunk %zu:", chunk + 1U);
        for (size_t i = begin; i < limit; ++i) {
            status = UmiAiTrainingGenerationSessionNext(session, &obtained[i]);
            if (status != UMI_STATUS_OK) goto cleanup;
            printf(" %02x", (unsigned)(obtained[i] - UMI_AI_BYTE_OFFSET));
        }
        putchar('\n');
    }
    if (memcmp(reference, obtained, sizeof reference) != 0) {
        fputs("full and chunked cached generation differ\n", stderr);
        goto cleanup;
    }
    status = UmiAiTrainingGenerationSessionInspect(session,
        &produced, &statistics);
    if (status != UMI_STATUS_OK || produced != 12U) goto cleanup;
    printf("Token count: %zu | computed rows: %" PRIu64
           " | window rebuilds: %" PRIu64
           " | retained: %zu\n", produced, statistics.computedRows,
           statistics.windowRebuilds, statistics.retainedTokens);
    puts("All streamed token IDs match one-shot cached generation.");
    puts("Cache counters are workload indicators, not speedup claims.");
    outcome = 0;
cleanup:
    if (outcome != 0)
        fprintf(stderr, "native generation session demonstration failed (status=%d)\n", (int)status);
    UmiAiTrainingGenerationSessionDestroy(session);
    UmiAiTrainingDecoderDestroy(model);
    return outcome;
}

int main(int argc, char **argv)
{
    (void)argv;
    if (argc != 1) {
        fputs("usage: umicom-native-llm-streaming-demo\n", stderr);
        return 2;
    }
    return Demonstrate();
}
