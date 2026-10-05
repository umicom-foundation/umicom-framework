/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/source_commit.h
 *
 * PURPOSE:
 *   Define the reusable structured source-control commit contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SOURCE_COMMIT_H
#define UMICOM_WORKBENCH_SELECTION_SOURCE_COMMIT_H

#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection source commit data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionSourceCommit {
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
} UmiWorkbenchSelectionSourceCommit;

/**
 * Initialise workbench selection source commit from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_source_commit_init(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *record_id);
/**
 * Check that workbench selection source commit satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_source_commit_validate(
    const UmiWorkbenchSelectionSourceCommit *record);
/**
 * Provide the workbench selection source commit set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_commit_set_source(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *source_id);
/**
 * Provide the workbench selection source commit set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_commit_set_subject(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *subject_id);
/**
 * Provide the workbench selection source commit set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_source_commit_set_secondary(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *secondary_id);
/**
 * Provide the workbench selection source commit set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_commit_set_group(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *group_id);
/**
 * Provide the workbench selection source commit set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_source_commit_set_label(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *label);
/**
 * Provide the workbench selection source commit hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_source_commit_hash(
    const UmiWorkbenchSelectionSourceCommit *record);
/**
 * Provide the workbench selection source commit touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_source_commit_touch(
    UmiWorkbenchSelectionSourceCommit *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_source_commit_archive_encode(const UmiWorkbenchSelectionSourceCommit *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_source_commit_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionSourceCommit *value);

#ifdef __cplusplus
}
#endif
#endif
