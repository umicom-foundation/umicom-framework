/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/selection_state.h
 *
 * PURPOSE:
 *   Define the reusable selection runtime state contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SELECTION_STATE_H
#define UMICOM_WORKBENCH_SELECTION_SELECTION_STATE_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionSelectionState {
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
} UmiWorkbenchSelectionSelectionState;

void umi_workbench_selection_selection_state_init(
    UmiWorkbenchSelectionSelectionState *record,
    const char *record_id);
UmiStatus umi_workbench_selection_selection_state_validate(
    const UmiWorkbenchSelectionSelectionState *record);
UmiStatus umi_workbench_selection_selection_state_set_source(
    UmiWorkbenchSelectionSelectionState *record,
    const char *source_id);
UmiStatus umi_workbench_selection_selection_state_set_subject(
    UmiWorkbenchSelectionSelectionState *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_selection_state_set_secondary(
    UmiWorkbenchSelectionSelectionState *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_selection_state_set_group(
    UmiWorkbenchSelectionSelectionState *record,
    const char *group_id);
UmiStatus umi_workbench_selection_selection_state_set_label(
    UmiWorkbenchSelectionSelectionState *record,
    const char *label);
uint64_t umi_workbench_selection_selection_state_hash(
    const UmiWorkbenchSelectionSelectionState *record);
void umi_workbench_selection_selection_state_touch(
    UmiWorkbenchSelectionSelectionState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
