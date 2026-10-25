/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/selected_loss.c
 * PURPOSE:
 *   Reuse tape-owned matrix selection and cross-entropy to backpropagate only
 *   through explicitly supervised token positions in a decoder window.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/selected_loss.h"

#include <stdlib.h>

UmiStatus UmiAiTrainingSelectedCrossEntropy(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *logits,
    const size_t *targets, size_t targetCount,
    const uint8_t *lossMask, size_t maskCount,
    UmiAiTrainingTensor **outLoss)
{
    size_t rows = 0U, vocabulary = 0U, selected = 0U;
    UmiStatus status;
    if (tape == NULL || logits == NULL || targets == NULL ||
        lossMask == NULL || outLoss == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(logits, &rows, &vocabulary);
    if (status != UMI_STATUS_OK) return status;
    if (rows == 0U || vocabulary < 2U ||
        vocabulary > UMI_AI_TRAINING_MAX_ELEMENTS / rows ||
        targetCount != rows || maskCount != rows)
        return UMI_STATUS_INVALID_ARGUMENT;

    for (size_t i = 0U; i < rows; ++i) {
        if (lossMask[i] > 1U) return UMI_STATUS_INVALID_ARGUMENT;
        if (lossMask[i] == 1U) {
            if (targets[i] >= vocabulary) return UMI_STATUS_INVALID_ARGUMENT;
            ++selected;
        }
    }
    if (selected == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (selected == rows)
        return UmiAiTrainingCrossEntropy(tape, logits, targets, rows, outLoss);
    if (selected > UMI_AI_TRAINING_MAX_ELEMENTS / rows ||
        selected > UMI_AI_TRAINING_MAX_ELEMENTS / vocabulary)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    const size_t selectorCount = selected * rows;
    float *selector = calloc(selectorCount, sizeof(*selector));
    size_t *chosen = malloc(selected * sizeof(*chosen));
    if (selector == NULL || chosen == NULL) {
        free(chosen);
        free(selector);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t at = 0U;
    for (size_t row = 0U; row < rows; ++row) {
        if (lossMask[row] != 1U) continue;
        selector[at * rows + row] = 1.0F;
        chosen[at] = targets[row];
        ++at;
    }

    size_t checkpoint = 0U;
    UmiAiTrainingTensor *selection = NULL, *subset = NULL, *loss = NULL;
    status = UmiAiTrainingTapeCheckpoint(tape, &checkpoint);
    if (status != UMI_STATUS_OK) goto finish;
    status = UmiAiTrainingTensorCreate(tape, selected, rows,
        selector, selectorCount, 0, &selection);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingMatmul(tape, selection, logits, &subset);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingCrossEntropy(tape, subset, chosen, selected, &loss);
    if (status != UMI_STATUS_OK) goto rollback;
    *outLoss = loss;
    goto finish;

rollback:
    /* The original logits/parameter graph survives; only new nodes vanish.
     * Existing gradients become stale under the tape's rollback contract. */
    (void)UmiAiTrainingTapeRollback(tape, checkpoint);
finish:
    free(chosen);
    free(selector);
    return status;
}
