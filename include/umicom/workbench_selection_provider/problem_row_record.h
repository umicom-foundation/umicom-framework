/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/problem_row_record.h
 *
 * PURPOSE:
 *   Define the reusable structured problem row record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_ROW_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_ROW_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider problem row record data shared with callers
 * of this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderProblemRowRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderProblemRowRecord;

/**
 * Initialise workbench selection provider problem row record from caller-provided values
 * so later operations receive a known state.
 */
void umi_workbench_selection_provider_problem_row_record_init(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider problem row record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_problem_row_record_validate(
    const UmiWorkbenchSelectionProviderProblemRowRecord *record);
/**
 * Provide the workbench selection provider problem row record set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_problem_row_record_set_provider(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider problem row record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_problem_row_record_set_source(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider problem row record set subject operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_problem_row_record_set_subject(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider problem row record set related operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_problem_row_record_set_related(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider problem row record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_problem_row_record_set_group(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider problem row record set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_problem_row_record_set_description(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider problem row record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_problem_row_record_hash(
    const UmiWorkbenchSelectionProviderProblemRowRecord *record);
/**
 * Provide the workbench selection provider problem row record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_problem_row_record_touch(
    UmiWorkbenchSelectionProviderProblemRowRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
