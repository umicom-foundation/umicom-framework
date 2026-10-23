/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_generation.c
 * PURPOSE: Autoregressive rolling context, determinism, and output isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

/* Nucleus sampling must preserve the original top-K path at p=1. The
 * selector is exercised independently of decoder weights so regression
 * failures identify policy/ranking bugs instead of training differences. */
static int CheckNucleusSelection(void)
{
    const float logits[8] = {0.0F, 0.0F, 0.0F, 0.0F,
                             3.0F, 2.0F, 0.0F, 0.0F};
    float poisoned[8];
    UmiAiTrainingSamplingOptions legacy = {1.0F, 4U};
    UmiAiTrainingNucleusOptions pOne = {1.0F, 4U, 1.0F};
    UmiAiTrainingNucleusOptions tight = {1.0F, 4U, 0.5F};
    UmiAiTrainingNucleusOptions wide = {1.0F, 4U, 0.9F};
    UmiAiTrainingRandomState left, right;
    size_t a = SIZE_MAX, b = SIZE_MAX, retained = 999U;
    CHECK(UmiAiTrainingRandomSeed(&left, 88U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRandomSeed(&right, 88U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
        &legacy, &left, &a) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &pOne, &right, &b, &retained) == UMI_STATUS_OK);
    CHECK(a == b && retained == 4U && left.state == right.state);

    /* exp(0) / sum(exp(0), exp(-1), exp(-3), exp(-3)) > 0.5.
     * The first token alone reaches the tight mass threshold. */
    retained = 999U; a = SIZE_MAX;
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &tight, &right, &a, &retained) == UMI_STATUS_OK);
    CHECK(a == 4U && retained == 1U);
    CHECK(left.state == right.state); /* greedy nucleus uses no RNG */
    CHECK(UmiAiTrainingRandomSeed(&left, 32U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRandomSeed(&right, 32U) == UMI_STATUS_OK);
    retained = 0U;
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &wide, &left, &a, &retained) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &wide, &right, &b, NULL) == UMI_STATUS_OK);
    CHECK(a == b && a >= 4U && a <= 5U && retained == 2U);
    CHECK(left.state == right.state);

    /* The first four token IDs are excluded from the allowed interval;
     * an invalid candidate outside topK must still be rejected. */
    memcpy(poisoned, logits, sizeof poisoned);
    poisoned[7] = NAN;
    UmiAiTrainingRandomState unchanged = left;
    a = SIZE_MAX; retained = 999U;
    CHECK(UmiAiTrainingSampleLogitsNucleus(poisoned, 8U, 4U, 8U,
        &wide, &left, &a, &retained) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(a == SIZE_MAX && retained == 999U && left.state == unchanged.state);
    UmiAiTrainingNucleusOptions wrong = {1.0F, 4U, 0.0F};
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &wrong, &left, &a, &retained) == UMI_STATUS_INVALID_ARGUMENT);
    wrong.topP = 1.1F;
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &wrong, &left, &a, &retained) == UMI_STATUS_INVALID_ARGUMENT);
    wrong.topP = NAN;
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &wrong, &left, &a, &retained) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(a == SIZE_MAX && retained == 999U && left.state == unchanged.state);
    CHECK(UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
        &wide, &left, &a, &a) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(a == SIZE_MAX && left.state == unchanged.state);

    /* The existing top-K==1 contract allows NULL RNG, including ties. */
    const float ties[6] = {0, 0, 0, 0, 1, 1};
    UmiAiTrainingNucleusOptions one = {1.0F, 1U, 0.25F};
    CHECK(UmiAiTrainingSampleLogitsNucleus(ties, 6U, 4U, 6U,
        &one, NULL, &a, &retained) == UMI_STATUS_OK);
    CHECK(a == 4U && retained == 1U);
    return 0;
}

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(99887)};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingSamplingOptions greedy = {1.0F, 1U};
    UmiAiTrainingSamplingOptions sample = {0.8F, 12U};
    size_t prompt[3] = {109U, 114U, 120U}; /* byte encoding of "int" */
    size_t first[32] = {0}, second[32] = {0};
    size_t produced = 999U;
    UmiAiTrainingDecoderConfig inspected;
    uint64_t stepCount = UINT64_MAX;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 24U, 0U, first, 32U, &produced) == UMI_STATUS_OK);
    CHECK(produced == 24U);
    for (size_t i = 0U; i < produced; ++i)
        CHECK(first[i] >= UMI_AI_BYTE_OFFSET && first[i] < UMI_AI_BYTE_VOCABULARY);
    produced = 0U;
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 24U, 1234U, second, 32U, &produced) == UMI_STATUS_OK);
    CHECK(produced == 24U && memcmp(first, second, 24U * sizeof(size_t)) == 0);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &sample, 24U, 42U, first, 32U, &produced) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &sample, 24U, 42U, second, 32U, &produced) == UMI_STATUS_OK);
    CHECK(memcmp(first, second, 24U * sizeof(size_t)) == 0);
    CHECK(UmiAiTrainingDecoderInspect(model, &inspected, &stepCount) == UMI_STATUS_OK);
    CHECK(stepCount == 0U && inspected.contextLength == 8U);

    size_t guarded[32];
    for (size_t i = 0U; i < 32U; ++i) guarded[i] = SIZE_MAX;
    produced = 999U;
    /* The requested count is within the published ceiling, but larger than
     * the provided buffer: neither buffer nor count can be partially written. */
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 33U, 0U, guarded, 32U, &produced) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(produced == 999U);
    for (size_t i = 0U; i < 32U; ++i) CHECK(guarded[i] == SIZE_MAX);
    size_t invalidPrompt[] = {UMI_AI_BYTE_VOCABULARY};
    CHECK(UmiAiTrainingDecoderGenerate(model, invalidPrompt, 1U,
                &greedy, 1U, 0U, guarded, 32U, &produced) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(produced == 999U && guarded[0] == SIZE_MAX);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 0U, 0U, guarded, 32U, &produced) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(guarded[0] == SIZE_MAX);

    /* The decoder owns its neural parameters; generation cannot change them. */
    size_t checkpointSize = 0U, written = 0U;
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize) == UMI_STATUS_OK);
    uint8_t *before = malloc(checkpointSize), *after = malloc(checkpointSize);
    CHECK(before != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
                checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
    CHECK(UmiAiTrainingDecoderGenerate(model, NULL, 0U,
                &greedy, 4U, 1U, first, 32U, &produced) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
                checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
    CHECK(memcmp(before, after, checkpointSize) == 0);
    /* TopP=1 must be byte-identical to existing seeded generation, even
     * after training and/or checkpoint reload. Outputs are still atomic. */
    UmiAiTrainingNucleusOptions legacyEquivalent = {0.8F, 12U, 1.0F};
    UmiAiTrainingNucleusOptions nucleus = {0.8F, 12U, 0.75F};
    size_t legacyTokens[24], nucleusTokens[24], nucleusAgain[24];
    size_t legacyCount = 0U, nucleusCount = 0U;
    CHECK(CheckNucleusSelection() == 0);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U, &sample,
        24U, 42U, legacyTokens, 24U, &legacyCount) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerateNucleus(model, prompt, 3U,
        &legacyEquivalent, 24U, 42U, nucleusTokens, 24U, &nucleusCount) == UMI_STATUS_OK);
    CHECK(legacyCount == nucleusCount &&
        memcmp(legacyTokens, nucleusTokens, sizeof legacyTokens) == 0);
    CHECK(UmiAiTrainingDecoderGenerateNucleus(model, prompt, 3U,
        &nucleus, 24U, 42U, nucleusTokens, 24U, &nucleusCount) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerateNucleus(model, prompt, 3U,
        &nucleus, 24U, 42U, nucleusAgain, 24U, &nucleusCount) == UMI_STATUS_OK);
    CHECK(memcmp(nucleusTokens, nucleusAgain, sizeof nucleusTokens) == 0);
    for (size_t i = 0U; i < nucleusCount; ++i)
        CHECK(nucleusTokens[i] >= UMI_AI_BYTE_OFFSET &&
              nucleusTokens[i] < UMI_AI_BYTE_VOCABULARY);
    for (size_t i = 0U; i < 32U; ++i) guarded[i] = SIZE_MAX;
    produced = 999U;
    UmiAiTrainingNucleusOptions invalidP = nucleus;
    invalidP.topP = 0.0F;
    CHECK(UmiAiTrainingDecoderGenerateNucleus(model, prompt, 3U,
        &invalidP, 4U, 9U, guarded, 32U, &produced) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderGenerateNucleus(model, prompt, 3U,
        &nucleus, 33U, 9U, guarded, 32U, &produced) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(produced == 999U);
    for (size_t i = 0U; i < 32U; ++i) CHECK(guarded[i] == SIZE_MAX);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
        checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
    CHECK(memcmp(before, after, checkpointSize) == 0);
    free(after);
    free(before);
    UmiAiTrainingDecoderDestroy(model);
    puts("decoder generation, context, determinism, atomic results: PASS");
    return 0;
}
