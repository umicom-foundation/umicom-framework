/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/experiment_adamw.h
 *
 * PURPOSE:
 *   Run repeatable AdamW source-training experiments with absolute-step
 *   learning-rate schedules and exact model-plus-optimiser continuation.
 *
 * ARCHITECTURE:
 *   Reuse the existing immutable source sampler, canonical decoder, AdamW
 *   optimiser, held-out evaluator and combined checkpoint codec. A successful
 *   run transfers two owned handles. On error or observer cancellation no
 *   partially trained model or optimiser is published. The caller destroys
 *   the optimiser BEFORE destroying its associated decoder.
 *
 * SECURITY AND PROVENANCE:
 *   Checkpoints preserve weights and optimiser moments, NOT the source
 *   manifest, source contents, scheduler or the complete experiment plan.
 *   Callers MUST verify those independently before claiming exact resume.
 *   This module never runs source files or communicates over a network.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_EXPERIMENT_ADAMW_H
#define UMICOM_AI_TRAINING_EXPERIMENT_ADAMW_H

#include "umicom/ai/training/experiment.h"
#include "umicom/ai/training/optimizer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Validate the same bounded decoder/dataset training shapes as the existing
 * experiment service, an approved AdamW configuration and an absolute-step
 * schedule covering every requested step. Legacy config.learningRate must
 * agree with schedule.peakLearningRate to avoid ambiguous plan settings.
 * The original single-batch training contract is retained: accumulated AdamW
 * microbatches require a separate explicitly qualified implementation. */
UmiStatus UmiAiTrainingExperimentAdamWValidate(
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule);

/* Train a new model from its configured seed. Observers see the initial
 * held-out baseline and each scheduled evaluation, just as in the canonical
 * SGD experiment. Results and both output handles remain unchanged on error.
 * The schedule can extend beyond config.steps to permit deliberate checkpoint
 * publication at an intermediate step without changing later learning rates. */
UmiStatus UmiAiTrainingExperimentRunAdamW(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult);

/* Resume only from a bound decoder+AdamW pair originating from the combined
 * checkpoint format. Clone BOTH through that codec before training; a failure
 * or observer cancellation cannot mutate the supplied checkpoint handles.
 * The optimiser options, model architecture and exact completed step count
 * must match the supplied experiment and schedule. The caller is responsible
 * for separately verifying dataset content, seeds and schedule identity. */
UmiStatus UmiAiTrainingExperimentResumeAdamW(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingAdamW *checkpointOptimizer,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult);

/* Opt-in effective-batch AdamW experiments. The approved dataset sampler
 * still owns every source window; each step processes its configured
 * batchSequences in chunks of 1..batchSequences. Exactly one optimiser update
 * occurs per effective batch. Existing direct-batch RunAdamW APIs are intact.
 * For byte-identical resume, the source bytes, seeds, microbatch count, AdamW
 * options and absolute learning-rate schedule must be unchanged. */
UmiStatus UmiAiTrainingExperimentRunAdamWAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    size_t microbatchSequences,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult);

UmiStatus UmiAiTrainingExperimentResumeAdamWAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingAdamW *checkpointOptimizer,
    size_t microbatchSequences,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult);

/* New opt-in clipped AdamW entry points. These preserve the established
 * deterministic sampler, absolute-step schedule, held-out evaluator and
 * combined checkpoint format. Exactly one global L2 clipping decision is made
 * AFTER token-weighted microbatch reduction, BEFORE each AdamW update.
 *
 * `microbatchSequences` is in [1, config->batchSequences]. A full batch can
 * be represented by passing config->batchSequences; unlike legacy callers,
 * clipped callers always use the canonical accumulation implementation.
 * All results are committed together after success. If no update is needed
 * (resume already at config.steps), outLastClipReport is zero-initialised.
 *
 * These APIs do NOT validate external provenance: for a file-based resume,
 * check the separate immutable policy companion record against the exact
 * checkpoint bytes before invoking them. */
UmiStatus UmiAiTrainingExperimentRunAdamWClipped(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    size_t microbatchSequences, float maximumGradientNorm,
    UmiAiTrainingExperimentObserver observer, void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult,
    UmiAiTrainingGradientClipReport *outLastClipReport);

UmiStatus UmiAiTrainingExperimentResumeAdamWClipped(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingAdamW *checkpointOptimizer,
    size_t microbatchSequences, float maximumGradientNorm,
    UmiAiTrainingExperimentObserver observer, void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult,
    UmiAiTrainingGradientClipReport *outLastClipReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_EXPERIMENT_ADAMW_H */
