/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/project_node.h
 *
 * PURPOSE:
 *   Define the reusable project tree node contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROJECT_NODE_H
#define UMICOM_WORKBENCH_SELECTION_PROJECT_NODE_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection project node data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionProjectNode {
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
} UmiWorkbenchSelectionProjectNode;

/**
 * Initialise workbench selection project node from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_project_node_init(
    UmiWorkbenchSelectionProjectNode *record,
    const char *record_id);
/**
 * Check that workbench selection project node satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_project_node_validate(
    const UmiWorkbenchSelectionProjectNode *record);
/**
 * Provide the workbench selection project node set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_project_node_set_source(
    UmiWorkbenchSelectionProjectNode *record,
    const char *source_id);
/**
 * Provide the workbench selection project node set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_project_node_set_subject(
    UmiWorkbenchSelectionProjectNode *record,
    const char *subject_id);
/**
 * Provide the workbench selection project node set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_project_node_set_secondary(
    UmiWorkbenchSelectionProjectNode *record,
    const char *secondary_id);
/**
 * Provide the workbench selection project node set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_project_node_set_group(
    UmiWorkbenchSelectionProjectNode *record,
    const char *group_id);
/**
 * Provide the workbench selection project node set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_project_node_set_label(
    UmiWorkbenchSelectionProjectNode *record,
    const char *label);
/**
 * Provide the workbench selection project node hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_project_node_hash(
    const UmiWorkbenchSelectionProjectNode *record);
/**
 * Provide the workbench selection project node touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_project_node_touch(
    UmiWorkbenchSelectionProjectNode *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
