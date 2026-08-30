/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/test_result.h
 *
 * PURPOSE:
 *   Define the reusable structured test result contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_TEST_RESULT_H
#define UMICOM_WORKBENCH_SELECTION_TEST_RESULT_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionTestResult {
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
} UmiWorkbenchSelectionTestResult;

void umi_workbench_selection_test_result_init(
    UmiWorkbenchSelectionTestResult *record,
    const char *record_id);
UmiStatus umi_workbench_selection_test_result_validate(
    const UmiWorkbenchSelectionTestResult *record);
UmiStatus umi_workbench_selection_test_result_set_source(
    UmiWorkbenchSelectionTestResult *record,
    const char *source_id);
UmiStatus umi_workbench_selection_test_result_set_subject(
    UmiWorkbenchSelectionTestResult *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_test_result_set_secondary(
    UmiWorkbenchSelectionTestResult *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_test_result_set_group(
    UmiWorkbenchSelectionTestResult *record,
    const char *group_id);
UmiStatus umi_workbench_selection_test_result_set_label(
    UmiWorkbenchSelectionTestResult *record,
    const char *label);
uint64_t umi_workbench_selection_test_result_hash(
    const UmiWorkbenchSelectionTestResult *record);
void umi_workbench_selection_test_result_touch(
    UmiWorkbenchSelectionTestResult *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
