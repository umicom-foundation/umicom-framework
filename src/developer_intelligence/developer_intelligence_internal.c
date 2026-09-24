/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/developer_intelligence_internal.c
 *
 * PURPOSE:
 *   Evaluate reusable developer-intelligence readiness, ranking, routing,
 *   budgeting and approval policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "developer_intelligence_internal.h"

#include <stdio.h>

static double ClampPercent(double value)
{
    if (value < 0.0) return 0.0;
    if (value > 100.0) return 100.0;
    return value;
}

static double WeightedScore(
    const UmiDeveloperIntelligencePolicy *policy,
    const UmiDeveloperIntelligenceInput *input)
{
    double weight =
        policy->lexicalWeight +
        policy->semanticWeight +
        policy->contextWeight +
        policy->confidenceWeight;
    double score;

    if (weight <= 0.0) return 0.0;
    score =
        ClampPercent(input->lexicalScore) * policy->lexicalWeight +
        ClampPercent(input->semanticScore) * policy->semanticWeight +
        ClampPercent(input->contextScore) * policy->contextWeight +
        ClampPercent(input->confidence) * policy->confidenceWeight;
    return ClampPercent(score / weight);
}

UmiStatus UmiDeveloperIntelligenceEvaluatePolicy(
    const UmiDeveloperIntelligencePolicy *policy,
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    int sourceReady;
    int trustReady;
    int riskAcceptable;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    UmiDeveloperIntelligenceSnapshotInit(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label
            : policy->label);

    outSnapshot->score = WeightedScore(policy, input);
    outSnapshot->confidence = ClampPercent(input->confidence);
    outSnapshot->risk = ClampPercent(input->risk);
    outSnapshot->delta = outSnapshot->score - input->baselineScore;
    outSnapshot->matched = input->matched;
    outSnapshot->total = input->total;
    outSnapshot->conflicts = input->conflicts;
    outSnapshot->citations = input->citations;
    outSnapshot->sourceCount = input->sourceCount;
    outSnapshot->budgetRemaining =
        input->budgetUnits > input->usedUnits
            ? input->budgetUnits - input->usedUnits
            : 0U;
    outSnapshot->revision = input->revision;
    outSnapshot->localPreferred =
        policy->preferLocal && input->localAvailable;
    outSnapshot->remoteUsable =
        input->remoteAvailable && input->remoteAllowed;

    sourceReady =
        policy->requiredSources == 0U ||
        input->sourceCount >= policy->requiredSources;
    trustReady = !policy->requireTrust || input->trusted;
    riskAcceptable = outSnapshot->risk <= policy->maximumRisk;
    outSnapshot->blocked = !trustReady;

    switch (policy->kind) {
    case UMI_DEVELOPER_INTELLIGENCE_SCORE:
        outSnapshot->ready =
            sourceReady && trustReady && riskAcceptable &&
            outSnapshot->score >= policy->minimumScore;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_ROUTE:
        outSnapshot->ready =
            trustReady && riskAcceptable &&
            (input->localAvailable || outSnapshot->remoteUsable) &&
            outSnapshot->score >= policy->minimumScore;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_BUDGET:
        outSnapshot->ready =
            trustReady &&
            (input->budgetUnits == 0U || input->usedUnits <= input->budgetUnits);
        outSnapshot->attention =
            input->budgetUnits > 0U && input->usedUnits > input->budgetUnits;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_FRESHNESS:
        outSnapshot->ready =
            sourceReady && trustReady &&
            outSnapshot->score >= policy->minimumScore &&
            input->contextScore >= policy->minimumScore;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_CONFLICT:
        outSnapshot->ready =
            trustReady && input->conflicts == 0U;
        outSnapshot->attention = input->conflicts > 0U;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_APPROVAL:
        outSnapshot->approvalRequired =
            (!riskAcceptable || input->conflicts > 0U) && !input->approved;
        outSnapshot->ready =
            sourceReady && trustReady &&
            outSnapshot->score >= policy->minimumScore &&
            !outSnapshot->approvalRequired &&
            (riskAcceptable || input->approved);
        outSnapshot->attention = outSnapshot->approvalRequired;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_TRUST:
        outSnapshot->approvalRequired =
            input->risk > policy->maximumRisk && !input->approved;
        outSnapshot->ready =
            trustReady && !outSnapshot->approvalRequired;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_RETRIEVAL:
        outSnapshot->ready =
            sourceReady && trustReady &&
            input->matched > 0U &&
            outSnapshot->score >= policy->minimumScore;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_CITATION:
        outSnapshot->ready =
            sourceReady && trustReady &&
            input->citations > 0U &&
            input->citations <= input->sourceCount &&
            outSnapshot->score >= policy->minimumScore;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_RISK:
        outSnapshot->approvalRequired =
            !riskAcceptable && !input->approved;
        outSnapshot->ready =
            trustReady && !outSnapshot->approvalRequired &&
            outSnapshot->score >= policy->minimumScore;
        outSnapshot->attention = outSnapshot->approvalRequired;
        break;

    case UMI_DEVELOPER_INTELLIGENCE_MASTERY: {
        double evidence = input->total > 0U
            ? ((double)input->matched / (double)input->total) * 100.0
            : 0.0;
        if (evidence > outSnapshot->score) outSnapshot->score = ClampPercent(evidence);
        outSnapshot->ready =
            sourceReady && trustReady &&
            outSnapshot->score >= policy->minimumScore;
        outSnapshot->attention = !outSnapshot->ready;
        break;
    }

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (outSnapshot->approvalRequired) {
        outSnapshot->ready = 0;
    }
    return UmiDeveloperIntelligenceSnapshotValidate(outSnapshot);
}
