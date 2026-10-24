/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/prefix-work/profile_prefix_generation.c
 * PURPOSE:
 *   Demonstrate that a deliberately unpadded generation prefix can reuse
 *   key/value rows until learned absolute positions require rollover.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_generation.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261023)
    };
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {109U, 114U, 120U}; /* ASCII "int" plus 4. */
    const size_t generatedTokens = 12U;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingPrefixGenerationWorkEstimate estimate = {0};
    UmiAiTrainingGenerationCacheStats observed = {0};
    size_t generated[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t written = 0U;
    UmiStatus status;
    (void)argv;

    if (argc != 1) {
        fputs("usage: umicom-native-llm-prefix-generation\n", stderr);
        return 2;
    }
    status = UmiAiTrainingPrefixGenerationWorkEstimateBuild(&config,
        sizeof prompt / sizeof prompt[0], generatedTokens, &estimate);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingDecoderGeneratePrefixNucleus(model,
            prompt, sizeof prompt / sizeof prompt[0], &policy,
            generatedTokens, UINT64_C(7801),
            generated, UMI_AI_GENERATION_MAX_NEW_TOKENS, &written, &observed);
    UmiAiTrainingDecoderDestroy(model);

    if (status != UMI_STATUS_OK || written != generatedTokens ||
        observed.computedRows != estimate.cachedComputedRows ||
        observed.windowRebuilds != estimate.cachedWindowRebuilds) {
        fprintf(stderr, "Prefix generation refused or workload mismatch: %d\n",
                (int)status);
        return 1;
    }
    printf("Umicom Native LLM — growing-prefix generation\n"
           "Generated tokens: %zu\n"
           "Canonical right-padded decoder rows: %" PRIu64 "\n"
           "Incremental cache rows: %" PRIu64 "\n"
           "Rows avoided: %" PRIu64 "\n"
           "Full-window rebuilds: %" PRIu64 "\n"
           "Retained token rows: %zu\n"
           "One cache heap allocation: %zu bytes\n",
           written, estimate.canonicalComputedRows,
           observed.computedRows, estimate.computedRowsAvoided,
           observed.windowRebuilds, observed.retainedTokens,
           estimate.cacheHeapBytes);
    puts("This is opt-in UNPADDED generation; it is NOT the legacy BOS-left-padded policy.");
    puts("Learned absolute positions still require recomputation after rollover.");
    return 0;
}
