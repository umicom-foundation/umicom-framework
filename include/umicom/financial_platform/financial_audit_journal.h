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

void umi_financial_audit_journal_init(UmiFinancialAuditJournal *journal);
UmiStatus umi_financial_audit_journal_append(
    UmiFinancialAuditJournal *journal,
    const UmiFinancialAuditRecord *record);
UmiStatus umi_financial_audit_journal_mark_persisted(
    UmiFinancialAuditJournal *journal,
    uint64_t sequence);
size_t umi_financial_audit_journal_persisted_count(
    const UmiFinancialAuditJournal *journal);

#ifdef __cplusplus
}
#endif
#endif
