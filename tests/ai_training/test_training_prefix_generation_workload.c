/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_prefix_generation_workload.c
 * PURPOSE:
 *   Exhaustively verify bounded prefix-generation row accounting around
 *   the first rollover and all subsequent rebuilds, using an independent
 *   step-by-step count rather than another closed-form expression.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_generation.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    const size_t contexts[] = {1U, 2U, 4U, 8U, 16U, 32U};
    size_t proposals = 0U;

    for (size_t c = 0U; c < sizeof contexts / sizeof contexts[0]; ++c) {
        const size_t context = contexts[c];
        const UmiAiTrainingDecoderConfig config = {
            UMI_AI_BYTE_VOCABULARY, (uint32_t)context,
            8U, 16U, 2U, UINT64_C(73174)
        };
        for (size_t prompt = 1U; prompt <= context; ++prompt) {
            for (size_t generated = 1U;
                 generated <= UMI_AI_GENERATION_MAX_NEW_TOKENS;
                 ++generated) {
                UmiAiTrainingPrefixGenerationWorkEstimate estimate = {0};
                uint64_t independentlyCountedRows = (uint64_t)prompt;
                uint64_t independentlyCountedRebuilds = 0U;
                size_t retained = prompt;
                CHECK(UmiAiTrainingPrefixGenerationWorkEstimateBuild(
                    &config, prompt, generated, &estimate) == UMI_STATUS_OK);

                /* Only G-1 appends: the last sampled token is pending. */
                for (size_t step = 1U; step < generated; ++step) {
                    if (retained < context) {
                        ++retained;
                        ++independentlyCountedRows;
                    } else {
                        independentlyCountedRows += (uint64_t)context;
                        ++independentlyCountedRebuilds;
                    }
                }
                CHECK(estimate.cacheHeapBytes > 0U);
                CHECK(estimate.canonicalComputedRows ==
                    (uint64_t)context * (uint64_t)generated);
                CHECK(estimate.cachedComputedRows == independentlyCountedRows);
                CHECK(estimate.cachedWindowRebuilds ==
                    independentlyCountedRebuilds);
                CHECK(estimate.computedRowsAvoided ==
                    estimate.canonicalComputedRows - estimate.cachedComputedRows);
                CHECK(estimate.promptTokens == prompt);
                CHECK(estimate.generatedTokens == generated);
                CHECK(estimate.contextTokens == context);
                ++proposals;
            }
        }
    }
    printf("prefix generation: %zu independent workload proposals PASS\n",
           proposals);
    return 0;
}
