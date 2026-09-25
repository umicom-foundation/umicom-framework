/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/service.h
 *
 * PURPOSE:
 *   Discover strategy/backtest/replay/optimisation/study/simulation research
 *   capabilities and project canonical trading workspace readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_SERVICE_H
#define UMICOM_STRATEGY_RESEARCH_SERVICE_H

#include <stddef.h>

#include "umicom/strategy_research/types.h"
#include "umicom/trading/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStrategyResearchWorkspaceSnapshot {
    uint64_t revision;
    char selectedInstrumentId[UMI_FINANCE_ID_CAPACITY];

    size_t selectedBarCount;
    size_t retainedTradeCount;
    size_t visibleTradeCount;

    int marketDataReady;
    int riskReady;
    int healthReady;
    int strategyReady;
    int replayReady;
    int studiesReady;
    int simulationReady;
    int optimisationReady;
} UmiStrategyResearchWorkspaceSnapshot;

size_t umi_strategy_research_service_count(void);

UmiStatus umi_strategy_research_service_id_at(
    size_t index,
    const char **outId);

UmiStatus umi_strategy_research_service_evaluate(
    const char *id,
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

UmiStatus umi_strategy_research_workspace_snapshot(
    UmiTradingWorkspace *workspace,
    UmiStrategyResearchWorkspaceSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
