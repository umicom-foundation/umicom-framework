/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/multi_selection.h
 *
 * PURPOSE:
 *   Define the reusable multi-selection state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_MULTI_SELECTION_H
#define UMICOM_WORKBENCH_SELECTION_MULTI_SELECTION_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection multi selection data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionMultiSelection {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionMultiSelection;

/**
 * Initialise workbench selection multi selection from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_multi_selection_init(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *record_id);
/**
 * Check that workbench selection multi selection satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_multi_selection_validate(
    const UmiWorkbenchSelectionMultiSelection *record);
/**
 * Provide the workbench selection multi selection set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_source(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *source_id);
/**
 * Provide the workbench selection multi selection set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_subject(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *subject_id);
/**
 * Provide the workbench selection multi selection set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_related(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *related_id);
/**
 * Provide the workbench selection multi selection set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_group(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *group_id);
/**
 * Provide the workbench selection multi selection set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_multi_selection_set_description(
    UmiWorkbenchSelectionMultiSelection *record,
    const char *description);
/**
 * Provide the workbench selection multi selection hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_multi_selection_hash(
    const UmiWorkbenchSelectionMultiSelection *record);
/**
 * Provide the workbench selection multi selection touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_multi_selection_touch(
    UmiWorkbenchSelectionMultiSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
