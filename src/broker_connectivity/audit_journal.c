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
#include <stdint.h>
#include "umicom/broker_connectivity/audit_journal.h"

#include <string.h>

void umi_broker_audit_journal_init(UmiBrokerAuditJournal *journal)
{
    if (journal == NULL) return;
    (void)memset(journal, 0, sizeof(*journal));
    journal->revision = 1U;
}

/* Preserve prior mutation and counting implementations for review. The active
 * owner below validates complete retained state and sequence arithmetic before
 * publishing a row, so corrupt counts or exhausted sequences cannot hide gaps. */
#if 0
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_broker_audit_journal_append(
    UmiBrokerAuditJournal *journal,
    const UmiBrokerAuditRecord *record)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (journal != NULL && journal->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_broker_audit_journal_mark_persisted(
    UmiBrokerAuditJournal *journal,
    uint64_t sequence)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (journal != NULL && journal->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

#endif

/* Validate fixed fields before any consumer can treat them as C strings.
 * Optional causation/detail may be empty. This owner retains chronological
 * sequence evidence; it does not claim cryptographic integrity or persistence. */
static int AuditText(const char *text, size_t capacity, int allowEmpty)
{
    const char *end = memchr(text, '\0', capacity);
    return end != NULL && (allowEmpty || end != text);
}
UmiStatus UmiBrokerAuditRecordValidate(const UmiBrokerAuditRecord *record)
{
    if (record == NULL || record->sequence == 0U ||
        !AuditText(record->eventId, sizeof(record->eventId), 0) ||
        !AuditText(record->causationId, sizeof(record->causationId), 1) ||
        !AuditText(record->source, sizeof(record->source), 0) ||
        !AuditText(record->kind, sizeof(record->kind), 0) ||
        !AuditText(record->entityId, sizeof(record->entityId), 0) ||
        !AuditText(record->detail, sizeof(record->detail), 1) ||
        record->eventTimeMilliseconds < 0 ||
        (unsigned)record->status > (unsigned)UMI_STATUS_BUSY ||
        (record->persisted != 0 && record->persisted != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiBrokerAuditJournalValidate(const UmiBrokerAuditJournal *journal)
{
    if (journal == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (journal->count > UMI_BROKER_AUDIT_CAPACITY || journal->revision == 0U)
        return UMI_STATUS_INVALID_STATE;
    uint64_t gaps = 0U;
    for (size_t i = 0U; i < journal->count; ++i) {
        if (UmiBrokerAuditRecordValidate(&journal->records[i]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        if (i != 0U) {
            uint64_t previous = journal->records[i - 1U].sequence;
            uint64_t current = journal->records[i].sequence;
            if (current <= previous) return UMI_STATUS_INVALID_STATE;
            uint64_t gap = current - previous - 1U;
            if (gap > UINT64_MAX - gaps) return UMI_STATUS_INVALID_STATE;
            gaps += gap;
        }
    }
    return gaps == journal->gapCount ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
UmiStatus umi_broker_audit_journal_append(UmiBrokerAuditJournal *journal,
                                         const UmiBrokerAuditRecord *record)
{
    if (journal == NULL || UmiBrokerAuditRecordValidate(record) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiBrokerAuditJournalValidate(journal);
    if (status != UMI_STATUS_OK) return status;
    if (journal->count == UMI_BROKER_AUDIT_CAPACITY || journal->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t gap = 0U;
    if (journal->count != 0U) {
        uint64_t previous = journal->records[journal->count - 1U].sequence;
        if (previous == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (record->sequence <= previous) return UMI_STATUS_INVALID_STATE;
        gap = record->sequence - previous - 1U;
        if (gap > UINT64_MAX - journal->gapCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Capture before publication so a record borrowed from this journal is safe.
     * Sequence gaps and revision become visible together with the new row. */
    UmiBrokerAuditRecord captured = *record;
    journal->records[journal->count++] = captured;
    journal->gapCount += gap;
    ++journal->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_broker_audit_journal_mark_persisted(UmiBrokerAuditJournal *journal, uint64_t sequence)
{
    if (journal == NULL || sequence == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiBrokerAuditJournalValidate(journal);
    if (status != UMI_STATUS_OK) return status;
    if (journal->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < journal->count; ++i) {
        if (journal->records[i].sequence == sequence) {
            journal->records[i].persisted = 1;
            ++journal->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_broker_audit_journal_persisted_count(const UmiBrokerAuditJournal *journal)
{
    if (UmiBrokerAuditJournalValidate(journal) != UMI_STATUS_OK) return 0U;
    size_t count = 0U;
    for (size_t i = 0U; i < journal->count; ++i)
        if (journal->records[i].persisted) ++count;
    return count;
}
