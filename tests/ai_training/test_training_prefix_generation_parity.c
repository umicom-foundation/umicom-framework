/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_prefix_generation_parity.c
 * PURPOSE:
 *   Qualify opt-in incremental generation against the independent ORIGINAL
 *   decoder, evaluated at the right-padded prefix's last retained position.
 *   Validate real token streams, exact workload and unchanged checkpoints.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_generation.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

static int CheckOneWorkload(UmiAiTrainingDecoder *model,
                            const UmiAiTrainingDecoderConfig *config,
                            const size_t *prompt, size_t promptTokens,
                            size_t requestedTokens,
                            const UmiAiTrainingNucleusOptions *policy,
                            uint64_t seed)
{
    UmiAiTrainingPrefixGenerationSession *session = NULL;
    UmiAiTrainingRandomState referenceRandom = {0};
    UmiAiTrainingPrefixGenerationWorkEstimate estimate = {0};
    UmiAiTrainingGenerationCacheStats stats = {0}, oneShotStats = {0};
    float fullLogits[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
    size_t context[UMI_AI_DECODER_MAX_CONTEXT];
    size_t sampled[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t oneShot[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t canonicalHistory[UMI_AI_DECODER_MAX_CONTEXT];
    size_t retained = promptTokens, observedCount = 0U, oneShotCount = 0U;
    const size_t vocabulary = (size_t)config->vocabularySize;
    const size_t contextLimit = (size_t)config->contextLength;

    CHECK(UmiAiTrainingPrefixGenerationWorkEstimateBuild(
        config, promptTokens, requestedTokens, &estimate) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRandomSeed(&referenceRandom, seed) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPrefixGenerationSessionCreate(model,
        prompt, promptTokens, policy, seed, &session) == UMI_STATUS_OK);
    CHECK(session != NULL);
    memcpy(canonicalHistory, prompt, promptTokens * sizeof(size_t));

    for (size_t step = 0U; step < requestedTokens; ++step) {
        size_t expected = 0U;
        /* Right-padding affects no earlier causal row. The independent
         * reference is the canonical decoder, NOT a second KV cache. */
        for (size_t index = 0U; index < contextLimit; ++index)
            context[index] = index < retained ?
                canonicalHistory[index] : UMI_AI_BYTE_BOS;
        CHECK(UmiAiTrainingDecoderLogits(model, context, contextLimit,
            fullLogits, contextLimit * vocabulary) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingSampleLogitsNucleus(
            &fullLogits[(retained - 1U) * vocabulary], vocabulary,
            UMI_AI_BYTE_OFFSET, vocabulary, policy,
            &referenceRandom, &expected, NULL) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingPrefixGenerationSessionNext(session,
            &sampled[step]) == UMI_STATUS_OK);
        CHECK(sampled[step] == expected);

        /* Mirror only the INPUT history that the next prediction will see;
         * the returned token stays pending in the session until next pull. */
        if (step + 1U < requestedTokens) {
            if (retained < contextLimit)
                canonicalHistory[retained++] = expected;
            else {
                memmove(canonicalHistory, &canonicalHistory[1],
                    (contextLimit - 1U) * sizeof(size_t));
                canonicalHistory[contextLimit - 1U] = expected;
            }
        }
    }

    CHECK(UmiAiTrainingPrefixGenerationSessionInspect(session,
        &observedCount, &stats) == UMI_STATUS_OK);
    CHECK(observedCount == requestedTokens);
    CHECK(stats.computedRows == estimate.cachedComputedRows);
    CHECK(stats.windowRebuilds == estimate.cachedWindowRebuilds);
    CHECK(stats.retainedTokens == retained);
    UmiAiTrainingPrefixGenerationSessionDestroy(session);

    CHECK(UmiAiTrainingDecoderGeneratePrefixNucleus(model,
        prompt, promptTokens, policy, requestedTokens, seed,
        oneShot, UMI_AI_GENERATION_MAX_NEW_TOKENS,
        &oneShotCount, &oneShotStats) == UMI_STATUS_OK);
    CHECK(oneShotCount == requestedTokens);
    CHECK(memcmp(sampled, oneShot,
        requestedTokens * sizeof(size_t)) == 0);
    CHECK(stats.computedRows == oneShotStats.computedRows);
    CHECK(stats.windowRebuilds == oneShotStats.windowRebuilds);
    CHECK(stats.retainedTokens == oneShotStats.retainedTokens);
    CHECK(estimate.computedRowsAvoided ==
        estimate.canonicalComputedRows - estimate.cachedComputedRows);
    return 0;
}

int main(void)
{
    const UmiAiTrainingDecoderConfig configs[] = {
        {UMI_AI_BYTE_VOCABULARY, 4U, 8U, 16U, 2U, UINT64_C(20261023)},
        {UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261024)}
    };
    const UmiAiTrainingNucleusOptions policies[] = {
        {1.0F, 1U, 1.0F},
        {0.85F, 12U, 0.90F}
    };
    const size_t prompt[] = {109U, 114U, 120U, 35U,
        57U, 90U, 119U, 125U};
    const size_t generatedCounts[] = {1U, 3U, 12U};
    const size_t promptCounts[] = {1U, 3U, 8U};
    for (size_t c = 0U; c < sizeof configs / sizeof configs[0]; ++c) {
        UmiAiTrainingDecoder *model = NULL;
        uint8_t *before = NULL, *after = NULL;
        size_t checkpointSize = 0U, written = 0U;
        const size_t limit = (size_t)configs[c].contextLength;
        CHECK(UmiAiTrainingDecoderCreate(&configs[c], &model) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize)
              == UMI_STATUS_OK);
        before = malloc(checkpointSize);
        after = malloc(checkpointSize);
        CHECK(before != NULL && after != NULL);
        CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
            checkpointSize, &written) == UMI_STATUS_OK);
        CHECK(written == checkpointSize);

        for (size_t p = 0U; p < sizeof policies / sizeof policies[0]; ++p)
            for (size_t q = 0U; q < sizeof promptCounts / sizeof promptCounts[0]; ++q)
                for (size_t n = 0U; n < sizeof generatedCounts / sizeof generatedCounts[0]; ++n)
                    if (promptCounts[q] <= limit)
                        CHECK(CheckOneWorkload(model, &configs[c],
                            prompt, promptCounts[q], generatedCounts[n],
                            &policies[p], UINT64_C(7801)) == 0);

        CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
            checkpointSize, &written) == UMI_STATUS_OK);
        CHECK(written == checkpointSize);
        CHECK(memcmp(before, after, checkpointSize) == 0);
        free(after);
        free(before);
        UmiAiTrainingDecoderDestroy(model);
    }
    puts("prefix generation: independent decoder tokens, workload, checkpoint PASS");
    return 0;
}
