/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/prediction_summary.inc
 *
 * PURPOSE:
 *   Aggregate evaluated-token statistics without retaining tensors or publishing invalid ratios.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <float.h>

/* Reporting validates structural invariants even for caller-created summaries.
 * It cannot establish which corpus, tokenizer or objective produced a value. */
static UmiStatus AdmitLossSummary(const UmiAiTrainingPredictionLossSummary *summary)
{
    if (summary == NULL || summary->selectedRows > summary->rows ||
        !isfinite(summary->negativeLogLikelihoodSum) ||
        summary->negativeLogLikelihoodSum < 0.0 ||
        (summary->selectedRows == 0U && summary->negativeLogLikelihoodSum != 0.0))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPredictionLossMerge(
    const UmiAiTrainingPredictionLossSummary *left,
    const UmiAiTrainingPredictionLossSummary *right,
    UmiAiTrainingPredictionLossSummary *outSummary)
{
    if (outSummary == NULL || AdmitLossSummary(left) != UMI_STATUS_OK ||
        AdmitLossSummary(right) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if (right->rows > SIZE_MAX - left->rows ||
        right->selectedRows > SIZE_MAX - left->selectedRows)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Complete all reads and checks before publishing. Exact in-place output
     * supports streaming without copying or keeping earlier batch logits. */
    UmiAiTrainingPredictionLossSummary candidate = {
        .rows = left->rows + right->rows,
        .selectedRows = left->selectedRows + right->selectedRows,
        .negativeLogLikelihoodSum = left->negativeLogLikelihoodSum +
            right->negativeLogLikelihoodSum
    };
    if (!isfinite(candidate.negativeLogLikelihoodSum))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *outSummary = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPredictionLossMean(
    const UmiAiTrainingPredictionLossSummary *summary, double *outMean)
{
    if (outMean == NULL || AdmitLossSummary(summary) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (summary->selectedRows == 0U) return UMI_STATUS_UNAVAILABLE;
    const double mean = summary->negativeLogLikelihoodSum / (double)summary->selectedRows;
    if (!isfinite(mean)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outMean = mean;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPredictionLossPerplexity(
    const UmiAiTrainingPredictionLossSummary *summary, double *outPerplexity)
{
    if (outPerplexity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    double mean;
    const UmiStatus status = UmiAiTrainingPredictionLossMean(summary, &mean);
    if (status != UMI_STATUS_OK) return status;
    if (mean > log(DBL_MAX)) return UMI_STATUS_CAPACITY_EXCEEDED;
    const double value = exp(mean);
    /* Keep the postcondition too: platform rounding near log(DBL_MAX) can
     * still overflow the exponential even after the conservative precheck. */
    if (!isfinite(value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outPerplexity = value;
    return UMI_STATUS_OK;
}
