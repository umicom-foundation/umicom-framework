/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_decoder_logit_adapter.c
 * PURPOSE: Verify real frozen-decoder logits, categorical adapter training,
 *          token validation, graph lifetime and frozen-model invariants.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder_logit_adapter.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "decoder logit adapter line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        8U, 4U, 4U, 8U, 2U, UINT64_C(2056)
    };
    const size_t context[] = {0U, 1U, 2U, 3U};
    const size_t invalidContext[] = {0U, 1U, 2U, 8U};
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingDecoderLogitAdapter *adapter = NULL;
    UmiAiTrainingDecoderLogitStep report = {0};
    UmiAiTrainingDecoderLogitInfo before = {0}, after = {0};
    float reference[32] = {0};
    float referenceAfter[32] = {0};
    float adapted[32] = {0};
    float initialLoss = 0.0F, finalLoss = 0.0F;
    size_t targets[4] = {0};

    CHECK(UmiAiTrainingDecoderCreate(&config, &decoder) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogitAdapterCreate(decoder, 3U,
        UINT64_C(71), &adapter) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogitAdapterInspect(adapter, &before) == UMI_STATUS_OK);
    CHECK(before.vocabularySize == 8U && before.rank == 3U);
    CHECK(before.trainableElements == 48U && before.completedAdapterSteps == 0U);

    CHECK(UmiAiTrainingDecoderLogits(decoder, context, 4U,
        reference, 32U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogitAdapterLogits(adapter, context, 4U,
        adapted, 32U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 32U; ++i)
        CHECK(fabsf(reference[i] - adapted[i]) <= 0.00001F);
    /* The canonical decoder supplies a real frozen-logit target distribution.
     * Self-selected class IDs make a deterministic, simple training problem. */
    for (size_t row = 0U; row < 4U; ++row) {
        size_t best = 0U;
        for (size_t token = 1U; token < 8U; ++token)
            if (reference[row * 8U + token] > reference[row * 8U + best])
                best = token;
        targets[row] = best;
    }
    CHECK(UmiAiTrainingDecoderLogitAdapterEvaluate(adapter,
        context, targets, 4U, &initialLoss) == UMI_STATUS_OK);
    CHECK(isfinite(initialLoss) && initialLoss > 0.0F);

    report.crossEntropyBeforeUpdate = 999.0F;
    CHECK(UmiAiTrainingDecoderLogitAdapterTrainStep(adapter,
        context, targets, 4U, NAN, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(report.crossEntropyBeforeUpdate == 999.0F);
    CHECK(UmiAiTrainingDecoderLogitAdapterLogits(adapter,
        invalidContext, 4U, adapted, 32U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderLogitAdapterLogits(adapter,
        context, 4U, adapted, 31U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingDecoderLogitAdapterInspect(adapter, &after) == UMI_STATUS_OK);
    CHECK(after.completedAdapterSteps == 0U);

    for (size_t step = 0U; step < 400U; ++step) {
        CHECK(UmiAiTrainingDecoderLogitAdapterTrainStep(adapter,
            context, targets, 4U, 0.10F, &report) == UMI_STATUS_OK);
        CHECK(isfinite(report.crossEntropyBeforeUpdate));
        CHECK(report.tokenCount == 4U &&
              report.completedAdapterSteps == (uint64_t)step + 1U);
    }
    CHECK(UmiAiTrainingDecoderLogitAdapterEvaluate(adapter,
        context, targets, 4U, &finalLoss) == UMI_STATUS_OK);
    CHECK(finalLoss < initialLoss);
    CHECK(UmiAiTrainingDecoderLogitAdapterInspect(adapter, &after) == UMI_STATUS_OK);
    CHECK(after.completedAdapterSteps == 400U);
    CHECK(after.frozenDecoderSteps == before.frozenDecoderSteps);
    CHECK(UmiAiTrainingDecoderLogits(decoder, context, 4U,
        referenceAfter, 32U) == UMI_STATUS_OK);
    CHECK(memcmp(reference, referenceAfter, sizeof(reference)) == 0);

    printf("frozen decoder CE %.6f -> %.6f; %llu adapter steps\n",
           (double)initialLoss, (double)finalLoss,
           (unsigned long long)after.completedAdapterSteps);
    UmiAiTrainingDecoderLogitAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(decoder);
    return 0;
}
