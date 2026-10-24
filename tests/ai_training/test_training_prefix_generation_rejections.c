/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_prefix_generation_rejections.c
 * PURPOSE:
 *   Validate invalid inputs, byte-vocabulary guard, output preservation,
 *   exhausted generation capacity and the pending-token inspection contract.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_generation.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        16U, 4U, 4U, 8U, 2U, UINT64_C(47)
    };
    UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingPrefixGenerationSession *session = NULL;
    UmiAiTrainingPrefixGenerationWorkEstimate estimate;
    UmiAiTrainingGenerationCacheStats cacheStats;
    const size_t prompt[] = {4U, 5U};
    const size_t invalidId[] = {16U};
    size_t outTokens[3], savedTokens[3];
    size_t count, next = 0U, snapshotCount = 0U;
    uint8_t singleByte = UINT8_C(0xA5);
    unsigned char savedEstimate[sizeof estimate];
    unsigned char savedStats[sizeof cacheStats];

    memset(&estimate, 0xAC, sizeof estimate);
    memcpy(savedEstimate, &estimate, sizeof estimate);
    CHECK(UmiAiTrainingPrefixGenerationWorkEstimateBuild(NULL,
        2U, 3U, &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixGenerationWorkEstimateBuild(&config,
        0U, 3U, &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixGenerationWorkEstimateBuild(&config,
        5U, 3U, &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixGenerationWorkEstimateBuild(&config,
        2U, 0U, &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixGenerationWorkEstimateBuild(&config,
        2U, UMI_AI_GENERATION_MAX_NEW_TOKENS + 1U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&estimate, savedEstimate, sizeof estimate) == 0);

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPrefixGenerationSessionCreate(model,
        NULL, 2U, &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixGenerationSessionCreate(model,
        prompt, 0U, &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixGenerationSessionCreate(model,
        invalidId, 1U, &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(session == NULL);

    outTokens[0] = 111U; outTokens[1] = 222U; outTokens[2] = 333U;
    memcpy(savedTokens, outTokens, sizeof outTokens);
    count = 444U;
    memset(&cacheStats, 0xAB, sizeof cacheStats);
    memcpy(savedStats, &cacheStats, sizeof cacheStats);

    CHECK(UmiAiTrainingDecoderGeneratePrefixNucleus(model,
        prompt, 2U, &policy, 3U, 1U,
        outTokens, 2U, &count, &cacheStats) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingDecoderGeneratePrefixNucleus(model,
        invalidId, 1U, &policy, 2U, 1U,
        outTokens, 3U, &count, &cacheStats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderGeneratePrefixNucleus(model,
        prompt, 2U, &policy, 0U, 1U,
        outTokens, 3U, &count, &cacheStats) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 0.0F;
    CHECK(UmiAiTrainingDecoderGeneratePrefixNucleus(model,
        prompt, 2U, &policy, 2U, 1U,
        outTokens, 3U, &count, &cacheStats) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 1.0F;
    CHECK(memcmp(outTokens, savedTokens, sizeof outTokens) == 0);
    CHECK(count == 444U);
    CHECK(memcmp(&cacheStats, savedStats, sizeof cacheStats) == 0);

    CHECK(UmiAiTrainingPrefixGenerationSessionCreate(model,
        prompt, 2U, &policy, 1U, &session) == UMI_STATUS_OK);
    next = 999U;
    CHECK(UmiAiTrainingPrefixGenerationSessionNext(session, NULL)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixGenerationSessionNextByte(session,
        &singleByte) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(singleByte == UINT8_C(0xA5));
    CHECK(UmiAiTrainingPrefixGenerationSessionInspect(session,
        &snapshotCount, &cacheStats) == UMI_STATUS_OK);
    CHECK(snapshotCount == 0U && cacheStats.retainedTokens == 2U);
    CHECK(cacheStats.computedRows == 2U && cacheStats.windowRebuilds == 0U);
    CHECK(UmiAiTrainingPrefixGenerationSessionNext(session, &next)
        == UMI_STATUS_OK);
    CHECK(next >= UMI_AI_BYTE_OFFSET && next < 16U);
    CHECK(UmiAiTrainingPrefixGenerationSessionInspect(session,
        &snapshotCount, &cacheStats) == UMI_STATUS_OK);
    CHECK(snapshotCount == 1U && cacheStats.computedRows == 2U);
    UmiAiTrainingPrefixGenerationSessionDestroy(session);

    /* Exhausted-session rejections must not change the existing output. */
    CHECK(UmiAiTrainingPrefixGenerationSessionCreate(model,
        prompt, 2U, &policy, 1U, &session) == UMI_STATUS_OK);
    for (size_t i = 0U; i < UMI_AI_GENERATION_MAX_NEW_TOKENS; ++i)
        CHECK(UmiAiTrainingPrefixGenerationSessionNext(session, &next)
              == UMI_STATUS_OK);
    next = 555U;
    CHECK(UmiAiTrainingPrefixGenerationSessionNext(session, &next)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(next == 555U);
    CHECK(UmiAiTrainingPrefixGenerationSessionInspect(session,
        &snapshotCount, &cacheStats) == UMI_STATUS_OK);
    CHECK(snapshotCount == UMI_AI_GENERATION_MAX_NEW_TOKENS);
    UmiAiTrainingPrefixGenerationSessionDestroy(session);
    UmiAiTrainingDecoderDestroy(model);
    puts("prefix generation: input rejection and failure-atomic outputs PASS");
    return 0;
}
