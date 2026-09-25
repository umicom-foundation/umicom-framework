/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy_research_internal.c
 *
 * PURPOSE:
 *   Evaluate deterministic strategy, backtest, replay, optimisation, study and
 *   simulation evidence without granting order-execution authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "strategy_research_internal.h"

#include <math.h>
#include <stdio.h>

static double clamp_score(double value)
{
    if (value < 0.0) return 0.0;
    if (value > 100.0) return 100.0;
    return value;
}

static double positive_abs(double value)
{
    return value < 0.0 ? -value : value;
}

static double ratio_percent(double numerator, double denominator)
{
    if (denominator == 0.0) return 0.0;
    return (numerator / denominator) * 100.0;
}

static double weighted_score(
    const UmiStrategyResearchPolicy *policy,
    const UmiStrategyResearchInput *input)
{
    double total =
        policy->weightOne +
        policy->weightTwo +
        policy->weightThree +
        policy->weightFour;

    if (total <= 0.0) return 0.0;
    return clamp_score(
        (clamp_score(input->scoreOne) * policy->weightOne +
         clamp_score(input->scoreTwo) * policy->weightTwo +
         clamp_score(input->scoreThree) * policy->weightThree +
         clamp_score(input->scoreFour) * policy->weightFour) / total);
}

static double safe_profit_factor(const UmiStrategyResearchInput *input)
{
    double loss = positive_abs(input->grossLoss);
    if (loss == 0.0) return input->grossProfit > 0.0 ? 100.0 : 0.0;
    return input->grossProfit / loss;
}

static double safe_win_rate(const UmiStrategyResearchInput *input)
{
    uint64_t completed = input->wins + input->losses;
    return completed > 0U
        ? ((double)input->wins / (double)completed) * 100.0
        : 0.0;
}

static double expectancy(const UmiStrategyResearchInput *input)
{
    uint64_t completed = input->wins + input->losses;
    double winProbability;
    double lossProbability;

    if (completed == 0U) return 0.0;
    winProbability = (double)input->wins / (double)completed;
    lossProbability = (double)input->losses / (double)completed;
    return winProbability * input->averageWin -
        lossProbability * positive_abs(input->averageLoss);
}

static double annualisation_factor(void)
{
    return sqrt(252.0);
}

UmiStatus umi_strategy_research_evaluate_policy(
    const UmiStrategyResearchPolicy *policy,
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    double denominator;
    double winRate;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_strategy_research_snapshot_init(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label
            : policy->label);
    outSnapshot->revision = input->revision;

    switch (policy->kind) {
    case UMI_STRATEGY_POLICY_SCORE:
        outSnapshot->score = weighted_score(policy, input);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_CONFIDENCE:
        outSnapshot->score = clamp_score(input->confidence);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_ENTRY_GATE:
        outSnapshot->score = weighted_score(policy, input);
        outSnapshot->blocked =
            !input->trusted || !input->signalValid || !input->strategyVersioned;
        outSnapshot->ready =
            !outSnapshot->blocked &&
            input->confidence >= 50.0;
        outSnapshot->attention = outSnapshot->blocked;
        break;

    case UMI_STRATEGY_POLICY_EXIT_GATE:
        outSnapshot->score = weighted_score(policy, input);
        outSnapshot->ready =
            input->trusted &&
            input->strategyVersioned &&
            (input->signalValid == 0 ||
             input->currentPrice <= input->stopPrice ||
             input->currentPrice >= input->targetPrice);
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_STRATEGY_POLICY_POSITION_SIZE:
        if (input->riskBudget > 0.0 &&
            input->entryPrice > 0.0 &&
            input->stopPrice > 0.0) {
            denominator = positive_abs(input->entryPrice - input->stopPrice);
            outSnapshot->value =
                denominator > 0.0 ? input->riskBudget / denominator : 0.0;
        }
        outSnapshot->risk = input->capital > 0.0
            ? ratio_percent(input->riskBudget, input->capital)
            : 0.0;
        outSnapshot->ready =
            input->trusted && outSnapshot->value > 0.0;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->risk >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_STOP_DISTANCE:
        outSnapshot->value =
            input->stopPrice > 0.0
                ? positive_abs(input->currentPrice - input->stopPrice)
                : 0.0;
        outSnapshot->ready =
            input->currentPrice > 0.0 && input->stopPrice > 0.0;
        break;

    case UMI_STRATEGY_POLICY_TARGET_DISTANCE:
        outSnapshot->value =
            input->targetPrice > 0.0
                ? positive_abs(input->targetPrice - input->currentPrice)
                : 0.0;
        outSnapshot->ready =
            input->currentPrice > 0.0 && input->targetPrice > 0.0;
        break;

    case UMI_STRATEGY_POLICY_RISK_REWARD: {
        double risk =
            positive_abs(input->entryPrice - input->stopPrice);
        double reward =
            positive_abs(input->targetPrice - input->entryPrice);
        outSnapshot->ratio =
            risk > 0.0 ? reward / risk : 0.0;
        outSnapshot->score =
            clamp_score(outSnapshot->ratio * 40.0);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;
    }

    case UMI_STRATEGY_POLICY_EXPIRY:
        outSnapshot->value =
            input->signalValidUntilMs > input->nowMs
                ? (double)(input->signalValidUntilMs - input->nowMs)
                : 0.0;
        outSnapshot->ready =
            input->signalValid &&
            input->signalValidUntilMs >= input->nowMs;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_STRATEGY_POLICY_READINESS:
        outSnapshot->ready =
            input->trusted &&
            input->strategyVersioned &&
            input->deterministic;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_STRATEGY_POLICY_NET_PROFIT:
        outSnapshot->pnl = input->netProfit;
        outSnapshot->value = input->netProfit;
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention = input->netProfit < 0.0;
        break;

    case UMI_STRATEGY_POLICY_GROSS_PROFIT:
        outSnapshot->value = input->grossProfit;
        outSnapshot->ready = input->trades > 0U;
        break;

    case UMI_STRATEGY_POLICY_GROSS_LOSS:
        outSnapshot->value = positive_abs(input->grossLoss);
        outSnapshot->ready = input->trades > 0U;
        break;

    case UMI_STRATEGY_POLICY_WIN_RATE:
        winRate = safe_win_rate(input);
        outSnapshot->ratio = winRate;
        outSnapshot->score = clamp_score(winRate);
        outSnapshot->ready =
            input->trades > 0U &&
            winRate >= policy->threshold;
        outSnapshot->attention =
            winRate < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_AVG_WIN:
        outSnapshot->value = input->averageWin;
        outSnapshot->ready = input->wins > 0U;
        break;

    case UMI_STRATEGY_POLICY_AVG_LOSS:
        outSnapshot->value = positive_abs(input->averageLoss);
        outSnapshot->ready = input->losses > 0U;
        break;

    case UMI_STRATEGY_POLICY_PROFIT_FACTOR:
        outSnapshot->ratio = safe_profit_factor(input);
        outSnapshot->score =
            clamp_score(outSnapshot->ratio * 50.0);
        outSnapshot->ready =
            input->trades > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_EXPECTANCY:
        outSnapshot->value = expectancy(input);
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention = outSnapshot->value < policy->threshold;
        break;

    case UMI_STRATEGY_POLICY_MAX_DRAWDOWN:
        outSnapshot->value = positive_abs(input->maxDrawdown);
        outSnapshot->ratio = input->peakEquity > 0.0
            ? ratio_percent(outSnapshot->value, input->peakEquity)
            : 0.0;
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_RECOVERY_FACTOR:
        denominator = positive_abs(input->maxDrawdown);
        outSnapshot->ratio =
            denominator > 0.0 ? input->netProfit / denominator : 0.0;
        outSnapshot->score =
            clamp_score(outSnapshot->ratio * 50.0);
        outSnapshot->ready =
            input->trades > 0U &&
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_COUNT:
        outSnapshot->value = (double)input->trades;
        outSnapshot->ready = input->trades > 0U;
        break;

    case UMI_STRATEGY_POLICY_TURNOVER:
        outSnapshot->value = input->turnover;
        outSnapshot->ready = input->trades > 0U;
        break;

    case UMI_STRATEGY_POLICY_EXPOSURE:
        outSnapshot->ratio = input->exposurePercent;
        outSnapshot->score =
            clamp_score(100.0 - input->exposurePercent);
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            input->exposurePercent >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_STREAK:
        outSnapshot->value = (double)(
            input->lossStreak > 0U ? input->lossStreak : input->winStreak);
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            (double)input->lossStreak >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_SHARPE:
        outSnapshot->ratio =
            input->returnStdDev > 0.0
                ? (input->meanReturn / input->returnStdDev) *
                  annualisation_factor()
                : 0.0;
        outSnapshot->score =
            clamp_score(50.0 + outSnapshot->ratio * 20.0);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_SORTINO:
        outSnapshot->ratio =
            input->downsideStdDev > 0.0
                ? (input->meanReturn / input->downsideStdDev) *
                  annualisation_factor()
                : 0.0;
        outSnapshot->score =
            clamp_score(50.0 + outSnapshot->ratio * 20.0);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_CALMAR:
        outSnapshot->ratio =
            input->maxDrawdown > 0.0
                ? input->annualisedReturn /
                  positive_abs(input->maxDrawdown)
                : 0.0;
        outSnapshot->score =
            clamp_score(50.0 + outSnapshot->ratio * 20.0);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_ANNUALISED_RETURN:
        outSnapshot->value = input->annualisedReturn;
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention = input->annualisedReturn < 0.0;
        break;

    case UMI_STRATEGY_POLICY_ANNUALISED_VOL:
        outSnapshot->value = positive_abs(input->annualisedVolatility);
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->value);
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_DOWNSIDE_DEV:
        outSnapshot->value = positive_abs(input->downsideStdDev);
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->value);
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_PAYOFF:
        outSnapshot->ratio =
            positive_abs(input->averageLoss) > 0.0
                ? input->averageWin / positive_abs(input->averageLoss)
                : 0.0;
        outSnapshot->score =
            clamp_score(outSnapshot->ratio * 50.0);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_ULCER:
        outSnapshot->value = positive_abs(input->ulcerIndex);
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->value * 4.0);
        outSnapshot->ready = input->trades > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_EFFICIENCY:
        denominator =
            positive_abs(input->maxFavourableExcursion) +
            positive_abs(input->maxAdverseExcursion);
        outSnapshot->ratio =
            denominator > 0.0
                ? ratio_percent(
                    positive_abs(input->netProfit), denominator)
                : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_STABILITY:
        outSnapshot->score = weighted_score(policy, input);
        outSnapshot->ready =
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_PROGRESS:
        outSnapshot->ratio = input->replayEvents > 0U
            ? ((double)input->replayProcessed /
               (double)input->replayEvents) * 100.0
            : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->replayActive &&
            input->replayProcessed <= input->replayEvents;
        break;

    case UMI_STRATEGY_POLICY_SPEED:
        outSnapshot->value = input->replaySpeed;
        outSnapshot->ready =
            input->replayActive && input->replaySpeed > 0.0;
        break;

    case UMI_STRATEGY_POLICY_EVENT_RATE:
        denominator =
            input->replayEndMs > input->replayStartMs
                ? (double)(input->replayEndMs - input->replayStartMs) /
                  1000.0
                : 0.0;
        outSnapshot->value =
            denominator > 0.0
                ? (double)input->replayProcessed / denominator
                : 0.0;
        outSnapshot->ready = input->replayActive;
        break;

    case UMI_STRATEGY_POLICY_GAP_COUNT:
        outSnapshot->value = (double)input->replayGaps;
        outSnapshot->ready = input->replayActive;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            input->replayGaps >= (uint64_t)policy->warning;
        break;

    case UMI_STRATEGY_POLICY_GAP_RATIO:
        outSnapshot->ratio = input->replayProcessed > 0U
            ? ((double)input->replayGaps /
               (double)input->replayProcessed) * 100.0
            : 0.0;
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready = input->replayActive;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_CLOCK_DRIFT:
        outSnapshot->value =
            input->replayNowMs >= input->replayExpectedMs
                ? (double)(input->replayNowMs - input->replayExpectedMs)
                : (double)(input->replayExpectedMs - input->replayNowMs);
        outSnapshot->ready = input->replayActive;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_SEEK_DISTANCE:
        outSnapshot->value =
            input->seekToMs >= input->seekFromMs
                ? (double)(input->seekToMs - input->seekFromMs)
                : (double)(input->seekFromMs - input->seekToMs);
        outSnapshot->ready = input->deterministic;
        break;

    case UMI_STRATEGY_POLICY_DATA_QUALITY:
        outSnapshot->ratio = input->replayProcessed > 0U
            ? ((double)(input->replayProcessed - input->replayGaps) /
               (double)input->replayProcessed) * 100.0
            : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->deterministic &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_CHECKPOINT:
        outSnapshot->ready =
            input->deterministic &&
            input->replayActive &&
            input->sequence > 0U;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    case UMI_STRATEGY_POLICY_OBJECTIVE:
    case UMI_STRATEGY_POLICY_WALK_FORWARD:
    case UMI_STRATEGY_POLICY_ROBUSTNESS:
    case UMI_STRATEGY_POLICY_PARETO:
        outSnapshot->score = weighted_score(policy, input);
        outSnapshot->ready =
            input->trusted &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_RANK:
        outSnapshot->value = input->scoreOne;
        outSnapshot->score = clamp_score(input->scoreOne);
        outSnapshot->ready = input->optimisationEvaluated > 0U;
        break;

    case UMI_STRATEGY_POLICY_OVERFIT_GAP:
        outSnapshot->value =
            positive_abs(input->trainScore - input->validationScore);
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->value);
        outSnapshot->ready =
            input->optimisationEvaluated > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_SENSITIVITY:
        outSnapshot->value =
            positive_abs(input->parameterSensitivity);
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->value);
        outSnapshot->ready =
            input->optimisationEvaluated > 0U;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_EARLY_STOP:
        outSnapshot->ready =
            input->optimisationBudget > 0U &&
            input->optimisationEvaluated >= input->optimisationBudget;
        outSnapshot->attention =
            !outSnapshot->ready &&
            input->validationScore < 20.0;
        break;

    case UMI_STRATEGY_POLICY_BUDGET:
        outSnapshot->ratio = input->optimisationBudget > 0U
            ? ((double)input->optimisationEvaluated /
               (double)input->optimisationBudget) * 100.0
            : 0.0;
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->ratio);
        outSnapshot->ready =
            input->optimisationBudget > 0U &&
            input->optimisationEvaluated <= input->optimisationBudget;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->ratio >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_SMA:
        outSnapshot->value = input->average;
        outSnapshot->ready = input->average != 0.0;
        break;

    case UMI_STRATEGY_POLICY_EMA:
        outSnapshot->value = input->secondaryAverage;
        outSnapshot->ready = input->secondaryAverage != 0.0;
        break;

    case UMI_STRATEGY_POLICY_RSI:
        outSnapshot->value = input->value;
        outSnapshot->score = clamp_score(input->value);
        outSnapshot->ready =
            input->value >= 0.0 && input->value <= 100.0;
        outSnapshot->attention =
            input->value >= 70.0 || input->value <= 30.0;
        break;

    case UMI_STRATEGY_POLICY_ATR:
        outSnapshot->value = positive_abs(input->value);
        outSnapshot->ready = input->value >= 0.0;
        break;

    case UMI_STRATEGY_POLICY_VWAP:
        outSnapshot->value = input->average;
        outSnapshot->ready =
            input->average != 0.0 && input->volume > 0.0;
        break;

    case UMI_STRATEGY_POLICY_BOLLINGER:
        denominator = positive_abs(input->secondaryAverage);
        outSnapshot->value =
            denominator > 0.0
                ? (input->value - input->average) / denominator
                : 0.0;
        outSnapshot->ready = denominator > 0.0;
        outSnapshot->attention =
            positive_abs(outSnapshot->value) >= 2.0;
        break;

    case UMI_STRATEGY_POLICY_MACD:
        outSnapshot->value = input->average - input->secondaryAverage;
        outSnapshot->ready =
            input->average != 0.0 ||
            input->secondaryAverage != 0.0;
        break;

    case UMI_STRATEGY_POLICY_ROC:
        outSnapshot->value =
            input->previousValue != 0.0
                ? ((input->value - input->previousValue) /
                   input->previousValue) * 100.0
                : 0.0;
        outSnapshot->ready = input->previousValue != 0.0;
        break;

    case UMI_STRATEGY_POLICY_VOLATILITY:
        outSnapshot->value =
            positive_abs(input->returnStdDev) *
            annualisation_factor() * 100.0;
        outSnapshot->score =
            clamp_score(100.0 - outSnapshot->value);
        outSnapshot->ready = input->returnStdDev >= 0.0;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_RANGE:
        outSnapshot->value =
            input->high >= input->low ? input->high - input->low : 0.0;
        outSnapshot->ready = input->high >= input->low;
        break;

    case UMI_STRATEGY_POLICY_CROSSOVER:
        outSnapshot->value = input->average - input->secondaryAverage;
        outSnapshot->ready =
            input->average != 0.0 ||
            input->secondaryAverage != 0.0;
        break;

    case UMI_STRATEGY_POLICY_TREND:
        outSnapshot->score = weighted_score(policy, input);
        outSnapshot->ready =
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_SUPPORT_DISTANCE:
        outSnapshot->value =
            input->currentPrice >= input->low
                ? input->currentPrice - input->low
                : 0.0;
        outSnapshot->ready =
            input->currentPrice > 0.0 && input->low > 0.0;
        break;

    case UMI_STRATEGY_POLICY_RESISTANCE_DISTANCE:
        outSnapshot->value =
            input->high >= input->currentPrice
                ? input->high - input->currentPrice
                : 0.0;
        outSnapshot->ready =
            input->currentPrice > 0.0 && input->high > 0.0;
        break;

    case UMI_STRATEGY_POLICY_VOLUME_RATIO:
        outSnapshot->ratio =
            input->averageVolume > 0.0
                ? (input->volume / input->averageVolume) * 100.0
                : 0.0;
        outSnapshot->score =
            clamp_score(outSnapshot->ratio * 0.5);
        outSnapshot->ready = input->averageVolume > 0.0;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_SLIPPAGE:
        outSnapshot->value = positive_abs(input->slippage);
        outSnapshot->ready = input->deterministic;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_COMMISSION:
        outSnapshot->value = positive_abs(input->commission);
        outSnapshot->ready = input->deterministic;
        break;

    case UMI_STRATEGY_POLICY_FILL_RATIO:
        outSnapshot->ratio =
            input->orderedQuantity > 0.0
                ? (input->filledQuantity /
                   input->orderedQuantity) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            outSnapshot->ratio >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->ratio < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_LATENCY:
        outSnapshot->value = positive_abs(input->latencyMs);
        outSnapshot->ready = input->deterministic;
        outSnapshot->attention =
            policy->warning > 0.0 &&
            outSnapshot->value >= policy->warning;
        break;

    case UMI_STRATEGY_POLICY_TRADE_PNL:
        outSnapshot->pnl =
            (input->exitPrice - input->entryPrice) *
            input->quantity -
            positive_abs(input->commission) -
            positive_abs(input->slippage);
        outSnapshot->value = outSnapshot->pnl;
        outSnapshot->ready =
            input->entryPrice > 0.0 &&
            input->exitPrice > 0.0;
        outSnapshot->attention = outSnapshot->pnl < 0.0;
        break;

    case UMI_STRATEGY_POLICY_CASH_CHANGE:
        outSnapshot->value = input->cashAfter - input->cashBefore;
        outSnapshot->ready = input->deterministic;
        break;

    case UMI_STRATEGY_POLICY_POSITION_CHANGE:
        outSnapshot->value =
            input->positionAfter - input->positionBefore;
        outSnapshot->ready = input->deterministic;
        break;

    case UMI_STRATEGY_POLICY_NOTIONAL:
        outSnapshot->value =
            positive_abs(input->quantity) *
            input->currentPrice;
        outSnapshot->ready = input->currentPrice > 0.0;
        break;

    case UMI_STRATEGY_POLICY_EVIDENCE:
        outSnapshot->ratio =
            input->sourceRecords > 0U
                ? ((double)input->evidenceRecords /
                   (double)input->sourceRecords) * 100.0
                : 0.0;
        outSnapshot->score = clamp_score(outSnapshot->ratio);
        outSnapshot->ready =
            input->deterministic &&
            outSnapshot->score >= policy->threshold;
        outSnapshot->attention =
            outSnapshot->score < policy->warning;
        break;

    case UMI_STRATEGY_POLICY_PROVENANCE:
        outSnapshot->ready =
            input->deterministic &&
            input->trusted &&
            input->sourceRecords > 0U &&
            input->sequence > 0U;
        outSnapshot->attention = !outSnapshot->ready;
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!input->trusted &&
        policy->kind != UMI_STRATEGY_POLICY_GROSS_PROFIT &&
        policy->kind != UMI_STRATEGY_POLICY_GROSS_LOSS &&
        policy->kind != UMI_STRATEGY_POLICY_NET_PROFIT) {
        outSnapshot->blocked = 1;
        outSnapshot->ready = 0;
    }

    return umi_strategy_research_snapshot_validate(outSnapshot);
}
