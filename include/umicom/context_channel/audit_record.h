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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_audit_record_archive_encode(const UmiContextAuditRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_audit_record_archive_decode(const void *bytes, size_t byte_count,
    UmiContextAuditRecord *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiContextAuditRecord object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_context_audit_record_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiContextAuditRecord *workspace, UmiContextAuditRecord *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_audit_record_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextAuditRecord *expected, UmiContextAuditRecord *value);


#ifdef __cplusplus
}
#endif
#endif
