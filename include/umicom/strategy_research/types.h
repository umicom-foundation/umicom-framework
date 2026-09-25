/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/types.h
 *
 * PURPOSE:
 *   Define deterministic strategy, backtest, replay, optimisation, study and
 *   simulation evidence for Trader, Studio and future financial applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_TYPES_H
#define UMICOM_STRATEGY_RESEARCH_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STRATEGY_RESEARCH_ID_CAPACITY 96U
#define UMI_STRATEGY_RESEARCH_LABEL_CAPACITY 128U
#define UMI_STRATEGY_RESEARCH_PARAMETER_CAPACITY 8U

typedef struct UmiStrategyResearchInput {
    const char *label;

    double scoreOne;
    double scoreTwo;
    double scoreThree;
    double scoreFour;
    double confidence;

    double entryPrice;
    double exitPrice;
    double currentPrice;
    double orderedQuantity;
    double filledQuantity;
    double stopPrice;
    double targetPrice;
    double quantity;
    double riskBudget;
    double capital;

    double grossProfit;
    double grossLoss;
    double netProfit;
    double peakEquity;
    double currentEquity;
    double maxDrawdown;
    double turnover;
    double exposurePercent;
    double commission;
    double slippage;
    double latencyMs;
    double cashBefore;
    double cashAfter;
    double positionBefore;
    double positionAfter;
    double maxFavourableExcursion;
    double maxAdverseExcursion;

    double averageWin;
    double averageLoss;
    double meanReturn;
    double returnStdDev;
    double downsideStdDev;
    double annualisedReturn;
    double annualisedVolatility;
    double ulcerIndex;

    double trainScore;
    double validationScore;
    double testScore;
    double neighbourScore;
    double parameterSensitivity;

    double value;
    double previousValue;
    double average;
    double secondaryAverage;
    double high;
    double low;
    double volume;
    double averageVolume;

    uint64_t trades;
    uint64_t wins;
    uint64_t losses;
    uint64_t winStreak;
    uint64_t lossStreak;

    uint64_t signalValidUntilMs;
    uint64_t nowMs;

    uint64_t replayEvents;
    uint64_t replayProcessed;
    uint64_t replayGaps;
    uint64_t replayStartMs;
    uint64_t replayEndMs;
    uint64_t replayNowMs;
    uint64_t replayExpectedMs;
    uint64_t seekFromMs;
    uint64_t seekToMs;
    double replaySpeed;

    uint64_t optimisationEvaluated;
    uint64_t optimisationBudget;

    uint64_t evidenceRecords;
    uint64_t sourceRecords;
    uint64_t sequence;

    int signalValid;
    int strategyVersioned;
    int replayActive;
    int replayPaused;
    int deterministic;
    int trusted;
    int approved;

    uint64_t revision;
} UmiStrategyResearchInput;

typedef struct UmiStrategyResearchSnapshot {
    char id[UMI_STRATEGY_RESEARCH_ID_CAPACITY];
    char label[UMI_STRATEGY_RESEARCH_LABEL_CAPACITY];

    double value;
    double secondaryValue;
    double score;
    double ratio;
    double pnl;
    double risk;
    double delta;

    int ready;
    int attention;
    int blocked;
    uint64_t revision;
} UmiStrategyResearchSnapshot;

typedef UmiStatus (*UmiStrategyResearchEvaluator)(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

void umi_strategy_research_input_init(UmiStrategyResearchInput *input);
void umi_strategy_research_snapshot_init(UmiStrategyResearchSnapshot *snapshot);
UmiStatus umi_strategy_research_snapshot_validate(
    const UmiStrategyResearchSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
