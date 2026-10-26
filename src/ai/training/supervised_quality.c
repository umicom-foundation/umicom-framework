/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/supervised_quality.c
 * PURPOSE: Compare two real held-out evaluations without a neural kernel.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_quality.h"

#include <math.h>
#include <stddef.h>

static int QualityObservationValid(
    const UmiAiTrainingSupervisedEvaluation *e)
{
    return e != NULL && e->exampleCount != 0U &&
        e->supervisedTokens != 0U &&
        e->correctTopOneTokens <= e->supervisedTokens &&
        isfinite(e->meanCrossEntropy) && e->meanCrossEntropy >= 0.0 &&
        isfinite(e->topOneAccuracy) && e->topOneAccuracy >= 0.0 &&
        e->topOneAccuracy <= 1.0 &&
        fabs(e->topOneAccuracy - (double)e->correctTopOneTokens /
             (double)e->supervisedTokens) <= 1e-12 &&
        (e->perplexityAvailable == 0 ||
         (e->perplexityAvailable == 1 &&
          isfinite(e->perplexity) && e->perplexity >= 1.0));
}

UmiStatus UmiAiTrainingSupervisedQualityEvaluate(
    const UmiAiTrainingSupervisedEvaluation *before,
    const UmiAiTrainingSupervisedEvaluation *after,
    const UmiAiTrainingSupervisedQualityPolicy *policy,
    UmiAiTrainingSupervisedQualityReport *outReport)
{
    UmiAiTrainingSupervisedQualityReport candidate = {0};
    if (outReport == NULL || policy == NULL ||
        !QualityObservationValid(before) || !QualityObservationValid(after) ||
        before->exampleCount != after->exampleCount ||
        before->supervisedTokens != after->supervisedTokens ||
        !isfinite(policy->maximumCrossEntropyIncrease) ||
        !isfinite(policy->minimumTopOneAccuracy) ||
        !isfinite(policy->minimumCrossEntropyReduction) ||
        policy->maximumCrossEntropyIncrease < 0.0 ||
        policy->minimumTopOneAccuracy < 0.0 ||
        policy->minimumTopOneAccuracy > 1.0 ||
        policy->minimumCrossEntropyReduction < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate.lossChange = after->meanCrossEntropy - before->meanCrossEntropy;
    candidate.accuracyChange = after->topOneAccuracy - before->topOneAccuracy;
    if (!isfinite(candidate.lossChange) || !isfinite(candidate.accuracyChange))
        return UMI_STATUS_INVALID_STATE;
    candidate.evaluatedTokens = before->supervisedTokens;
    candidate.lossWithinTolerance =
        candidate.lossChange <= policy->maximumCrossEntropyIncrease;
    candidate.accuracyAtLeastMinimum =
        after->topOneAccuracy >= policy->minimumTopOneAccuracy;
    candidate.improvementAtLeastMinimum =
        (before->meanCrossEntropy - after->meanCrossEntropy) >=
        policy->minimumCrossEntropyReduction;
    candidate.accepted = candidate.lossWithinTolerance &&
        candidate.accuracyAtLeastMinimum &&
        candidate.improvementAtLeastMinimum;
    *outReport = candidate;
    return UMI_STATUS_OK;
}
