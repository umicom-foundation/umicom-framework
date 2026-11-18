/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/categorical_loss.h
 *
 * PURPOSE:
 *   Share stable scalar row log-loss between graph construction and read-only evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_TRAINING_CATEGORICAL_LOSS_INTERNAL_H
#define UMICOM_AI_TRAINING_CATEGORICAL_LOSS_INTERNAL_H
#include <stddef.h>
#include <math.h>

/* Internal numerical kernel: callers admit a positive column count, finite
 * accessible values and an in-range target. It owns no memory. The public
 * callers decide selection, shape limits, publication and representability.
 * Subtract the target BEFORE adding log(sum): adding the small logarithm to
 * a large common offset first can erase the loss for equal large logits. */
static inline double TrainingCategoricalRowLoss(const float *values,
    size_t columns, size_t target)
{
    double maximum = (double)values[0];
    for (size_t column = 1U; column < columns; ++column)
        if ((double)values[column] > maximum) maximum = (double)values[column];
    double exponentialSum = 0.0;
    for (size_t column = 0U; column < columns; ++column)
        exponentialSum += exp((double)values[column] - maximum);
    return (maximum - (double)values[target]) + log(exponentialSum);
}
#endif
