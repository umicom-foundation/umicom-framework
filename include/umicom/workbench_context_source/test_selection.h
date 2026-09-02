/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/test_selection.h
 *
 * PURPOSE:
 *   Define the reusable test selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_TEST_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_TEST_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source test selection data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceTestSelection {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceTestSelection;

/**
 * Initialise workbench context source test selection from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_test_selection_init(
    UmiWorkbenchContextSourceTestSelection *record,
    const char *record_id);
/**
 * Check that workbench context source test selection satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_test_selection_validate(
    const UmiWorkbenchContextSourceTestSelection *record);
/**
 * Provide the workbench context source test selection set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_test_selection_set_source(
    UmiWorkbenchContextSourceTestSelection *record,
    const char *source_id);
/**
 * Provide the workbench context source test selection set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_test_selection_set_panel(
    UmiWorkbenchContextSourceTestSelection *record,
    const char *panel_id);
/**
 * Provide the workbench context source test selection set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_test_selection_set_subject(
    UmiWorkbenchContextSourceTestSelection *record,
    const char *subject_id);
/**
 * Provide the workbench context source test selection set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_test_selection_set_group(
    UmiWorkbenchContextSourceTestSelection *record,
    const char *group_id);
/**
 * Provide the workbench context source test selection set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_test_selection_set_label(
    UmiWorkbenchContextSourceTestSelection *record,
    const char *label);
/**
 * Provide the workbench context source test selection hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_test_selection_hash(
    const UmiWorkbenchContextSourceTestSelection *record);
/**
 * Provide the workbench context source test selection touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_test_selection_touch(
    UmiWorkbenchContextSourceTestSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
