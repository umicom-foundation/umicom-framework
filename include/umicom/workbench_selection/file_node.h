/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/file_node.h
 *
 * PURPOSE:
 *   Define the reusable file tree node contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FILE_NODE_H
#define UMICOM_WORKBENCH_SELECTION_FILE_NODE_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionFileNode {
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
} UmiWorkbenchSelectionFileNode;

void umi_workbench_selection_file_node_init(
    UmiWorkbenchSelectionFileNode *record,
    const char *record_id);
UmiStatus umi_workbench_selection_file_node_validate(
    const UmiWorkbenchSelectionFileNode *record);
UmiStatus umi_workbench_selection_file_node_set_source(
    UmiWorkbenchSelectionFileNode *record,
    const char *source_id);
UmiStatus umi_workbench_selection_file_node_set_subject(
    UmiWorkbenchSelectionFileNode *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_file_node_set_secondary(
    UmiWorkbenchSelectionFileNode *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_file_node_set_group(
    UmiWorkbenchSelectionFileNode *record,
    const char *group_id);
UmiStatus umi_workbench_selection_file_node_set_label(
    UmiWorkbenchSelectionFileNode *record,
    const char *label);
uint64_t umi_workbench_selection_file_node_hash(
    const UmiWorkbenchSelectionFileNode *record);
void umi_workbench_selection_file_node_touch(
    UmiWorkbenchSelectionFileNode *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
