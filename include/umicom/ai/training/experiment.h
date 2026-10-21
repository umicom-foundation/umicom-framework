/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/experiment.h
 *
 * PURPOSE:
 *   Define a reproducible, bounded native C23 decoder-training experiment.
 *   Share training, held-out evaluation, progress and provenance policy across
 *   Umicom LLM, Studio and the educational command-line laboratory.
 *
 * ARCHITECTURE:
 *   The Framework's existing decoder, dataset and checkpoint services own
 *   their own implementations. This layer composes them; it does not create a
 *   second mathematical engine or copy the dataset. The caller retains the
 *   immutable dataset while Run executes. Training is synchronous, single-
 *   threaded, and a model is published only on complete success.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_EXPERIMENT_H
#define UMICOM_AI_TRAINING_EXPERIMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/optimizer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_EXPERIMENT_MAX_STEPS 512U
#define UMI_AI_EXPERIMENT_MAX_EVALUATION_BATCHES 32U

typedef struct UmiAiTrainingExperimentConfig {
    UmiAiTrainingDecoderConfig decoder;
    uint32_t steps;
    uint32_t evalInterval;
    uint32_t evalBatches;
    uint32_t batchSequences;
    float learningRate;
    uint64_t trainSeed;
    uint64_t evalSeed;
} UmiAiTrainingExperimentConfig;

/* A snapshot of sampled held-out metrics. Perplexity is exp(mean cross-entropy)
 * measured on the same fixed evaluation windows at every observation. This is
 * NOT full-corpus evaluation; it measures neither task correctness nor general
 * language ability. `hasTrainingLoss` is zero at step 0 (initial baseline).
 * All scalar outputs are copied and remain valid after the callback returns. */
typedef struct UmiAiTrainingExperimentProgress {
    uint64_t completedSteps;
    double heldOutCrossEntropy;
    double heldOutPerplexity;
    float lastTrainingLoss;
    int hasTrainingLoss;
} UmiAiTrainingExperimentProgress;

typedef struct UmiAiTrainingExperimentResult {
    UmiAiTrainingDatasetSummary dataset;
    uint64_t completedSteps;
    uint32_t evaluationCount;
    double baselineHeldOutLoss;
    double finalHeldOutLoss;
    double bestObservedHeldOutLoss;
    uint64_t bestObservedStep;
    float lastTrainingLoss;
} UmiAiTrainingExperimentResult;

/* Observers may return UMI_STATUS_CANCELLED to stop. Any non-OK callback
 * result aborts the run, frees its unpublished decoder, and is returned to
 * the caller. Callbacks must not mutate the borrowed dataset or recursively
 * access the internal decoder. Their own external side effects cannot roll
 * back; the Framework never retains their borrowed userData pointer. */
typedef UmiStatus (*UmiAiTrainingExperimentObserver)(
    const UmiAiTrainingExperimentProgress *progress, void *userData);

/* Initialise a deterministic educational default plan. Does not allocate. */
UmiStatus UmiAiTrainingExperimentDefaults(
    UmiAiTrainingExperimentConfig *outConfig);

/* Validate dimensions, bounded element counts and numeric settings BEFORE
 * any model or source data is accessed. Configuration may still fail when a
 * decoder rejects a more specific parameter combination. */
UmiStatus UmiAiTrainingExperimentValidate(
    const UmiAiTrainingExperimentConfig *config);

/* Repeated, identical held-out evaluation windows, independently seeded from
 * training windows. Neither the dataset nor decoder weights are changed.
 * No output is modified on error. */
UmiStatus UmiAiTrainingExperimentEvaluate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    double *outMeanLoss, double *outPerplexity);

/* Create a FROM-SCRATCH model, report an initial held-out baseline, then
 * train on fixed-seed batches and evaluate at each evalInterval and at the
 * final step. Checkpoint publication is deliberately a separate approved
 * caller action using the existing Framework checkpoint owner.
 *
 * On success, outModel receives a new owned decoder; destroy it exactly once.
 * On any failure both outModel and outResult remain unchanged. When callbacks
 * request cancellation, no partially trained model escapes. */
UmiStatus UmiAiTrainingExperimentRun(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult);

/* Continue an existing model to the plan's TOTAL steps without mutating the
 * input snapshot. The Framework clones it through its existing versioned
 * checkpoint codec before training. The input checkpoint must have exactly
 * the model architecture and initialisation seed selected by the plan; its
 * completed steps must not exceed config->steps. Sampler identity is defined
 * by trainSeed and the absolute optimiser-step index, so an uninterrupted run
 * and an interrupted-and-resumed run are byte-identical when ALL plan fields
 * and training source bytes are unchanged. The checkpoint format DOES NOT
 * bind a source manifest or learning rate: callers must verify these before
 * approving a resume. Baseline metrics refer to the resume point. */
UmiStatus UmiAiTrainingExperimentResume(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingDecoder *checkpointModel,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult);

/* An opt-in deterministic learning-rate schedule for existing SGD runs.
 * The original Run/Resume API remains constant-rate and ABI compatible.
 * A resumed run must retain the original schedule horizon, seed and source
 * data; the weights-only checkpoint does not authenticate any of them. */
UmiStatus UmiAiTrainingExperimentRunScheduled(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingLearningRateSchedule *schedule,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult);
UmiStatus UmiAiTrainingExperimentResumeScheduled(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_EXPERIMENT_H */
