/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/financial_control_service.h
 *
 * PURPOSE:
 *   Compose shared financial reference, lifecycle, ledger, settlement,
 *   reconciliation, workflow and audit controls into one Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_FINANCIAL_CONTROL_SERVICE_H
#define UMICOM_FINANCIAL_PLATFORM_FINANCIAL_CONTROL_SERVICE_H

#include "umicom/financial_platform/currency_rate_book.h"
#include "umicom/financial_platform/relationship_store.h"
#include "umicom/financial_platform/instrument_link_store.h"
#include "umicom/financial_platform/trade_control_book.h"
#include "umicom/financial_platform/position_control_book.h"
#include "umicom/financial_platform/payment_control_book.h"
#include "umicom/financial_platform/ledger_batch.h"
#include "umicom/financial_platform/settlement_book.h"
#include "umicom/financial_platform/reconciliation_run.h"
#include "umicom/financial_platform/workflow_instance.h"
#include "umicom/financial_platform/financial_audit_journal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFinancialControlService {
    UmiFinancialRateBook rates;
    UmiFinancialRelationshipStore relationships;
    UmiFinancialInstrumentLinkStore instruments;
    UmiFinancialTradeControlBook trades;
    UmiFinancialPositionControlBook positions;
    UmiFinancialPaymentControlBook payments;
    UmiFinancialSettlementBook settlements;
    UmiFinancialAuditJournal audit;
    uint64_t revision;
} UmiFinancialControlService;

typedef struct UmiFinancialControlSnapshot {
    size_t rateCount;
    size_t relationshipCount;
    size_t instrumentCount;
    size_t tradeCount;
    size_t positionCount;
    size_t paymentCount;
    size_t settlementCount;
    size_t auditCount;
    uint64_t auditGapCount;
    size_t persistedAuditCount;
    uint64_t revision;
    int auditReady;
} UmiFinancialControlSnapshot;

void umi_financial_control_service_init(UmiFinancialControlService *service);
UmiStatus umi_financial_control_service_snapshot(
    const UmiFinancialControlService *service,
    UmiFinancialControlSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
