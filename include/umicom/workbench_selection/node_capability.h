/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/node_capability.h
 *
 * PURPOSE:
 *   Define the reusable structured node capability contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_NODE_CAPABILITY_H
#define UMICOM_WORKBENCH_SELECTION_NODE_CAPABILITY_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_selection_node_capability_init(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *record_id);
UmiStatus umi_workbench_selection_node_capability_validate(
    const UmiWorkbenchSelectionNodeCapability *record);
UmiStatus umi_workbench_selection_node_capability_set_source(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *source_id);
UmiStatus umi_workbench_selection_node_capability_set_subject(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_node_capability_set_related(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *related_id);
UmiStatus umi_workbench_selection_node_capability_set_group(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *group_id);
UmiStatus umi_workbench_selection_node_capability_set_description(
    UmiWorkbenchSelectionNodeCapability *record,
    const char *description);
uint64_t umi_workbench_selection_node_capability_hash(
    const UmiWorkbenchSelectionNodeCapability *record);
void umi_workbench_selection_node_capability_touch(
    UmiWorkbenchSelectionNodeCapability *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
