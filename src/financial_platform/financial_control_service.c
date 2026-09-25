/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/financial_control_service.c
 *
 * PURPOSE:
 *   Initialise and inspect the shared financial control-plane composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/financial_control_service.h"

#include <string.h>

void umi_financial_control_service_init(UmiFinancialControlService *service)
{
    if (service == NULL) return;
    (void)memset(service, 0, sizeof(*service));
    umi_financial_rate_book_init(&service->rates);
    umi_financial_relationship_store_init(&service->relationships);
    umi_financial_instrument_link_store_init(&service->instruments);
    umi_financial_trade_control_book_init(&service->trades);
    umi_financial_position_control_book_init(&service->positions);
    umi_financial_payment_control_book_init(&service->payments);
    umi_financial_settlement_book_init(&service->settlements);
    umi_financial_audit_journal_init(&service->audit);
    service->revision = 1U;
}

UmiStatus umi_financial_control_service_snapshot(
    const UmiFinancialControlService *service,
    UmiFinancialControlSnapshot *outSnapshot)
{
    uint64_t revision;
    if (service == NULL || outSnapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    outSnapshot->rateCount = service->rates.count;
    outSnapshot->relationshipCount = service->relationships.count;
    outSnapshot->instrumentCount = service->instruments.count;
    outSnapshot->tradeCount = service->trades.count;
    outSnapshot->positionCount = service->positions.count;
    outSnapshot->paymentCount = service->payments.count;
    outSnapshot->settlementCount = service->settlements.count;
    outSnapshot->auditCount = service->audit.count;
    outSnapshot->auditGapCount = service->audit.gapCount;
    outSnapshot->persistedAuditCount =
        umi_financial_audit_journal_persisted_count(&service->audit);

    revision = service->revision;
    if (service->rates.revision > revision) revision = service->rates.revision;
    if (service->relationships.revision > revision) revision = service->relationships.revision;
    if (service->instruments.revision > revision) revision = service->instruments.revision;
    if (service->trades.revision > revision) revision = service->trades.revision;
    if (service->positions.revision > revision) revision = service->positions.revision;
    if (service->payments.revision > revision) revision = service->payments.revision;
    if (service->settlements.revision > revision) revision = service->settlements.revision;
    if (service->audit.revision > revision) revision = service->audit.revision;
    outSnapshot->revision = revision;
    outSnapshot->auditReady =
        service->audit.count > 0U &&
        service->audit.gapCount == 0U &&
        outSnapshot->persistedAuditCount == service->audit.count;
    return UMI_STATUS_OK;
}
