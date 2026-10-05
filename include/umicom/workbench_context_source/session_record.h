/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/session_record.h
 *
 * PURPOSE:
 *   Define the reusable source session record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SESSION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SESSION_RECORD_H
#include "umicom/workbench_context_source/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source session record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceSessionRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceSessionRecord;

/**
 * Initialise workbench context source session record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_session_record_init(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *record_id);
/**
 * Check that workbench context source session record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_session_record_validate(
    const UmiWorkbenchContextSourceSessionRecord *record);
/**
 * Provide the workbench context source session record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_session_record_set_source(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source session record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_session_record_set_panel(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source session record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_session_record_set_subject(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source session record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_session_record_set_group(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source session record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_session_record_set_label(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *label);
/**
 * Provide the workbench context source session record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_session_record_hash(
    const UmiWorkbenchContextSourceSessionRecord *record);
/**
 * Provide the workbench context source session record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_session_record_touch(
    UmiWorkbenchContextSourceSessionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_source_session_record_archive_encode(const UmiWorkbenchContextSourceSessionRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_source_session_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextSourceSessionRecord *value);

#ifdef __cplusplus
}
#endif
#endif
