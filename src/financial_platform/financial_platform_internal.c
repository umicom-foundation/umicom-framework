/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/financial_platform_internal.c
 *
 * PURPOSE:
 *   Evaluate shared financial platform readiness, coverage, validation,
 *   reconciliation, workflow and audit controls without duplicating canonical
 *   finance, payments or accounting state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "financial_platform_internal.h"

#include <math.h>
#include <stdio.h>

static double clamp_score(double value)
{
    if (value < 0.0) return 0.0;
    if (value > 100.0) return 100.0;
    return value;
}

static double ratio_percent(uint64_t numerator, uint64_t denominator)
{
    if (denominator == 0U) return 0.0;
    if (numerator >= denominator) return 100.0;
    return ((double)numerator / (double)denominator) * 100.0;
}

static double weighted_health(
    const UmiFinancialPlatformPolicy *policy,
    const UmiFinancialPlatformInput *input)
{
    double one = input->accountReady ? 100.0 : 0.0;
    double two = input->tradeReady ? 100.0 : 0.0;
    double three = input->settlementReady ? 100.0 : 0.0;
    double four = input->auditReady ? 100.0 : 0.0;
    double total =
        policy->weightOne + policy->weightTwo +
        policy->weightThree + policy->weightFour;

    if (total <= 0.0) return 0.0;
    return clamp_score(
        (one * policy->weightOne +
         two * policy->weightTwo +
         three * policy->weightThree +
         four * policy->weightFour) / total);
}

UmiStatus umi_financial_platform_evaluate_policy(
    const UmiFinancialPlatformPolicy *policy,
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    double difference;
    double denominator;
    uint64_t failures;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_financial_platform_snapshot_init(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label : policy->label);
    outSnapshot->revision = input->revision;

    switch (policy->kind) {
    case UMI_FINANCIAL_POLICY_READINESS:
        outSnapshot->ready =
            input->trusted && input->active && input->validIdentity;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_FINANCIAL_POLICY_COVERAGE:
        outSnapshot->ratio = ratio_percent(
            input->matchedCount, input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_FRESHNESS:
        outSnapshot->value = (double)input->ageMilliseconds;
        outSnapshot->score =
            policy->warning > 0.0
                ? clamp_score(
                    100.0 -
                    ((double)input->ageMilliseconds / policy->warning) * 100.0)
                : 100.0;
        outSnapshot->ready =
            input->active &&
            (policy->warning <= 0.0 ||
             (double)input->ageMilliseconds <= policy->warning);
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_FINANCIAL_POLICY_VALUE:
        outSnapshot->value = input->value;
        outSnapshot->secondaryValue = input->secondaryValue;
        outSnapshot->ready = input->active;
        break;

    case UMI_FINANCIAL_POLICY_UTILISATION:
        if (input->limitValue > 0.0) {
            outSnapshot->ratio =
                (fabs(input->value) / input->limitValue) * 100.0;
        } else if (input->totalAmount > 0.0) {
            outSnapshot->ratio =
                (fabs(input->amount) / input->totalAmount) * 100.0;
        } else {
            outSnapshot->ratio = input->concentrationPercent;
        }
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = input->trusted;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_VALIDATION:
        outSnapshot->ready =
            input->validIdentity && input->trusted;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_FINANCIAL_POLICY_CONSISTENCY:
        difference = fabs(input->value - input->referenceValue);
        denominator = fabs(input->referenceValue);
        outSnapshot->value = difference;
        outSnapshot->ratio =
            denominator > 0.0 ? (difference / denominator) * 100.0 : difference;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            input->trusted &&
            (policy->threshold <= 0.0 ||
             outSnapshot->score >= policy->threshold);
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->score < policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_BALANCE:
        difference = fabs(input->debitTotal - input->creditTotal);
        denominator = fmax(fabs(input->debitTotal), fabs(input->creditTotal));
        outSnapshot->value = difference;
        outSnapshot->ratio =
            denominator > 0.0 ? (difference / denominator) * 100.0 : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            input->trusted &&
            (policy->threshold <= 0.0 ||
             outSnapshot->score >= policy->threshold);
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->score < policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_MATCH:
        outSnapshot->ratio = ratio_percent(
            input->matchedCount,
            input->matchedCount + input->mismatchCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            (input->matchedCount + input->mismatchCount) > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_TOLERANCE:
        difference = fabs(input->value - input->referenceValue);
        outSnapshot->value = difference;
        outSnapshot->secondaryValue =
            input->tolerance > 0.0 ? input->tolerance : policy->warning;
        outSnapshot->ready =
            outSnapshot->secondaryValue >= 0.0 &&
            difference <= outSnapshot->secondaryValue;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_FINANCIAL_POLICY_AGEING:
        outSnapshot->value = (double)input->ageMilliseconds;
        outSnapshot->ready =
            policy->warning <= 0.0 ||
            (double)input->ageMilliseconds <= policy->warning;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_FINANCIAL_POLICY_APPROVAL:
        outSnapshot->approvalRequired = !input->approved;
        outSnapshot->ready =
            input->trusted && input->approved;
        outSnapshot->attention = outSnapshot->approvalRequired;
        break;

    case UMI_FINANCIAL_POLICY_TRANSITION:
        outSnapshot->ready =
            input->trusted &&
            input->active &&
            input->transitionAllowed;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_FINANCIAL_POLICY_COMPLETION:
        outSnapshot->ratio = ratio_percent(
            input->completedCount, input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_INTEGRITY:
        failures =
            input->mismatchCount +
            input->rejectedCount +
            input->gapCount;
        outSnapshot->ratio =
            input->totalCount > 0U
                ? 100.0 -
                  ratio_percent(
                      failures > input->totalCount
                          ? input->totalCount : failures,
                      input->totalCount)
                : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_SEQUENCE:
        outSnapshot->value = (double)input->gapCount;
        outSnapshot->ready =
            input->auditCount > 0U &&
            input->gapCount == 0U;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_FINANCIAL_POLICY_EXPOSURE:
        outSnapshot->value = input->grossExposure;
        outSnapshot->secondaryValue = input->netExposure;
        outSnapshot->ratio =
            input->limitValue > 0.0
                ? (fabs(input->grossExposure) / input->limitValue) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = input->positionReady;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_PNL:
        outSnapshot->value = input->pnl;
        outSnapshot->ready = input->positionReady || input->tradeReady;
        outSnapshot->attention = input->pnl < 0.0;
        break;

    case UMI_FINANCIAL_POLICY_DUE:
        if (input->dueMilliseconds == 0U) {
            outSnapshot->ready = input->active;
            outSnapshot->value = 0.0;
        } else if (input->nowMilliseconds <= input->dueMilliseconds) {
            outSnapshot->ready = 1;
            outSnapshot->value =
                (double)(input->dueMilliseconds - input->nowMilliseconds);
        } else {
            outSnapshot->ready = 0;
            outSnapshot->attention = 1;
            outSnapshot->value =
                (double)(input->nowMilliseconds - input->dueMilliseconds);
        }
        break;

    case UMI_FINANCIAL_POLICY_LIMIT:
        outSnapshot->value =
            input->amount != 0.0 ? input->amount : input->value;
        outSnapshot->secondaryValue = input->limitValue;
        outSnapshot->ratio =
            input->limitValue > 0.0
                ? (fabs(outSnapshot->value) / input->limitValue) * 100.0
                : 0.0;
        outSnapshot->ready =
            input->limitValue <= 0.0 ||
            fabs(outSnapshot->value) <= input->limitValue;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_FINANCIAL_POLICY_AMOUNT:
        outSnapshot->value = input->amount;
        outSnapshot->ready =
            input->trusted && input->amount >= 0.0;
        outSnapshot->attention = input->amount < 0.0;
        break;

    case UMI_FINANCIAL_POLICY_HEALTH:
        outSnapshot->score = weighted_health(policy, input);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!input->trusted &&
        policy->kind != UMI_FINANCIAL_POLICY_VALUE &&
        policy->kind != UMI_FINANCIAL_POLICY_PNL) {
        outSnapshot->blocked = 1;
        outSnapshot->ready = 0;
    }
    if (outSnapshot->approvalRequired) {
        outSnapshot->ready = 0;
    }

    return umi_financial_platform_snapshot_validate(outSnapshot);
}
