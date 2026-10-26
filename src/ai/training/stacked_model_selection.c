/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_model_selection.c
 * PURPOSE: Hold-out comparison using the one canonical stacked evaluator.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_model_selection.h"

#include <math.h>
#include <string.h>

static int SameShape(const UmiAiTrainingStackedDecoderConfig *a,
                     const UmiAiTrainingStackedDecoderConfig *b)
{
    return a->vocabularySize == b->vocabularySize &&
        a->contextLength == b->contextLength &&
        a->channels == b->channels &&
        a->hiddenChannels == b->hiddenChannels &&
        a->attentionHeads == b->attentionHeads &&
        a->transformerLayers == b->transformerLayers;
}

UmiStatus UmiAiTrainingStackedModelSelectionCompare(
    UmiAiTrainingStackedDecoder *reference,
    UmiAiTrainingStackedDecoder *candidate,
    const UmiAiTrainingStackedEvaluationCase *heldout,
    size_t exampleCount,
    const UmiAiTrainingStackedSelectionPolicy *policy,
    UmiAiTrainingStackedSelectionReport *outReport)
{
    UmiAiTrainingStackedSelectionReport staged = {0};
    UmiAiTrainingStackedDecoderInfo baselineInfo = {0}, testedInfo = {0};
    UmiStatus status;
    if (reference == NULL || candidate == NULL || reference == candidate ||
        heldout == NULL || policy == NULL || outReport == NULL ||
        !isfinite(policy->maximumAllowedLossIncrease) ||
        policy->maximumAllowedLossIncrease < 0.0 ||
        !isfinite(policy->minimumTopOneAccuracy) ||
        policy->minimumTopOneAccuracy < 0.0 ||
        policy->minimumTopOneAccuracy > 1.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedDecoderInspect(reference, &baselineInfo);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedDecoderInspect(candidate, &testedInfo);
    if (status != UMI_STATUS_OK) return status;
    if (!SameShape(&baselineInfo.config, &testedInfo.config))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedEvaluationRun(
        reference, heldout, exampleCount, &staged.reference);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedEvaluationRun(
        candidate, heldout, exampleCount, &staged.candidate);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(staged.reference.orderedCaseSha256,
               staged.candidate.orderedCaseSha256, UMI_SHA256_BYTES) != 0)
        return UMI_STATUS_INVALID_STATE;
    staged.observedLossChange =
        staged.candidate.meanSelectedCrossEntropy -
        staged.reference.meanSelectedCrossEntropy;
    if (!isfinite(staged.observedLossChange))
        return UMI_STATUS_INVALID_STATE;
    staged.lossAccepted =
        staged.observedLossChange <= policy->maximumAllowedLossIncrease;
    staged.accuracyAccepted =
        staged.candidate.topOneAccuracy >= policy->minimumTopOneAccuracy;
    staged.candidateAccepted = staged.lossAccepted && staged.accuracyAccepted;
    *outReport = staged;
    return UMI_STATUS_OK;
}
