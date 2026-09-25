/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_audit_journal.c
 *
 * PURPOSE:
 *   Verify audit sequence, gap and persistence evidence.
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
#include "umicom/broker_connectivity/audit_journal.h"

static UmiBrokerAuditRecord record(uint64_t sequence)
{
    UmiBrokerAuditRecord value = {0};
    value.sequence = sequence;
    (void)strcpy(value.eventId, "event");
    (void)strcpy(value.source, "paper");
    (void)strcpy(value.kind, "order");
    (void)strcpy(value.entityId, "order-1");
    value.status = UMI_STATUS_OK;
    return value;
}

int main(void)
{
    UmiBrokerAuditJournal journal;
    UmiBrokerAuditRecord one = record(1U);
    UmiBrokerAuditRecord three = record(3U);

    umi_broker_audit_journal_init(&journal);
    assert(umi_broker_audit_journal_append(&journal, &one) == UMI_STATUS_OK);
    assert(umi_broker_audit_journal_append(&journal, &three) == UMI_STATUS_OK);
    assert(journal.gapCount == 1U);
    assert(umi_broker_audit_journal_mark_persisted(&journal, 1U) == UMI_STATUS_OK);
    assert(umi_broker_audit_journal_persisted_count(&journal) == 1U);
    return 0;
}
