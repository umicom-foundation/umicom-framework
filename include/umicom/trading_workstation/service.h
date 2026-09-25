/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/service.h
 *
 * PURPOSE:
 *   Discover professional trading-workstation calculations and project the
 *   canonical trading workspace into operator-facing readiness state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_SERVICE_H
#define UMICOM_TRADING_WORKSTATION_SERVICE_H

#include <stddef.h>

#include "umicom/trading/workspace.h"
#include "umicom/trading_workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingProfessionalWorkstationSnapshot {
    uint64_t revision;
    size_t capabilityCount;
    size_t readyCount;
    size_t attentionCount;

    int watchlistReady;
    int depthReady;
    int chartReady;
    int timeAndSalesReady;
    int orderTicketReady;
    int portfolioReady;
    int alertsReady;
    int scannerReady;
    int calendarContextReady;
    int linkedContextReady;
    int professionalReady;

    double selectedMid;
    double selectedSpread;
    double selectedSpreadBasisPoints;
    double selectedDepthImbalance;
    double selectedTopLiquidity;
    double selectedChangePercent;
    double realisedPnl;
} UmiTradingProfessionalWorkstationSnapshot;

size_t umi_trading_professional_service_count(void);

UmiStatus umi_trading_professional_service_id_at(
    size_t index,
    const char **outId);

UmiStatus umi_trading_professional_service_evaluate(
    const char *id,
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

UmiStatus umi_trading_professional_workstation_snapshot(
    UmiTradingWorkspace *workspace,
    UmiTradingProfessionalWorkstationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
