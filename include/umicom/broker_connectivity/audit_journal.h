/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/audit_journal.h
 *
 * PURPOSE:
 *   Retain bounded execution-control audit evidence with monotonic sequence,
 *   causation, source, entity and persistence state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_AUDIT_JOURNAL_H
#define UMICOM_BROKER_CONNECTIVITY_AUDIT_JOURNAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_AUDIT_CAPACITY 512U

typedef struct UmiBrokerAuditRecord {
    uint64_t sequence;
    char eventId[64];
    char causationId[64];
    char source[64];
    char kind[48];
    char entityId[64];
    char detail[192];
    int64_t eventTimeMilliseconds;
    UmiStatus status;
    int persisted;
} UmiBrokerAuditRecord;

typedef struct UmiBrokerAuditJournal {
    UmiBrokerAuditRecord records[UMI_BROKER_AUDIT_CAPACITY];
    size_t count;
    uint64_t gapCount;
    uint64_t revision;
} UmiBrokerAuditJournal;

/* Validate bounded fields, positive ordered sequences, exact retained gap count,
 * nonnegative timestamps and known status/persistence values. No mutation, I/O
 * or cryptographic verification occurs. Invalid count returns zero from the
 * compatibility persisted-count API; call Validate to distinguish that case. */
UmiStatus UmiBrokerAuditRecordValidate(const UmiBrokerAuditRecord *record);
UmiStatus UmiBrokerAuditJournalValidate(const UmiBrokerAuditJournal *journal);

void umi_broker_audit_journal_init(UmiBrokerAuditJournal *journal);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_broker_audit_journal_append(
    UmiBrokerAuditJournal *journal,
    const UmiBrokerAuditRecord *record);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_broker_audit_journal_mark_persisted(
    UmiBrokerAuditJournal *journal,
    uint64_t sequence);
size_t umi_broker_audit_journal_persisted_count(
    const UmiBrokerAuditJournal *journal);

#ifdef __cplusplus
}
#endif
#endif
