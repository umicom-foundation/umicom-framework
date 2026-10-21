/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/optimizer.h
 *
 * PURPOSE:
 *   Deterministic learning-rate schedules and stateful decoupled-weight-decay
 *   AdamW optimisation for native neural-network training.
 *
 * ARCHITECTURE AND OWNERSHIP:
 *   The existing training tape remains the only writer of trained parameters.
 *   AdamW owns its first and second moments and borrows tape/parameter handles.
 *   A decoder must outlive its optimiser. Calls are caller-serialised.
 *   Invalid operations preserve parameter values, optimiser state and outputs.
 *   A decoder-only checkpoint does not contain AdamW moments: it CANNOT
 *   provide exact AdamW training continuation without saved optimiser state.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_OPTIMIZER_H
#define UMICOM_AI_TRAINING_OPTIMIZER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/tape.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiAiTrainingScheduleKind {
    UMI_AI_TRAINING_SCHEDULE_CONSTANT = 1,
    UMI_AI_TRAINING_SCHEDULE_LINEAR = 2,
    UMI_AI_TRAINING_SCHEDULE_COSINE = 3
} UmiAiTrainingScheduleKind;

typedef struct UmiAiTrainingLearningRateSchedule {
    UmiAiTrainingScheduleKind kind;
    float peakLearningRate;
    float minimumLearningRate;
    uint64_t warmupSteps;
    uint64_t totalSteps;
} UmiAiTrainingLearningRateSchedule;

/* A schedule is evaluated at an ABSOLUTE, one-based optimiser step number.
 * An interrupted run must retain the original totalSteps and warmup settings.
 * This service never reads clock time or random state. */
UmiStatus UmiAiTrainingLearningRateValidate(
    const UmiAiTrainingLearningRateSchedule *schedule);
UmiStatus UmiAiTrainingLearningRateForStep(
    const UmiAiTrainingLearningRateSchedule *schedule,
    uint64_t step, float *outLearningRate);

typedef struct UmiAiTrainingAdamWConfig {
    float beta1;
    float beta2;
    float epsilon;
    float weightDecay;
} UmiAiTrainingAdamWConfig;

typedef struct UmiAiTrainingAdamW UmiAiTrainingAdamW;

/* Default AdamW hyperparameters; no allocations or model mutations. */
UmiStatus UmiAiTrainingAdamWDefaults(UmiAiTrainingAdamWConfig *outConfig);

/* Create an independent, initially empty moment state for borrowed trainable
 * leaves. The tape must remain alive and its parameter handles must not be
 * destroyed or replaced until the optimiser is destroyed. A failed creation
 * leaves the caller's output pointer unchanged. */
UmiStatus UmiAiTrainingAdamWCreate(
    UmiAiTrainingTape *tape,
    UmiAiTrainingTensor *const *parameters,
    size_t parameterCount,
    const UmiAiTrainingAdamWConfig *config,
    UmiAiTrainingAdamW **outOptimizer);
void UmiAiTrainingAdamWDestroy(UmiAiTrainingAdamW *optimizer);
UmiStatus UmiAiTrainingAdamWInspect(
    const UmiAiTrainingAdamW *optimizer,
    uint64_t *outCompletedSteps, size_t *outStateBytes);

/* Apply supplied gradients through the canonical tape's transactional update
 * operation. AdamW moments are committed only after ALL weight checks pass.
 * The caller supplies one array of finite gradients per parameter; array
 * lengths equal the corresponding tensor element counts. The tape must have
 * been rewound to its leaf-only parameter prefix. */
UmiStatus UmiAiTrainingAdamWStepWithGradients(
    UmiAiTrainingAdamW *optimizer,
    const float *const *gradients,
    float learningRate);

/* Convenience overload: read currently retained tensor gradients and apply.
 * Graph results must be rewound first. */
UmiStatus UmiAiTrainingAdamWStep(
    UmiAiTrainingAdamW *optimizer, float learningRate);

/* Decoder-facing wrappers reuse its canonical forward/loss/backward pipeline;
 * no alternative transformer or optimiser maths is defined by applications.
 * AdamW model training begins at optimiser step zero. Because the portable
 * model checkpoint stores weights but not moments, creating a NEW AdamW
 * optimiser on a previously stepped decoder is intentionally rejected. */
UmiStatus UmiAiTrainingDecoderAdamWCreate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamWConfig *config,
    UmiAiTrainingAdamW **outOptimizer);
UmiStatus UmiAiTrainingDecoderTrainAdamWStep(
    UmiAiTrainingDecoder *model,
    UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    float learningRate,
    float *outLoss);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_OPTIMIZER_H */
