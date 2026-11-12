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

/* Retained for review: the former coverage snapshot trusted child counts and
 * cleared output before evidence admission. The active composition below checks
 * storage limits and publishes one complete local observation. */
#if 0
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

#endif

/* This composition reports bounded coverage, not domain authorization. Admit
 * all storage counts before publishing them and use the checked journal query
 * so corrupt evidence cannot silently become a successful zero. */
UmiStatus umi_financial_control_service_snapshot(const UmiFinancialControlService *service,
    UmiFinancialControlSnapshot *outSnapshot)
{
    if (service == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->rates.count > UMI_FINANCIAL_RATE_CAPACITY ||
        service->relationships.count > UMI_FINANCIAL_RELATIONSHIP_CAPACITY ||
        service->instruments.count > UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY ||
        service->trades.count > UMI_FINANCIAL_TRADE_CONTROL_CAPACITY ||
        service->positions.count > UMI_FINANCIAL_POSITION_CONTROL_CAPACITY ||
        service->payments.count > UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY ||
        service->settlements.count > UMI_FINANCIAL_SETTLEMENT_CAPACITY ||
        service->audit.count > UMI_FINANCIAL_AUDIT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t revisions[] = {service->revision, service->rates.revision, service->relationships.revision,
        service->instruments.revision, service->trades.revision, service->positions.revision,
        service->payments.revision, service->settlements.revision, service->audit.revision};
    UmiFinancialControlSnapshot value = {0};
    for (size_t i = 0U; i < sizeof revisions / sizeof revisions[0]; ++i) {
        if (revisions[i] == 0U) return UMI_STATUS_INVALID_STATE;
        if (revisions[i] > value.revision) value.revision = revisions[i];
    }
    UmiStatus status = umi_financial_rate_book_validate(&service->rates);
    if (status == UMI_STATUS_OK) status = umi_financial_relationship_store_validate(&service->relationships);
    if (status == UMI_STATUS_OK) status = umi_financial_instrument_link_store_validate(&service->instruments);
    if (status == UMI_STATUS_OK) status = umi_financial_trade_control_book_validate(&service->trades);
    if (status == UMI_STATUS_OK) status = umi_financial_position_control_book_validate(&service->positions);
    if (status == UMI_STATUS_OK) status = umi_financial_payment_control_book_validate(&service->payments);
    if (status == UMI_STATUS_OK) status = umi_financial_settlement_book_validate(&service->settlements);
    if (status == UMI_STATUS_OK)
        status = umi_financial_audit_journal_persisted_count_checked(&service->audit, &value.persistedAuditCount);
    if (status != UMI_STATUS_OK) return status;
    value.rateCount = service->rates.count;
    value.relationshipCount = service->relationships.count;
    value.instrumentCount = service->instruments.count;
    value.tradeCount = service->trades.count;
    value.positionCount = service->positions.count;
    value.paymentCount = service->payments.count;
    value.settlementCount = service->settlements.count;
    value.auditCount = service->audit.count;
    value.auditGapCount = service->audit.gapCount;
    value.auditReady = value.auditCount > 0U && value.auditGapCount == 0U &&
        value.persistedAuditCount == value.auditCount;
    *outSnapshot = value;
    return UMI_STATUS_OK;
}
