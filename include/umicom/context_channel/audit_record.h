/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/audit_record.h
 *
 * PURPOSE:
 *   Retain security and operational context audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_AUDIT_RECORD_H
#define UMICOM_CONTEXT_CHANNEL_AUDIT_RECORD_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_AUDIT_RECORD_MAX_ITEMS 256U
typedef struct UmiContextAuditRecord {
    uint32_t structure_size;
    char audit_id[UMI_CONTEXT_TEXT_CAPACITY];
    char actor_id[UMI_CONTEXT_TEXT_CAPACITY];
    char action_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char target_id[UMI_CONTEXT_TEXT_CAPACITY];
    UmiStatus status;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiContextAuditRecord;
typedef struct UmiContextAuditRecordStore {
    UmiContextAuditRecord items[UMI_CONTEXT_AUDIT_RECORD_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextAuditRecordStore;
void umi_context_audit_record_init(UmiContextAuditRecord *record);
UmiStatus umi_context_audit_record_validate(const UmiContextAuditRecord *record);
void umi_context_audit_record_store_init(UmiContextAuditRecordStore *store);
UmiStatus umi_context_audit_record_store_put(UmiContextAuditRecordStore *store,const UmiContextAuditRecord *record);
UmiStatus umi_context_audit_record_store_remove(UmiContextAuditRecordStore *store,const char *identity);
UmiContextAuditRecord *umi_context_audit_record_store_find(UmiContextAuditRecordStore *store,const char *identity);
const UmiContextAuditRecord *umi_context_audit_record_store_find_const(const UmiContextAuditRecordStore *store,const char *identity);
size_t umi_context_audit_record_store_count(const UmiContextAuditRecordStore *store);
UmiStatus umi_context_audit_record_store_snapshot(const UmiContextAuditRecordStore *store,UmiContextAuditRecord *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
