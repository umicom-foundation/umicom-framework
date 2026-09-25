/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/trading_workstation_internal.c
 *
 * PURPOSE:
 *   Evaluate professional trading workstation calculations and readiness
 *   without mutating market, order, portfolio or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "trading_workstation_internal.h"

#include <math.h>
#include <stdio.h>

#define REQUIRE_MARKET     UINT32_C(1)
#define REQUIRE_BROKER     UINT32_C(2)
#define REQUIRE_RISK       UINT32_C(4)
#define REQUIRE_HEALTH     UINT32_C(8)
#define REQUIRE_INSTRUMENT UINT32_C(16)
#define REQUIRE_QUOTE      UINT32_C(32)
#define REQUIRE_DEPTH      UINT32_C(64)
#define REQUIRE_BAR        UINT32_C(128)
#define REQUIRE_TAPE       UINT32_C(256)
#define REQUIRE_CONTEXT    UINT32_C(512)
#define REQUIRE_TRUST      UINT32_C(1024)

static double clamp_score(double value)
{
    if (value < 0.0) return 0.0;
    if (value > 100.0) return 100.0;
    return value;
}

static int required_flags_ready(
    uint32_t flags,
    const UmiTradingProfessionalInput *input)
{
    if ((flags & REQUIRE_MARKET) != 0U && !input->marketDataReady) return 0;
    if ((flags & REQUIRE_BROKER) != 0U && !input->brokerReady) return 0;
    if ((flags & REQUIRE_RISK) != 0U && !input->riskReady) return 0;
    if ((flags & REQUIRE_HEALTH) != 0U && !input->healthReady) return 0;
    if ((flags & REQUIRE_INSTRUMENT) != 0U && !input->hasInstrument) return 0;
    if ((flags & REQUIRE_QUOTE) != 0U && !input->hasQuote) return 0;
    if ((flags & REQUIRE_DEPTH) != 0U && !input->hasDepth) return 0;
    if ((flags & REQUIRE_BAR) != 0U && !input->hasBar) return 0;
    if ((flags & REQUIRE_TAPE) != 0U && !input->tradeTapeReady) return 0;
    if ((flags & REQUIRE_CONTEXT) != 0U && !input->linkedContextReady) return 0;
    if ((flags & REQUIRE_TRUST) != 0U && !input->trusted) return 0;
    return 1;
}

static double workstation_score(
    const UmiTradingProfessionalPolicy *policy,
    const UmiTradingProfessionalInput *input)
{
    double volumeRatio =
        input->averageVolume > 0.0
            ? clamp_score((input->volume / input->averageVolume) * 50.0)
            : 0.0;
    double liquidity = clamp_score(input->liquidityScore);
    double momentum = clamp_score(50.0 + input->momentumScore * 0.5);
    double spreadQuality = clamp_score(input->spreadQualityScore);
    double weight =
        policy->weightOne + policy->weightTwo +
        policy->weightThree + policy->weightFour;

    if (weight <= 0.0) return 0.0;
    return clamp_score(
        (momentum * policy->weightOne +
         volumeRatio * policy->weightTwo +
         liquidity * policy->weightThree +
         spreadQuality * policy->weightFour) / weight);
}

UmiStatus umi_trading_professional_evaluate_policy(
    const UmiTradingProfessionalPolicy *policy,
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    double mid;
    double spread;
    double denominator;
    int prerequisites;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_trading_professional_snapshot_init(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label
            : policy->label);
    outSnapshot->revision = input->revision;

    mid = input->bid > 0.0 && input->ask > 0.0
        ? (input->bid + input->ask) * 0.5
        : input->last;
    spread = input->ask >= input->bid && input->bid > 0.0
        ? input->ask - input->bid
        : 0.0;
    prerequisites = required_flags_ready(policy->requiredFlags, input);

    switch (policy->kind) {
    case UMI_TRADING_POLICY_MIDPRICE:
        outSnapshot->value = mid;
        outSnapshot->ready = prerequisites && mid > 0.0;
        break;

    case UMI_TRADING_POLICY_SPREAD:
        outSnapshot->value = spread;
        outSnapshot->secondaryValue = mid;
        outSnapshot->ready = prerequisites && spread >= 0.0;
        break;

    case UMI_TRADING_POLICY_SPREAD_BPS:
        outSnapshot->value = mid > 0.0 ? (spread / mid) * 10000.0 : 0.0;
        outSnapshot->ready = prerequisites && mid > 0.0;
        outSnapshot->attention =
            policy->warning > 0.0 && outSnapshot->value >= policy->warning;
        break;

    case UMI_TRADING_POLICY_CHANGE:
        outSnapshot->value = input->last - input->previousClose;
        outSnapshot->ready = prerequisites && input->previousClose > 0.0;
        break;

    case UMI_TRADING_POLICY_CHANGE_PERCENT:
        outSnapshot->value = input->previousClose > 0.0
            ? ((input->last - input->previousClose) / input->previousClose) * 100.0
            : 0.0;
        outSnapshot->ready = prerequisites && input->previousClose > 0.0;
        break;

    case UMI_TRADING_POLICY_RANGE:
        outSnapshot->value =
            input->high >= input->low ? input->high - input->low : 0.0;
        outSnapshot->ready = prerequisites && input->high >= input->low;
        break;

    case UMI_TRADING_POLICY_RANGE_POSITION:
        denominator = input->high - input->low;
        outSnapshot->ratio = denominator > 0.0
            ? ((input->last - input->low) / denominator) * 100.0
            : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready = prerequisites && denominator > 0.0;
        break;

    case UMI_TRADING_POLICY_STALENESS:
        outSnapshot->value = (double)input->ageMilliseconds;
        outSnapshot->ready = prerequisites;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_TRADING_POLICY_HEALTH: {
        unsigned int readyCount = 0U;
        unsigned int expected = 4U;
        readyCount += input->marketDataReady ? 1U : 0U;
        readyCount += input->riskReady ? 1U : 0U;
        readyCount += input->healthReady ? 1U : 0U;
        readyCount += input->linkedContextReady ? 1U : 0U;
        outSnapshot->score = ((double)readyCount / (double)expected) * 100.0;
        outSnapshot->ready =
            prerequisites &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            !outSnapshot->ready &&
            outSnapshot->score < policy->warning;
        break;
    }

    case UMI_TRADING_POLICY_READINESS:
        outSnapshot->ready = prerequisites && !input->killSwitchEngaged;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_TRADING_POLICY_COUNT:
        outSnapshot->value = (double)input->count;
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_RATIO:
        outSnapshot->ratio = input->count > 0U
            ? ((double)input->visibleCount / (double)input->count) * 100.0
            : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            prerequisites &&
            (policy->threshold <= 0.0 ||
             outSnapshot->ratio >= policy->threshold);
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_TRADING_POLICY_CAPACITY:
        outSnapshot->ratio = input->capacity > 0U
            ? ((double)input->count / (double)input->capacity) * 100.0
            : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = prerequisites;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_TRADING_POLICY_SCORE:
        outSnapshot->score = workstation_score(policy, input);
        outSnapshot->ready =
            prerequisites &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_TRADING_POLICY_MOMENTUM:
        outSnapshot->value = input->momentumScore;
        outSnapshot->score = clamp_score(50.0 + input->momentumScore * 0.5);
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_VOLUME:
        outSnapshot->ratio = input->averageVolume > 0.0
            ? (input->volume / input->averageVolume) * 100.0
            : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio * 0.5);
        outSnapshot->ready = prerequisites;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_TRADING_POLICY_SPREAD_QUALITY:
        outSnapshot->score =
            mid > 0.0
                ? clamp_score(100.0 - (spread / mid) * 10000.0)
                : 0.0;
        outSnapshot->ready =
            prerequisites &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_TRADING_POLICY_CONTEXT:
        outSnapshot->ready = prerequisites;
        outSnapshot->score = outSnapshot->ready ? 100.0 : 0.0;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_TRADING_POLICY_IMBALANCE:
        outSnapshot->value = input->depthImbalance;
        outSnapshot->score =
            clamp_score(50.0 + input->depthImbalance * 50.0);
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_MICROPRICE:
        denominator = input->bidSize + input->askSize;
        outSnapshot->value = denominator > 0.0
            ? (input->ask * input->bidSize +
               input->bid * input->askSize) / denominator
            : mid;
        outSnapshot->ready = prerequisites && outSnapshot->value > 0.0;
        break;

    case UMI_TRADING_POLICY_LIQUIDITY:
        outSnapshot->value = input->topLiquidity;
        outSnapshot->secondaryValue =
            input->bidDepth + input->askDepth;
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_PRESSURE:
        denominator = input->bidDepth + input->askDepth;
        outSnapshot->ratio = denominator > 0.0
            ? ((input->bidDepth - input->askDepth) / denominator) * 100.0
            : input->depthImbalance * 100.0;
        outSnapshot->score =
            clamp_score(50.0 + outSnapshot->ratio * 0.5);
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_SPREAD_TICKS:
        outSnapshot->value = input->tickSize > 0.0
            ? spread / input->tickSize
            : 0.0;
        outSnapshot->ready = prerequisites && input->tickSize > 0.0;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_TRADING_POLICY_VOLATILITY:
        outSnapshot->value = input->volatilityScore;
        outSnapshot->score = clamp_score(input->volatilityScore);
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_RATE:
        outSnapshot->value = input->tradeRate;
        outSnapshot->ratio = input->averageTradeRate > 0.0
            ? (input->tradeRate / input->averageTradeRate) * 100.0
            : 0.0;
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_ACTIVITY:
        outSnapshot->value = input->tradeRate;
        outSnapshot->ready =
            prerequisites && (input->active || input->tradeRate > 0.0);
        outSnapshot->attention = prerequisites && !outSnapshot->ready;
        break;

    case UMI_TRADING_POLICY_ORDER_RISK:
        outSnapshot->notional =
            fabs(input->orderQuantity) *
            (input->referencePrice > 0.0 ? input->referencePrice : mid);
        outSnapshot->ratio = input->riskLimit > 0.0
            ? (outSnapshot->notional / input->riskLimit) * 100.0
            : 0.0;
        outSnapshot->blocked =
            !prerequisites || input->killSwitchEngaged ||
            !input->canPreviewOrder;
        outSnapshot->attention =
            outSnapshot->ratio >= 80.0 || outSnapshot->blocked;
        outSnapshot->ready = !outSnapshot->blocked;
        break;

    case UMI_TRADING_POLICY_NOTIONAL:
        outSnapshot->notional =
            fabs(input->orderQuantity) *
            (input->referencePrice > 0.0 ? input->referencePrice : mid);
        outSnapshot->value = outSnapshot->notional;
        outSnapshot->ready = prerequisites && outSnapshot->notional >= 0.0;
        break;

    case UMI_TRADING_POLICY_DISTANCE: {
        double target = input->limitPrice > 0.0
            ? input->limitPrice
            : input->stopPrice;
        double reference = input->referencePrice > 0.0
            ? input->referencePrice
            : mid;
        outSnapshot->value =
            reference > 0.0 && target > 0.0
                ? fabs(target - reference)
                : 0.0;
        outSnapshot->ratio =
            reference > 0.0
                ? (outSnapshot->value / reference) * 100.0
                : 0.0;
        outSnapshot->ready = prerequisites && reference > 0.0 && target > 0.0;
        break;
    }

    case UMI_TRADING_POLICY_UTILISATION: {
        double used = input->riskUsed > 0.0
            ? input->riskUsed
            : input->marginUsed > 0.0
                ? input->marginUsed
                : input->buyingPowerUsed;
        double limit = input->riskLimit > 0.0
            ? input->riskLimit
            : input->marginLimit > 0.0
                ? input->marginLimit
                : input->buyingPowerLimit;
        outSnapshot->ratio = limit > 0.0 ? (used / limit) * 100.0 : 0.0;
        outSnapshot->score = clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = prerequisites && limit > 0.0;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;
    }

    case UMI_TRADING_POLICY_PROGRESS:
        outSnapshot->ratio = input->orderedQuantity > 0.0
            ? (input->filledQuantity / input->orderedQuantity) * 100.0
            : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready = prerequisites;
        break;

    case UMI_TRADING_POLICY_POSITION_VALUE:
        outSnapshot->notional =
            fabs(input->positionQuantity) * input->marketPrice;
        outSnapshot->value = outSnapshot->notional;
        outSnapshot->ready = prerequisites && input->marketPrice > 0.0;
        break;

    case UMI_TRADING_POLICY_UNREALISED_PNL:
        outSnapshot->pnl =
            input->positionQuantity *
            (input->marketPrice - input->averageCost);
        outSnapshot->value = outSnapshot->pnl;
        outSnapshot->ready = prerequisites;
        outSnapshot->attention = outSnapshot->pnl < 0.0;
        break;

    case UMI_TRADING_POLICY_REALISED_PNL:
        outSnapshot->pnl = input->realisedPnl;
        outSnapshot->value = input->realisedPnl;
        outSnapshot->ready = prerequisites;
        outSnapshot->attention = input->realisedPnl < 0.0;
        break;

    case UMI_TRADING_POLICY_EXPOSURE:
        outSnapshot->value = input->grossExposure;
        outSnapshot->secondaryValue = input->netExposure;
        outSnapshot->ratio = input->riskLimit > 0.0
            ? (input->grossExposure / input->riskLimit) * 100.0
            : 0.0;
        outSnapshot->ready = prerequisites;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_TRADING_POLICY_ALERT:
        outSnapshot->value = input->alertValue;
        outSnapshot->secondaryValue = input->alertThreshold;
        outSnapshot->ready = prerequisites;
        outSnapshot->attention =
            input->unacknowledgedAlerts > 0U ||
            (input->alertThreshold != 0.0 &&
             input->alertValue >= input->alertThreshold);
        break;

    case UMI_TRADING_POLICY_EVENT:
        outSnapshot->value = input->eventImpact;
        outSnapshot->secondaryValue = input->minutesToEvent;
        outSnapshot->score = clamp_score(input->eventImpact);
        outSnapshot->ready = prerequisites;
        outSnapshot->attention =
            (policy->threshold > 0.0 &&
             input->eventImpact >= policy->threshold) ||
            (policy->warning > 0.0 &&
             input->minutesToEvent >= 0.0 &&
             input->minutesToEvent <= policy->warning);
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!prerequisites && policy->kind != UMI_TRADING_POLICY_ORDER_RISK) {
        outSnapshot->blocked = 1;
        outSnapshot->ready = 0;
    }

    return umi_trading_professional_snapshot_validate(outSnapshot);
}
