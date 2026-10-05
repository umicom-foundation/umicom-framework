/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/export_record.h
 *
 * PURPOSE:
 *   Define the reusable context-link export record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_EXPORT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_EXPORT_RECORD_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link export record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkExportRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char destination_uri[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkExportRecord;

/**
 * Initialise workbench context link export record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_export_record_init(UmiWorkbenchContextLinkExportRecord *record,
                                           const char *identity);
/**
 * Check that workbench context link export record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_export_record_validate(
    const UmiWorkbenchContextLinkExportRecord *record);
/**
 * Copy workbench context link export record into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_export_record_copy(
    UmiWorkbenchContextLinkExportRecord *destination,
    const UmiWorkbenchContextLinkExportRecord *source);
/**
 * Provide the workbench context link export record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_export_record_hash(
    const UmiWorkbenchContextLinkExportRecord *record);
/**
 * Provide the workbench context link export record set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_export_record_set_primary(
    UmiWorkbenchContextLinkExportRecord *record,
    const char *value);
/**
 * Provide the workbench context link export record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_export_record_set_secondary(
    UmiWorkbenchContextLinkExportRecord *record,
    const char *value);
/**
 * Provide the workbench context link export record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_export_record_touch(
    UmiWorkbenchContextLinkExportRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_export_record_archive_encode(const UmiWorkbenchContextLinkExportRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_export_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkExportRecord *value);

#ifdef __cplusplus
}
#endif

#endif
