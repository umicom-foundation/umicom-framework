/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_financial_control_service.c
 *
 * PURPOSE:
 *   Verify composition and snapshot of the shared financial control service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/financial_platform/financial_control_service.h"

int main(void)
{
    UmiFinancialControlService service;
    UmiFinancialControlSnapshot snapshot;
    UmiFinancialRateRecord rate = {0};
    UmiFinancialAuditRecord audit = {0};

    umi_financial_control_service_init(&service);
    (void)strcpy(rate.base, "GBP");
    (void)strcpy(rate.quote, "USD");
    rate.rate = 1.25;
    rate.eventMilliseconds = 1000U;
    assert(umi_financial_rate_book_upsert(&service.rates, &rate) == UMI_STATUS_OK);

    audit.sequence = 1U;
    (void)strcpy(audit.eventId, "event-1");
    (void)strcpy(audit.lineageId, "lineage-1");
    (void)strcpy(audit.actorId, "system");
    (void)strcpy(audit.domain, "currency");
    (void)strcpy(audit.entityId, "GBPUSD");
    (void)strcpy(audit.action, "rate");
    audit.status = UMI_STATUS_OK;
    audit.persisted = 1;
    assert(umi_financial_audit_journal_append(&service.audit, &audit) ==
           UMI_STATUS_OK);

    assert(umi_financial_control_service_snapshot(&service, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.rateCount == 1U);
    assert(snapshot.auditCount == 1U);
    assert(snapshot.auditReady);
    return 0;
}
