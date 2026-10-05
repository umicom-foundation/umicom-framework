/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/audit_record.h
 *
 * PURPOSE:
 *   Define the reusable context-link audit record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_AUDIT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_AUDIT_RECORD_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link audit record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkAuditRecord {
    uint32_t structure_size;
    char audit_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char action_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkAuditRecord;

/**
 * Initialise workbench context link audit record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_audit_record_init(UmiWorkbenchContextLinkAuditRecord *record,
                                           const char *identity);
/**
 * Check that workbench context link audit record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_audit_record_validate(
    const UmiWorkbenchContextLinkAuditRecord *record);
/**
 * Copy workbench context link audit record into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_audit_record_copy(
    UmiWorkbenchContextLinkAuditRecord *destination,
    const UmiWorkbenchContextLinkAuditRecord *source);
/**
 * Provide the workbench context link audit record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_audit_record_hash(
    const UmiWorkbenchContextLinkAuditRecord *record);
/**
 * Provide the workbench context link audit record set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_audit_record_set_primary(
    UmiWorkbenchContextLinkAuditRecord *record,
    const char *value);
/**
 * Provide the workbench context link audit record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_audit_record_set_secondary(
    UmiWorkbenchContextLinkAuditRecord *record,
    const char *value);
/**
 * Provide the workbench context link audit record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_audit_record_touch(
    UmiWorkbenchContextLinkAuditRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_audit_record_archive_encode(const UmiWorkbenchContextLinkAuditRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_audit_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkAuditRecord *value);

#ifdef __cplusplus
}
#endif

#endif
