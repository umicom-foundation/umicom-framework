/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/simulation_evidence.c
 *
 * PURPOSE:
 *   Implement bounded deterministic simulation evidence aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/simulation_evidence.h"

#include <string.h>

void umi_strategy_simulation_evidence_init(
    UmiStrategySimulationEvidence *evidence)
{
    if (evidence == NULL) return;
    (void)memset(evidence, 0, sizeof(*evidence));
    evidence->revision = 1U;
}

UmiStatus umi_strategy_simulation_record_fill(
    UmiStrategySimulationEvidence *evidence,
    double orderedQuantity,
    double filledQuantity,
    double commission,
    double slippage,
    double latencyMilliseconds,
    double realisedPnl,
    int hasProvenance)
{
    if (evidence == NULL || orderedQuantity <= 0.0 ||
        filledQuantity < 0.0 || filledQuantity > orderedQuantity ||
        commission < 0.0 || slippage < 0.0 ||
        latencyMilliseconds < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    evidence->requestedFills += 1U;
    if (filledQuantity > 0.0) evidence->completedFills += 1U;
    evidence->orderedQuantity += orderedQuantity;
    evidence->filledQuantity += filledQuantity;
    evidence->commission += commission;
    evidence->slippage += slippage;
    evidence->totalLatencyMilliseconds += latencyMilliseconds;
    if (latencyMilliseconds > evidence->maximumLatencyMilliseconds) {
        evidence->maximumLatencyMilliseconds = latencyMilliseconds;
    }
    evidence->realisedPnl += realisedPnl;
    evidence->sourceRecords += 1U;
    if (hasProvenance) evidence->provenanceRecords += 1U;
    evidence->revision =
        evidence->revision == UINT64_MAX ? 1U : evidence->revision + 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_simulation_snapshot(
    const UmiStrategySimulationEvidence *evidence,
    UmiStrategySimulationSnapshot *outSnapshot)
{
    if (evidence == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    outSnapshot->fillRatio =
        evidence->orderedQuantity > 0.0
            ? (evidence->filledQuantity / evidence->orderedQuantity) * 100.0
            : 0.0;
    outSnapshot->averageLatencyMilliseconds =
        evidence->requestedFills > 0U
            ? evidence->totalLatencyMilliseconds /
              (double)evidence->requestedFills
            : 0.0;
    outSnapshot->maximumLatencyMilliseconds =
        evidence->maximumLatencyMilliseconds;
    outSnapshot->commission = evidence->commission;
    outSnapshot->slippage = evidence->slippage;
    outSnapshot->realisedPnl = evidence->realisedPnl;
    outSnapshot->evidenceCoverage =
        evidence->sourceRecords > 0U
            ? ((double)evidence->provenanceRecords /
               (double)evidence->sourceRecords) * 100.0
            : 0.0;
    outSnapshot->requestedFills = evidence->requestedFills;
    outSnapshot->completedFills = evidence->completedFills;
    outSnapshot->revision = evidence->revision;
    return UMI_STATUS_OK;
}
