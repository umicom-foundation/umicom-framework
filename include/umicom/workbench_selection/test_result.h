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

/**
 * Represent the workbench selection test result data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench selection test result from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_test_result_init(
    UmiWorkbenchSelectionTestResult *record,
    const char *record_id);
/**
 * Check that workbench selection test result satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_test_result_validate(
    const UmiWorkbenchSelectionTestResult *record);
/**
 * Provide the workbench selection test result set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_test_result_set_source(
    UmiWorkbenchSelectionTestResult *record,
    const char *source_id);
/**
 * Provide the workbench selection test result set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_test_result_set_subject(
    UmiWorkbenchSelectionTestResult *record,
    const char *subject_id);
/**
 * Provide the workbench selection test result set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_test_result_set_secondary(
    UmiWorkbenchSelectionTestResult *record,
    const char *secondary_id);
/**
 * Provide the workbench selection test result set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_test_result_set_group(
    UmiWorkbenchSelectionTestResult *record,
    const char *group_id);
/**
 * Provide the workbench selection test result set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_test_result_set_label(
    UmiWorkbenchSelectionTestResult *record,
    const char *label);
/**
 * Provide the workbench selection test result hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_test_result_hash(
    const UmiWorkbenchSelectionTestResult *record);
/**
 * Provide the workbench selection test result touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_test_result_touch(
    UmiWorkbenchSelectionTestResult *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
