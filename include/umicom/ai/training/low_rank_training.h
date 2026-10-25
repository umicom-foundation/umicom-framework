/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/low_rank_training.h
 * PURPOSE:
 *   Train an existing low-rank projection with the canonical C23 tape,
 *   reverse-mode differentiation and failure-atomic multi-parameter SGD.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_LOW_RANK_TRAINING_H
#define UMICOM_AI_TRAINING_LOW_RANK_TRAINING_H

#include "umicom/ai/training/low_rank.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingLowRankStepReport {
    float lossBeforeUpdate;
    size_t exampleCount;
    size_t outputChannels;
} UmiAiTrainingLowRankStepReport;

/* One supervised step for a frozen dense projection plus two trainable
 * low-rank leaves. Input and target are existing, tape-owned [rows, channels]
 * tensors. Target columns must equal projection.outputChannels.
 *
 * This function uses LowRankForward, MeanSquaredError, Backward and
 * SgdStepMany; it does NOT implement another optimiser or gradient kernel.
 * An internal tape checkpoint is restored before returning, whether the step
 * succeeds or fails, so successive calls do not accumulate graph nodes.
 * On invalid inputs, build failures or a rejected SGD update, neither adapter
 * weight is updated and *outReport remains unchanged. On a successful step,
 * lossBeforeUpdate describes the objective BEFORE the parameter update.
 * The original frozen base weights are never passed to the optimiser.
 *
 * The caller owns and serialises the tape, projection, input and target,
 * retains all leaf tensors and must not rewind past them. SGD controls only
 * this bounded dense projection: complete decoder fine-tuning, AdamW state
 * persistence and generation-quality validation require separate services.
 * Concurrent access to the same tape is not supported. */
UmiStatus UmiAiTrainingLowRankTrainStep(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTensor *target,
    const UmiAiTrainingLowRankProjection *projection,
    float learningRate,
    UmiAiTrainingLowRankStepReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
