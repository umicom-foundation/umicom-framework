/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/history_entry.h
 *
 * PURPOSE:
 *   Define the reusable provider history entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_HISTORY_ENTRY_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_HISTORY_ENTRY_H

#include "umicom/workbench_selection_provider/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider history entry data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderHistoryEntry {
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
} UmiWorkbenchSelectionProviderHistoryEntry;

/**
 * Initialise workbench selection provider history entry from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_history_entry_init(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *record_id);
/**
 * Check that workbench selection provider history entry satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_history_entry_validate(
    const UmiWorkbenchSelectionProviderHistoryEntry *record);
/**
 * Provide the workbench selection provider history entry set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_history_entry_set_provider(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider history entry set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_history_entry_set_source(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *source_id);
/**
 * Provide the workbench selection provider history entry set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_history_entry_set_subject(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider history entry set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_history_entry_set_related(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *related_id);
/**
 * Provide the workbench selection provider history entry set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_history_entry_set_group(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *group_id);
/**
 * Provide the workbench selection provider history entry set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_history_entry_set_description(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *description);
/**
 * Provide the workbench selection provider history entry hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_history_entry_hash(
    const UmiWorkbenchSelectionProviderHistoryEntry *record);
/**
 * Provide the workbench selection provider history entry touch operation used by this
 * module and its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_provider_history_entry_touch(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_provider_history_entry_archive_encode(const UmiWorkbenchSelectionProviderHistoryEntry *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_provider_history_entry_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionProviderHistoryEntry *value);

/** Publish a reviewed history entry value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_provider_history_entry_replace_if_current(UmiWorkbenchSelectionProviderHistoryEntry *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionProviderHistoryEntry *proposal);

#ifdef __cplusplus
}
#endif
#endif
