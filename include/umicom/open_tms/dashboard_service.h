/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/dashboard_service.h
 *
 * PURPOSE:
 *   Compose treasury cash, liquidity, position, risk, settlement, accounting
 *   and reconciliation state into one operator dashboard snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_DASHBOARD_SERVICE_H
#define UMICOM_OPEN_TMS_DASHBOARD_SERVICE_H
#include "umicom/open_tms/cash_book.h"
#include "umicom/open_tms/liquidity_ladder.h"
#include "umicom/open_tms/position_book.h"
#include "umicom/open_tms/risk_book.h"
#include "umicom/open_tms/settlement_book.h"
#include "umicom/open_tms/accounting_book.h"
#include "umicom/open_tms/reconciliation_book.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiOpenTmsDashboardSnapshot {
 uint64_t revision;double cashHeadroom;double liquidityGap;double grossExposure;double netExposure;double unrealisedPnl;
 size_t riskBreaches;size_t settlementFailures;size_t overdueSettlements;size_t reconciliationBreaks;
 int cashReady;int liquidityReady;int positionReady;int riskReady;int settlementReady;int accountingReady;int reconciliationReady;int operationsReady;
} UmiOpenTmsDashboardSnapshot;
/* Validate all seven owners and finite aggregates before publishing. Failure
 * leaves outSnapshot byte-for-byte unchanged. Cash and position totals each
 * require a single currency label; no FX conversion or cross-metric addition
 * occurs. Liquidity has no currency field: the caller supplies consistent units.
 * Approved accounting is rechecked for per-currency balance. Ready flags retain
 * their existing local coverage meaning; they are not authority to move money.
 * Serialize all owners during capture, with output disjoint from them. Revision
 * remains the maximum child revision for compatibility; it is not a unique
 * change token or concurrency guard across independent books. */
UmiStatus umi_open_tms_dashboard_snapshot(
 const UmiOpenTmsCashBook *cash,const UmiOpenTmsLiquidityLadder *liquidity,
 const UmiOpenTmsPositionBook *positions,const UmiOpenTmsRiskBook *risk,
 const UmiOpenTmsSettlementBook *settlements,const UmiOpenTmsAccountingBook *accounting,
 const UmiOpenTmsReconciliationBook *reconciliation,uint64_t nowMilliseconds,
 UmiOpenTmsDashboardSnapshot *outSnapshot);
#ifdef __cplusplus
}
#endif
#endif
