/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/node_identity.h
 *
 * PURPOSE:
 *   Define the reusable structured node identity contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_NODE_IDENTITY_H
#define UMICOM_WORKBENCH_SELECTION_NODE_IDENTITY_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionNodeIdentity {
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
} UmiWorkbenchSelectionNodeIdentity;

void umi_workbench_selection_node_identity_init(
    UmiWorkbenchSelectionNodeIdentity *record,
    const char *record_id);
UmiStatus umi_workbench_selection_node_identity_validate(
    const UmiWorkbenchSelectionNodeIdentity *record);
UmiStatus umi_workbench_selection_node_identity_set_source(
    UmiWorkbenchSelectionNodeIdentity *record,
    const char *source_id);
UmiStatus umi_workbench_selection_node_identity_set_subject(
    UmiWorkbenchSelectionNodeIdentity *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_node_identity_set_related(
    UmiWorkbenchSelectionNodeIdentity *record,
    const char *related_id);
UmiStatus umi_workbench_selection_node_identity_set_group(
    UmiWorkbenchSelectionNodeIdentity *record,
    const char *group_id);
UmiStatus umi_workbench_selection_node_identity_set_description(
    UmiWorkbenchSelectionNodeIdentity *record,
    const char *description);
uint64_t umi_workbench_selection_node_identity_hash(
    const UmiWorkbenchSelectionNodeIdentity *record);
void umi_workbench_selection_node_identity_touch(
    UmiWorkbenchSelectionNodeIdentity *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
