/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/node_capability.h
 *
 * PURPOSE:
 *   Define the reusable structured node capability contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_NODE_CAPABILITY_H
#define UMICOM_WORKBENCH_SELECTION_NODE_CAPABILITY_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection node capability data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionNodeCapability {
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
} UmiWorkbenchSelectionNodeCapability;

/**
 * Initialise workbench selection node capability from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_node_capability_init(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *record_id);
/**
 * Check that workbench selection node capability satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_node_capability_validate(
    const UmiWorkbenchSelectionNodeCapability *record);
/**
 * Provide the workbench selection node capability set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_node_capability_set_source(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *source_id);
/**
 * Provide the workbench selection node capability set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_node_capability_set_subject(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *subject_id);
/**
 * Provide the workbench selection node capability set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_node_capability_set_related(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *related_id);
/**
 * Provide the workbench selection node capability set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_node_capability_set_group(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *group_id);
/**
 * Provide the workbench selection node capability set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_node_capability_set_description(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *description);
/**
 * Provide the workbench selection node capability hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_node_capability_hash(
    const UmiWorkbenchSelectionNodeCapability *record);
/**
 * Provide the workbench selection node capability touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_node_capability_touch(
    UmiWorkbenchSelectionNodeCapability *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
