/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/application_item.h
 *
 * PURPOSE:
 *   Define the reusable structured application item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_APPLICATION_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_APPLICATION_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionApplicationItem {
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
} UmiWorkbenchSelectionApplicationItem;

void umi_workbench_selection_application_item_init(
    UmiWorkbenchSelectionApplicationItem *record,
    const char *record_id);
UmiStatus umi_workbench_selection_application_item_validate(
    const UmiWorkbenchSelectionApplicationItem *record);
UmiStatus umi_workbench_selection_application_item_set_source(
    UmiWorkbenchSelectionApplicationItem *record,
    const char *source_id);
UmiStatus umi_workbench_selection_application_item_set_subject(
    UmiWorkbenchSelectionApplicationItem *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_application_item_set_secondary(
    UmiWorkbenchSelectionApplicationItem *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_application_item_set_group(
    UmiWorkbenchSelectionApplicationItem *record,
    const char *group_id);
UmiStatus umi_workbench_selection_application_item_set_label(
    UmiWorkbenchSelectionApplicationItem *record,
    const char *label);
uint64_t umi_workbench_selection_application_item_hash(
    const UmiWorkbenchSelectionApplicationItem *record);
void umi_workbench_selection_application_item_touch(
    UmiWorkbenchSelectionApplicationItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
