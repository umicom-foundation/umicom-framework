/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/open_tms_internal.c
 *
 * PURPOSE:
 *   Evaluate Open TMS lifecycle, liquidity, pricing, risk, settlement,
 *   accounting and reconciliation evidence without mutating canonical finance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "open_tms_internal.h"

#include <math.h>
#include <stdio.h>

static double clamp_score(double value)
{
    if (value < 0.0) return 0.0;
    if (value > 100.0) return 100.0;
    return value;
}

static double coverage(uint64_t matched, uint64_t total)
{
    if (total == 0U) return 0.0;
    if (matched >= total) return 100.0;
    return ((double)matched / (double)total) * 100.0;
}

static double weighted_health(
    const UmiOpenTmsPolicy *policy,
    const UmiOpenTmsInput *input)
{
    double one = input->cashReady ? 100.0 : 0.0;
    double two = input->liquidityReady ? 100.0 : 0.0;
    double three = input->riskReady ? 100.0 : 0.0;
    double four = input->settlementReady ? 100.0 : 0.0;
    double total = policy->weightOne + policy->weightTwo +
        policy->weightThree + policy->weightFour;

    if (total <= 0.0) return 0.0;
    return clamp_score(
        (one * policy->weightOne +
         two * policy->weightTwo +
         three * policy->weightThree +
         four * policy->weightFour) / total);
}

/* The direct writer is retained for review. Its calculation now runs on a
 * private candidate so invalid observations cannot erase the previous result.
 * The wrapper also rejects non-finite signals before domain arithmetic. */
#if 0
UmiStatus umi_open_tms_evaluate_policy(
    const UmiOpenTmsPolicy *policy,
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    double denominator;
    double difference;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_open_tms_snapshot_init(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label : policy->label);
    outSnapshot->revision = input->revision;

    switch (policy->kind) {
    case UMI_OPEN_TMS_POLICY_READINESS:
        outSnapshot->ready =
            input->trusted && input->active && input->validIdentity;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_OPEN_TMS_POLICY_VALUE:
        outSnapshot->value = input->value;
        outSnapshot->secondaryValue = input->secondaryValue;
        outSnapshot->ready = input->active;
        break;

    case UMI_OPEN_TMS_POLICY_AMOUNT:
        outSnapshot->value =
            input->amount != 0.0 ? input->amount : input->notional;
        outSnapshot->ready = input->trusted;
        break;

    case UMI_OPEN_TMS_POLICY_COVERAGE:
        outSnapshot->ratio = coverage(input->matchedCount, input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_FRESHNESS:
        outSnapshot->value = (double)input->ageMilliseconds;
        outSnapshot->score = policy->warning > 0.0
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

    case UMI_OPEN_TMS_POLICY_LIMIT:
        outSnapshot->value = input->value;
        outSnapshot->secondaryValue = input->limitValue;
        outSnapshot->ratio =
            input->limitValue > 0.0
                ? (fabs(input->value) / input->limitValue) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            input->limitValue <= 0.0 ||
            fabs(input->value) <= input->limitValue;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_UTILISATION:
        outSnapshot->ratio =
            input->limitValue > 0.0
                ? (fabs(input->value) / input->limitValue) * 100.0
                : input->concentrationPercent;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = input->trusted;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_CASH_NET:
        outSnapshot->value =
            input->liquidityInflows - input->liquidityOutflows;
        outSnapshot->ready = input->cashReady;
        outSnapshot->attention = outSnapshot->value < 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_SHORTFALL:
        outSnapshot->value =
            input->projectedCash < input->minimumCash
                ? input->minimumCash - input->projectedCash
                : input->fundingRequired > input->fundingAvailable
                    ? input->fundingRequired - input->fundingAvailable
                    : 0.0;
        outSnapshot->ready = input->cashReady || input->liquidityReady;
        outSnapshot->attention = outSnapshot->value > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_SURPLUS:
        outSnapshot->value =
            input->projectedCash > input->minimumCash
                ? input->projectedCash - input->minimumCash
                : 0.0;
        outSnapshot->ready = input->cashReady;
        break;

    case UMI_OPEN_TMS_POLICY_LIQUIDITY_GAP:
        outSnapshot->value =
            input->liquidityInflows + input->liquidityBuffer -
            input->liquidityOutflows;
        outSnapshot->ready = input->liquidityReady;
        outSnapshot->attention = outSnapshot->value < 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_LIQUIDITY_BUFFER:
        outSnapshot->value = input->liquidityBuffer;
        outSnapshot->secondaryValue =
            input->liquidityOutflows - input->liquidityInflows;
        outSnapshot->ready = input->liquidityReady;
        outSnapshot->attention =
            outSnapshot->secondaryValue > 0.0 &&
            input->liquidityBuffer < outSnapshot->secondaryValue;
        break;

    case UMI_OPEN_TMS_POLICY_RATIO:
        denominator =
            input->liquidityOutflows > 0.0
                ? input->liquidityOutflows
                : input->referenceValue;
        outSnapshot->ratio =
            denominator > 0.0
                ? ((input->liquidityInflows + input->liquidityBuffer) /
                   denominator) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_SCORE:
        outSnapshot->score = clamp_score(
            input->value * policy->weightOne +
            input->secondaryValue * policy->weightTwo +
            (input->marketDataReady ? 100.0 : 0.0) * policy->weightThree +
            (input->auditReady ? 100.0 : 0.0) * policy->weightFour);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_HEALTH:
        outSnapshot->score = weighted_health(policy, input);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_FORWARD_RATE:
        if (input->forwardRate > 0.0) {
            outSnapshot->value = input->forwardRate;
        } else if (input->spotRate > 0.0) {
            denominator =
                1.0 + input->foreignRate * input->yearFraction;
            outSnapshot->value =
                denominator != 0.0
                    ? input->spotRate *
                      (1.0 + input->domesticRate * input->yearFraction) /
                      denominator
                    : 0.0;
        }
        outSnapshot->ready =
            input->marketDataReady && outSnapshot->value > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_FORWARD_POINTS:
        outSnapshot->value =
            input->forwardRate > 0.0 && input->spotRate > 0.0
                ? input->forwardRate - input->spotRate
                : 0.0;
        outSnapshot->ready =
            input->marketDataReady &&
            input->forwardRate > 0.0 &&
            input->spotRate > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_DISCOUNT:
        outSnapshot->value =
            input->discountFactor > 0.0
                ? input->discountFactor
                : 1.0 / (1.0 + input->domesticRate * input->yearFraction);
        outSnapshot->ready =
            input->curvesReady &&
            outSnapshot->value > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_PV:
        outSnapshot->value =
            input->presentValue != 0.0
                ? input->presentValue
                : input->amount * input->discountFactor;
        outSnapshot->ready =
            input->curvesReady && input->marketDataReady;
        break;

    case UMI_OPEN_TMS_POLICY_ACCRUAL:
        outSnapshot->value =
            input->accruedAmount != 0.0
                ? input->accruedAmount
                : input->notional * input->domesticRate *
                  input->yearFraction;
        outSnapshot->ready = input->accountingReady || input->curvesReady;
        break;

    case UMI_OPEN_TMS_POLICY_PNL:
        outSnapshot->pnl = input->pnl;
        outSnapshot->value = input->pnl;
        outSnapshot->ready =
            input->positionReady || input->accountingReady;
        outSnapshot->attention = input->pnl < 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_EXPOSURE:
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

    case UMI_OPEN_TMS_POLICY_RISK_SCORE:
        outSnapshot->value = input->value;
        outSnapshot->score = clamp_score(100.0 - fabs(input->value));
        outSnapshot->ready =
            input->riskReady &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_BREACH:
        outSnapshot->value =
            (double)(input->breachedCount + input->failedCount + input->breakCount);
        outSnapshot->ready = outSnapshot->value == 0.0;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_OPEN_TMS_POLICY_DUE:
        if (input->dueMilliseconds == 0U) {
            outSnapshot->ready = input->active;
        } else if (input->nowMilliseconds <= input->dueMilliseconds) {
            outSnapshot->ready = 1;
            outSnapshot->value =
                (double)(input->dueMilliseconds - input->nowMilliseconds);
        } else {
            outSnapshot->value =
                (double)(input->nowMilliseconds - input->dueMilliseconds);
            outSnapshot->attention = 1;
        }
        break;

    case UMI_OPEN_TMS_POLICY_TRANSITION:
        outSnapshot->ready =
            input->trusted &&
            input->active &&
            input->transitionAllowed;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_OPEN_TMS_POLICY_APPROVAL:
        outSnapshot->approvalRequired = !input->approved;
        outSnapshot->ready =
            input->trusted && input->approved;
        outSnapshot->attention = outSnapshot->approvalRequired;
        break;

    case UMI_OPEN_TMS_POLICY_MATCH:
        outSnapshot->ratio = coverage(
            input->matchedCount,
            input->matchedCount + input->breakCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            (input->matchedCount + input->breakCount) > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_COMPLETION:
        outSnapshot->ratio = coverage(
            input->completedCount, input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_CONSISTENCY:
        difference = fabs(input->value - input->referenceValue);
        denominator = fabs(input->referenceValue);
        outSnapshot->value = difference;
        outSnapshot->ratio =
            denominator > 0.0 ? (difference / denominator) * 100.0 : difference;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_BALANCE:
        difference = fabs(input->value - input->secondaryValue);
        denominator = fmax(fabs(input->value), fabs(input->secondaryValue));
        outSnapshot->value = difference;
        outSnapshot->ratio =
            denominator > 0.0 ? (difference / denominator) * 100.0 : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!input->trusted &&
        policy->kind != UMI_OPEN_TMS_POLICY_VALUE &&
        policy->kind != UMI_OPEN_TMS_POLICY_PNL) {
        outSnapshot->blocked = 1;
        outSnapshot->ready = 0;
    }
    if (outSnapshot->approvalRequired) outSnapshot->ready = 0;

    return umi_open_tms_snapshot_validate(outSnapshot);
}
#endif

static UmiStatus umi_open_tms_evaluate_policy_build_candidate(
    const UmiOpenTmsPolicy *policy,
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    double denominator;
    double difference;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_open_tms_snapshot_init(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label : policy->label);
    outSnapshot->revision = input->revision;

    switch (policy->kind) {
    case UMI_OPEN_TMS_POLICY_READINESS:
        outSnapshot->ready =
            input->trusted && input->active && input->validIdentity;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_OPEN_TMS_POLICY_VALUE:
        outSnapshot->value = input->value;
        outSnapshot->secondaryValue = input->secondaryValue;
        outSnapshot->ready = input->active;
        break;

    case UMI_OPEN_TMS_POLICY_AMOUNT:
        outSnapshot->value =
            input->amount != 0.0 ? input->amount : input->notional;
        outSnapshot->ready = input->trusted;
        break;

    case UMI_OPEN_TMS_POLICY_COVERAGE:
        outSnapshot->ratio = coverage(input->matchedCount, input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_FRESHNESS:
        outSnapshot->value = (double)input->ageMilliseconds;
        outSnapshot->score = policy->warning > 0.0
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

    case UMI_OPEN_TMS_POLICY_LIMIT:
        outSnapshot->value = input->value;
        outSnapshot->secondaryValue = input->limitValue;
        outSnapshot->ratio =
            input->limitValue > 0.0
                ? (fabs(input->value) / input->limitValue) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            input->limitValue <= 0.0 ||
            fabs(input->value) <= input->limitValue;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_UTILISATION:
        outSnapshot->ratio =
            input->limitValue > 0.0
                ? (fabs(input->value) / input->limitValue) * 100.0
                : input->concentrationPercent;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = input->trusted;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_CASH_NET:
        outSnapshot->value =
            input->liquidityInflows - input->liquidityOutflows;
        outSnapshot->ready = input->cashReady;
        outSnapshot->attention = outSnapshot->value < 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_SHORTFALL:
        outSnapshot->value =
            input->projectedCash < input->minimumCash
                ? input->minimumCash - input->projectedCash
                : input->fundingRequired > input->fundingAvailable
                    ? input->fundingRequired - input->fundingAvailable
                    : 0.0;
        outSnapshot->ready = input->cashReady || input->liquidityReady;
        outSnapshot->attention = outSnapshot->value > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_SURPLUS:
        outSnapshot->value =
            input->projectedCash > input->minimumCash
                ? input->projectedCash - input->minimumCash
                : 0.0;
        outSnapshot->ready = input->cashReady;
        break;

    case UMI_OPEN_TMS_POLICY_LIQUIDITY_GAP:
        outSnapshot->value =
            input->liquidityInflows + input->liquidityBuffer -
            input->liquidityOutflows;
        outSnapshot->ready = input->liquidityReady;
        outSnapshot->attention = outSnapshot->value < 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_LIQUIDITY_BUFFER:
        outSnapshot->value = input->liquidityBuffer;
        outSnapshot->secondaryValue =
            input->liquidityOutflows - input->liquidityInflows;
        outSnapshot->ready = input->liquidityReady;
        outSnapshot->attention =
            outSnapshot->secondaryValue > 0.0 &&
            input->liquidityBuffer < outSnapshot->secondaryValue;
        break;

    case UMI_OPEN_TMS_POLICY_RATIO:
        denominator =
            input->liquidityOutflows > 0.0
                ? input->liquidityOutflows
                : input->referenceValue;
        outSnapshot->ratio =
            denominator > 0.0
                ? ((input->liquidityInflows + input->liquidityBuffer) /
                   denominator) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_SCORE:
        outSnapshot->score = clamp_score(
            input->value * policy->weightOne +
            input->secondaryValue * policy->weightTwo +
            (input->marketDataReady ? 100.0 : 0.0) * policy->weightThree +
            (input->auditReady ? 100.0 : 0.0) * policy->weightFour);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_HEALTH:
        outSnapshot->score = weighted_health(policy, input);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_FORWARD_RATE:
        if (input->forwardRate > 0.0) {
            outSnapshot->value = input->forwardRate;
        } else if (input->spotRate > 0.0) {
            denominator =
                1.0 + input->foreignRate * input->yearFraction;
            outSnapshot->value =
                denominator != 0.0
                    ? input->spotRate *
                      (1.0 + input->domesticRate * input->yearFraction) /
                      denominator
                    : 0.0;
        }
        outSnapshot->ready =
            input->marketDataReady && outSnapshot->value > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_FORWARD_POINTS:
        outSnapshot->value =
            input->forwardRate > 0.0 && input->spotRate > 0.0
                ? input->forwardRate - input->spotRate
                : 0.0;
        outSnapshot->ready =
            input->marketDataReady &&
            input->forwardRate > 0.0 &&
            input->spotRate > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_DISCOUNT:
        outSnapshot->value =
            input->discountFactor > 0.0
                ? input->discountFactor
                : 1.0 / (1.0 + input->domesticRate * input->yearFraction);
        outSnapshot->ready =
            input->curvesReady &&
            outSnapshot->value > 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_PV:
        outSnapshot->value =
            input->presentValue != 0.0
                ? input->presentValue
                : input->amount * input->discountFactor;
        outSnapshot->ready =
            input->curvesReady && input->marketDataReady;
        break;

    case UMI_OPEN_TMS_POLICY_ACCRUAL:
        outSnapshot->value =
            input->accruedAmount != 0.0
                ? input->accruedAmount
                : input->notional * input->domesticRate *
                  input->yearFraction;
        outSnapshot->ready = input->accountingReady || input->curvesReady;
        break;

    case UMI_OPEN_TMS_POLICY_PNL:
        outSnapshot->pnl = input->pnl;
        outSnapshot->value = input->pnl;
        outSnapshot->ready =
            input->positionReady || input->accountingReady;
        outSnapshot->attention = input->pnl < 0.0;
        break;

    case UMI_OPEN_TMS_POLICY_EXPOSURE:
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

    case UMI_OPEN_TMS_POLICY_RISK_SCORE:
        outSnapshot->value = input->value;
        outSnapshot->score = clamp_score(100.0 - fabs(input->value));
        outSnapshot->ready =
            input->riskReady &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_BREACH:
        outSnapshot->value =
            (double)(input->breachedCount + input->failedCount + input->breakCount);
        outSnapshot->ready = outSnapshot->value == 0.0;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_OPEN_TMS_POLICY_DUE:
        if (input->dueMilliseconds == 0U) {
            outSnapshot->ready = input->active;
        } else if (input->nowMilliseconds <= input->dueMilliseconds) {
            outSnapshot->ready = 1;
            outSnapshot->value =
                (double)(input->dueMilliseconds - input->nowMilliseconds);
        } else {
            outSnapshot->value =
                (double)(input->nowMilliseconds - input->dueMilliseconds);
            outSnapshot->attention = 1;
        }
        break;

    case UMI_OPEN_TMS_POLICY_TRANSITION:
        outSnapshot->ready =
            input->trusted &&
            input->active &&
            input->transitionAllowed;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_OPEN_TMS_POLICY_APPROVAL:
        outSnapshot->approvalRequired = !input->approved;
        outSnapshot->ready =
            input->trusted && input->approved;
        outSnapshot->attention = outSnapshot->approvalRequired;
        break;

    case UMI_OPEN_TMS_POLICY_MATCH:
        outSnapshot->ratio = coverage(
            input->matchedCount,
            input->matchedCount + input->breakCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            (input->matchedCount + input->breakCount) > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_COMPLETION:
        outSnapshot->ratio = coverage(
            input->completedCount, input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_CONSISTENCY:
        difference = fabs(input->value - input->referenceValue);
        denominator = fabs(input->referenceValue);
        outSnapshot->value = difference;
        outSnapshot->ratio =
            denominator > 0.0 ? (difference / denominator) * 100.0 : difference;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_OPEN_TMS_POLICY_BALANCE:
        difference = fabs(input->value - input->secondaryValue);
        denominator = fmax(fabs(input->value), fabs(input->secondaryValue));
        outSnapshot->value = difference;
        outSnapshot->ratio =
            denominator > 0.0 ? (difference / denominator) * 100.0 : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!input->trusted &&
        policy->kind != UMI_OPEN_TMS_POLICY_VALUE &&
        policy->kind != UMI_OPEN_TMS_POLICY_PNL) {
        outSnapshot->blocked = 1;
        outSnapshot->ready = 0;
    }
    if (outSnapshot->approvalRequired) outSnapshot->ready = 0;

    return umi_open_tms_snapshot_validate(outSnapshot);
}
/* These bounded value snapshots contain no owned resources. Keeping the result
 * separate also permits input labels to borrow the previous output's storage. */
UmiStatus umi_open_tms_evaluate_policy(
    const UmiOpenTmsPolicy *policy,
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    if (policy == NULL || input == NULL || outSnapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!isfinite(input->value) ||
        !isfinite(input->secondaryValue) ||
        !isfinite(input->referenceValue) ||
        !isfinite(input->limitValue) ||
        !isfinite(input->amount) ||
        !isfinite(input->notional) ||
        !isfinite(input->spotRate) ||
        !isfinite(input->domesticRate) ||
        !isfinite(input->foreignRate) ||
        !isfinite(input->yearFraction) ||
        !isfinite(input->forwardRate) ||
        !isfinite(input->discountFactor) ||
        !isfinite(input->presentValue) ||
        !isfinite(input->accruedAmount) ||
        !isfinite(input->pnl) ||
        !isfinite(input->grossExposure) ||
        !isfinite(input->netExposure) ||
        !isfinite(input->concentrationPercent) ||
        !isfinite(input->liquidityInflows) ||
        !isfinite(input->liquidityOutflows) ||
        !isfinite(input->liquidityBuffer) ||
        !isfinite(input->fundingAvailable) ||
        !isfinite(input->fundingRequired) ||
        !isfinite(input->settledCash) ||
        !isfinite(input->projectedCash) ||
        !isfinite(input->minimumCash))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (input->label != NULL) {
        size_t length = 0U;
        while (length < sizeof(outSnapshot->label) && input->label[length] != '\0') ++length;
        if (length == sizeof(outSnapshot->label)) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiOpenTmsSnapshot candidate;
    static_assert(sizeof(candidate) <= 4096U, "Use heap staging when extending this snapshot.");
    UmiStatus status = umi_open_tms_evaluate_policy_build_candidate(policy, input, &candidate);
    if (status != UMI_STATUS_OK) return status;
    /* Finite inputs can still overflow a derived calculation. Refuse that
     * evidence instead of publishing an infinite balance, score or ratio. */
    if (!isfinite(candidate.value) ||
        !isfinite(candidate.secondaryValue) ||
        !isfinite(candidate.score) ||
        !isfinite(candidate.ratio) ||
        !isfinite(candidate.pnl))
        return UMI_STATUS_INVALID_STATE;
    memcpy(outSnapshot, &candidate, sizeof(candidate));
    return UMI_STATUS_OK;
}
