/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/health_record.h
 *
 * PURPOSE:
 *   Define the reusable selection health record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_HEALTH_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_HEALTH_RECORD_H

#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection health record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionHealthRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionHealthRecord;

/**
 * Initialise workbench selection health record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_health_record_init(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *record_id);
/**
 * Check that workbench selection health record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_health_record_validate(
    const UmiWorkbenchSelectionHealthRecord *record);
/**
 * Provide the workbench selection health record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_health_record_set_source(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection health record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_health_record_set_subject(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection health record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_health_record_set_secondary(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection health record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_health_record_set_group(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection health record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_health_record_set_label(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *label);
/**
 * Provide the workbench selection health record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_health_record_hash(
    const UmiWorkbenchSelectionHealthRecord *record);
/**
 * Provide the workbench selection health record touch operation used by this module and
 * its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_health_record_touch(
    UmiWorkbenchSelectionHealthRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_health_record_archive_encode(const UmiWorkbenchSelectionHealthRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_health_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionHealthRecord *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiWorkbenchSelectionHealthRecord object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_workbench_selection_health_record_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionHealthRecord *workspace, UmiWorkbenchSelectionHealthRecord *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_selection_health_record_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchSelectionHealthRecord *expected, UmiWorkbenchSelectionHealthRecord *value);


/** Publish a reviewed health record value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_health_record_replace_if_current(UmiWorkbenchSelectionHealthRecord *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionHealthRecord *proposal);

#ifdef __cplusplus
}
#endif
#endif
