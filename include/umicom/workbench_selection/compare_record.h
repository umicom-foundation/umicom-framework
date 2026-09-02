/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/compare_record.h
 *
 * PURPOSE:
 *   Define the reusable selection comparison record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_COMPARE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_COMPARE_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection compare record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionCompareRecord {
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
} UmiWorkbenchSelectionCompareRecord;

/**
 * Initialise workbench selection compare record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_compare_record_init(
    UmiWorkbenchSelectionCompareRecord *record,
    const char *record_id);
/**
 * Check that workbench selection compare record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_compare_record_validate(
    const UmiWorkbenchSelectionCompareRecord *record);
/**
 * Provide the workbench selection compare record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_compare_record_set_source(
    UmiWorkbenchSelectionCompareRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection compare record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_compare_record_set_subject(
    UmiWorkbenchSelectionCompareRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection compare record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_compare_record_set_related(
    UmiWorkbenchSelectionCompareRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection compare record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_compare_record_set_group(
    UmiWorkbenchSelectionCompareRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection compare record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_compare_record_set_description(
    UmiWorkbenchSelectionCompareRecord *record,
    const char *description);
/**
 * Provide the workbench selection compare record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_compare_record_hash(
    const UmiWorkbenchSelectionCompareRecord *record);
/**
 * Provide the workbench selection compare record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_compare_record_touch(
    UmiWorkbenchSelectionCompareRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
