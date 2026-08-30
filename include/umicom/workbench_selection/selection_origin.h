/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/selection_origin.h
 *
 * PURPOSE:
 *   Define the reusable selection origin contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SELECTION_ORIGIN_H
#define UMICOM_WORKBENCH_SELECTION_SELECTION_ORIGIN_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionSelectionOrigin {
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
} UmiWorkbenchSelectionSelectionOrigin;

void umi_workbench_selection_selection_origin_init(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *record_id);
UmiStatus umi_workbench_selection_selection_origin_validate(
    const UmiWorkbenchSelectionSelectionOrigin *record);
UmiStatus umi_workbench_selection_selection_origin_set_source(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *source_id);
UmiStatus umi_workbench_selection_selection_origin_set_subject(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_selection_origin_set_secondary(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_selection_origin_set_group(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *group_id);
UmiStatus umi_workbench_selection_selection_origin_set_label(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *label);
uint64_t umi_workbench_selection_selection_origin_hash(
    const UmiWorkbenchSelectionSelectionOrigin *record);
void umi_workbench_selection_selection_origin_touch(
    UmiWorkbenchSelectionSelectionOrigin *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
