/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/low_rank_training.c
 * PURPOSE:
 *   Coordinate one bounded low-rank supervised update without introducing
 *   another tensor, optimiser or neural-network implementation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/low_rank_training.h"

#include <math.h>

UmiStatus UmiAiTrainingLowRankTrainStep(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTensor *target,
    const UmiAiTrainingLowRankProjection *projection,
    float learningRate,
    UmiAiTrainingLowRankStepReport *outReport)
{
    UmiAiTrainingTensor *predicted = NULL;
    UmiAiTrainingTensor *loss = NULL;
    UmiAiTrainingTensor *down = NULL;
    UmiAiTrainingTensor *up = NULL;
    UmiAiTrainingTensor *trainable[2] = {NULL, NULL};
    UmiAiTrainingLowRankStepReport candidate = {0};
    size_t inputRows = 0U, inputColumns = 0U;
    size_t targetRows = 0U, targetColumns = 0U;
    size_t retainedNodes = 0U;
    UmiStatus status;
    UmiStatus cleanupStatus;

    if (tape == NULL || input == NULL || target == NULL ||
        projection == NULL || outReport == NULL ||
        !isfinite(learningRate) || learningRate <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingTensorShape(input, &inputRows, &inputColumns);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorShape(target, &targetRows, &targetColumns);
    if (status != UMI_STATUS_OK) return status;
    if (inputRows == 0U || inputRows != targetRows ||
        inputColumns != projection->inputChannels ||
        targetColumns != projection->outputChannels)
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingLowRankTrainables(projection, &down, &up);
    if (status != UMI_STATUS_OK) return status;
    trainable[0] = down;
    trainable[1] = up;

    status = UmiAiTrainingTapeCheckpoint(tape, &retainedNodes);
    if (status != UMI_STATUS_OK) return status;

    status = UmiAiTrainingLowRankForward(tape, input, projection, &predicted);
    if (status != UMI_STATUS_OK) goto restore;
    status = UmiAiTrainingMeanSquaredError(tape, predicted, target, &loss);
    if (status != UMI_STATUS_OK) goto restore;
    status = UmiAiTrainingTensorValues(loss, &candidate.lossBeforeUpdate, 1U);
    if (status != UMI_STATUS_OK) goto restore;
    if (!isfinite(candidate.lossBeforeUpdate) || candidate.lossBeforeUpdate < 0.0F) {
        status = UMI_STATUS_INVALID_STATE;
        goto restore;
    }
    status = UmiAiTrainingBackward(tape, loss);
    if (status != UMI_STATUS_OK) goto restore;

    /* SgdStepMany checks BOTH candidate matrices before updating either.
     * The base parameter is intentionally excluded from this transaction. */
    status = UmiAiTrainingSgdStepMany(tape, trainable, 2U, learningRate);
    if (status != UMI_STATUS_OK) goto restore;
    candidate.exampleCount = inputRows;
    candidate.outputChannels = targetColumns;

restore:
    /* Remove only graph nodes introduced by this step. The retained input,
     * target, base and adapter leaf tensors stay alive for the next update. */
    cleanupStatus = UmiAiTrainingTapeRollback(tape, retainedNodes);
    if (cleanupStatus != UMI_STATUS_OK)
        return cleanupStatus;
    if (status == UMI_STATUS_OK)
        *outReport = candidate;
    return status;
}
