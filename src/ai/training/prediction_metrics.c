/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/prediction_metrics.c
 *
 * PURPOSE:
 *   Centralize token rank counting for model audits and selected evaluation rows without sorting or allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/prediction_metrics.h"
#include <math.h>
#include "categorical_loss.h"

/* Rank scoring and read-only loss now share complete input admission. Rank ordering and publication are preserved; the previous self-contained score implementation remains available for review. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiTrainingPredictionScore(const float *logits, size_t rows,
    size_t vocabulary, const size_t *targets, const uint8_t *selected,
    size_t selectedCount, size_t topK, UmiAiTrainingPredictionCounts *outCounts)
{
    if (logits == NULL || targets == NULL || outCounts == NULL || rows == 0U ||
        vocabulary == 0U || topK == 0U || topK > vocabulary ||
        (selected == NULL && selectedCount != 0U) ||
        (selected != NULL && selectedCount != rows))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (vocabulary > UMI_AI_TRAINING_MAX_ELEMENTS ||
        rows > UMI_AI_TRAINING_MAX_ELEMENTS / vocabulary)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiTrainingPredictionCounts candidate = {.rows=rows, .topK=topK};
    /* Rank only the target instead of sorting or copying each vocabulary row.
     * Validate excluded rows too; selection cannot hide invalid numerical data.
     * Publish once after every row has passed the complete input contract. */
    for (size_t row=0U; row<rows; ++row) {
        if (targets[row] >= vocabulary || (selected != NULL && selected[row] > 1U))
            return UMI_STATUS_INVALID_ARGUMENT;
        const size_t target=targets[row];
        const float targetScore=logits[row*vocabulary+target];
        if (!isfinite(targetScore)) return UMI_STATUS_INVALID_ARGUMENT;
        size_t rank=0U;
        for (size_t column=0U; column<vocabulary; ++column) {
            const float score=logits[row*vocabulary+column];
            if (!isfinite(score)) return UMI_STATUS_INVALID_ARGUMENT;
            if (score > targetScore || (score == targetScore && column < target))
                ++rank;
        }
        if (selected == NULL || selected[row] != 0U) {
            ++candidate.selectedRows;
            if (rank == 0U) ++candidate.topOneCorrect;
            if (rank < topK) ++candidate.topKCorrect;
        }
    }
    *outCounts=candidate;
    return UMI_STATUS_OK;
}

#endif
/* Rank and loss reports share admission so a selection cannot hide malformed
 * targets, mask bytes or numerical data in either evaluation route. */
static UmiStatus AdmitPredictionRows(const float *logits, size_t rows,
    size_t vocabulary, const size_t *targets, const uint8_t *selected,
    size_t selectedCount, size_t *outSelectedRows)
{
    if (logits == NULL || targets == NULL || rows == 0U || vocabulary == 0U ||
        (selected == NULL && selectedCount != 0U) ||
        (selected != NULL && selectedCount != rows))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (vocabulary > UMI_AI_TRAINING_MAX_ELEMENTS ||
        rows > UMI_AI_TRAINING_MAX_ELEMENTS / vocabulary)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t selectedRows = 0U;
    for (size_t row = 0U; row < rows; ++row) {
        if (targets[row] >= vocabulary || (selected != NULL && selected[row] > 1U))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t column = 0U; column < vocabulary; ++column)
            if (!isfinite(logits[row * vocabulary + column]))
                return UMI_STATUS_INVALID_ARGUMENT;
        if (selected == NULL || selected[row] != 0U) ++selectedRows;
    }
    *outSelectedRows = selectedRows;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPredictionScore(const float *logits, size_t rows,
    size_t vocabulary, const size_t *targets, const uint8_t *selected,
    size_t selectedCount, size_t topK, UmiAiTrainingPredictionCounts *outCounts)
{
    if (outCounts == NULL || topK == 0U || topK > vocabulary)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t selectedRows;
    const UmiStatus status = AdmitPredictionRows(logits, rows, vocabulary,
        targets, selected, selectedCount, &selectedRows);
    if (status != UMI_STATUS_OK) return status;
    UmiAiTrainingPredictionCounts candidate = {
        .rows = rows, .selectedRows = selectedRows, .topK = topK
    };
    for (size_t row = 0U; row < rows; ++row) {
        if (selected != NULL && selected[row] == 0U) continue;
        const size_t target = targets[row];
        const float targetScore = logits[row * vocabulary + target];
        size_t rank = 0U;
        for (size_t column = 0U; column < vocabulary; ++column) {
            const float score = logits[row * vocabulary + column];
            if (score > targetScore || (score == targetScore && column < target)) ++rank;
        }
        if (rank == 0U) ++candidate.topOneCorrect;
        if (rank < topK) ++candidate.topKCorrect;
    }
    *outCounts = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPredictionLoss(const float *logits, size_t rows,
    size_t vocabulary, const size_t *targets, const uint8_t *selected,
    size_t selectedCount, UmiAiTrainingPredictionLossSummary *outSummary)
{
    if (outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t selectedRows;
    const UmiStatus status = AdmitPredictionRows(logits, rows, vocabulary,
        targets, selected, selectedCount, &selectedRows);
    if (status != UMI_STATUS_OK) return status;
    UmiAiTrainingPredictionLossSummary candidate = {
        .rows = rows, .selectedRows = selectedRows, .negativeLogLikelihoodSum = 0.0
    };
    for (size_t row = 0U; row < rows; ++row) {
        if (selected != NULL && selected[row] == 0U) continue;
        const double rowLoss = TrainingCategoricalRowLoss(logits + row * vocabulary,
            vocabulary, targets[row]);
        candidate.negativeLogLikelihoodSum += rowLoss;
        if (!isfinite(candidate.negativeLogLikelihoodSum))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    *outSummary = candidate;
    return UMI_STATUS_OK;
}

/* Reporting composes existing loss summaries; it never constructs a tape. */
#include "prediction_summary.inc"
