/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/multi_selection.h
 *
 * PURPOSE:
 *   Define the reusable multi-selection state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_MULTI_SELECTION_H
#define UMICOM_WORKBENCH_SELECTION_MULTI_SELECTION_H
#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection multi selection data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionMultiSelection {
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
} UmiWorkbenchSelectionMultiSelection;

/**
 * Initialise workbench selection multi selection from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_multi_selection_init(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *record_id);
/**
 * Check that workbench selection multi selection satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_multi_selection_validate(
    const UmiWorkbenchSelectionMultiSelection *record);
/**
 * Provide the workbench selection multi selection set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_source(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *source_id);
/**
 * Provide the workbench selection multi selection set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_subject(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *subject_id);
/**
 * Provide the workbench selection multi selection set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_related(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *related_id);
/**
 * Provide the workbench selection multi selection set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_group(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *group_id);
/**
 * Provide the workbench selection multi selection set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_description(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *description);
/**
 * Provide the workbench selection multi selection hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_multi_selection_hash(
    const UmiWorkbenchSelectionMultiSelection *record);
/**
 * Provide the workbench selection multi selection touch operation used by this module and
 * its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_multi_selection_touch(
    UmiWorkbenchSelectionMultiSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_multi_selection_archive_encode(const UmiWorkbenchSelectionMultiSelection *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_multi_selection_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionMultiSelection *value);

/** Publish a reviewed multi selection value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_multi_selection_replace_if_current(UmiWorkbenchSelectionMultiSelection *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionMultiSelection *proposal);

#ifdef __cplusplus
}
#endif
#endif
