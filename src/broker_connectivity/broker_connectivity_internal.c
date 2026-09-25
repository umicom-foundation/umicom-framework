/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/broker_connectivity_internal.c
 *
 * PURPOSE:
 *   Evaluate reusable broker connectivity, account, order, execution, risk,
 *   reconciliation, audit and provider-boundary evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "broker_connectivity_internal.h"

#include <math.h>
#include <stdio.h>

static double clamp_score(double value)
{
    if (value < 0.0) return 0.0;
    if (value > 100.0) return 100.0;
    return value;
}

static double coverage_ratio(uint64_t matched, uint64_t total)
{
    if (total == 0U) return 0.0;
    if (matched >= total) return 100.0;
    return ((double)matched / (double)total) * 100.0;
}

static double weighted_health(
    const UmiBrokerConnectivityPolicy *policy,
    const UmiBrokerConnectivityInput *input)
{
    double one = input->connected ? 100.0 : 0.0;
    double two = input->authenticated ? 100.0 : 0.0;
    double three = input->riskReady ? 100.0 : 0.0;
    double four = input->auditReady ? 100.0 : 0.0;
    double weights =
        policy->weightOne + policy->weightTwo +
        policy->weightThree + policy->weightFour;

    if (weights <= 0.0) return 0.0;
    return clamp_score(
        (one * policy->weightOne +
         two * policy->weightTwo +
         three * policy->weightThree +
         four * policy->weightFour) / weights);
}

UmiStatus umi_broker_connectivity_evaluate_policy(
    const UmiBrokerConnectivityPolicy *policy,
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    double limit;
    double used;
    uint64_t reconciliationTotal;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_broker_connectivity_snapshot_init(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label : policy->label);
    outSnapshot->revision = input->revision;

    switch (policy->kind) {
    case UMI_BROKER_POLICY_READINESS:
        outSnapshot->ready =
            input->trusted &&
            input->providerReady &&
            !input->killSwitchEngaged;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_CONNECTION:
        outSnapshot->ready = input->connected && input->providerReady;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_AUTH:
        outSnapshot->ready =
            input->connected && input->authenticated && input->trusted;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_AGE:
    case UMI_BROKER_POLICY_FRESHNESS:
        outSnapshot->value = (double)input->ageMilliseconds;
        outSnapshot->score = policy->warning > 0.0
            ? clamp_score(100.0 -
                ((double)input->ageMilliseconds / policy->warning) * 100.0)
            : 100.0;
        outSnapshot->ready =
            input->providerReady &&
            (policy->warning <= 0.0 ||
             (double)input->ageMilliseconds < policy->warning);
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_LATENCY:
        outSnapshot->value = (double)input->latencyMilliseconds;
        outSnapshot->score = policy->warning > 0.0
            ? clamp_score(100.0 -
                ((double)input->latencyMilliseconds / policy->warning) * 100.0)
            : 100.0;
        outSnapshot->ready =
            input->connected &&
            (policy->warning <= 0.0 ||
             (double)input->latencyMilliseconds < policy->warning);
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_RECONNECT:
        outSnapshot->value = (double)input->reconnectAttempts;
        outSnapshot->ready =
            input->connected ||
            (policy->warning <= 0.0 ||
             (double)input->reconnectAttempts < policy->warning);
        outSnapshot->attention =
            !input->connected &&
            policy->warning > 0.0 &&
            (double)input->reconnectAttempts >= policy->warning;
        break;

    case UMI_BROKER_POLICY_COVERAGE:
        outSnapshot->ratio = coverage_ratio(
            input->mappedCount, input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_BROKER_POLICY_APPROVAL:
        outSnapshot->approvalRequired =
            input->liveCapable && !input->liveApproved;
        outSnapshot->ready =
            input->providerReady &&
            input->connected &&
            input->authenticated &&
            input->liveCapable &&
            input->liveApproved &&
            input->trusted &&
            !input->killSwitchEngaged;
        outSnapshot->attention =
            outSnapshot->approvalRequired || !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_HEALTH:
        outSnapshot->score = weighted_health(policy, input);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_BROKER_POLICY_VALUE:
        outSnapshot->value = input->value;
        outSnapshot->secondaryValue = input->secondaryValue;
        outSnapshot->ready = input->accountReady || input->positionReady ||
                             input->executionReady;
        break;

    case UMI_BROKER_POLICY_UTILISATION:
        used = input->value;
        limit = input->limitValue;
        if (limit <= 0.0) {
            if (input->marginLimit > 0.0) {
                used = input->marginUsed;
                limit = input->marginLimit;
            } else if (input->maxOrderNotional > 0.0) {
                used = input->orderNotional;
                limit = input->maxOrderNotional;
            }
        }
        outSnapshot->ratio =
            limit > 0.0 ? (used / limit) * 100.0 : 0.0;
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = limit > 0.0 && input->trusted;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_BROKER_POLICY_PNL:
        outSnapshot->value = input->value;
        outSnapshot->ready = input->accountReady || input->positionReady;
        outSnapshot->attention = input->value < 0.0;
        break;

    case UMI_BROKER_POLICY_RECONCILIATION:
        reconciliationTotal = input->matchedCount + input->mismatchCount;
        outSnapshot->ratio = coverage_ratio(
            input->matchedCount, reconciliationTotal);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            reconciliationTotal > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            reconciliationTotal > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_BROKER_POLICY_ORDER_GATE:
        outSnapshot->blocked =
            !input->trusted ||
            !input->connected ||
            !input->authenticated ||
            !input->providerReady ||
            !input->accountReady ||
            !input->orderReady ||
            !input->riskReady ||
            input->restrictedInstrument ||
            input->killSwitchEngaged;
        outSnapshot->ready = !outSnapshot->blocked;
        outSnapshot->attention = outSnapshot->blocked;
        break;

    case UMI_BROKER_POLICY_CANCEL_GATE:
        outSnapshot->blocked =
            !input->trusted ||
            !input->connected ||
            !input->authenticated ||
            !input->providerReady ||
            !input->orderReady;
        outSnapshot->ready = !outSnapshot->blocked;
        outSnapshot->attention = outSnapshot->blocked;
        break;

    case UMI_BROKER_POLICY_REPLACE_GATE:
        outSnapshot->blocked =
            !input->trusted ||
            !input->connected ||
            !input->authenticated ||
            !input->providerReady ||
            !input->orderReady ||
            !input->riskReady ||
            input->killSwitchEngaged;
        outSnapshot->ready = !outSnapshot->blocked;
        outSnapshot->attention = outSnapshot->blocked;
        break;

    case UMI_BROKER_POLICY_INTEGRITY:
        outSnapshot->ratio = coverage_ratio(
            input->totalCount -
                (input->duplicateCount +
                 input->mismatchCount +
                 input->gapCount <= input->totalCount
                    ? input->duplicateCount +
                      input->mismatchCount +
                      input->gapCount
                    : input->totalCount),
            input->totalCount);
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->totalCount > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            input->totalCount > 0U &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_BROKER_POLICY_REJECTION:
        outSnapshot->value =
            (double)(input->rejectionCount + input->lateCount);
        outSnapshot->ready =
            input->rejectionCount == 0U && input->lateCount == 0U;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_PROGRESS:
        outSnapshot->ratio =
            input->orderedQuantity > 0.0
                ? (input->filledQuantity /
                   input->orderedQuantity) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->executionReady &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_BROKER_POLICY_DISTANCE:
        outSnapshot->value =
            fabs(input->averageFillPrice - input->referencePrice);
        outSnapshot->ratio =
            input->referencePrice > 0.0
                ? (outSnapshot->value / input->referencePrice) * 10000.0
                : 0.0;
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = input->executionReady;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_BROKER_POLICY_NOTIONAL:
        outSnapshot->value =
            input->orderNotional != 0.0
                ? input->orderNotional
                : fabs(input->orderQuantity * input->orderPrice);
        outSnapshot->ready = input->orderReady || input->positionReady;
        break;

    case UMI_BROKER_POLICY_LIMIT:
        outSnapshot->value = fabs(input->value);
        outSnapshot->secondaryValue = fabs(input->limitValue);
        outSnapshot->ratio =
            input->limitValue > 0.0
                ? (fabs(input->value) / input->limitValue) * 100.0
                : 0.0;
        outSnapshot->ready =
            input->limitValue > 0.0 &&
            fabs(input->value) <= input->limitValue &&
            input->trusted;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_BROKER_POLICY_LOSS_LIMIT:
        outSnapshot->value = input->dailyPnl;
        outSnapshot->secondaryValue = input->maxDailyLoss;
        outSnapshot->ratio =
            input->maxDailyLoss > 0.0 && input->dailyPnl < 0.0
                ? (fabs(input->dailyPnl) /
                   input->maxDailyLoss) * 100.0
                : 0.0;
        outSnapshot->ready =
            input->maxDailyLoss > 0.0 &&
            input->dailyPnl >= -input->maxDailyLoss;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_BROKER_POLICY_BUYING_POWER:
        outSnapshot->value = input->orderNotional;
        outSnapshot->secondaryValue = input->buyingPower;
        outSnapshot->ratio =
            input->buyingPower > 0.0
                ? (input->orderNotional / input->buyingPower) * 100.0
                : 0.0;
        outSnapshot->ready =
            input->buyingPower > 0.0 &&
            input->orderNotional <= input->buyingPower &&
            input->accountReady;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_PRICE_BAND:
        outSnapshot->value = fabs(input->priceDeviationPercent);
        outSnapshot->ready =
            outSnapshot->value <= policy->warning &&
            input->orderReady;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_KILL_SWITCH:
        outSnapshot->blocked = input->killSwitchEngaged;
        outSnapshot->ready =
            input->riskReady && !input->killSwitchEngaged;
        outSnapshot->attention = input->killSwitchEngaged;
        break;

    case UMI_BROKER_POLICY_SEQUENCE:
        outSnapshot->value = (double)input->gapCount;
        outSnapshot->ready =
            input->gapCount == 0U &&
            input->auditCount > 0U;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_BROKER_POLICY_GAP:
        outSnapshot->value = (double)input->gapCount;
        outSnapshot->ready =
            policy->warning <= 0.0 ||
            (double)input->gapCount < policy->warning;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!input->trusted &&
        policy->kind != UMI_BROKER_POLICY_VALUE &&
        policy->kind != UMI_BROKER_POLICY_PNL) {
        outSnapshot->blocked = 1;
        outSnapshot->ready = 0;
    }
    if (outSnapshot->approvalRequired) outSnapshot->ready = 0;

    return umi_broker_connectivity_snapshot_validate(outSnapshot);
}
