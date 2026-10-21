/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/optimizer.c
 *
 * PURPOSE:
 *   Provide deterministic learning rates and AdamW moment updates using the
 *   established, failure-atomic Framework training-tape weight writer.
 *
 * ENGINEERING:
 *   Candidate first/second moments and effective gradients are calculated
 *   without touching the model. The tape validates every candidate FP32
 *   weight before committing. Only then do new optimiser moments become live.
 *   AdamW uses decoupled weight decay; it is NOT the same as L2-regularised
 *   Adam. Current moment state is process-local and not a model checkpoint.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/optimizer.h"
#include "umicom/ai/training/gradient_accumulation.h"
#include "optimizer_internal.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Portable double constant rather than a platform-dependent M_PI definition. */
#define UMI_AI_OPTIMIZER_PI 3.14159265358979323846264338327950288

struct UmiAiTrainingAdamW {
    UmiAiTrainingTape *tape;
    UmiAiTrainingTensor **parameters;
    size_t *counts;
    size_t *offsets;
    size_t parameterCount;
    size_t totalElements;
    UmiAiTrainingAdamWConfig config;
    double *firstMoment;
    double *secondMoment;
    uint64_t completedSteps;
};

UmiStatus UmiAiTrainingLearningRateValidate(
    const UmiAiTrainingLearningRateSchedule *schedule)
{
    if (schedule == NULL ||
        (schedule->kind != UMI_AI_TRAINING_SCHEDULE_CONSTANT &&
         schedule->kind != UMI_AI_TRAINING_SCHEDULE_LINEAR &&
         schedule->kind != UMI_AI_TRAINING_SCHEDULE_COSINE) ||
        !isfinite(schedule->peakLearningRate) ||
        !isfinite(schedule->minimumLearningRate) ||
        schedule->peakLearningRate <= 0.0F ||
        schedule->minimumLearningRate <= 0.0F ||
        schedule->minimumLearningRate > schedule->peakLearningRate ||
        schedule->totalSteps == 0U ||
        schedule->warmupSteps >= schedule->totalSteps)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingLearningRateForStep(
    const UmiAiTrainingLearningRateSchedule *schedule,
    uint64_t step, float *outLearningRate)
{
    double rate;
    if (outLearningRate == NULL ||
        UmiAiTrainingLearningRateValidate(schedule) != UMI_STATUS_OK ||
        step == 0U || step > schedule->totalSteps)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (step <= schedule->warmupSteps) {
        rate = (double)schedule->peakLearningRate *
            (double)step / (double)schedule->warmupSteps;
    } else if (schedule->kind == UMI_AI_TRAINING_SCHEDULE_CONSTANT) {
        rate = (double)schedule->peakLearningRate;
    } else {
        double position = (double)(step - schedule->warmupSteps) /
                          (double)(schedule->totalSteps - schedule->warmupSteps);
        double peak = (double)schedule->peakLearningRate;
        double minimum = (double)schedule->minimumLearningRate;
        if (schedule->kind == UMI_AI_TRAINING_SCHEDULE_LINEAR) {
            rate = peak + (minimum - peak) * position;
        } else {
            rate = minimum + (peak - minimum) *
                   0.5 * (1.0 + cos(UMI_AI_OPTIMIZER_PI * position));
        }
    }
    if (!isfinite(rate) || rate <= 0.0 || rate > (double)FLT_MAX ||
        (float)rate <= 0.0F)
        return UMI_STATUS_INVALID_STATE;
    *outLearningRate = (float)rate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdamWDefaults(UmiAiTrainingAdamWConfig *outConfig)
{
    if (outConfig == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outConfig = (UmiAiTrainingAdamWConfig){0.9F, 0.999F, 1e-8F, 0.01F};
    return UMI_STATUS_OK;
}

static bool OptionsAreValid(const UmiAiTrainingAdamWConfig *config)
{
    return config != NULL &&
        isfinite(config->beta1) && config->beta1 >= 0.0F && config->beta1 < 1.0F &&
        isfinite(config->beta2) && config->beta2 >= 0.0F && config->beta2 < 1.0F &&
        isfinite(config->epsilon) && config->epsilon > 0.0F &&
        isfinite(config->weightDecay) && config->weightDecay >= 0.0F;
}

void UmiAiTrainingAdamWDestroy(UmiAiTrainingAdamW *optimizer)
{
    if (optimizer == NULL) return;
    free(optimizer->secondMoment);
    free(optimizer->firstMoment);
    free(optimizer->offsets);
    free(optimizer->counts);
    free(optimizer->parameters);
    free(optimizer);
}

UmiStatus UmiAiTrainingAdamWCreate(
    UmiAiTrainingTape *tape,
    UmiAiTrainingTensor *const *parameters,
    size_t parameterCount,
    const UmiAiTrainingAdamWConfig *config,
    UmiAiTrainingAdamW **outOptimizer)
{
    UmiAiTrainingAdamW *created;
    if (tape == NULL || parameters == NULL || outOptimizer == NULL ||
        !OptionsAreValid(config) || parameterCount == 0U ||
        parameterCount > UMI_AI_TRAINING_MAX_TENSORS)
        return UMI_STATUS_INVALID_ARGUMENT;
    created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->parameters = calloc(parameterCount, sizeof(*created->parameters));
    created->counts = calloc(parameterCount, sizeof(*created->counts));
    created->offsets = calloc(parameterCount, sizeof(*created->offsets));
    if (created->parameters == NULL || created->counts == NULL ||
        created->offsets == NULL) {
        UmiAiTrainingAdamWDestroy(created);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t index = 0U; index < parameterCount; ++index) {
        size_t rows = 0U, columns = 0U;
        if (parameters[index] == NULL) {
            UmiAiTrainingAdamWDestroy(created);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        for (size_t previous = 0U; previous < index; ++previous) {
            if (parameters[index] == parameters[previous]) {
                UmiAiTrainingAdamWDestroy(created);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
        }
        UmiStatus status = UmiAiTrainingTensorShape(
            parameters[index], &rows, &columns);
        if (status != UMI_STATUS_OK || columns == 0U ||
            rows > SIZE_MAX / columns ||
            rows * columns > SIZE_MAX - created->totalElements) {
            UmiAiTrainingAdamWDestroy(created);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        created->offsets[index] = created->totalElements;
        created->counts[index] = rows * columns;
        created->totalElements += rows * columns;
        created->parameters[index] = parameters[index];
    }
    if (created->totalElements == 0U ||
        created->totalElements > SIZE_MAX / sizeof(double)) {
        UmiAiTrainingAdamWDestroy(created);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    created->firstMoment = calloc(created->totalElements, sizeof(double));
    created->secondMoment = calloc(created->totalElements, sizeof(double));
    if (created->firstMoment == NULL || created->secondMoment == NULL) {
        UmiAiTrainingAdamWDestroy(created);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    created->tape = tape;
    created->parameterCount = parameterCount;
    created->config = *config;
    *outOptimizer = created;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdamWInspect(
    const UmiAiTrainingAdamW *optimizer,
    uint64_t *outCompletedSteps, size_t *outStateBytes)
{
    if (optimizer == NULL || outCompletedSteps == NULL || outStateBytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (optimizer->totalElements > (SIZE_MAX - sizeof(*optimizer) -
        optimizer->parameterCount * (sizeof(*optimizer->parameters) +
        sizeof(*optimizer->counts) + sizeof(*optimizer->offsets))) /
        (2U * sizeof(double))) return UMI_STATUS_INTERNAL_ERROR;
    *outCompletedSteps = optimizer->completedSteps;
    *outStateBytes = sizeof(*optimizer) +
        optimizer->parameterCount * (sizeof(*optimizer->parameters) +
        sizeof(*optimizer->counts) + sizeof(*optimizer->offsets)) +
        optimizer->totalElements * 2U * sizeof(double);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdamWValidateBinding(
    const UmiAiTrainingAdamW *optimizer,
    const UmiAiTrainingTape *tape,
    UmiAiTrainingTensor *const *parameters,
    size_t count,
    uint64_t expectedSteps)
{
    if (optimizer == NULL || tape == NULL || parameters == NULL ||
        optimizer->tape != tape || optimizer->parameterCount != count ||
        optimizer->completedSteps != expectedSteps)
        return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < count; ++index) {
        if (optimizer->parameters[index] != parameters[index])
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdamWStepWithGradients(
    UmiAiTrainingAdamW *optimizer,
    const float *const *gradients,
    float learningRate)
{
    double *candidateMoments = NULL, *first, *second;
    float *effective = NULL, *weights = NULL;
    const float *provided[UMI_AI_TRAINING_MAX_TENSORS] = {0};
    double firstCorrection, secondCorrection;
    UmiStatus status = UMI_STATUS_OK;
    size_t count;
    if (optimizer == NULL || gradients == NULL ||
        !isfinite(learningRate) || learningRate <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (optimizer->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    count = optimizer->totalElements;
    if (count > SIZE_MAX / (2U * sizeof(double)) ||
        count > SIZE_MAX / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidateMoments = malloc(count * 2U * sizeof(double));
    effective = malloc(count * sizeof(float));
    weights = malloc(count * sizeof(float));
    if (candidateMoments == NULL || effective == NULL || weights == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto complete;
    }
    first = candidateMoments;
    second = candidateMoments + count;
    firstCorrection = 1.0 - pow((double)optimizer->config.beta1,
                                 (double)(optimizer->completedSteps + UINT64_C(1)));
    secondCorrection = 1.0 - pow((double)optimizer->config.beta2,
                                  (double)(optimizer->completedSteps + UINT64_C(1)));
    if (firstCorrection <= 0.0 || secondCorrection <= 0.0) {
        status = UMI_STATUS_INVALID_STATE;
        goto complete;
    }
    for (size_t parameter = 0U; parameter < optimizer->parameterCount; ++parameter) {
        size_t offset = optimizer->offsets[parameter];
        size_t width = optimizer->counts[parameter];
        if (gradients[parameter] == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            goto complete;
        }
        status = UmiAiTrainingTensorValues(optimizer->parameters[parameter],
                                         &weights[offset], width);
        if (status != UMI_STATUS_OK) goto complete;
        provided[parameter] = &effective[offset];
        for (size_t i = 0U; i < width; ++i) {
            const size_t at = offset + i;
            double gradient = (double)gradients[parameter][i];
            double firstValue, secondValue, corrected, decay;
            if (!isfinite(gradient) || !isfinite(weights[at])) {
                status = UMI_STATUS_INVALID_ARGUMENT;
                goto complete;
            }
            firstValue = (double)optimizer->config.beta1 * optimizer->firstMoment[at] +
                         (1.0 - (double)optimizer->config.beta1) * gradient;
            secondValue = (double)optimizer->config.beta2 * optimizer->secondMoment[at] +
                         (1.0 - (double)optimizer->config.beta2) * gradient * gradient;
            corrected = (firstValue / firstCorrection) /
                (sqrt(secondValue / secondCorrection) +
                 (double)optimizer->config.epsilon);
            decay = (double)optimizer->config.weightDecay * (double)weights[at];
            if (!isfinite(firstValue) || !isfinite(secondValue) ||
                !isfinite(corrected) || !isfinite(decay) ||
                !isfinite(corrected + decay) ||
                fabs(corrected + decay) > (double)FLT_MAX) {
                status = UMI_STATUS_INVALID_ARGUMENT;
                goto complete;
            }
            first[at] = firstValue;
            second[at] = secondValue;
            effective[at] = (float)(corrected + decay);
        }
    }
    /* The Framework tape remains the sole authority for writing weights.
     * Rejected candidates do not change either weights or moment state. */
    status = UmiAiTrainingSgdStepWithGradients(optimizer->tape,
        optimizer->parameters, provided, optimizer->parameterCount,
        learningRate);
    if (status != UMI_STATUS_OK) goto complete;
    memcpy(optimizer->firstMoment, first, count * sizeof(double));
    memcpy(optimizer->secondMoment, second, count * sizeof(double));
    ++optimizer->completedSteps;
complete:
    free(weights);
    free(effective);
    free(candidateMoments);
    return status;
}

UmiStatus UmiAiTrainingAdamWStep(
    UmiAiTrainingAdamW *optimizer, float learningRate)
{
    float *copied = NULL;
    const float *arrays[UMI_AI_TRAINING_MAX_TENSORS] = {0};
    UmiStatus status = UMI_STATUS_OK;
    if (optimizer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (optimizer->totalElements > SIZE_MAX / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    copied = malloc(optimizer->totalElements * sizeof(float));
    if (copied == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t p = 0U; p < optimizer->parameterCount; ++p) {
        arrays[p] = &copied[optimizer->offsets[p]];
        status = UmiAiTrainingTensorGradients(optimizer->parameters[p],
                        &copied[optimizer->offsets[p]], optimizer->counts[p]);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingAdamWStepWithGradients(optimizer, arrays, learningRate);
    free(copied);
    return status;
}

/* Read-only checkpoint inspection permits a portable, bounded codec without
 * moving the private optimiser structure into a public ABI surface. */
UmiStatus UmiAiTrainingAdamWInternalInspect(
    const UmiAiTrainingAdamW *optimizer,
    UmiAiTrainingAdamWConfig *outConfig,
    uint64_t *outSteps, size_t *outParameterCount, size_t *outElements,
    const size_t **outCounts, const double **outFirst,
    const double **outSecond)
{
    if (optimizer == NULL || outConfig == NULL || outSteps == NULL ||
        outParameterCount == NULL || outElements == NULL ||
        outCounts == NULL || outFirst == NULL || outSecond == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outConfig = optimizer->config;
    *outSteps = optimizer->completedSteps;
    *outParameterCount = optimizer->parameterCount;
    *outElements = optimizer->totalElements;
    *outCounts = optimizer->counts;
    *outFirst = optimizer->firstMoment;
    *outSecond = optimizer->secondMoment;
    return UMI_STATUS_OK;
}

/* Publish restored moments only after validating EVERY element. No existing
 * model weight, tape graph or optimiser field is changed on rejection. */
UmiStatus UmiAiTrainingAdamWInternalRestore(
    UmiAiTrainingAdamW *optimizer, const double *first,
    const double *second, size_t elementCount, uint64_t completedSteps)
{
    if (optimizer == NULL || first == NULL || second == NULL ||
        elementCount != optimizer->totalElements ||
        optimizer->completedSteps != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < elementCount; ++index) {
        if (!isfinite(first[index]) || !isfinite(second[index]) ||
            second[index] < 0.0 ||
            (completedSteps == 0U &&
             (first[index] != 0.0 || second[index] != 0.0)))
            return UMI_STATUS_PARSE_ERROR;
    }
    memcpy(optimizer->firstMoment, first, elementCount * sizeof(double));
    memcpy(optimizer->secondMoment, second, elementCount * sizeof(double));
    optimizer->completedSteps = completedSteps;
    return UMI_STATUS_OK;
}
