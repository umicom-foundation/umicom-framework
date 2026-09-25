/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_financial_audit_journal.c
 *
 * PURPOSE:
 *   Verify financial audit sequence gaps and persistence evidence.
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
#include "umicom/financial_platform/financial_audit_journal.h"

static UmiFinancialAuditRecord record(uint64_t sequence)
{
    UmiFinancialAuditRecord r = {0};
    r.sequence = sequence;
    (void)strcpy(r.eventId, "event");
    (void)strcpy(r.lineageId, "lineage");
    (void)strcpy(r.actorId, "system");
    (void)strcpy(r.domain, "trade");
    (void)strcpy(r.entityId, "trade-1");
    (void)strcpy(r.action, "confirm");
    r.status = UMI_STATUS_OK;
    return r;
}

int main(void)
{
    UmiFinancialAuditJournal journal;
    UmiFinancialAuditRecord one = record(1U);
    UmiFinancialAuditRecord three = record(3U);
    umi_financial_audit_journal_init(&journal);
    assert(umi_financial_audit_journal_append(&journal, &one) == UMI_STATUS_OK);
    assert(umi_financial_audit_journal_append(&journal, &three) == UMI_STATUS_OK);
    assert(journal.gapCount == 1U);
    assert(umi_financial_audit_journal_mark_persisted(&journal, 1U) ==
           UMI_STATUS_OK);
    assert(umi_financial_audit_journal_persisted_count(&journal) == 1U);
    return 0;
}
