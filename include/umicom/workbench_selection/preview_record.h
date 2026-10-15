/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/preview_record.h
 *
 * PURPOSE:
 *   Define the reusable selection preview record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PREVIEW_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PREVIEW_RECORD_H
#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection preview record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionPreviewRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionPreviewRecord;

/**
 * Initialise workbench selection preview record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_preview_record_init(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *record_id);
/**
 * Check that workbench selection preview record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_preview_record_validate(
    const UmiWorkbenchSelectionPreviewRecord *record);
/**
 * Provide the workbench selection preview record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_preview_record_set_source(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection preview record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_preview_record_set_subject(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection preview record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_preview_record_set_related(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection preview record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_preview_record_set_group(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection preview record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_preview_record_set_description(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *description);
/**
 * Provide the workbench selection preview record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_preview_record_hash(
    const UmiWorkbenchSelectionPreviewRecord *record);
/**
 * Provide the workbench selection preview record touch operation used by this module and
 * its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_preview_record_touch(
    UmiWorkbenchSelectionPreviewRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_preview_record_archive_encode(const UmiWorkbenchSelectionPreviewRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_preview_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionPreviewRecord *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_selection_preview_record_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchSelectionPreviewRecord *expected, UmiWorkbenchSelectionPreviewRecord *value);


/** Publish a reviewed preview record value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_preview_record_replace_if_current(UmiWorkbenchSelectionPreviewRecord *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionPreviewRecord *proposal);

#ifdef __cplusplus
}
#endif
#endif
