/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_low_rank.c
 * PURPOSE: Show that native low-rank adapters reduce held-in-memory teaching
 *          loss while the canonical base projection remains frozen.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/low_rank.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(value) do { \
    if (!(value)) { \
        fprintf(stderr, "Low-rank assertion %d: %s\n", __LINE__, #value); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *input = NULL, *target = NULL;
    UmiAiTrainingTensor *prediction = NULL, *loss = NULL;
    UmiAiTrainingTensor *down = NULL, *up = NULL;
    UmiAiTrainingTensor *trainables[2] = {NULL, NULL};
    UmiAiTrainingLowRankProjection projection = {0};
    const float base[4] = {1.0F, 0.0F, 0.0F, 1.0F};
    const float samples[8] = {1.0F, 0.0F, 0.0F, 1.0F,
                              1.0F, 1.0F, -1.0F, 0.5F};
    const float expected[8] = {1.4F, 0.2F, -0.25F, 1.3F,
                               1.15F, 1.5F, -1.525F, 0.45F};
    float original[4] = {0};
    float later[4] = {0};
    float baseline[8] = {0};
    float firstLoss = 0.0F, finalLoss = 0.0F;
    size_t retained = 0U;

    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 2U,
        samples, 8U, 0, &input) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 2U,
        expected, 8U, 0, &target) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingLowRankCreate(tape, base, 2U, 2U, 2U,
        UINT64_C(37), &projection) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingLowRankTrainables(&projection, &down, &up) == UMI_STATUS_OK);
    trainables[0] = down;
    trainables[1] = up;
    CHECK(UmiAiTrainingTensorValues(projection.baseWeights, original, 4U) ==
        UMI_STATUS_OK);
    CHECK(memcmp(original, base, sizeof(base)) == 0);
    CHECK(UmiAiTrainingTapeCheckpoint(tape, &retained) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingLowRankForward(tape, input, &projection,
        &prediction) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(prediction, baseline, 8U) == UMI_STATUS_OK);
    CHECK(memcmp(baseline, samples, sizeof(samples)) == 0);
    CHECK(UmiAiTrainingTapeRollback(tape, retained) == UMI_STATUS_OK);

    for (size_t epoch = 0U; epoch < 600U; ++epoch) {
        CHECK(UmiAiTrainingLowRankForward(tape, input, &projection,
            &prediction) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingMeanSquaredError(tape, prediction, target, &loss) ==
            UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorValues(loss, &finalLoss, 1U) == UMI_STATUS_OK);
        CHECK(isfinite(finalLoss));
        if (epoch == 0U) firstLoss = finalLoss;
        CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingSgdStepMany(tape, trainables, 2U, 0.12F) ==
            UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeRollback(tape, retained) == UMI_STATUS_OK);
    }
    CHECK(finalLoss < firstLoss * 0.45F);
    CHECK(UmiAiTrainingTensorValues(projection.baseWeights, later, 4U) ==
        UMI_STATUS_OK);
    CHECK(memcmp(original, later, sizeof(original)) == 0);
    CHECK(UmiAiTrainingLowRankCreate(tape, base, 2U, 2U, 0U,
        UINT64_C(99), &projection) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiTrainingTapeDestroy(tape);
    printf("Frozen projection adaptation MSE: %.6f -> %.6f\n",
           (double)firstLoss, (double)finalLoss);
    return 0;
}
