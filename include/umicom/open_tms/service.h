/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/service.h
 *
 * PURPOSE:
 *   Discover Open TMS controls and compose one treasury operational snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_SERVICE_H
#define UMICOM_OPEN_TMS_SERVICE_H
#include <stddef.h>
#include "umicom/open_tms/types.h"
#include "umicom/open_tms/fx_forward_book.h"
#include "umicom/open_tms/cash_book.h"
#include "umicom/open_tms/liquidity_ladder.h"
#include "umicom/open_tms/position_book.h"
#include "umicom/open_tms/risk_book.h"
#include "umicom/open_tms/settlement_book.h"
#include "umicom/open_tms/accounting_book.h"
#include "umicom/open_tms/reconciliation_book.h"
#include "umicom/open_tms/dashboard_service.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiOpenTmsService {
    UmiOpenTmsFxForwardBook forwards;
    UmiOpenTmsCashBook cash;
    UmiOpenTmsLiquidityLadder liquidity;
    UmiOpenTmsPositionBook positions;
    UmiOpenTmsRiskBook risk;
    UmiOpenTmsSettlementBook settlements;
    UmiOpenTmsAccountingBook accounting;
    UmiOpenTmsReconciliationBook reconciliation;
    uint64_t revision;
} UmiOpenTmsService;

typedef struct UmiOpenTmsServiceSnapshot {
    uint64_t revision;
    size_t policyCount;
    size_t forwardCount;
    size_t cashCount;
    size_t liquidityBucketCount;
    size_t positionCount;
    size_t riskLimitCount;
    size_t settlementCount;
    size_t reconciliationCount;
    UmiOpenTmsDashboardSnapshot dashboard;
} UmiOpenTmsServiceSnapshot;

void umi_open_tms_service_init(UmiOpenTmsService *service);
size_t umi_open_tms_service_count(void);
UmiStatus umi_open_tms_service_id_at(size_t index,const char **outId);
UmiStatus umi_open_tms_service_evaluate(
    const char *id,const UmiOpenTmsInput *input,UmiOpenTmsSnapshot *outSnapshot);
UmiStatus umi_open_tms_service_snapshot(
    const UmiOpenTmsService *service,uint64_t nowMilliseconds,
    UmiOpenTmsServiceSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
