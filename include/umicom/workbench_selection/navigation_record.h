/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/navigation_record.h
 *
 * PURPOSE:
 *   Define the reusable selection navigation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_NAVIGATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_NAVIGATION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionNavigationRecord {
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
} UmiWorkbenchSelectionNavigationRecord;

void umi_workbench_selection_navigation_record_init(
    UmiWorkbenchSelectionNavigationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_navigation_record_validate(
    const UmiWorkbenchSelectionNavigationRecord *record);
UmiStatus umi_workbench_selection_navigation_record_set_source(
    UmiWorkbenchSelectionNavigationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_navigation_record_set_subject(
    UmiWorkbenchSelectionNavigationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_navigation_record_set_secondary(
    UmiWorkbenchSelectionNavigationRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_navigation_record_set_group(
    UmiWorkbenchSelectionNavigationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_navigation_record_set_label(
    UmiWorkbenchSelectionNavigationRecord *record,
    const char *label);
uint64_t umi_workbench_selection_navigation_record_hash(
    const UmiWorkbenchSelectionNavigationRecord *record);
void umi_workbench_selection_navigation_record_touch(
    UmiWorkbenchSelectionNavigationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
