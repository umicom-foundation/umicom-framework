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
/**
 * Represent the context audit record data shared with callers of this public contract.
 */
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
/**
 * Represent the context audit record store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextAuditRecordStore {
    UmiContextAuditRecord items[UMI_CONTEXT_AUDIT_RECORD_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextAuditRecordStore;
/**
 * Initialise context audit record from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_audit_record_init(UmiContextAuditRecord *record);
/**
 * Check that context audit record satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_audit_record_validate(const UmiContextAuditRecord *record);
/**
 * Initialise context audit record store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_audit_record_store_init(UmiContextAuditRecordStore *store);
/**
 * Provide the context audit record store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_audit_record_store_put(UmiContextAuditRecordStore *store,const UmiContextAuditRecord *record);
/**
 * Remove context audit record store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_audit_record_store_remove(UmiContextAuditRecordStore *store,const char *identity);
/**
 * Find context audit record store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextAuditRecord *umi_context_audit_record_store_find(UmiContextAuditRecordStore *store,const char *identity);
/**
 * Provide the context audit record store find const operation used by this module and its
 * client applications.
 */
const UmiContextAuditRecord *umi_context_audit_record_store_find_const(const UmiContextAuditRecordStore *store,const char *identity);
/**
 * Return the number of records represented by context audit record store without changing
 * their state.
 */
size_t umi_context_audit_record_store_count(const UmiContextAuditRecordStore *store);
/**
 * Provide the context audit record store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_audit_record_store_snapshot(const UmiContextAuditRecordStore *store,UmiContextAuditRecord *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
