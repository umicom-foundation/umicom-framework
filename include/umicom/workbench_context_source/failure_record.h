/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/failure_record.h
 *
 * PURPOSE:
 *   Define the reusable source failure record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_FAILURE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_FAILURE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source failure record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceFailureRecord {
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
} UmiWorkbenchContextSourceFailureRecord;

/**
 * Initialise workbench context source failure record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_failure_record_init(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *record_id);
/**
 * Check that workbench context source failure record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_failure_record_validate(
    const UmiWorkbenchContextSourceFailureRecord *record);
/**
 * Provide the workbench context source failure record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_source(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source failure record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_panel(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source failure record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_subject(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source failure record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_group(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source failure record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_label(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *label);
/**
 * Provide the workbench context source failure record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_failure_record_hash(
    const UmiWorkbenchContextSourceFailureRecord *record);
/**
 * Provide the workbench context source failure record touch operation used by this module
 * and its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_context_source_failure_record_touch(
    UmiWorkbenchContextSourceFailureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_source_failure_record_archive_encode(const UmiWorkbenchContextSourceFailureRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_source_failure_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextSourceFailureRecord *value);

/** Publish a reviewed failure record value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_context_source_failure_record_replace_if_current(UmiWorkbenchContextSourceFailureRecord *value,
    uint64_t expected_revision, const UmiWorkbenchContextSourceFailureRecord *proposal);

#ifdef __cplusplus
}
#endif
#endif
