/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/financial_audit_journal.h
 *
 * PURPOSE:
 *   Retain immutable cross-domain financial audit evidence with sequence,
 *   causation, lineage, actor and persistence state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_FINANCIAL_AUDIT_JOURNAL_H
#define UMICOM_FINANCIAL_PLATFORM_FINANCIAL_AUDIT_JOURNAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_AUDIT_CAPACITY 1024U

typedef struct UmiFinancialAuditRecord {
    uint64_t sequence;
    char eventId[64];
    char causationId[64];
    char lineageId[64];
    char actorId[64];
    char domain[32];
    char entityId[64];
    char action[48];
    int64_t eventTimeMilliseconds;
    UmiStatus status;
    int persisted;
} UmiFinancialAuditRecord;

typedef struct UmiFinancialAuditJournal {
    UmiFinancialAuditRecord records[UMI_FINANCIAL_AUDIT_CAPACITY];
    size_t count;
    uint64_t gapCount;
    uint64_t revision;
} UmiFinancialAuditJournal;

/* These are local, caller-owned observations, not tamper-evident or durable
 * records. Inline strings must terminate; causation may be empty. Sequence is
 * nonzero, status is a named Framework status, persisted is 0 or 1. Signed event
 * timestamps remain supported. The caller serializes access and owns storage. */
UmiStatus umi_financial_audit_record_validate(const UmiFinancialAuditRecord *record);
/* Count is checked before traversal. Sequences strictly increase; the first
 * row establishes the observed interval, so earlier missing numbers are not
 * counted. Repeated event IDs are allowed; sequence identifies each row.
 * gapCount must exactly match missing sequences within that interval.
 * Unused rows and bytes after string terminators are ignored. */
UmiStatus umi_financial_audit_journal_validate(const UmiFinancialAuditJournal *journal);
/* Complete count or unchanged output, disjoint from journal storage. The legacy
 * scalar count returns zero on refusal; use status to distinguish unavailable.
 * Marking persisted changes a local flag only. It does not write or verify a
 * file, database, signature or remote receipt. A final UINT64_MAX sequence can
 * be marked but no later row can be appended; exhaustion refuses atomically. */
UmiStatus umi_financial_audit_journal_persisted_count_checked(
    const UmiFinancialAuditJournal *journal, size_t *outCount);
void umi_financial_audit_journal_init(UmiFinancialAuditJournal *journal);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_financial_audit_journal_append(
    UmiFinancialAuditJournal *journal,
    const UmiFinancialAuditRecord *record);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_financial_audit_journal_mark_persisted(
    UmiFinancialAuditJournal *journal,
    uint64_t sequence);
size_t umi_financial_audit_journal_persisted_count(
    const UmiFinancialAuditJournal *journal);

#ifdef __cplusplus
}
#endif
#endif
