/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/prediction_metrics.h
 *
 * PURPOSE:
 *   Count bounded selected next-token rankings without executing a model or allocating a score copy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_PREDICTION_METRICS_H
#define UMICOM_AI_TRAINING_PREDICTION_METRICS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ai/training/tape.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingPredictionCounts {
    size_t rows;
    size_t selectedRows;
    size_t topOneCorrect;
    size_t topKCorrect;
    size_t topK;
} UmiAiTrainingPredictionCounts;

/* Score row-major logits for one true token ID per row. Each dimension is
 * positive and rows*vocabulary <= UMI_AI_TRAINING_MAX_ELEMENTS. topK is in
 * 1..vocabulary. Larger finite scores rank first; ties rank lower token IDs
 * first, including equal signed zeros. No softmax or probability conversion.
 *
 * selected == NULL requires selectedCount == 0 and includes every row.
 * Otherwise selectedCount == rows and every byte is exactly zero or one.
 * All targets must be in range and ALL logits finite, including excluded rows.
 * An all-zero selection succeeds with zero selected/correct counts: there is
 * no accuracy ratio in that case. The caller must never divide by zero or
 * present no evaluated rows as perfect accuracy.
 *
 * Scores/targets/selection are stable borrowed arrays valid for their declared
 * lengths; output is disjoint caller-owned storage. Nothing is retained. Any
 * failure preserves the complete output; no allocation, file I/O, RNG, model
 * execution, weight updates or content-generation quality claim occurs.
 * Aggregate matching counts before dividing across uneven batches; averaging
 * per-batch percentages changes the weighting. Selection is an evaluation
 * decision, separate from attention masks and from dataset split membership. */
UmiStatus UmiAiTrainingPredictionScore(const float *logits, size_t rows,
    size_t vocabulary, const size_t *targets, const uint8_t *selected,
    size_t selectedCount, size_t topK, UmiAiTrainingPredictionCounts *outCounts);
/* Keep the numerator and denominator together for uneven-batch aggregation.
 * No mean or perplexity is defined when selectedRows is zero. */
typedef struct UmiAiTrainingPredictionLossSummary {
    size_t rows;
    size_t selectedRows;
    double negativeLogLikelihoodSum;
} UmiAiTrainingPredictionLossSummary;

/* Sum categorical negative log-likelihood in natural-log units (nats) over
 * selected rows. Shape, target, finite-input and binary-selection rules match
 * PredictionScore; all excluded rows are still validated. An empty selection
 * succeeds with a zero count and sum, not a zero-loss quality claim.
 *
 * Row loss uses stable log-sum-exp shared with the training tape, subtracting
 * the target before adding log(sum). Accumulate in double without materializing
 * probabilities, a tensor, gradients or a model. If arithmetic cannot remain
 * finite, return INVALID_ARGUMENT and preserve the complete output.
 *
 * Add sums and selected counts across compatible batches before dividing.
 * Compare only matching tokenization, contexts, masks and dataset policy.
 * exp(mean) is perplexity only for the stated categorical objective and may
 * overflow; this service does not exponentiate or claim generation quality.
 * Inputs and output are stable, accessible, disjoint caller-owned storage.
 * No allocation, I/O, retained pointers, training or model invocation occurs. */
UmiStatus UmiAiTrainingPredictionLoss(const float *logits, size_t rows,
    size_t vocabulary, const size_t *targets, const uint8_t *selected,
    size_t selectedCount, UmiAiTrainingPredictionLossSummary *outSummary);

/* Merge already-computed compatible batch summaries without averaging batch
 * means. Rows and selected counts are checked against SIZE_MAX; sums must be
 * finite and nonnegative, selectedRows <= rows, and a zero selected count must
 * have zero sum. A completely zero summary is the identity. An aggregate may
 * exceed the per-call tensor element limit because it retains no tensors.
 *
 * Output may be exactly left or right for streaming accumulation; otherwise
 * all three objects must be disjoint, valid and stable during the call. Every
 * refusal leaves output unchanged. Count or finite-sum overflow returns
 * CAPACITY_EXCEEDED; malformed summaries return INVALID_ARGUMENT.
 * No provenance or deduplication is inferred: callers must use matching
 * objectives, tokenization, datasets and selection policies, and avoid counting
 * a batch twice. Floating-point addition is not exactly associative. */
UmiStatus UmiAiTrainingPredictionLossMerge(
    const UmiAiTrainingPredictionLossSummary *left,
    const UmiAiTrainingPredictionLossSummary *right,
    UmiAiTrainingPredictionLossSummary *outSummary);

/* Compute mean nats per selected token. Zero selected tokens return UNAVAILABLE,
 * not zero loss. The result and input must be disjoint; failure preserves the
 * result. No model, memory allocation or input mutation occurs. */
UmiStatus UmiAiTrainingPredictionLossMean(
    const UmiAiTrainingPredictionLossSummary *summary, double *outMean);

/* Compute exp(mean nats) using the same admission/denominator as LossMean.
 * Zero selected tokens return UNAVAILABLE. An unrepresentable exponential
 * returns CAPACITY_EXCEEDED and preserves output; the finite mean remains
 * available separately. This is not a generated-text quality assessment. */
UmiStatus UmiAiTrainingPredictionLossPerplexity(
    const UmiAiTrainingPredictionLossSummary *summary, double *outPerplexity);

#ifdef __cplusplus
}
#endif
#endif
