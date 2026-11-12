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
#include <stdint.h>
#include "umicom/financial_platform/financial_audit_journal.h"

#include <string.h>

#include "umicom/base/snapshot_validation.h"

/* Retained for review: the former journal trusted inline storage and formed
 * last sequence + 1 without checking exhaustion. The replacement admits all
 * records, reconstructs gap evidence and preserves state on every refusal. */
#if 0
void umi_financial_audit_journal_init(UmiFinancialAuditJournal *journal)
{
    if (journal == NULL) return;
    (void)memset(journal, 0, sizeof(*journal));
    journal->revision = 1U;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_financial_audit_journal_append(
    UmiFinancialAuditJournal *journal,
    const UmiFinancialAuditRecord *record)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (journal != NULL && journal->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_financial_audit_journal_mark_persisted(
    UmiFinancialAuditJournal *journal,
    uint64_t sequence)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (journal != NULL && journal->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

#endif

UmiStatus umi_financial_audit_record_validate(const UmiFinancialAuditRecord *record)
{
    static const UmiSnapshotTextField fields[] = {
        {"eventId", offsetof(UmiFinancialAuditRecord, eventId), 64U, 1},
        {"causationId", offsetof(UmiFinancialAuditRecord, causationId), 64U, 0},
        {"lineageId", offsetof(UmiFinancialAuditRecord, lineageId), 64U, 1},
        {"actorId", offsetof(UmiFinancialAuditRecord, actorId), 64U, 1},
        {"domain", offsetof(UmiFinancialAuditRecord, domain), 32U, 1},
        {"entityId", offsetof(UmiFinancialAuditRecord, entityId), 64U, 1},
        {"action", offsetof(UmiFinancialAuditRecord, action), 48U, 1}
    };
    if (record == NULL || UmiSnapshotValidateTextFields(record, sizeof(*record), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK || record->sequence == 0U ||
        record->status < UMI_STATUS_OK || record->status > UMI_STATUS_BUSY ||
        (record->persisted != 0 && record->persisted != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_audit_journal_validate(const UmiFinancialAuditJournal *journal)
{
    if (journal == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (journal->count > UMI_FINANCIAL_AUDIT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (journal->revision == 0U) return UMI_STATUS_INVALID_STATE;
    uint64_t gaps = 0U;
    for (size_t i = 0U; i < journal->count; ++i) {
        const UmiFinancialAuditRecord *row = &journal->records[i];
        if (umi_financial_audit_record_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        if (i != 0U) {
            uint64_t previous = journal->records[i - 1U].sequence;
            if (row->sequence <= previous) return UMI_STATUS_INVALID_STATE;
            uint64_t missing = row->sequence - previous - 1U;
            if (missing > UINT64_MAX - gaps) return UMI_STATUS_CAPACITY_EXCEEDED;
            gaps += missing;
        }
    }
    return gaps == journal->gapCount ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
void umi_financial_audit_journal_init(UmiFinancialAuditJournal *journal)
{
    if (journal == NULL) return;
    memset(journal, 0, sizeof(*journal));
    journal->revision = 1U;
}
UmiStatus umi_financial_audit_journal_append(UmiFinancialAuditJournal *journal,
    const UmiFinancialAuditRecord *record)
{
    if (journal != NULL && journal->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_audit_journal_validate(journal);
    if (status == UMI_STATUS_OK) status = umi_financial_audit_record_validate(record);
    if (status != UMI_STATUS_OK) return status;
    if (journal->count == UMI_FINANCIAL_AUDIT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t missing = 0U;
    if (journal->count != 0U) {
        uint64_t previous = journal->records[journal->count - 1U].sequence;
        if (previous == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (record->sequence <= previous) return UMI_STATUS_INVALID_STATE;
        /* Subtract ordered values instead of forming previous + 1, which can
         * wrap at the last sequence. The first row establishes the interval. */
        missing = record->sequence - previous - 1U;
        if (missing > UINT64_MAX - journal->gapCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    journal->records[journal->count++] = *record;
    journal->gapCount += missing;
    ++journal->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_audit_journal_mark_persisted(UmiFinancialAuditJournal *journal,
    uint64_t sequence)
{
    if (journal != NULL && journal->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (sequence == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_financial_audit_journal_validate(journal);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < journal->count; ++i)
        if (journal->records[i].sequence == sequence) {
            /* Preserve the legacy repeat-mark behavior: each accepted mark
             * advances the local revision, but does not perform filesystem I/O. */
            journal->records[i].persisted = 1;
            ++journal->revision;
            return UMI_STATUS_OK;
        }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_financial_audit_journal_persisted_count_checked(
    const UmiFinancialAuditJournal *journal, size_t *outCount)
{
    if (outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_financial_audit_journal_validate(journal);
    if (status != UMI_STATUS_OK) return status;
    size_t count = 0U;
    for (size_t i = 0U; i < journal->count; ++i)
        if (journal->records[i].persisted) ++count;
    *outCount = count;
    return UMI_STATUS_OK;
}
size_t umi_financial_audit_journal_persisted_count(const UmiFinancialAuditJournal *journal)
{
    size_t count;
    return umi_financial_audit_journal_persisted_count_checked(journal, &count) == UMI_STATUS_OK ? count : 0U;
}
