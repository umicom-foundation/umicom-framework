/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/test_item.h
 *
 * PURPOSE:
 *   Define the reusable structured Test Explorer item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_TEST_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_TEST_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionTestItem {
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
} UmiWorkbenchSelectionTestItem;

void umi_workbench_selection_test_item_init(
    UmiWorkbenchSelectionTestItem *record,
    const char *record_id);
UmiStatus umi_workbench_selection_test_item_validate(
    const UmiWorkbenchSelectionTestItem *record);
UmiStatus umi_workbench_selection_test_item_set_source(
    UmiWorkbenchSelectionTestItem *record,
    const char *source_id);
UmiStatus umi_workbench_selection_test_item_set_subject(
    UmiWorkbenchSelectionTestItem *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_test_item_set_secondary(
    UmiWorkbenchSelectionTestItem *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_test_item_set_group(
    UmiWorkbenchSelectionTestItem *record,
    const char *group_id);
UmiStatus umi_workbench_selection_test_item_set_label(
    UmiWorkbenchSelectionTestItem *record,
    const char *label);
uint64_t umi_workbench_selection_test_item_hash(
    const UmiWorkbenchSelectionTestItem *record);
void umi_workbench_selection_test_item_touch(
    UmiWorkbenchSelectionTestItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
