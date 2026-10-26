/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_experiment.h
 * PURPOSE: Native, bounded, source-group-aware training and held-out
 *          evaluation of the new stacked decoder, with repeatable ordering.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_EXPERIMENT_H
#define UMICOM_AI_TRAINING_STACKED_EXPERIMENT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_decoder.h"
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/base/sha256.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_STACKED_MAX_EXAMPLES 64U
#define UMI_AI_STACKED_MAX_EPOCHS 16U

typedef struct UmiAiTrainingStackedExample {
    const size_t *inputIds;
    const size_t *targetIds;
    const uint8_t *lossMask;
    size_t tokenCount;
    UmiAiTrainingPartition partition;
    uint8_t originalSourceSha256[UMI_SHA256_BYTES];
} UmiAiTrainingStackedExample;

typedef struct UmiAiTrainingStackedExperimentPlan {
    size_t epochs;
    float learningRate;
    uint64_t shuffleSeed;
} UmiAiTrainingStackedExperimentPlan;

typedef struct UmiAiTrainingStackedExperimentReport {
    size_t trainingExamples;
    size_t evaluationExamples;
    size_t epochsCompleted;
    uint64_t completedTrainingSteps;
    uint64_t supervisedTrainingTokens;
    double heldoutLossBefore;
    double heldoutLossAfter;
    double epochTrainingLoss[UMI_AI_STACKED_MAX_EPOCHS];
    double epochHeldoutLoss[UMI_AI_STACKED_MAX_EPOCHS];
    uint8_t orderedTrainingSourceSha256[UMI_SHA256_BYTES];
} UmiAiTrainingStackedExperimentReport;

/* The caller must have used the Framework's source-admission and reviewed
 * corpus services BEFORE constructing these token windows. A digest supplied
 * here alone is NOT evidence of permission to train. Same original source
 * cannot occur in both partitions. Preflight rejects invalid masks, source
 * digests, dimensions, empty partitions and repeated exact windows.
 * These APIs do NOT access files, devices, networks or application state.
 * A rejected preflight never modifies the model or output. */
UmiStatus UmiAiTrainingStackedExperimentValidate(
    const UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedExample *examples, size_t count,
    const UmiAiTrainingStackedExperimentPlan *plan);

/* Train with the existing Framework RNG's deterministic permutation each
 * epoch. Evaluate the held-out partition BEFORE training and at every epoch.
 * Mean loss across examples is weighted by selected TARGET counts, never
 * averaged indiscriminately over records. No held-out example is trained.
 *
 * A successful call publishes one complete aggregate report. Rejected
 * configuration leaves weights and outReport untouched. An unexpected error
 * after training started can leave already completed SGD steps committed:
 * the entire multi-epoch run is NOT a transaction. Save a trusted Framework
 * checkpoint before running if application-level rollback is required.
 * This does not implement AdamW, gradient accumulation or automatic release. */
UmiStatus UmiAiTrainingStackedExperimentRun(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedExample *examples, size_t count,
    const UmiAiTrainingStackedExperimentPlan *plan,
    UmiAiTrainingStackedExperimentReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
