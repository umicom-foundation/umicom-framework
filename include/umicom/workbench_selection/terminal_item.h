/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/terminal_item.h
 *
 * PURPOSE:
 *   Define the reusable structured terminal item contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_TERMINAL_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_TERMINAL_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionTerminalItem {
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
} UmiWorkbenchSelectionTerminalItem;

void umi_workbench_selection_terminal_item_init(
    UmiWorkbenchSelectionTerminalItem *record,
    const char *record_id);
UmiStatus umi_workbench_selection_terminal_item_validate(
    const UmiWorkbenchSelectionTerminalItem *record);
UmiStatus umi_workbench_selection_terminal_item_set_source(
    UmiWorkbenchSelectionTerminalItem *record,
    const char *source_id);
UmiStatus umi_workbench_selection_terminal_item_set_subject(
    UmiWorkbenchSelectionTerminalItem *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_terminal_item_set_secondary(
    UmiWorkbenchSelectionTerminalItem *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_terminal_item_set_group(
    UmiWorkbenchSelectionTerminalItem *record,
    const char *group_id);
UmiStatus umi_workbench_selection_terminal_item_set_label(
    UmiWorkbenchSelectionTerminalItem *record,
    const char *label);
uint64_t umi_workbench_selection_terminal_item_hash(
    const UmiWorkbenchSelectionTerminalItem *record);
void umi_workbench_selection_terminal_item_touch(
    UmiWorkbenchSelectionTerminalItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
