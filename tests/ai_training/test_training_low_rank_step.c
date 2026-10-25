/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_low_rank_step.c
 * PURPOSE: Verify adapter-only training reduces a supervised loss, retains
 *          graph capacity, and never modifies a frozen base projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/low_rank_training.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "low-rank step line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *input = NULL, *target = NULL;
    UmiAiTrainingTensor *prediction = NULL, *loss = NULL;
    UmiAiTrainingLowRankProjection projection = {0};
    UmiAiTrainingLowRankStepReport report = {0};
    const float base[4] = {1.0F, 0.0F, 0.0F, 1.0F};
    const float samples[8] = {1.0F, 0.0F, 0.0F, 1.0F,
                              1.0F, 1.0F, -1.0F, 0.5F};
    const float desired[8] = {1.4F, 0.2F, -0.25F, 1.3F,
                              1.15F, 1.5F, -1.525F, 0.45F};
    float frozenBefore[4] = {0}, frozenAfter[4] = {0};
    float originalLoss = 0.0F, trainedLoss = 0.0F;
    size_t leafCount = 0U, afterCount = 0U;

    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 2U, samples, 8U, 0,
                                    &input) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 2U, desired, 8U, 0,
                                    &target) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingLowRankCreate(tape, base, 2U, 2U, 2U,
                                    UINT64_C(37), &projection) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(projection.baseWeights,
                                    frozenBefore, 4U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(tape, &leafCount) == UMI_STATUS_OK);

    report.lossBeforeUpdate = 456.0F;
    CHECK(UmiAiTrainingLowRankTrainStep(tape, input, target, &projection,
          NAN, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(report.lossBeforeUpdate == 456.0F);
    CHECK(UmiAiTrainingTapeCheckpoint(tape, &afterCount) == UMI_STATUS_OK);
    CHECK(afterCount == leafCount);

    for (size_t step = 0U; step < 500U; ++step) {
        CHECK(UmiAiTrainingLowRankTrainStep(tape, input, target, &projection,
              0.12F, &report) == UMI_STATUS_OK);
        CHECK(isfinite(report.lossBeforeUpdate));
        CHECK(report.exampleCount == 4U && report.outputChannels == 2U);
        if (step == 0U) originalLoss = report.lossBeforeUpdate;
        CHECK(UmiAiTrainingTapeCheckpoint(tape, &afterCount) == UMI_STATUS_OK);
        CHECK(afterCount == leafCount);
    }

    CHECK(UmiAiTrainingLowRankForward(tape, input, &projection,
                                      &prediction) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMeanSquaredError(tape, prediction, target,
                                        &loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(loss, &trainedLoss, 1U) == UMI_STATUS_OK);
    CHECK(trainedLoss < originalLoss * 0.50F);
    CHECK(UmiAiTrainingTapeRollback(tape, leafCount) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(projection.baseWeights,
                                    frozenAfter, 4U) == UMI_STATUS_OK);
    CHECK(memcmp(frozenBefore, frozenAfter, sizeof(frozenBefore)) == 0);

    UmiAiTrainingTapeDestroy(tape);
    printf("Low-rank supervised loss %.6f -> %.6f; base unchanged\n",
           (double)originalLoss, (double)trainedLoss);
    return 0;
}
