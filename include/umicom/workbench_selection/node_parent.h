/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/node_parent.h
 *
 * PURPOSE:
 *   Define the reusable structured node parent relationship contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_NODE_PARENT_H
#define UMICOM_WORKBENCH_SELECTION_NODE_PARENT_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection node parent data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionNodeParent {
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
} UmiWorkbenchSelectionNodeParent;

/**
 * Initialise workbench selection node parent from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_node_parent_init(
    UmiWorkbenchSelectionNodeParent *record,
    const char *record_id);
/**
 * Check that workbench selection node parent satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_node_parent_validate(
    const UmiWorkbenchSelectionNodeParent *record);
/**
 * Provide the workbench selection node parent set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_node_parent_set_source(
    UmiWorkbenchSelectionNodeParent *record,
    const char *source_id);
/**
 * Provide the workbench selection node parent set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_node_parent_set_subject(
    UmiWorkbenchSelectionNodeParent *record,
    const char *subject_id);
/**
 * Provide the workbench selection node parent set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_node_parent_set_related(
    UmiWorkbenchSelectionNodeParent *record,
    const char *related_id);
/**
 * Provide the workbench selection node parent set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_node_parent_set_group(
    UmiWorkbenchSelectionNodeParent *record,
    const char *group_id);
/**
 * Provide the workbench selection node parent set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_node_parent_set_description(
    UmiWorkbenchSelectionNodeParent *record,
    const char *description);
/**
 * Provide the workbench selection node parent hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_node_parent_hash(
    const UmiWorkbenchSelectionNodeParent *record);
/**
 * Provide the workbench selection node parent touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_node_parent_touch(
    UmiWorkbenchSelectionNodeParent *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
