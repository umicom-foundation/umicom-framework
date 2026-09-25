/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/dashboard_service.c
 *
 * PURPOSE:
 *   Compose one bounded Open TMS operational snapshot over Framework-owned data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/dashboard_service.h"
#include <string.h>

UmiStatus umi_open_tms_dashboard_snapshot(
    const UmiOpenTmsCashBook *cash,
    const UmiOpenTmsLiquidityLadder *liquidity,
    const UmiOpenTmsPositionBook *positions,
    const UmiOpenTmsRiskBook *risk,
    const UmiOpenTmsSettlementBook *settlements,
    const UmiOpenTmsAccountingBook *accounting,
    const UmiOpenTmsReconciliationBook *reconciliation,
    uint64_t nowMilliseconds,
    UmiOpenTmsDashboardSnapshot *outSnapshot)
{
    uint64_t revision = 0U;
    size_t index;

    if (cash == NULL || liquidity == NULL || positions == NULL || risk == NULL ||
        settlements == NULL || accounting == NULL || reconciliation == NULL ||
        outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    for (index = 0U; index < cash->count; ++index) {
        outSnapshot->cashHeadroom +=
            umi_open_tms_cash_record_headroom(&cash->records[index]);
    }
    outSnapshot->liquidityGap = umi_open_tms_liquidity_ladder_gap(liquidity);
    outSnapshot->grossExposure = umi_open_tms_position_book_gross(positions);
    outSnapshot->netExposure = umi_open_tms_position_book_net(positions);
    outSnapshot->unrealisedPnl =
        umi_open_tms_position_book_unrealised_pnl(positions);
    outSnapshot->riskBreaches = umi_open_tms_risk_book_breach_count(risk);
    outSnapshot->settlementFailures =
        umi_open_tms_settlement_book_failed_count(settlements);
    outSnapshot->overdueSettlements =
        umi_open_tms_settlement_book_overdue_count(
            settlements, nowMilliseconds);
    outSnapshot->reconciliationBreaks = reconciliation->breakCount;

    outSnapshot->cashReady = cash->count > 0U;
    outSnapshot->liquidityReady = liquidity->count > 0U;
    outSnapshot->positionReady = positions->count > 0U;
    outSnapshot->riskReady = risk->count > 0U &&
        outSnapshot->riskBreaches == 0U;
    outSnapshot->settlementReady = settlements->count > 0U &&
        outSnapshot->settlementFailures == 0U &&
        outSnapshot->overdueSettlements == 0U;
    outSnapshot->accountingReady = accounting->approved && accounting->posted;
    outSnapshot->reconciliationReady = reconciliation->count > 0U &&
        reconciliation->breakCount == 0U;
    outSnapshot->operationsReady =
        outSnapshot->cashReady &&
        outSnapshot->liquidityReady &&
        outSnapshot->positionReady &&
        outSnapshot->riskReady &&
        outSnapshot->settlementReady &&
        outSnapshot->accountingReady &&
        outSnapshot->reconciliationReady;

    if (cash->revision > revision) revision = cash->revision;
    if (liquidity->revision > revision) revision = liquidity->revision;
    if (positions->revision > revision) revision = positions->revision;
    if (risk->revision > revision) revision = risk->revision;
    if (settlements->revision > revision) revision = settlements->revision;
    if (accounting->revision > revision) revision = accounting->revision;
    if (reconciliation->revision > revision) {
        revision = reconciliation->revision;
    }
    outSnapshot->revision = revision;
    return UMI_STATUS_OK;
}
