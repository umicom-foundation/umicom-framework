/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/node_path.h
 *
 * PURPOSE:
 *   Define the reusable structured node path contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_NODE_PATH_H
#define UMICOM_WORKBENCH_SELECTION_NODE_PATH_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_selection_node_path_init(
    UmiWorkbenchSelectionNodePath *record,
    const char *record_id);
UmiStatus umi_workbench_selection_node_path_validate(
    const UmiWorkbenchSelectionNodePath *record);
UmiStatus umi_workbench_selection_node_path_set_source(
    UmiWorkbenchSelectionNodePath *record,
    const char *source_id);
UmiStatus umi_workbench_selection_node_path_set_subject(
    UmiWorkbenchSelectionNodePath *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_node_path_set_related(
    UmiWorkbenchSelectionNodePath *record,
    const char *related_id);
UmiStatus umi_workbench_selection_node_path_set_group(
    UmiWorkbenchSelectionNodePath *record,
    const char *group_id);
UmiStatus umi_workbench_selection_node_path_set_description(
    UmiWorkbenchSelectionNodePath *record,
    const char *description);
uint64_t umi_workbench_selection_node_path_hash(
    const UmiWorkbenchSelectionNodePath *record);
void umi_workbench_selection_node_path_touch(
    UmiWorkbenchSelectionNodePath *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
