/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/account_item.h
 *
 * PURPOSE:
 *   Define the reusable structured account item contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_ACCOUNT_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_ACCOUNT_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionAccountItem {
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
} UmiWorkbenchSelectionAccountItem;

void umi_workbench_selection_account_item_init(
    UmiWorkbenchSelectionAccountItem *record,
    const char *record_id);
UmiStatus umi_workbench_selection_account_item_validate(
    const UmiWorkbenchSelectionAccountItem *record);
UmiStatus umi_workbench_selection_account_item_set_source(
    UmiWorkbenchSelectionAccountItem *record,
    const char *source_id);
UmiStatus umi_workbench_selection_account_item_set_subject(
    UmiWorkbenchSelectionAccountItem *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_account_item_set_secondary(
    UmiWorkbenchSelectionAccountItem *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_account_item_set_group(
    UmiWorkbenchSelectionAccountItem *record,
    const char *group_id);
UmiStatus umi_workbench_selection_account_item_set_label(
    UmiWorkbenchSelectionAccountItem *record,
    const char *label);
uint64_t umi_workbench_selection_account_item_hash(
    const UmiWorkbenchSelectionAccountItem *record);
void umi_workbench_selection_account_item_touch(
    UmiWorkbenchSelectionAccountItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
