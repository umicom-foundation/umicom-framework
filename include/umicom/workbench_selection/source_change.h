/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/source_change.h
 *
 * PURPOSE:
 *   Define the reusable structured source-control change contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SOURCE_CHANGE_H
#define UMICOM_WORKBENCH_SELECTION_SOURCE_CHANGE_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionSourceChange {
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
} UmiWorkbenchSelectionSourceChange;

void umi_workbench_selection_source_change_init(
    UmiWorkbenchSelectionSourceChange *record,
    const char *record_id);
UmiStatus umi_workbench_selection_source_change_validate(
    const UmiWorkbenchSelectionSourceChange *record);
UmiStatus umi_workbench_selection_source_change_set_source(
    UmiWorkbenchSelectionSourceChange *record,
    const char *source_id);
UmiStatus umi_workbench_selection_source_change_set_subject(
    UmiWorkbenchSelectionSourceChange *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_source_change_set_secondary(
    UmiWorkbenchSelectionSourceChange *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_source_change_set_group(
    UmiWorkbenchSelectionSourceChange *record,
    const char *group_id);
UmiStatus umi_workbench_selection_source_change_set_label(
    UmiWorkbenchSelectionSourceChange *record,
    const char *label);
uint64_t umi_workbench_selection_source_change_hash(
    const UmiWorkbenchSelectionSourceChange *record);
void umi_workbench_selection_source_change_touch(
    UmiWorkbenchSelectionSourceChange *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
