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

void umi_broker_audit_journal_init(UmiBrokerAuditJournal *journal);
UmiStatus umi_broker_audit_journal_append(
    UmiBrokerAuditJournal *journal,
    const UmiBrokerAuditRecord *record);
UmiStatus umi_broker_audit_journal_mark_persisted(
    UmiBrokerAuditJournal *journal,
    uint64_t sequence);
size_t umi_broker_audit_journal_persisted_count(
    const UmiBrokerAuditJournal *journal);

#ifdef __cplusplus
}
#endif
#endif
