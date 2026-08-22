/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/debug_item.h
 *
 * PURPOSE:
 *   Define the reusable structured debugger item contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_DEBUG_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_DEBUG_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionDebugItem {
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
} UmiWorkbenchSelectionDebugItem;

void umi_workbench_selection_debug_item_init(
    UmiWorkbenchSelectionDebugItem *record,
    const char *record_id);
UmiStatus umi_workbench_selection_debug_item_validate(
    const UmiWorkbenchSelectionDebugItem *record);
UmiStatus umi_workbench_selection_debug_item_set_source(
    UmiWorkbenchSelectionDebugItem *record,
    const char *source_id);
UmiStatus umi_workbench_selection_debug_item_set_subject(
    UmiWorkbenchSelectionDebugItem *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_debug_item_set_secondary(
    UmiWorkbenchSelectionDebugItem *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_debug_item_set_group(
    UmiWorkbenchSelectionDebugItem *record,
    const char *group_id);
UmiStatus umi_workbench_selection_debug_item_set_label(
    UmiWorkbenchSelectionDebugItem *record,
    const char *label);
uint64_t umi_workbench_selection_debug_item_hash(
    const UmiWorkbenchSelectionDebugItem *record);
void umi_workbench_selection_debug_item_touch(
    UmiWorkbenchSelectionDebugItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
