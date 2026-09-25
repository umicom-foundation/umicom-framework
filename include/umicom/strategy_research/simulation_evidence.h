/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/simulation_evidence.h
 *
 * PURPOSE:
 *   Retain bounded deterministic simulation evidence for fills, costs, latency
 *   and provenance without representing a live broker connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_SIMULATION_EVIDENCE_H
#define UMICOM_STRATEGY_RESEARCH_SIMULATION_EVIDENCE_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStrategySimulationEvidence {
    uint64_t requestedFills;
    uint64_t completedFills;
    uint64_t provenanceRecords;
    uint64_t sourceRecords;
    double orderedQuantity;
    double filledQuantity;
    double commission;
    double slippage;
    double totalLatencyMilliseconds;
    double maximumLatencyMilliseconds;
    double realisedPnl;
    uint64_t revision;
} UmiStrategySimulationEvidence;

typedef struct UmiStrategySimulationSnapshot {
    double fillRatio;
    double averageLatencyMilliseconds;
    double maximumLatencyMilliseconds;
    double commission;
    double slippage;
    double realisedPnl;
    double evidenceCoverage;
    uint64_t requestedFills;
    uint64_t completedFills;
    uint64_t revision;
} UmiStrategySimulationSnapshot;

void umi_strategy_simulation_evidence_init(
    UmiStrategySimulationEvidence *evidence);

UmiStatus umi_strategy_simulation_record_fill(
    UmiStrategySimulationEvidence *evidence,
    double orderedQuantity,
    double filledQuantity,
    double commission,
    double slippage,
    double latencyMilliseconds,
    double realisedPnl,
    int hasProvenance);

UmiStatus umi_strategy_simulation_snapshot(
    const UmiStrategySimulationEvidence *evidence,
    UmiStrategySimulationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
