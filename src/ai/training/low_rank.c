/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/low_rank.c
 * PURPOSE:
 *   Reuse the native training tape to express a frozen dense projection plus
 *   two trainable low-rank matrices with deterministic initialisation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/low_rank.h"
#include "umicom/ai/training/initialisation.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static int LowRankShapeValid(size_t rows, size_t columns)
{
    return rows > 0U && columns > 0U &&
           columns <= SIZE_MAX / rows &&
           rows * columns <= UMI_AI_TRAINING_MAX_ELEMENTS;
}

UmiStatus UmiAiTrainingLowRankCreate(
    UmiAiTrainingTape *tape,
    const float *baseWeights,
    size_t inputChannels, size_t outputChannels, size_t rank,
    uint64_t initialSeed,
    UmiAiTrainingLowRankProjection *outProjection)
{
    UmiAiTrainingLowRankProjection candidate = {0};
    UmiAiTrainingRandomState random = {0};
    float *downValues = NULL;
    float *upValues = NULL;
    UmiStatus status;
    size_t checkpoint = 0U;
    size_t downCount, upCount;
    float limit;

    if (tape == NULL || baseWeights == NULL || outProjection == NULL ||
        !LowRankShapeValid(inputChannels, outputChannels) ||
        !LowRankShapeValid(inputChannels, rank) ||
        !LowRankShapeValid(rank, outputChannels))
        return UMI_STATUS_INVALID_ARGUMENT;

    downCount = inputChannels * rank;
    upCount = rank * outputChannels;
    downValues = malloc(downCount * sizeof(*downValues));
    upValues = calloc(upCount, sizeof(*upValues));
    if (downValues == NULL || upValues == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    status = UmiAiTrainingRandomSeed(&random, initialSeed);
    if (status != UMI_STATUS_OK) goto cleanup;
    limit = 1.0F / sqrtf((float)inputChannels);
    if (!isfinite(limit) || limit <= 0.0F) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto cleanup;
    }
    status = UmiAiTrainingRandomFillUniform(
        &random, -limit, limit, downValues, downCount);
    if (status != UMI_STATUS_OK) goto cleanup;

    status = UmiAiTrainingTapeCheckpoint(tape, &checkpoint);
    if (status != UMI_STATUS_OK) goto cleanup;

    status = UmiAiTrainingTensorCreate(tape, inputChannels, outputChannels,
        baseWeights, inputChannels * outputChannels, 0,
        &candidate.baseWeights);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingTensorCreate(tape, inputChannels, rank,
        downValues, downCount, 1, &candidate.adapterDown);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingTensorCreate(tape, rank, outputChannels,
        upValues, upCount, 1, &candidate.adapterUp);
    if (status != UMI_STATUS_OK) goto rollback;

    candidate.inputChannels = inputChannels;
    candidate.outputChannels = outputChannels;
    candidate.rank = rank;
    *outProjection = candidate;
    status = UMI_STATUS_OK;
    goto cleanup;

rollback:
    /* The tape owns leaf lifetime: remove only the suffix created here. */
    (void)UmiAiTrainingTapeRollback(tape, checkpoint);
cleanup:
    free(upValues);
    free(downValues);
    return status;
}

UmiStatus UmiAiTrainingLowRankForward(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingLowRankProjection *projection,
    UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *baseline = NULL;
    UmiAiTrainingTensor *hidden = NULL;
    UmiAiTrainingTensor *delta = NULL;
    UmiAiTrainingTensor *result = NULL;
    size_t batch = 0U, width = 0U;
    size_t rows = 0U, columns = 0U;
    size_t checkpoint = 0U;
    UmiStatus status;

    if (tape == NULL || input == NULL || projection == NULL || outTensor == NULL ||
        projection->baseWeights == NULL || projection->adapterDown == NULL ||
        projection->adapterUp == NULL ||
        !LowRankShapeValid(projection->inputChannels, projection->outputChannels) ||
        !LowRankShapeValid(projection->inputChannels, projection->rank) ||
        !LowRankShapeValid(projection->rank, projection->outputChannels))
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingTensorShape(input, &batch, &width);
    if (status != UMI_STATUS_OK || width != projection->inputChannels ||
        !LowRankShapeValid(batch, projection->outputChannels) ||
        !LowRankShapeValid(batch, projection->rank))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(projection->baseWeights, &rows, &columns);
    if (status != UMI_STATUS_OK || rows != projection->inputChannels ||
        columns != projection->outputChannels)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(projection->adapterDown, &rows, &columns);
    if (status != UMI_STATUS_OK || rows != projection->inputChannels ||
        columns != projection->rank)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(projection->adapterUp, &rows, &columns);
    if (status != UMI_STATUS_OK || rows != projection->rank ||
        columns != projection->outputChannels)
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingTapeCheckpoint(tape, &checkpoint);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingMatmul(tape, input, projection->baseWeights, &baseline);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingMatmul(tape, input, projection->adapterDown, &hidden);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingMatmul(tape, hidden, projection->adapterUp, &delta);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingAdd(tape, baseline, delta, &result);
    if (status != UMI_STATUS_OK) goto rollback;
    *outTensor = result;
    return UMI_STATUS_OK;

rollback:
    (void)UmiAiTrainingTapeRollback(tape, checkpoint);
    return status;
}

UmiStatus UmiAiTrainingLowRankTrainables(
    const UmiAiTrainingLowRankProjection *projection,
    UmiAiTrainingTensor **outDown,
    UmiAiTrainingTensor **outUp)
{
    if (projection == NULL || outDown == NULL || outUp == NULL ||
        outDown == outUp || projection->adapterDown == NULL ||
        projection->adapterUp == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outDown = projection->adapterDown;
    *outUp = projection->adapterUp;
    return UMI_STATUS_OK;
}
