/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_service_catalogue.c
 *
 * PURPOSE:
 *   Verify the complete strategy-research catalogue is discoverable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/strategy_research/service.h"

int main(void)
{
    UmiStrategyResearchInput input;
    UmiStrategyResearchSnapshot snapshot;
    size_t index;

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
    input.maxDrawdown = 500.0;
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
    input.signalValidUntilMs = 2000U;
    input.nowMs = 1000U;
    input.signalValid = 1;
    input.strategyVersioned = 1;
    input.replayActive = 1;
    input.deterministic = 1;
    input.trusted = 1;
    input.approved = 1;
    input.revision = 99U;

    assert(umi_strategy_research_service_count() == 80U);
    for (index = 0U; index < umi_strategy_research_service_count(); ++index) {
        const char *id = NULL;
        assert(umi_strategy_research_service_id_at(index, &id) == UMI_STATUS_OK);
        assert(id != NULL && id[0] != '\0');
        assert(umi_strategy_research_service_evaluate(
                   id, &input, &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.id, id) == 0);
        assert(snapshot.revision == 99U);
    }

    assert(umi_strategy_research_service_evaluate(
               "missing.capability", &input, &snapshot) ==
           UMI_STATUS_NOT_FOUND);
    return 0;
}
