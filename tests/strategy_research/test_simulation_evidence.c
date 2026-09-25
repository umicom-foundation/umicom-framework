/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_simulation_evidence.c
 *
 * PURPOSE:
 *   Verify deterministic fill, cost, latency and provenance evidence aggregation.
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

#include "umicom/strategy_research/simulation_evidence.h"

int main(void)
{
    UmiStrategySimulationEvidence evidence;
    UmiStrategySimulationSnapshot snapshot;

    umi_strategy_simulation_evidence_init(&evidence);
    assert(umi_strategy_simulation_record_fill(
               &evidence, 10.0, 10.0, 1.0, 0.5, 20.0, 25.0, 1) ==
           UMI_STATUS_OK);
    assert(umi_strategy_simulation_record_fill(
               &evidence, 10.0, 5.0, 1.0, 0.75, 40.0, -10.0, 0) ==
           UMI_STATUS_OK);
    assert(umi_strategy_simulation_snapshot(&evidence, &snapshot) ==
           UMI_STATUS_OK);

    assert(snapshot.fillRatio > 74.9 && snapshot.fillRatio < 75.1);
    assert(snapshot.averageLatencyMilliseconds > 29.9 &&
           snapshot.averageLatencyMilliseconds < 30.1);
    assert(snapshot.maximumLatencyMilliseconds == 40.0);
    assert(fabs(snapshot.realisedPnl - 15.0) < 0.001);
    assert(snapshot.evidenceCoverage > 49.9 &&
           snapshot.evidenceCoverage < 50.1);
    return 0;
}
