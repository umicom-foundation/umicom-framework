/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/group_binding.h
 *
 * PURPOSE:
 *   Define the reusable context-group selection binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_GROUP_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_GROUP_BINDING_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection group binding data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionGroupBinding {
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
} UmiWorkbenchSelectionGroupBinding;

/**
 * Initialise workbench selection group binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_group_binding_init(
    UmiWorkbenchSelectionGroupBinding *record,
    const char *record_id);
/**
 * Check that workbench selection group binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_group_binding_validate(
    const UmiWorkbenchSelectionGroupBinding *record);
/**
 * Provide the workbench selection group binding set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_group_binding_set_source(
    UmiWorkbenchSelectionGroupBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection group binding set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_group_binding_set_subject(
    UmiWorkbenchSelectionGroupBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection group binding set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_group_binding_set_secondary(
    UmiWorkbenchSelectionGroupBinding *record,
    const char *secondary_id);
/**
 * Provide the workbench selection group binding set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_group_binding_set_group(
    UmiWorkbenchSelectionGroupBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection group binding set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_group_binding_set_label(
    UmiWorkbenchSelectionGroupBinding *record,
    const char *label);
/**
 * Provide the workbench selection group binding hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_group_binding_hash(
    const UmiWorkbenchSelectionGroupBinding *record);
/**
 * Provide the workbench selection group binding touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_group_binding_touch(
    UmiWorkbenchSelectionGroupBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
