/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/event_record.h
 *
 * PURPOSE:
 *   Define the reusable provider event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_EVENT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_EVENT_RECORD_H

#include "umicom/workbench_selection_provider/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider event record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderEventRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderEventRecord;

/**
 * Initialise workbench selection provider event record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_event_record_init(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider event record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_event_record_validate(
    const UmiWorkbenchSelectionProviderEventRecord *record);
/**
 * Provide the workbench selection provider event record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_event_record_set_provider(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider event record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_event_record_set_source(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider event record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_event_record_set_subject(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider event record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_event_record_set_related(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider event record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_event_record_set_group(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider event record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_event_record_set_description(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider event record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_event_record_hash(
    const UmiWorkbenchSelectionProviderEventRecord *record);
/**
 * Provide the workbench selection provider event record touch operation used by this
 * module and its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_provider_event_record_touch(
    UmiWorkbenchSelectionProviderEventRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_provider_event_record_archive_encode(const UmiWorkbenchSelectionProviderEventRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_provider_event_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionProviderEventRecord *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_selection_provider_event_record_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchSelectionProviderEventRecord *expected, UmiWorkbenchSelectionProviderEventRecord *value);


/** Publish a reviewed event record value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_provider_event_record_replace_if_current(UmiWorkbenchSelectionProviderEventRecord *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionProviderEventRecord *proposal);

#ifdef __cplusplus
}
#endif
#endif
