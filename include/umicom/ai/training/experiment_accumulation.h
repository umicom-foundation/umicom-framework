/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/experiment_accumulation.h
 *
 * PURPOSE:
 *   Configure reproducible effective batches built from small training graphs
 *   without modifying existing experiment configuration or checkpoint ABI.
 *
 * ARCHITECTURE:
 *   The legacy experiment runs unchanged. Opt-in training draws whole source
 *   sequences deterministically, then invokes the canonical decoder's
 *   gradient-accumulation function ONCE per optimiser step. Evaluation batch
 *   size remains controlled by experimentConfig.batchSequences; the effective
 *   training batch size is an independent choice. Decoders are caller-serialised.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_EXPERIMENT_ACCUMULATION_H
#define UMICOM_AI_TRAINING_EXPERIMENT_ACCUMULATION_H

#include <stdint.h>
#include "umicom/ai/training/experiment.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Limit allocations and sampler work per optimiser step independently of
 * the decoder's larger low-level accumulated-token ceiling. */
#define UMI_AI_EXPERIMENT_MAX_EFFECTIVE_SEQUENCES 64U

typedef struct UmiAiTrainingExperimentAccumulation {
    uint32_t effectiveSequences;
    uint32_t microbatchSequences;
} UmiAiTrainingExperimentAccumulation;

/* Check settings against the canonical model capacity planner. The count of
 * microbatch sequences must be positive, not exceed the effective count, and
 * fit one bounded decoder graph. No model or dataset is changed. */
UmiStatus UmiAiTrainingExperimentAccumulationValidate(
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingExperimentAccumulation *accumulation);

/* Opt-in experiment execution. A single optimiser step draws effectiveSequences
 * contexts using whole-file training partition only, then processes them in
 * microbatchSequences-sized graphs. A deterministic draw identifier combines
 * the absolute optimiser step and the draw number so interrupted-and-resumed
 * runs sample exactly the same source windows as uninterrupted runs.
 *
 * The legacy Run/Resume APIs remain unchanged. These calls preserve the
 * original failure-atomic output, observer, checkpoint and evaluation contracts.
 * A checkpoint does NOT record sampling parameters, learning rate or corpus
 * identity: callers must verify them before approving any resume. */
UmiStatus UmiAiTrainingExperimentRunAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingExperimentAccumulation *accumulation,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult);

UmiStatus UmiAiTrainingExperimentResumeAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingExperimentAccumulation *accumulation,
    const UmiAiTrainingDecoder *checkpointModel,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_EXPERIMENT_ACCUMULATION_H */
