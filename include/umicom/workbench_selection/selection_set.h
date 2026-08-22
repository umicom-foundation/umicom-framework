/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/selection_set.h
 *
 * PURPOSE:
 *   Define the reusable multi-item selection set contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SELECTION_SET_H
#define UMICOM_WORKBENCH_SELECTION_SELECTION_SET_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionSelectionSet {
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
} UmiWorkbenchSelectionSelectionSet;

void umi_workbench_selection_selection_set_init(
    UmiWorkbenchSelectionSelectionSet *record,
    const char *record_id);
UmiStatus umi_workbench_selection_selection_set_validate(
    const UmiWorkbenchSelectionSelectionSet *record);
UmiStatus umi_workbench_selection_selection_set_set_source(
    UmiWorkbenchSelectionSelectionSet *record,
    const char *source_id);
UmiStatus umi_workbench_selection_selection_set_set_subject(
    UmiWorkbenchSelectionSelectionSet *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_selection_set_set_related(
    UmiWorkbenchSelectionSelectionSet *record,
    const char *related_id);
UmiStatus umi_workbench_selection_selection_set_set_group(
    UmiWorkbenchSelectionSelectionSet *record,
    const char *group_id);
UmiStatus umi_workbench_selection_selection_set_set_description(
    UmiWorkbenchSelectionSelectionSet *record,
    const char *description);
uint64_t umi_workbench_selection_selection_set_hash(
    const UmiWorkbenchSelectionSelectionSet *record);
void umi_workbench_selection_selection_set_touch(
    UmiWorkbenchSelectionSelectionSet *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
