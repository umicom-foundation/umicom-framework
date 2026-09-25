/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/audit_journal.c
 *
 * PURPOSE:
 *   Implement monotonic bounded execution-control audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/audit_journal.h"

#include <string.h>

void umi_broker_audit_journal_init(UmiBrokerAuditJournal *journal)
{
    if (journal == NULL) return;
    (void)memset(journal, 0, sizeof(*journal));
    journal->revision = 1U;
}

UmiStatus umi_broker_audit_journal_append(
    UmiBrokerAuditJournal *journal,
    const UmiBrokerAuditRecord *record)
{
    uint64_t expected;

    if (journal == NULL || record == NULL || record->sequence == 0U ||
        record->eventId[0] == '\0' || record->source[0] == '\0' ||
        record->kind[0] == '\0' || record->entityId[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (journal->count >= UMI_BROKER_AUDIT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    expected = journal->count == 0U
        ? record->sequence
        : journal->records[journal->count - 1U].sequence + 1U;
    if (record->sequence < expected) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (record->sequence > expected) {
        journal->gapCount += record->sequence - expected;
    }
    journal->records[journal->count++] = *record;
    journal->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_audit_journal_mark_persisted(
    UmiBrokerAuditJournal *journal,
    uint64_t sequence)
{
    size_t index;
    if (journal == NULL || sequence == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < journal->count; ++index) {
        if (journal->records[index].sequence == sequence) {
            journal->records[index].persisted = 1;
            journal->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_broker_audit_journal_persisted_count(
    const UmiBrokerAuditJournal *journal)
{
    size_t index;
    size_t count = 0U;
    if (journal == NULL) return 0U;
    for (index = 0U; index < journal->count; ++index) {
        if (journal->records[index].persisted) count += 1U;
    }
    return count;
}
