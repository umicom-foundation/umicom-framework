/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/model_quality_gate.c
 *
 * PURPOSE:
 *   Qualify a proposed native decoder checkpoint against a measured baseline
 *   using canonical held-out source windows and exact evaluation contracts.
 *
 * ARCHITECTURE:
 *   No new token sampler, decoder, loss calculation, optimiser or checkpoint
 *   codec. CompareReports checks copied Framework audit evidence; Evaluate
 *   obtains that evidence from the existing read-only ModelAudit service.
 *   No filesystem/network operations or process creation are performed here.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_quality_gate.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

UmiStatus UmiAiTrainingModelQualityDefaults(
    UmiAiTrainingModelQualityPolicy *outPolicy)
{
    if (outPolicy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outPolicy = (UmiAiTrainingModelQualityPolicy){
        .minimumCrossEntropyReduction = 0.0,
        .maximumTopOneAccuracyRegression = 0.0,
        .maximumTopFiveAccuracyRegression = 0.0,
        .minimumEvaluatedTokens = 32U
    };
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingModelQualityValidate(
    const UmiAiTrainingModelQualityPolicy *policy)
{
    if (policy == NULL ||
        !isfinite(policy->minimumCrossEntropyReduction) ||
        policy->minimumCrossEntropyReduction < 0.0 ||
        !isfinite(policy->maximumTopOneAccuracyRegression) ||
        policy->maximumTopOneAccuracyRegression < 0.0 ||
        policy->maximumTopOneAccuracyRegression > 1.0 ||
        !isfinite(policy->maximumTopFiveAccuracyRegression) ||
        policy->maximumTopFiveAccuracyRegression < 0.0 ||
        policy->maximumTopFiveAccuracyRegression > 1.0 ||
        policy->minimumEvaluatedTokens == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static bool SameDatasetSummary(const UmiAiTrainingDatasetSummary *left,
                               const UmiAiTrainingDatasetSummary *right)
{
    /* Never compare native struct padding or assume an ABI-specific layout. */
    return left->documentCount == right->documentCount &&
           left->trainingDocuments == right->trainingDocuments &&
           left->evaluationDocuments == right->evaluationDocuments &&
           left->trainingBytes == right->trainingBytes &&
           left->evaluationBytes == right->evaluationBytes;
}

static bool ValidAudit(const UmiAiTrainingModelAudit *audit)
{
    return audit != NULL && audit->evaluatedTokens != 0U &&
           audit->topOneCorrect <= audit->evaluatedTokens &&
           audit->topFiveCorrect <= audit->evaluatedTokens &&
           audit->topFiveCorrect >= audit->topOneCorrect &&
           audit->dataset.documentCount > 0U &&
           audit->dataset.trainingDocuments > 0U &&
           audit->dataset.evaluationDocuments > 0U &&
           audit->dataset.trainingDocuments <= audit->dataset.documentCount &&
           audit->dataset.evaluationDocuments <= audit->dataset.documentCount &&
           audit->dataset.trainingDocuments ==
               audit->dataset.documentCount - audit->dataset.evaluationDocuments &&
           isfinite(audit->heldOutCrossEntropy) &&
           audit->heldOutCrossEntropy >= 0.0 &&
           isfinite(audit->heldOutPerplexity) &&
           audit->heldOutPerplexity >= 1.0;
}

UmiStatus UmiAiTrainingModelQualityCompareReports(
    const UmiAiTrainingModelAudit *baseline,
    const UmiAiTrainingModelAudit *candidate,
    const UmiAiTrainingModelQualityPolicy *policy,
    UmiAiTrainingModelQualityReport *outReport)
{
    UmiAiTrainingModelQualityReport staged = {0};
    UmiStatus status;
    if (baseline == NULL || candidate == NULL || outReport == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingModelQualityValidate(policy);
    if (status != UMI_STATUS_OK) return status;
    if (!ValidAudit(baseline) || !ValidAudit(candidate) ||
        baseline->planFingerprint != candidate->planFingerprint ||
        baseline->datasetFingerprint != candidate->datasetFingerprint ||
        baseline->evaluatedTokens != candidate->evaluatedTokens ||
        !SameDatasetSummary(&baseline->dataset, &candidate->dataset))
        return UMI_STATUS_INVALID_ARGUMENT;

    staged.baseline = *baseline;
    staged.candidate = *candidate;
    staged.crossEntropyReduction = baseline->heldOutCrossEntropy -
                                   candidate->heldOutCrossEntropy;
    const double tokenCount = (double)baseline->evaluatedTokens;
    staged.topOneAccuracyChange =
        (double)candidate->topOneCorrect / tokenCount -
        (double)baseline->topOneCorrect / tokenCount;
    staged.topFiveAccuracyChange =
        (double)candidate->topFiveCorrect / tokenCount -
        (double)baseline->topFiveCorrect / tokenCount;

    /* A tie is not an improvement. A checkpoint with exactly the same model
     * fingerprint is also not a NEW candidate, regardless of its score.
     * These are deterministic arithmetic comparisons, not significance tests. */
    staged.accepted =
        (baseline->modelFingerprint != candidate->modelFingerprint &&
         baseline->evaluatedTokens >= policy->minimumEvaluatedTokens &&
         staged.crossEntropyReduction > 0.0 &&
         staged.crossEntropyReduction >= policy->minimumCrossEntropyReduction &&
         staged.topOneAccuracyChange >= -policy->maximumTopOneAccuracyRegression &&
         staged.topFiveAccuracyChange >= -policy->maximumTopFiveAccuracyRegression) ? 1 : 0;

    *outReport = staged;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingModelQualityEvaluate(
    UmiAiTrainingDecoder *baselineModel,
    UmiAiTrainingDecoder *candidateModel,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *commonPlan,
    const UmiAiTrainingModelQualityPolicy *policy,
    UmiAiTrainingModelQualityReport *outReport)
{
    UmiAiTrainingModelAudit baseline = {0};
    UmiAiTrainingModelAudit candidate = {0};
    UmiStatus status;
    if (baselineModel == NULL || candidateModel == NULL || dataset == NULL ||
        commonPlan == NULL || outReport == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingModelQualityValidate(policy);
    if (status != UMI_STATUS_OK) return status;

    status = UmiAiTrainingModelAuditEvaluate(
        baselineModel, dataset, commonPlan, &baseline);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingModelAuditEvaluate(
        candidateModel, dataset, commonPlan, &candidate);
    if (status != UMI_STATUS_OK) return status;
    /* The comparison stages the whole report and does not commit its output
     * until every validity and comparability check has completed. */
    return UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, policy, outReport);
}
