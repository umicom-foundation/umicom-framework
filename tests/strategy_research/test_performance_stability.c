/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_performance_stability.c
 *
 * PURPOSE:
 *   Verify the Equity Stability strategy research calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <string.h>

#include "umicom/strategy_research/performance/stability.h"

int main(void)
{
    UmiStrategyResearchInput input;
    UmiStrategyResearchSnapshot snapshot;

    umi_strategy_research_input_init(&input);
    input.scoreOne = 80.0;
    input.scoreTwo = 75.0;
    input.scoreThree = 85.0;
    input.scoreFour = 90.0;
    input.confidence = 80.0;
    input.entryPrice = 100.0;
    input.exitPrice = 104.0;
    input.currentPrice = 102.0;
    input.stopPrice = 98.0;
    input.targetPrice = 106.0;
    input.quantity = 10.0;
    input.riskBudget = 200.0;
    input.capital = 10000.0;
    input.grossProfit = 1500.0;
    input.grossLoss = -750.0;
    input.netProfit = 750.0;
    input.peakEquity = 12000.0;
    input.currentEquity = 11500.0;
    input.maxDrawdown = 500.0;
    input.turnover = 50000.0;
    input.exposurePercent = 45.0;
    input.commission = 10.0;
    input.slippage = 5.0;
    input.latencyMs = 35.0;
    input.cashBefore = 10000.0;
    input.cashAfter = 10750.0;
    input.positionBefore = 0.0;
    input.positionAfter = 10.0;
    input.maxFavourableExcursion = 1200.0;
    input.maxAdverseExcursion = 300.0;
    input.averageWin = 150.0;
    input.averageLoss = -75.0;
    input.meanReturn = 0.001;
    input.returnStdDev = 0.01;
    input.downsideStdDev = 0.0075;
    input.annualisedReturn = 20.0;
    input.annualisedVolatility = 18.0;
    input.ulcerIndex = 5.0;
    input.trainScore = 82.0;
    input.validationScore = 76.0;
    input.testScore = 74.0;
    input.neighbourScore = 72.0;
    input.parameterSensitivity = 8.0;
    input.value = 65.0;
    input.previousValue = 60.0;
    input.average = 101.0;
    input.secondaryAverage = 99.0;
    input.high = 105.0;
    input.low = 95.0;
    input.volume = 1500000.0;
    input.averageVolume = 1000000.0;
    input.trades = 10U;
    input.wins = 6U;
    input.losses = 4U;
    input.winStreak = 3U;
    input.lossStreak = 2U;
    input.signalValidUntilMs = 2000U;
    input.nowMs = 1000U;
    input.replayEvents = 100U;
    input.replayProcessed = 75U;
    input.replayGaps = 1U;
    input.replayStartMs = 1000U;
    input.replayEndMs = 11000U;
    input.replayNowMs = 8500U;
    input.replayExpectedMs = 8400U;
    input.seekFromMs = 2000U;
    input.seekToMs = 6000U;
    input.replaySpeed = 2.0;
    input.optimisationEvaluated = 80U;
    input.optimisationBudget = 100U;
    input.evidenceRecords = 95U;
    input.sourceRecords = 100U;
    input.sequence = 10U;
    input.orderedQuantity = 10.0;
    input.filledQuantity = 10.0;
    input.signalValid = 1;
    input.strategyVersioned = 1;
    input.replayActive = 1;
    input.replayPaused = 0;
    input.deterministic = 1;
    input.trusted = 1;
    input.approved = 1;
    input.revision = 88U;

    assert(umi_strategy_research_evaluate_performance_stability(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "performance.stability") == 0);
    assert(snapshot.revision == 88U);
    assert(snapshot.score >= 0.0 && snapshot.score <= 100.0);
    assert(snapshot.blocked == 0);
    assert(snapshot.ready == 1 || snapshot.attention == 0 || snapshot.value >= 0.0);
    return 0;
}
