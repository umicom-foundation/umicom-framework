/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/financial_audit_journal.c
 *
 * PURPOSE:
 *   Implement immutable sequence-aware cross-domain financial audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/financial_audit_journal.h"

#include <string.h>

void umi_financial_audit_journal_init(UmiFinancialAuditJournal *journal)
{
    if (journal == NULL) return;
    (void)memset(journal, 0, sizeof(*journal));
    journal->revision = 1U;
}

UmiStatus umi_financial_audit_journal_append(
    UmiFinancialAuditJournal *journal,
    const UmiFinancialAuditRecord *record)
{
    uint64_t expected;
    if (journal == NULL || record == NULL ||
        record->sequence == 0U ||
        record->eventId[0] == '\0' ||
        record->lineageId[0] == '\0' ||
        record->actorId[0] == '\0' ||
        record->domain[0] == '\0' ||
        record->entityId[0] == '\0' ||
        record->action[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (journal->count >= UMI_FINANCIAL_AUDIT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    expected = journal->count == 0U
        ? record->sequence
        : journal->records[journal->count - 1U].sequence + 1U;
    if (record->sequence < expected)
        return UMI_STATUS_INVALID_STATE;
    if (record->sequence > expected)
        journal->gapCount += record->sequence - expected;
    journal->records[journal->count++] = *record;
    journal->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_audit_journal_mark_persisted(
    UmiFinancialAuditJournal *journal,
    uint64_t sequence)
{
    size_t i;
    if (journal == NULL || sequence == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < journal->count; ++i) {
        if (journal->records[i].sequence == sequence) {
            journal->records[i].persisted = 1;
            journal->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_financial_audit_journal_persisted_count(
    const UmiFinancialAuditJournal *journal)
{
    size_t i, count = 0U;
    if (journal == NULL) return 0U;
    for (i = 0U; i < journal->count; ++i)
        if (journal->records[i].persisted) count += 1U;
    return count;
}
