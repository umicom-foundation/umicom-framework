/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/node_path.h
 *
 * PURPOSE:
 *   Define the reusable structured node path contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_NODE_PATH_H
#define UMICOM_WORKBENCH_SELECTION_NODE_PATH_H
#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection node path data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionNodePath {
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
} UmiWorkbenchSelectionNodePath;

/**
 * Initialise workbench selection node path from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_selection_node_path_init(
    UmiWorkbenchSelectionNodePath *record,
    const char *record_id);
/**
 * Check that workbench selection node path satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_node_path_validate(
    const UmiWorkbenchSelectionNodePath *record);
/**
 * Provide the workbench selection node path set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_node_path_set_source(
    UmiWorkbenchSelectionNodePath *record,
    const char *source_id);
/**
 * Provide the workbench selection node path set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_node_path_set_subject(
    UmiWorkbenchSelectionNodePath *record,
    const char *subject_id);
/**
 * Provide the workbench selection node path set related operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_node_path_set_related(
    UmiWorkbenchSelectionNodePath *record,
    const char *related_id);
/**
 * Provide the workbench selection node path set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_node_path_set_group(
    UmiWorkbenchSelectionNodePath *record,
    const char *group_id);
/**
 * Provide the workbench selection node path set description operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_node_path_set_description(
    UmiWorkbenchSelectionNodePath *record,
    const char *description);
/**
 * Provide the workbench selection node path hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_node_path_hash(
    const UmiWorkbenchSelectionNodePath *record);
/**
 * Provide the workbench selection node path touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_node_path_touch(
    UmiWorkbenchSelectionNodePath *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_node_path_archive_encode(const UmiWorkbenchSelectionNodePath *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_node_path_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionNodePath *value);

#ifdef __cplusplus
}
#endif
#endif
