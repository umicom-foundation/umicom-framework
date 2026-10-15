/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/rate_record.h
 *
 * PURPOSE:
 *   Define the reusable selection rate record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_RATE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_RATE_RECORD_H
#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection rate record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionRateRecord {
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
} UmiWorkbenchSelectionRateRecord;

/**
 * Initialise workbench selection rate record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_rate_record_init(
    UmiWorkbenchSelectionRateRecord *record,
    const char *record_id);
/**
 * Check that workbench selection rate record satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_rate_record_validate(
    const UmiWorkbenchSelectionRateRecord *record);
/**
 * Provide the workbench selection rate record set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_source(
    UmiWorkbenchSelectionRateRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection rate record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_subject(
    UmiWorkbenchSelectionRateRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection rate record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_related(
    UmiWorkbenchSelectionRateRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection rate record set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_group(
    UmiWorkbenchSelectionRateRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection rate record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_description(
    UmiWorkbenchSelectionRateRecord *record,
    const char *description);
/**
 * Provide the workbench selection rate record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_rate_record_hash(
    const UmiWorkbenchSelectionRateRecord *record);
/**
 * Provide the workbench selection rate record touch operation used by this module and its
 * client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_rate_record_touch(
    UmiWorkbenchSelectionRateRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_rate_record_archive_encode(const UmiWorkbenchSelectionRateRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_rate_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionRateRecord *value);

/** Publish a reviewed rate record value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_rate_record_replace_if_current(UmiWorkbenchSelectionRateRecord *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionRateRecord *proposal);

#ifdef __cplusplus
}
#endif
#endif
