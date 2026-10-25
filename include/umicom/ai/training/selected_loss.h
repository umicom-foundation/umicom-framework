/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/selected_loss.h
 * PURPOSE:
 *   Compute assistant-response-only cross-entropy without adding a second
 *   softmax or backward implementation to the native C23 training tape.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SELECTED_LOSS_H
#define UMICOM_AI_TRAINING_SELECTED_LOSS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/tape.h"

#ifdef __cplusplus
extern "C" {
#endif

/* logits is [rows, vocabulary] and targets/mask each contain exactly rows.
 * Every mask element must be 0 (ignore) or 1 (supervise), and at least one
 * row must be selected. Only selected target IDs need to be in vocabulary;
 * ignored targets are never used. The returned scalar is the mean negative
 * log-likelihood OVER SELECTED ROWS, not over the entire context.
 *
 * Internally this builds a nontrainable selection matrix, selects rows with
 * the existing tape Matmul operator, then delegates to the ORIGINAL tape
 * CrossEntropy. Consequently Backward propagates into the original logits
 * and adapter parameters while ignored positions contribute zero gradient.
 * All selected-row semantics and numerical computations remain Framework-owned.
 *
 * For an all-ones mask, this calls canonical CrossEntropy directly; the
 * extra matrix exists only for genuine row selection. This bounded design
 * creates a dense [selectedRows, rows] selector for explainability and must
 * fit UMI_AI_TRAINING_MAX_ELEMENTS. An efficient fused tape operator may
 * replace the composition after numerical qualification, not by bypassing
 * this contract. No files, text parsing or model sampling occur here.
 *
 * On rejection, *outLoss is unchanged and no graph suffix is retained.
 * A failed composition uses TapeRollback; as documented by the existing
 * tape contract, rollback clears previously computed gradient readiness.
 * A successful output is tape-owned and is removed by its caller's rewind. */
UmiStatus UmiAiTrainingSelectedCrossEntropy(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *logits,
    const size_t *targets, size_t targetCount,
    const uint8_t *lossMask, size_t maskCount,
    UmiAiTrainingTensor **outLoss);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SELECTED_LOSS_H */
