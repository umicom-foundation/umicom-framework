/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/capability_record.h
 *
 * PURPOSE:
 *   Define the reusable provider capability record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CAPABILITY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CAPABILITY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider capability record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderCapabilityRecord {
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
} UmiWorkbenchSelectionProviderCapabilityRecord;

/**
 * Initialise workbench selection provider capability record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_capability_record_init(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider capability record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_capability_record_validate(
    const UmiWorkbenchSelectionProviderCapabilityRecord *record);
/**
 * Provide the workbench selection provider capability record set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_provider(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider capability record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_source(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider capability record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_subject(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider capability record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_related(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider capability record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_group(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider capability record set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_description(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider capability record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_capability_record_hash(
    const UmiWorkbenchSelectionProviderCapabilityRecord *record);
/**
 * Provide the workbench selection provider capability record touch operation used by this
 * module and its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_provider_capability_record_touch(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_provider_capability_record_archive_encode(const UmiWorkbenchSelectionProviderCapabilityRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_provider_capability_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionProviderCapabilityRecord *value);

/** Publish a reviewed capability record value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_provider_capability_record_replace_if_current(UmiWorkbenchSelectionProviderCapabilityRecord *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionProviderCapabilityRecord *proposal);

#ifdef __cplusplus
}
#endif
#endif
