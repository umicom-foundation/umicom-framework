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

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_selection_source_commit_init(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *record_id);
UmiStatus umi_workbench_selection_source_commit_validate(
    const UmiWorkbenchSelectionSourceCommit *record);
UmiStatus umi_workbench_selection_source_commit_set_source(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *source_id);
UmiStatus umi_workbench_selection_source_commit_set_subject(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_source_commit_set_secondary(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_source_commit_set_group(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *group_id);
UmiStatus umi_workbench_selection_source_commit_set_label(
    UmiWorkbenchSelectionSourceCommit *record,
    const char *label);
uint64_t umi_workbench_selection_source_commit_hash(
    const UmiWorkbenchSelectionSourceCommit *record);
void umi_workbench_selection_source_commit_touch(
    UmiWorkbenchSelectionSourceCommit *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
